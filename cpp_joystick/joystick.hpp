#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <string>
#include <ostream>
#include <vector>
#include "fd_guard.hpp"

class Joystick {
    
public:
    Joystick(std::string location);

    // Updates the internal state by polling the fd.
    void update();
    friend std::ostream& operator<<(std::ostream& os, const Joystick& js);

private:
    fd_guard fd;
    bool initialized = false;

    char num_axes = 0;
    char num_buttons = 0;

    std::vector<int16_t> axes;
    std::vector<int16_t> buttons;
};


#endif
