
#include "pch.h"
#include "fmodPCH.h"

#include "Bank.h"

std::string BankHelper::GetPath(FMOD::Studio::EventDescription* d)
{
    if (!d) return {};
    int len = 0;
    d->getPath(nullptr, 0, &len);
    std::string s(len, '\0');
    d->getPath(s.data(), len, &len);
    if (!s.empty() && s.back() == '\0') s.pop_back();
    return s;
}

std::string BankHelper::GetPath(FMOD::Studio::Bus* b)
{
    if (!b) return {};
    int len = 0;
    b->getPath(nullptr, 0, &len);
    std::string s(len, '\0');
    b->getPath(s.data(), len, &len);
    if (!s.empty() && s.back() == '\0') s.pop_back();
    return s;
}

std::string BankHelper::GetPath(FMOD::Studio::VCA* v)
{
    if (!v) return {};
    int len = 0;
    v->getPath(nullptr, 0, &len);
    std::string s(len, '\0');
    v->getPath(s.data(), len, &len);
    if (!s.empty() && s.back() == '\0') s.pop_back();
    return s;
}

void BankHelper::IndexBankContent(const std::string& bankPath, FMOD::Studio::Bank* bank)
{
    BankContent content;
    content.bank = bank;

    // --- Events ---
    int evCount = 0;
    FMOD_RESULT r = bank->getEventCount(&evCount);
    CheckFMOD(r, "Bank::getEventCount");

    if (r == FMOD_OK && evCount > 0)
    {
        std::vector<FMOD::Studio::EventDescription*> list(evCount);
        r = bank->getEventList(list.data(), evCount, &evCount);
        CheckFMOD(r, "Bank::getEventList");

        for (int i = 0; i < evCount; ++i)
        {
            auto path = GetPath(list[i]);
            if (!path.empty())
            {
                content.events.push_back(path);
                mEventRef[path]++; // refcount 증가
            }
        }
    }

    // --- Buses ---
    int busCount = 0;
    r = bank->getBusCount(&busCount);
    CheckFMOD(r, "Bank::getBusCount");

    if (r == FMOD_OK && busCount > 0)
    {
        std::vector<FMOD::Studio::Bus*> list(busCount);
        r = bank->getBusList(list.data(), busCount, &busCount);
        CheckFMOD(r, "Bank::getBusList");

        for (int i = 0; i < busCount; ++i)
        {
            auto path = GetPath(list[i]);
            if (!path.empty())
            {
                content.buses.push_back(path);
                mBusRef[path]++;
            }
        }
    }

    // --- VCAs ---
    int vcaCount = 0;
    r = bank->getVCACount(&vcaCount);
    CheckFMOD(r, "Bank::getVCACount");

    if (r == FMOD_OK && vcaCount > 0)
    {
        std::vector<FMOD::Studio::VCA*> list(vcaCount);
        r = bank->getVCAList(list.data(), vcaCount, &vcaCount);
        CheckFMOD(r, "Bank::getVCAList");

        for (int i = 0; i < vcaCount; ++i)
        {
            auto path = GetPath(list[i]);
            if (!path.empty())
            {
                content.vcas.push_back(path);
                mVcaRef[path]++;
            }
        }
    }

    mBankContents[bankPath] = std::move(content);
}

