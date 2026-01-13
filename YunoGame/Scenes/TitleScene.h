#pragma once
#include <iostream>

#include "IScene.h"

class Quad;
class Building;
class ObjectManager;

class TitleScene final : public IScene
{
public:
    bool OnCreate() override;
    void OnEnter() override { std::cout << "[TitleScene] OnEnter\n"; }
    void OnExit() override { std::cout << "[TitleScene] OnExit\n"; }
    void OnDestroy() override;

    void Update(float dt) override;
    void Submit(IRenderer* renderer) override;

    const char* GetDebugName() const override { return "TitleScene"; }

private:
<<<<<<< HEAD
    Quad* m_plane = nullptr;   // 간단하게 raw로 들고, OnDestroy에서 delete
=======
    Quad* m_plane = nullptr;   // �����ϰ� raw�� ���, OnDestroy���� delete
    Building* m_building = nullptr;   // �����ϰ� raw�� ���, OnDestroy���� delete
<<<<<<< HEAD
>>>>>>> 933f6d3 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
=======
>>>>>>> 1178914 (화면에 뜨는데 파싱할 때 텍스쳐 핸들까지는 받는데 적용이 안됌)
    float m_lastDt = 0.0f;
    ObjectManager* m_ObjManager;
};
