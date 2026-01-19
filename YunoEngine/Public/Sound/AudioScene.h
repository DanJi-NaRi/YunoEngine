#pragma once
#include <ranges>

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
    void Load(std::string bankName) override
    {
        mBankName = bankName;
        AudioSystem::Get().LoadBank(mBankName);
    }

    // 씬 떠날 시
    void Unload() override
    {
        ClearInstList();
        AudioSystem::Get().UnloadBank(mBankName);
    }

    void Update(float dt) override
    {
        // 필요하면 여기서 씬 전용 파라미터(예: 체력) 갱신
        // mInstList["name"].SetParameter("Health", playerHealth);
    }

    // 2D음원 재생 시, eventName만 기입.
    // 3D음원 재생 시, eventName, is3D, pos, dir 값 전달 모두 필요
    void PlayEvent(const std::string& eventName, bool is3D = false, XMFLOAT3 pos = {}, XMFLOAT3 dir = {});

    // 씬에서 만든 인스턴스 정리
    void ClearInstList();

private:
    std::string mBankName;
    std::unordered_map<std::string, EventHandle> mInstList; // 재생 음원들
    std::unordered_map<std::string, FMOD_STUDIO_PARAMETER_ID> mParamList; // 파라미터들
    std::unordered_map<std::string, std::vector<FMOD_STUDIO_PARAMETER_ID>> mParamCaches; // 이벤트 별 파라미터 관리
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

    void SetParam(std::string_view name, float value);
    void SetState();
};
