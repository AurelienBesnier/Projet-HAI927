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

struct Triplet
{
    Item anchor, positive, negative;
};

Item buildItem(std::filesystem::path folder, std::string name, std::string index)
{
    return Item{std::move(name), std::move(index)};
}

std::vector<Triplet> genFromPairs(std::filesystem::path folder, std::filesystem::path pairFile)
{
    std::ifstream file(pairFile);
    int size;
    file >> size;
    std::vector<Triplet> result(size);
    std::vector<std::string> names(size); auto name_it = names.begin();
    for(Triplet& triplet : result)
    {
        std::string& name = *name_it++;
        std::string index;
        file >> name;
        file >> index;
        triplet.anchor = buildItem(folder, name, index);
        file >> index;
        triplet.positive = buildItem(folder, name, index);
    }
    std::vector<std::pair<std::string,std::string>> others(size * 2);
    for(auto& [name, index] : others)
    {
        file >> name;
        file >> index;
    }
    std::ranges::shuffle(others, std::mt19937(std::random_device{}()));
    name_it = names.begin();
    for(Triplet& triplet : result)
    {
        std::string& name = *name_it++;
        auto other_it = others.crbegin();
        while(other_it->first == name) ++other_it;
        triplet.negative = buildItem(folder, other_it->first, other_it->second);
        others.erase(other_it.base()-1);
    }
    return result;
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
    std::vector<Triplet> triplets;
    if(isNumber(argv[2]))
    {
        //TODO : implement pulling random triplets
        return EXIT_FAILURE;
    }
    else
    {
        triplets = genFromPairs(folder, argv[2]);
    }
    std::cout << triplets.size() << std::endl;
    for(const Triplet& triplet : triplets)
    {
        std::cout << triplet.anchor.name << ' ' << triplet.anchor.index << ' '
            << triplet.positive.index << ' '
            << triplet.negative.name << ' ' << triplet.negative.index << std::endl;
    }
    return 0;
}
