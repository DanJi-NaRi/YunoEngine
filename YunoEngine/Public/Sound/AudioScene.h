#pragma once

class IAudioScene
{
public:
    virtual void Load(std::string bankName) = 0;
    virtual void Unload() = 0;
    virtual void Update(float dt) = 0;
};


// Base AudioScene
class AudioScene : public IAudioScene
{
public:
    // 씬 진입 시
    void Load(std::string bankName) override;

    // 씬 떠날 시
    void Unload() override;

    void Update(float dt) override
    {
        // 필요하면 여기서 씬 전용 파라미터(예: 체력) 갱신
        // m_InstList["name"].SetParameter("Health", playerHealth);
    }

    // 3D음원이 있다면 리스너 업데이트 필수!
    void Listener3DUpdate(XMFLOAT3 pos, XMFLOAT3 forward, XMFLOAT3 up = { 0, 1, 0 }, XMFLOAT3 vel = {});
    // 3D음원의 위치가 바뀌었다면? 업데이트 필수!
    void Emitter3DUpdate(std::string eventName, XMFLOAT3 pos, XMFLOAT3 forward, XMFLOAT3 up = { 0, 1, 0 }, XMFLOAT3 vel = {});

    // 2D음원 재생 시, eventName만 기입.
    // 3D음원 재생 시, eventName, is3D, pos, dir 값 전달 모두 필요
    void PlayEvent(const std::string& eventName, bool is3D = false, XMFLOAT3 pos = {}, XMFLOAT3 forward = {}, XMFLOAT3 up = { 0, 1, 0 }, XMFLOAT3 vel = {});

    // 씬에서 만든 인스턴스 정리
    void ClearInstList();

private:
    std::string m_BankName;
    std::unordered_map<std::string, EventHandle> m_InstList; // 재생 음원들
    std::unordered_map<std::string, FMOD_STUDIO_PARAMETER_ID> m_ParamList; // key: 파라미터 이름, value: 파라미터ID
    std::unordered_map<std::string, std::vector<std::string>> m_EventParameterNames; // key: 이벤트 이름, value: 파라미터 이름
};


class TitleAudio : public AudioScene
{
public:
    void Enter()
    {
        Load("TitleScene");
    }
    void Exit()
    {
        Unload();
    }

    void SetParam(std::string name, float value);
    void SetState();
};
