#include "pch.h"

#include "YunoShaderCompiler.h"



namespace
{
    // ID3DInclude 구현: include 디렉터리를 순회하며 파일을 찾는다.
    class IncludeHandler final : public ID3DInclude
    {
    public:
        explicit IncludeHandler(const std::vector<std::wstring>& includeDirs)
            : m_includeDirs(includeDirs) {
        }

        HRESULT __stdcall Open(D3D_INCLUDE_TYPE /*IncludeType*/,
            LPCSTR pFileName,
            LPCVOID /*pParentData*/,
            LPCVOID* ppData,
            UINT* pBytes) override
        {
            if (!pFileName || !ppData || !pBytes)
                return E_INVALIDARG;

            // pFileName은 ANSI 경로
            std::wstring wFileName;
            {
                const int len = static_cast<int>(strlen(pFileName));
                wFileName.resize(len);
                for (int i = 0; i < len; ++i) wFileName[i] = static_cast<wchar_t>(pFileName[i]);
            }

            // includeDirs + fileName으로 파일 열기 시도
            for (const auto& dir : m_includeDirs)
            {
                std::wstring full = dir;
                if (!full.empty() && full.back() != L'\\' && full.back() != L'/')
                    full += L'\\';
                full += wFileName;

                FILE* fp = nullptr;
                _wfopen_s(&fp, full.c_str(), L"rb");
                if (!fp) continue;

                fseek(fp, 0, SEEK_END);
                const long size = ftell(fp);
                fseek(fp, 0, SEEK_SET);

                if (size <= 0)
                {
                    fclose(fp);
                    continue;
                }

                char* mem = new char[size];
                fread(mem, 1, size, fp);
                fclose(fp);

                *ppData = mem;
                *pBytes = static_cast<UINT>(size);
                return S_OK;
            }

            return E_FAIL;
        }

        HRESULT __stdcall Close(LPCVOID pData) override
        {
            delete[] reinterpret_cast<const char*>(pData);
            return S_OK;
        }

    private:
        const std::vector<std::wstring>& m_includeDirs;
    };
}

void YunoShaderCompiler::AddIncludeDir(std::wstring dir)
{
    m_includeDirs.emplace_back(std::move(dir));
}

ComPtr<ID3DBlob> YunoShaderCompiler::CompileFromFile(
    const std::wstring& filePath,
    const std::string& entryPoint,
    const std::string& profile,
    const std::vector<std::pair<std::string, std::string>>& defines) const
{
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(_DEBUG)
    flags |= D3DCOMPILE_DEBUG;
    flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

    // 매크로 정의 구성
    std::vector<D3D_SHADER_MACRO> macros;
    macros.reserve(defines.size() + 1);
    for (const auto& kv : defines)
    {
        D3D_SHADER_MACRO m{};
        m.Name = kv.first.c_str();
        m.Definition = kv.second.c_str();
        macros.push_back(m);
    }
    macros.push_back({ nullptr, nullptr });

    IncludeHandler includeHandler(m_includeDirs);

    ComPtr<ID3DBlob> bytecode;
    ComPtr<ID3DBlob> errors;

    const HRESULT hr = D3DCompileFromFile(
        filePath.c_str(),
        macros.data(),
        &includeHandler,
        entryPoint.c_str(),
        profile.c_str(),
        flags,
        0,
        bytecode.GetAddressOf(),
        &errors);

    if (FAILED(hr))
    {
        std::string msg = "D3DCompileFromFile failed.\n";

        // 1) HRESULT 코드/설명
        char hrBuf[64]{};
        sprintf_s(hrBuf, "HRESULT=0x%08X\n", (unsigned)hr);
        msg += hrBuf;

        // 2) 어떤 파일/엔트리/프로파일인지
        msg += "File: ";
        msg += std::string(filePath.begin(), filePath.end());
        msg += "\nEntry: " + entryPoint + "\nProfile: " + profile + "\n";

        // 3) 컴파일러가 준 상세 에러 메시지(가장 중요)
        if (errors)
        {
            msg += "---- Compiler Output ----\n";
            msg += std::string((const char*)errors->GetBufferPointer(),
                errors->GetBufferSize());
            msg += "\n-------------------------\n";
        }
        else
        {
            msg += "(No error blob)\n";
        }

        throw std::runtime_error(msg);
    }


    return bytecode;
}
