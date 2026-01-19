#include <iostream>

#include "NetworkServer.h"

int main(int argc, char** argv) {
    unsigned short port = 9000;
    if (argc > 1) {
        port = static_cast<unsigned short>(std::stoi(argv[1]));
    }

    try {
        NetworkServer server(port);
        std::cout << "YunoServer listening on port " << port << '\n';
        server.Run();
    }
    catch (const std::exception& exception) {
        std::cerr << "Failed to start server: " << exception.what() << '\n';
        return 1;
    }

    return 0;
}
