#include <iostream>

#include "YunoServerNetwork.h"

int main(int argc, char** argv)
{
    std::uint16_t port = 9000;

    yuno::server::YunoServerNetwork serverNet;
    if (!serverNet.Start(port)) return 1;

    std::cout << "[YunoServer] Running. port=" << port << "\n";

    // 단일 스레드 서버 루프
    while (true)
    {
        serverNet.Tick();                 // asio 이벤트 처리
        std::this_thread::sleep_for(
            std::chrono::milliseconds(1)  // CPU 점유율 완화
        );
    }

    // 여기 도달 안 함 (강제 종료)
    // serverNet.Stop();
    // return 0;
}
