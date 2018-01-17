#ifndef FD_GUARD_H
#define FD_GUARD_H

#include <unistd.h>

class fd_guard {
public:
    // Store the fd internally.
    fd_guard(int fd) : fd(fd) {}

    // Close the fd on destruction.
    ~fd_guard() {
        if (fd > -1) {
            // TODO: Error checking?
            ::close(fd);
            fd = -1;
        }
    }

    // Allow for comparisons to be performed.
    operator int() const {
        return fd;
    }

    // Delete copy constructor and copy assignment operator
    fd_guard(const fd_guard& other) = delete;
    fd_guard& operator=(const fd_guard& other) = delete;

private:
    int fd;
};

#endif
