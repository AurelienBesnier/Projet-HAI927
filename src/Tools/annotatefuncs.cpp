#include "tools.h"
#include "faceloc.h"
#include <filesystem>
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
            std::clog << img_path << '\n';
            loadFaceWithLoc(img_path);
        }
    }
}
