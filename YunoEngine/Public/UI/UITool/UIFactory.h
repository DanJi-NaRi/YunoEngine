#pragma once
class UIManager;
class Float2;

//Widget한테 UIManager의 일부 기능을 공급하는 팩토리
class UIFactory
{
public:
    UIFactory() = delete;
    UIFactory(UIManager& uiManager);
    ~UIFactory() = default;

    template<typename T>
    T* CreateWidget(const std::wstring& name, XMFLOAT3 pos); // 위젯 생성

    Float2 GetCanvasSize(); // 캔버스 사이즈 or 클라이언트 사이즈 반환

private:
    UIManager& m_uiManager;
};

