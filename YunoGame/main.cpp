#include "pch.h"

#include "YunoEngine.h"
#include "GameApp.h"

int main()
{
#ifdef _DEBUG
    // CRT 디버그 메모리 누수 체크 활성화
    int dbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    dbgFlag |= _CRTDBG_ALLOC_MEM_DF;      // 할당 추적
    dbgFlag |= _CRTDBG_LEAK_CHECK_DF;     // 종료 시 자동 Dump
    _CrtSetDbgFlag(dbgFlag);
#endif


    GameApp game;
    YunoEngine engine;

    std::cout << "[CWD] " << std::filesystem::current_path().string() << "\n";
    if (!engine.Initialize(&game, L"YunoGame", 1280, 720)) 
    {
        int n;
        std::cin >> n;
        return -1;
    }



    return engine.Run();
}
