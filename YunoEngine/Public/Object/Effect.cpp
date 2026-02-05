#include "pch.h"

#include "Effect.h"
#include "YunoCamera.h"

Effect::Effect()
{
    m_pRenderer = YunoEngine::GetRenderer();
}

void Effect::SetTemplate(const EffectTemplate& temp)
{
    // 템플릿 리소스 적용
    m_renderItem.meshHandle = temp.mesh;
    m_renderItem.materialHandle = temp.material;

    m_lifetime = temp.lifetime;
    m_emissive = temp.emissive;
    m_emissiveCol = temp.color;
    m_vRot = temp.rot;
    m_frameCount = temp.frameCount;
    m_cols = temp.cols;
    m_rows = temp.rows;

    flipbook = (m_frameCount > 1);

    billboard = temp.billboard;
}

void Effect::Play(const XMFLOAT3& pos, const XMFLOAT3& scale, const XMFLOAT3& dir)
{
    m_age = 0.0f;
    m_active = true;
    SetPos(pos);
    SetScale(scale);
    m_vDir = dir;
}

void Effect::Reset()
{
    // 상태 초기화
    m_active = false;
    m_age = 0.0f;

    // Transform 초기화
    m_vScale = { 1,1,1 };
    m_vRot = { 0,0,0 };

    // 렌더링 값 초기화
    m_renderItem.Constant.opacity = 1.0f;
    m_renderItem.Constant.baseColor = { 1,1,1,1 };
    m_renderItem.Constant.emissiveColor = { 1,1,1,1 };

    // Effect 데이터 초기화
    m_renderItem.isEffect = false;
    m_renderItem.effectConst.effectData = { 0,0,0,0 };
}

void Effect::UpdateWorldMatrix()
{
    XMMATRIX S = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);
    XMMATRIX T = XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z);

    XMMATRIX R1 = XMMatrixRotationRollPitchYaw(m_vRot.x, m_vRot.y, m_vRot.z);
    XMMATRIX R2 = UpdateBillBoard();
    XMMATRIX R = R1 * R2;

    XMMATRIX mTM;

    if (m_Parent)
    {
        mTM = S * R * T * m_Parent->GetAttachMatrixForChild(this);
    }
    else
        mTM = S * R * T;

    XMStoreFloat4x4(&m_mWorld, mTM);
}

XMMATRIX Effect::UpdateBillBoard()
{
    switch(billboard)
    {
    case BillboardMode::ScreenAligned :
        return UpdateScreenAlign();
    case BillboardMode::WorldAligned:
        return UpdateWorldUpAlign();
    case BillboardMode::AxisLocked :
        return UpdateAxisLock();
    case BillboardMode::Beam:
        return UpdateBeam();
    default:
        return XMMatrixIdentity();
    }
}

XMMATRIX Effect::UpdateBeam()
{
    if (m_Parent)
    {

    }
    XMVECTOR right = XMVector3Normalize(
        XMLoadFloat3(&m_vDir)
    );

    XMVECTOR toCam = XMVector3Normalize(
        XMLoadFloat3(&m_pRenderer->GetCamera().Position()) - XMLoadFloat3(&m_vPos)
    );

    XMVECTOR up = XMVector3Normalize(
        XMVector3Cross(right, toCam)
    );

    XMVECTOR front = XMVector3Normalize(
        XMVector3Cross(right, up)
    );

    XMMATRIX R;
    R.r[0] = right;
    R.r[1] = up;
    R.r[2] = front;
    R.r[3] = XMVectorSet(0, 0, 0, 1);

    return R;
}

XMMATRIX Effect::UpdateScreenAlign()
{
    XMMATRIX V = m_pRenderer->GetCamera().View();

    // View 행렬의 역행렬 = Camera World Transform
    XMMATRIX invView = XMMatrixInverse(nullptr, V);

    // Camera 기준 축
    XMVECTOR right = invView.r[0]; // 카메라 X축
    XMVECTOR up = invView.r[1]; // 카메라 Y축
    XMVECTOR look = invView.r[2]; // 카메라 Z축

    // Billboard 회전행렬 생성
    XMMATRIX R;
    R.r[0] = right;
    R.r[1] = up;
    R.r[2] = look;
    R.r[3] = XMVectorSet(0, 0, 0, 1);

    return R;
}

XMMATRIX Effect::UpdateWorldUpAlign()
{
    // 카메라 위치 가져오기
    const XMFLOAT3& camPos = m_pRenderer->GetCamera().Position();

    XMVECTOR P = XMLoadFloat3(&m_vPos);
    XMVECTOR C = XMLoadFloat3(&camPos);

    XMVECTOR worldUp = XMVectorSet(0, 1, 0, 0);

    // 카메라 방향 = 카메라 → 이펙트
    XMVECTOR look = XMVector3Normalize(P - C);
    look = XMVectorSetY(look, 0.0f);

    if (XMVector3LengthSq(look).m128_f32[0] < 0.0001f)
    {
        look = XMVectorSet(0, 0, 1, 0);
    }

    look = XMVector3Normalize(look);
    XMVECTOR right = XMVector3Normalize(XMVector3Cross(worldUp, look));

    XMVECTOR up = worldUp;

    // billboard 회전 행렬 생성
    XMMATRIX R;
    R.r[0] = right;
    R.r[1] = up;
    R.r[2] = look;
    R.r[3] = XMVectorSet(0, 0, 0, 1);

    return R;
}

XMMATRIX Effect::UpdateAxisLock()
{
    XMVECTOR forward = XMVector3Normalize(XMLoadFloat3(&m_vDir));
    XMVECTOR worldup = XMVectorSet(0, 1, 0, 0);

    // 카메라 forward 가져오기 (빌보드 두께 유지용)
    XMVECTOR toCam = XMVector3Normalize(
        XMLoadFloat3(&m_pRenderer->GetCamera().Position()) - XMLoadFloat3(&m_vPos)
    );
    // right = 카메라 기준으로 빔이 얇아지지 않게
    XMVECTOR right = XMVector3Normalize(
        XMVector3Cross(toCam, forward)
    );

    // up = forward x right
    XMVECTOR up = XMVector3Normalize(
        XMVector3Cross(forward, right)
    );

    /*right = XMVector3Normalize(
        XMVector3Cross(worldup, forward)
    );*/
    //XMVECTOR up = -camForward;

    // 회전행렬 생성
    XMMATRIX R;
    R.r[0] = right;
    R.r[1] = up;
    R.r[2] = forward;
    R.r[3] = XMVectorSet(0, 0, 0, 1);

    return R;
}

void Effect::UpdateFrame()
{
    if (!flipbook) return;

    float t = m_age / m_lifetime;
    int frame = (int)(t * m_frameCount);

    frame = std::min(frame, m_frameCount - 1);

    m_renderItem.isEffect = true;
    m_renderItem.Constant.emissive = m_emissive;
    m_renderItem.Constant.emissiveColor = m_emissiveCol;
    m_renderItem.effectConst.effectData =
    {
        (float)frame,
        m_age,
        (float)m_cols,
        (float)m_rows
    };
}

bool Effect::Update(float dt)
{
    m_age += dt;

    if (m_age >= m_lifetime)
        return false;

    UpdateWorldMatrix();
    UpdateFrame();

    return true;
}

bool Effect::Submit(float dt)
{
    if (!m_active) return true;

    m_renderItem.Constant.world = m_mWorld;
    m_renderItem.Constant.worldPos = m_vPos;

    m_pRenderer->Submit(m_renderItem);

    return true;
}
