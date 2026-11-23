#include "controller.h"

Controller::Controller() {}

Controller::convertInputToTone(std::string input) {
    if (input == "W" || input == "w") {
        return 1; // Forward
    } else if (input == "S" || input == "s") {
        return 2; // Backward
    } else if (input == "A" || input == "a") {
        return 3; // Left
    } else if (input == "D" || input == "d") {
        return 4; // Right
    }
}

Controller::~Controller();