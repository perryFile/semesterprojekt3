#include "DSP.h"
#include <iostream>
#include <stdexcept>
#include <system_error>
#include <vector>
#include <cerrno>
#include <poll.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>

namespace {

enum class Key {
    Up,
    Down,
    Left,
    Right,
    Other
};

class TerminalInput {
public:
    TerminalInput() {
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

    ~TerminalInput() {
        if (active_) {
            tcsetattr(STDIN_FILENO, TCSANOW, &original_);
        }
    }

    Key readKey() const {
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

private:
    char readByte() const {
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

    bool readByteWithTimeout(char& value) const {
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

    termios original_{};
    bool active_ = false;
};

}

int main()
{
    int robotIDudp = 0;
    int robotIDtcp = 1;
    std::vector<int> data = {0, 0, 0, 0}; 
    
    int blocksize = 379; 
    int samplerate = 8000; 
    int channels = 1; 
    std::vector<int> toner = {};

    DSP dsp(blocksize, samplerate, channels, {robotIDudp, robotIDtcp});
    
    int c = 0;
    TerminalInput input;

    
    
    while(1)
    {
        c = 0;

        switch (input.readKey()) {
            case Key::Up:
                std::cout << "Up arrow pressed" << std::endl;
                data = {robotIDudp, 7, 7, 7};
                dsp.sendData(data); 
                break;
            case Key::Down:
                std::cout << "Down arrow pressed" << std::endl;
                data = {robotIDudp, 0, 0, 0};
                dsp.sendData(data); 
                break;
            case Key::Left:
                std::cout << "Left arrow pressed" << std::endl;
                data = {robotIDudp, 15, 15, 15};
                dsp.sendData(data); 
                break;
            case Key::Right:
                std::cout << "Right arrow pressed" << std::endl;
                data = {robotIDudp, 0, 0, 0};
                dsp.sendData(data); 
                break;
            default:
                std::cout << "Other key pressed" << std::endl;
                break;
            }

    }
    

    return 0;
}