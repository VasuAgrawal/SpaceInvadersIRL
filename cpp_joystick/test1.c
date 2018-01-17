#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <thread>
#include <chrono>

#include <sys/ioctl.h>
#include <linux/joystick.h>

int main(int argc, char** argv) {

    int fd = open("/dev/input/js0", O_RDONLY | O_NONBLOCK);
    struct js_event e;

    std::this_thread::sleep_for(std::chrono::seconds(5));

    char num_axes, num_buttons;
    int driver_version;

    if (ioctl(fd, JSIOCGAXES, &num_axes) < 0) {
        printf("Shit\n");
    }
    ioctl(fd, JSIOCGBUTTONS, &num_buttons);
    ioctl(fd, JSIOCGVERSION, &driver_version);
    printf("Num axes: %d, num buttons: %d, driver version: %d\n", 
            num_axes, num_buttons, driver_version);

    while(true) {
        if (read(fd, &e, sizeof(e)) > 0) {
            printf("Time: %u, value: %d, type: %d, number: %d\n",
                    e.time, e.value, e.type, e.number); 
        }
    }


    return 0;
}
