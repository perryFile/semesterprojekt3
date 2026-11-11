//Controller.h
#ifndef CONTROLLER_H
#define CONTROLLER_h

#include <string>
#include <termios.h>

enum class Key {
    Up,
    Down,
    Left,
    Right,
    Other
};

class Controller {
public:
    Controller();
    ~Controller();

    Controller(const Controller&) = delete;
    Controller& operator=(const Controller&) = delete;

    Key readKey() const;

private:
    char readByte() const;
    bool readByteWithTimeout(char& value) const;

    termios original_{};
    bool active_ = false;
};

#endif  // CONTROLLER_H