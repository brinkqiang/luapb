
#include "yaml-cpp/yaml.h"
#include <iostream>
#include <string>
#include "yaml-cpp/emittermanip.h"
#include "yaml-cpp/emittermanip.h"
#include "dmutil.h"

int main(int argc, char* argv[]) {
    DMSetWorkPath();
    {
        // test.yaml
        // - foo
        // - primes: [2, 3, 5, 7, 11]
        //   odds: [1, 3, 5, 7, 9, 11]
        // - [x, y]
        auto root = YAML::LoadFile("config/test.yaml");

        std::cout << root[0].as<std::string>() << std::endl;       // "foo"
        std::cout << root[1]["primes"][3].as<int>() << std::endl;  // "7"
        std::cout << root[1]["odds"][5].as<int>() << std::endl;    // "11"

        root[2].push_back(5);
        root[3] = "Hello, World";
        root[0].reset();
        root[4]["key"] = "value";

        std::cout << root << std::endl;
    }

    return 0;
}
