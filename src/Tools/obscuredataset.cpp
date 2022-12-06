#include "tools.h"
#include <iostream>


int main(int argc, char* argv[])
{
    if(argc<5)
    {
        std::cerr << "Usage : " << argv[0] << " dataset.txt from_folder to_folder method1 [param] method2 [param]...\n";
        return EXIT_FAILURE;
    }

    std::vector<std::string> method_descriptor(argv + 4, argv + argc);
    obscureDataSet(std::cout, argv[1], argv[2], argv[3], method_descriptor);
  
    return EXIT_SUCCESS;
}