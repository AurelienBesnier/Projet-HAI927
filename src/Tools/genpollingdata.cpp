#include "tools.h"
#include <iostream>

namespace
{
    bool isNumber(std::string_view s)
    {
        for(char c : s) if(!isdigit(c)) return false;
        return !s.empty();
    }
}

int main(int argc, char* argv[])
{
    if(argc < 3 || argc > 4) return EXIT_FAILURE;
    std::filesystem::path folder = argv[1];
    if(isNumber(argv[2]))
    {
        genPairDataset(std::cout, folder, std::atoi(argv[2]));
    }
    else if (!isNumber(argv[3]))
    {
        return EXIT_FAILURE;
    }
    else
    {
        genPairDataset(std::cout, folder, argv[2], std::atoi(argv[3]));
    }

    return 0;
}
