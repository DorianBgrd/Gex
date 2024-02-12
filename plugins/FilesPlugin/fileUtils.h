#ifndef PIPELINELIBRARY2COMPLETE_FILEUTILS_H
#define PIPELINELIBRARY2COMPLETE_FILEUTILS_H

#include <string>
#include <cstdlib>


std::string TempFilename(const std::string& name, const std::string& extension)
{
    int random = std::rand();

    return name + std::to_string(random) + extension;
}


#endif //PIPELINELIBRARY2COMPLETE_FILEUTILS_H
