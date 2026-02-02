#pragma once
#pragma once
#include "WidgetGridSystem.h"
#include "PieceHelper.h"
#include "TileHelper.h"

class MinimapQ;
class IInput;

enum class NG_M : int   // NowGrid_Minimap
{
    One,
    Max
};

class MinimapGridSystem : public WidgetGridSystem
{
private:
    IInput* m_input = nullptr;

public:
    MinimapGridSystem(UIManager* uimng);                   // 씬에서 uiManager 받기
    MinimapGridSystem(UIManager* uimng, IInput* input);
    virtual ~MinimapGridSystem();

    void CreateObject(float x, float y, float z) override;
    void Update(float dt) override;

private:
    void Init();
    void CreateTileAndPiece(float x, float y, float z);
    void CheckMyQ();
    void CheckInput();

private:
    void MoveEvent(const GamePiece& pieceType, int cx, int cy);
    bool CheckExisting(const GamePiece pieceType);

private:
    void ChangeTileTO(int cx, int cy, const TileOccupy to);
    const TileOccupy GetTileTO(int cx, int cy);

    Float2 GetUIwPos(int cx, int cy);
    Float2 GetwPos(float uwx, float uwy);

    Direction GetDir(float oldcx, float oldcy, float cx, float cy);

    Direction GetCollisionDir(float oldcx, float oldcy, float cx, float cy);
    Float2 GetCollisionPos(Direction dir, Direction pieceDir, int cx, int cy);

private:
    Float3 m_vPos{};
    float m_wz;                                             // 기물의 world y
    float m_uiScale;

    NG_M m_nowG;

    std::vector<TileState> m_tiles;                         // 타일 상태
    std::vector<uint32_t> m_tilesIDs;                       // 타일 오브젝트 아이디
    std::unordered_map<GamePiece, PieceInfo> m_pieces;      // 기물 정보

    std::unique_ptr<MinimapQ> m_minimapQ;
};

