
#include "pch.h"
#include "fmodPCH.h"

#include "Bank.h"

BankHelper::~BankHelper()
{
    Clear();
}

void BankHelper::Clear()
{
    m_BankContents.clear();
    m_EventRef.clear();
}

const char* BankHelper::GetBankPath(std::string name)
{
    tmpPath = (bankPath + name + extension); return tmpPath.c_str();
}

std::string BankHelper::GetPath(FMOD::Studio::EventDescription* d)
{
    if (!d) return {};
    int len = 0;
    d->getPath(nullptr, 0, &len);
    std::string s(len, '\0');
    d->getPath(s.data(), len, &len);
    std::string res = s.substr(s.find("/")+1);
    if (!res.empty() && res.back() == '\0') res.pop_back();
    return res;
}

void BankHelper::IndexBankContent(const std::string& bankPath, FMOD::Studio::Bank* bank)
{
    BankContent content;
    content.bank = bank;

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
                m_EventRef[path]++; // refcount 증가
            }
        }
    }

    m_BankContents[bankPath] = std::move(content);
}

