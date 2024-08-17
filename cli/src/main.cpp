#include "cli/include/exec.h"


int main(int argc, char** argv)
{

    if (argc < 1)
    {
        std::cerr << "A command is required." << std::endl;
        return -1;
    }

    std::string command = argv[1];
    if (command == "exec")
    {
        return Exec::ExecuteGraph(argc, argv, 2);
    }

    return 0;
}