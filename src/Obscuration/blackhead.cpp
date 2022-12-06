#include "DisplaySystem.h"
#include "obscuration.h"

int main(int argc, char* argv[])
{
    if(argc != 2) throw 42;

    processImage(argv[1], blackhead);

    return 0;
}