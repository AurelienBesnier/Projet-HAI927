#include "tools.h"
#include <iostream>


int main(int argc, char* argv[])
{
    using namespace std::string_literals;
    if(argc<5)
    {
        std::cerr << "Usage : " << argv[0] << " dataset.txt from_folder to_folder [random] method1 [param] [method2 [param]...]\n";
        return EXIT_FAILURE;
    }
    bool random = argv[4] == "random"s;
    std::vector<std::string> method_descriptor(argv + 4 + random, argv + argc);
    obscureDataSet(std::cout, argv[1], argv[2], argv[3], method_descriptor, random);
  
    return EXIT_SUCCESS;
}