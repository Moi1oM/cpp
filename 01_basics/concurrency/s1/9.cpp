#include <iostream>
#include <thread>
#include <queue>
#include <chrono>

using namespace std;

enum struct Commands {
    noCommand = 0,
    clean = 1,
    fullSpeed = 2,
    stopEngine = 3,
    stopProgram = 100
};

class CleaningCrew {

};