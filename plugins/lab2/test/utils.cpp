#include <iostream>

extern "C" void loop_start() { std::cout << "[INFO] LOOP START" << "\n"; }

extern "C" void loop_end() { std::cout << "[INFO] LOOP END" << "\n"; }
