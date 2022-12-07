#include "tools.h"
#include <string_view>
#include <filesystem>
#include <vector>
#include <fstream>
#include <random>
#include <algorithm>
#include <cmath>


namespace
{
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

    std::vector<Triplet> genFromPairs(std::filesystem::path folder, std::filesystem::path pairFile, int n = 0)
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
        auto rng = std::mt19937(std::random_device{}());
        std::ranges::shuffle(result, rng);
        std::ranges::shuffle(others, rng);
        name_it = names.begin();
        for(Triplet& triplet : result)
        {
            std::string& name = *name_it++;
            auto other_it = others.crbegin();
            while(other_it->first == name) ++other_it;
            triplet.negative = buildItem(folder, other_it->first, other_it->second);
            others.erase(other_it.base()-1);
        }
        if(n) result.resize(std::min<int>(n, result.size()));
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

    void writeTriplets(std::ostream& output, const std::vector<Triplet>& triplets)
    {
        output << triplets.size() << std::endl;
        for(const Triplet& triplet : triplets)
        {
            output << triplet.anchor.name << ' ' << triplet.anchor.index << ' '
                << triplet.positive.index << ' '
                << triplet.negative.name << ' ' << triplet.negative.index << std::endl;
        }
    }

    void writePairs(std::ostream& output, std::vector<Triplet> triplets)
    {
        int n = triplets.size();
        output << n << std::endl;
        
        unsigned int cpt = 0;
        bool pos = true;
        srand (time(NULL));
        std::shuffle(std::begin(triplets),std::end(triplets),std::default_random_engine());
        for(const Triplet& triplet : triplets)
        {
            if(cpt++ >= n)
                break;
            else if (rand() % n > n/2)
                pos = !pos;

            if(pos)
            {
                output << triplet.anchor.name << ' ' << triplet.anchor.index << ' '
                << triplet.positive.name<<' '<< triplet.positive.index << ' ' <<std::endl;
            }
            else
            {
                output << triplet.anchor.name << ' ' << triplet.anchor.index << ' '
                << triplet.negative.name<<' '<< triplet.negative.index << ' ' <<std::endl;
            }
        }
    }
}

void genTripletDataset(std::ostream& output, std::filesystem::path folder, int n)
{
    writeTriplets(output, genNTriplets(folder, n));
}

void genTripletDataset(std::ostream& output, std::filesystem::path folder, std::filesystem::path pairFile, int n)
{
    writeTriplets(output, genFromPairs(folder, pairFile, n));
}

void genPairDataset(std::ostream& output, std::filesystem::path folder, int n)
{
    writePairs(output, genNTriplets(folder, n));
}

void genPairDataset(std::ostream& output, std::filesystem::path folder, std::filesystem::path pairFile, int n)
{
    writePairs(output, genFromPairs(folder, pairFile, n));
}
