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
    if(argc != 3) return EXIT_FAILURE;
    std::filesystem::path folder = argv[1];
    if(isNumber(argv[2]))
    {
        genTripletDataset(std::cout, folder, std::stoi(argv[2]));
    }
    else
    {
        genTripletDataset(std::cout, folder, argv[2]);
    }
    return 0;
}