#include "DisplaySystem.h"
#include "obscuration.h"

int main(int argc, char* argv[])
{
    if(argc != 2) throw 42;

    processImage(argv[1], blur, Param{"Radius", 255});

    return 0;
}