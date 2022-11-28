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

template<typename T>
T moveAndPopLast(std::vector<T>& v)
{
    T temp = std::move(v.back());
    v.pop_back();
    return temp; //Copy ellision guaranteed
}

std::vector<Triplet> genNTriplets(std::filesystem::path folder, int n)
{
    std::mt19937 rng(std::random_device{}());

    std::vector<Triplet> result(n);
    std::vector<Item> people_1;
    std::vector<std::pair<std::string, std::vector<int>>> people_n;
    for(auto entry : std::filesystem::directory_iterator(folder))
    {
        if(!entry.is_directory()) continue;
        int count = 0;
        for(auto file : std::filesystem::directory_iterator(entry))
        {
            if(file.path().extension() == ".jpg")
            {
                ++count;
            }
        }
        if(count == 1) people_1.emplace_back(entry.path().filename(), "1");
        else
        {
            std::vector<int> v(count);
            std::iota(v.begin(), v.end(), 1);
            std::ranges::shuffle(v, rng);
            if(v.size() > 5) v.resize(5); //Prevent overfitting on one person
            people_n.emplace_back(entry.path().filename(), std::move(v));
        }
    }
    std::ranges::shuffle(people_1, rng);
    std::ranges::shuffle(people_n, rng);

    auto takeOneFromPeopleN = [&]()
    {
        Item result;
        result.name = people_n.back().first;
        result.index = std::to_string(moveAndPopLast(people_n.back().second));
        if(people_n.back().second.size() < 2)
        {
            auto [name, indexes] = moveAndPopLast(people_n);
            if(indexes.size() > 0) people_1.emplace_back(std::move(name), std::to_string(indexes.back()));
        }
        return result;
    };

    for(auto& [anchor, positive, negative] : result)
    {
        if(!people_1.empty()) negative = moveAndPopLast(people_1);
        else negative = takeOneFromPeopleN();

        anchor.name = people_n.back().first;
        anchor.index = std::to_string(moveAndPopLast(people_n.back().second));
        positive = takeOneFromPeopleN();
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
        triplets = genNTriplets(folder, std::stoi(argv[2]));
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
