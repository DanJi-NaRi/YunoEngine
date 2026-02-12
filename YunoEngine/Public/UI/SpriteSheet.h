#pragma once
#include "Widget.h"

// 각 오브젝트 헤더 파일 최상단에는 Widget 헤더가 있어야됨
class SpriteSheet : public Widget {
public:
    explicit SpriteSheet(UIFactory& uiFactory);
    virtual ~SpriteSheet();

    bool Create(const std::wstring& name, uint32_t id, Float2 sizePx, XMFLOAT3 vPos, float rotZ, XMFLOAT3 vScale) override;
    bool Start() override;
    bool Update(float dTime = 0) override;
    bool Submit(float dTime = 0) override;

    void SetSpriteSheet(const std::wstring& texturePath, int cols, int rows, int frameCount, float fps, bool loop = true, bool reverse = false);
    void SetFrameIndex(int frameIndex);
    void SetFrameCount(int frameCount);
    void SetGrid(int cols, int rows);
    void SetFPS(float fps);
    void SetLoop(bool loop);
    void SetReverse(bool reverse);

    void Play();
    void Pause();
    void Stop(bool resetFrame = true);

    int GetFrameIndex() const { return m_frameIndex; }
    int GetFrameCount() const { return m_frameCount; }
    int GetCols() const { return m_cols; }
    int GetRows() const { return m_rows; }
    bool IsPlaying() const { return m_playing; }
    bool IsFinished() const { return m_finished; }
    bool IsReverse() const { return m_reverse; }


    virtual WidgetType GetWidgetType() override { return WidgetType::SpriteSheet; }
    virtual WidgetClass GetWidgetClass() override { return WidgetClass::SpriteSheet; }

private:
    bool CreateMaterial() override;
    bool ApplySpriteMaterial();

    void AdvanceAnimation(float dTime);
    void UpdateFrameConstants();
    int PackGrid() const;

    std::wstring m_spriteSheetPath = L"../Assets/Textures/black.png";
    int m_cols = 1;
    int m_rows = 1;
    int m_frameCount = 1;
    int m_frameIndex = 0;
    float m_fps = 12.0f;
    float m_frameTimer = 0.0f;
    float m_elapsed = 0.0f;
    bool m_loop = true;
    bool m_playing = true;
    bool m_reverse = false;
    bool m_finished = false;
};
