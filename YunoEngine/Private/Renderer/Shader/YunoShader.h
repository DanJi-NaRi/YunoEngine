#pragma once



struct ID3D11Device;
struct ID3D11VertexShader;
struct ID3D11PixelShader;

class YunoShader
{
public:
    bool CreateVertexShader(ID3D11Device* device, ID3DBlob* bytecode);
    bool CreatePixelShader(ID3D11Device* device, ID3DBlob* bytecode);

    ID3D11VertexShader* VS() const { return m_vs.Get(); }
    ID3D11PixelShader* PS() const { return m_ps.Get(); }

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  m_ps;
};
