#include <iostream>
#include <chrono>

#include "joystick.hpp"

int main(int argc, char** argv) {
    Joystick joystick("/dev/input/js0");

    auto start = std::chrono::steady_clock::now();
    while (true) {
        joystick.update();

        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        if (elapsed_seconds.count() > 0.1) {
            std::cout << joystick;
            start = end;
        }
    }
    return 0;
}
