#include "ArgParse/ArgParse.h"
#include "Gex/include/Gex.h"

#define EXECUTE "exec"


int main(int argc, char** argv)
{
    Parser parser;

    if (argc < 1)
    {
        std::cerr << "A command is reauired." << std::endl;
        return -1;
    }


    return 0;
}