#include <iostream>

#include "jsonny.hpp"


int main() {
    jsonny::JsonDocument doc;
    std::string json_string = R"({"key1": "value_for_key_1",
                                  "key2": false})";

    std::cout << "jsonny test run ..." << std::endl;

    doc.ParseString(json_string);

    return 0;
}