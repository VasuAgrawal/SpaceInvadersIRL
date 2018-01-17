#include "joystick.hpp"

#include <ios>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>

Joystick::Joystick(std::string name) : 
    fd(::open(name.c_str(), O_RDONLY | O_NONBLOCK)) {

    initialized = fd > -1;

    // Need to figure out how many buttons and axes there are.
    if (!initialized) {
        return;
    }

    ioctl(fd, JSIOCGAXES, &num_axes);
    ioctl(fd, JSIOCGBUTTONS, &num_buttons);

    axes.resize(num_axes);
    buttons.resize(num_buttons);
}

void Joystick::update() {
    if (!initialized) {
        return;
    }
    
    struct js_event e;
    while (read(fd, &e, sizeof(e)) > 0) {
        switch(e.type) {
            case JS_EVENT_BUTTON:
                buttons[e.number] = e.value;
                break;
            case JS_EVENT_AXIS:
                axes[e.number] = e.value;
                break;
        }
    }
}

std::ostream& operator<<(std::ostream& os, const Joystick& js) {
    if (!js.initialized) {
        os << "Joystick is not initialized!" << std::endl;
    } else {
        os << "Joystick open at fd: " << static_cast<int>(js.fd) << std::endl;

        os << "    Axes: [ ";
        for (auto axis : js.axes) {
            os.fill(' ');
            os.width(6);
            os << axis << ", ";
        }
        os << "]" << std::endl;

        os << "    Buttons: [ ";
        for (auto button : js.buttons) {
            os << button << ", ";
        }
        os << "]" << std::endl;
    }
}
