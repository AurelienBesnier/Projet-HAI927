#include "tools.h"
#include <fstream>


int main(int argc, char* argv[])
{
    if(argc != 3) return -1;
    std::ifstream file(argv[2]);
    processFile(argv[1], file);
    return 0;
}
