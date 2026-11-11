#include "controller.h"

#include <cerrno>
#include <poll.h>
#include <stdexcept>
#include <system_error>
#include <termios.h>
#include <unistd.h>

Controller::Controller() {
    if (tcgetattr(STDIN_FILENO, &original_) == -1) {
        throw std::system_error(errno, std::generic_category(), "tcgetattr");
    }

    termios raw = original_;
    raw.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO));
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == -1) {
        throw std::system_error(errno, std::generic_category(), "tcsetattr");
    }
    active_ = true;
}

Controller::~Controller() {
    if (active_) {
        tcsetattr(STDIN_FILENO, TCSANOW, &original_);
    }
}

char Controller::readByte() const {
    char value;
    const ssize_t count = ::read(STDIN_FILENO, &value, 1);
    if (count == -1) {
        throw std::system_error(errno, std::generic_category(), "read");
    }
    if (count == 0) {
        throw std::runtime_error("standard input closed");
    }
    return value;
}

bool Controller::readByteWithTimeout(char& value) const {
    pollfd input{STDIN_FILENO, POLLIN, 0};
    const int ready = poll(&input, 1, 100);
    if (ready == -1) {
        throw std::system_error(errno, std::generic_category(), "poll");
    }
    if (ready == 0) {
        return false;
    }
    value = readByte();
    return true;
}

Key Controller::readKey() const {
    const char first = readByte();
    if (first != '\x1b') {
        return Key::Other;
    }

    char second;
    char third;
    if (!readByteWithTimeout(second) || second != '[' ||
        !readByteWithTimeout(third)) {
        return Key::Other;
    }

    switch (third) {
        case 'A': return Key::Up;
        case 'B': return Key::Down;
        case 'D': return Key::Left;
        case 'C': return Key::Right;
        default: return Key::Other;
    }
}
