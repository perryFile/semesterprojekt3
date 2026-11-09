//Controller.h
#ifndef CONTROLLER_H
#define CONTROLLER_h

#include <array>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <map>
#include <optional>
#include <queue>
#include <string>
#include <vector>

class Controller {
    public:
        Controller();
        int convertInputToTone(std::string input);

        ~Controller() = default;

    private:
        
};


#endif  // CONTROLLER_H