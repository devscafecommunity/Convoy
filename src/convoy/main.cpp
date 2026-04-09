// src/convoy/main.cpp
#include <iostream>

#include "core/window_manager.h"

int main(int, char*[])
{
    try
    {
        convoy::WindowManager window_manager;
        window_manager.initialize("Convoy - The Codex Art Station", 1600, 900);
        window_manager.run_loop();
        window_manager.shutdown();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
