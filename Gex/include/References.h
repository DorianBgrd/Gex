#ifndef GEX_REFERENCES_H
#define GEX_REFERENCES_H

#include <vector>
#include <string>

#include "api.h"

namespace Gex
{
    class GEX_API References
    {
        static References* instance;

        std::vector<std::string> directories;
    private:
        References();

    public:
        static References* GetLoader();

        void AddDirectory(const std::string& dir);

        bool RemDirectory(const std::string& dir);

        std::string ResolvePath(const std::string& name) const;

        std::vector<std::string> GetAvailableReferences() const;

        std::vector<std::string> GetDirectories() const;
    };
}

#endif //GEX_REFERENCES_H
