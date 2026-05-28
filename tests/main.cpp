#include <iostream>
#include <chrono>

#include "jsonny.hpp"

int main() {
    jsonny::JsonDocument doc;
    std::string json_string = R"({"BTN_NEW_GAME": "Новая игра",
                                  "BTN_LOAD_GAME": "Загрузить",
                                  "BTN_EXIT": "Выход",
                                  "LOADING": "Загрузка"})";

    std::cout << "jsonny test run ..." << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    bool success = doc.ParseString(json_string);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    if (success) {
        std::cout << "Parsing DONE" << std::endl;
        std::cout << "Time taken: " << elapsed.count() << " ms" << std::endl;
        doc.DebugPrint(); 
    } else {
        std::cout << "Parsing FAILED" << std::endl;
    }

    return 0;
}