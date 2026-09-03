#pragma once

struct BankContent
{
    FMOD::Studio::Bank* bank = nullptr;
    std::vector<std::string> events; // "event:/..."
};

// 목적: 깔끔. AudioCore의 BankHelper.
// 게임 처음부터 끝날 때까지 갖고가야 할 Bank 따로 관리
// Bank Load 시, 필요한 fullPath 얻어오기 위한 함수
// Bank Unload 시, 관련 캐시만 지우기 위한 헬퍼 map과 함수들
class BankHelper
{
private:
    std::string bankPath = "../Assets/Sound/Desktop/";
    std::string extension = ".bank";
    std::string tmpPath;

private:
    friend class AudioCore;

    BankHelper() = default;
    ~BankHelper();

    void Clear();

    const char* GetBankPath(std::string name);

    static std::string GetPath(FMOD::Studio::EventDescription* d);

    void IndexBankContent(const std::string& bankPath, FMOD::Studio::Bank* bank);

private:
    // bank별 구성 목록
    std::unordered_map<std::string, BankContent> m_BankContents;

    // refcount (여러 bank가 같은 경로를 “공유”하는 경우 안전)
    std::unordered_map<std::string, int> m_EventRef;
};
