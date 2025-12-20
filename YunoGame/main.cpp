#include "pch.h"
#include "YunoEngine.h"
#include "GameApp.h"


int main()
{
    YunoEngine engine;
    GameApp game;

    if (!engine.Initialize(&game))
        return -1;

    return engine.Run();
}