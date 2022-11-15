#include "faceloc.h"
#include <fstream>
#include <iostream>

void processFile(std::filesystem::path folder, std::istream& listing)
{
    std::string last_name;
    std::string token;
    int count;
    listing >> count;
    listing >> last_name;
    while(listing >> token)
    {
        if(!std::isdigit(token.front()))
        {
            last_name = std::move(token);
        }
        else
        {
            token.insert(0, 4-token.length(), '0');
            std::filesystem::path img_path = folder / last_name / (last_name + '_' + token + ".jpg");
            std::cout << img_path << '\n';
            loadFaceWithLoc(img_path);
        }
    }
}

int main(int argc, char* argv[])
{
    if(argc != 3) return -1;
    std::ifstream file(argv[2]);
    processFile(argv[1], file);
    return 0;
}
