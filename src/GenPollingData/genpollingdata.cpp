#include <string_view>
#include <filesystem>
#include <vector>
#include <fstream>
#include <iostream>
#include <random>
#include <algorithm>

struct Item
{
    std::string name, index;
};

struct Pair
{
    Item anchor, obscure;
};

Item buildItem(std::filesystem::path folder, std::string name, std::string index)
{
    return Item{std::move(name), std::move(index)};
}

std::vector<Pair> genPollingDataset(std::filesystem::path folder, std::filesystem::path pairFile, unsigned int size)
{
    //TODO: make a dataset of obscure images to compare with an anchor
    //It should be made like this: 
    //anchor vs blur, anchor vs pixellisation, anchor vs blackhead
    //next anchor ...
    
    
}

bool isNumber(std::string_view s)
{
    for(char c : s) if(!isdigit(c)) return false;
    return !s.empty();
}

int main(int argc, char* argv[])
{
    if(argc != 3) return EXIT_FAILURE;
    std::filesystem::path folder = argv[1];
    std::vector<Pair> pairs = genPollingDataset(folder, argv[2], 10);

    std::cout << pairs.size() << std::endl;
    for(const Pair& pair : pairs)
    {
        std::cout << pair.anchor.name << ' ' << pair.anchor.index << ' '
            << pair.obscure.name << ' ' << pair.obscure.index << std::endl;
    }

    return EXIT_SUCCESS;
}
