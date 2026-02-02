#pragma once



class YunoShaderCompiler
{
public:
    void AddIncludeDir(std::wstring dir);

    // HLSL 파일 컴파일
    Microsoft::WRL::ComPtr<ID3DBlob> CompileFromFile(
        const std::wstring& filePath,
        const std::string& entryPoint,
        const std::string& profile,
        const std::vector<std::pair<std::string, std::string>>& defines = {}) const;

private:
    std::vector<std::wstring> m_includeDirs;
};
