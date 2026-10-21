#ifndef TSYS_DEFAULT_TYPES
#define TSYS_DEFAULT_TYPES

#include "api.h"

#include <string>
#include <unordered_map>


namespace TSys
{
    typedef std::pair<std::string, unsigned int> EnumValue;


    class TSYS_API Enum
    {
        std::vector<EnumValue> _enumValues;
        EnumValue _current;

    public:
        Enum() = default;

        explicit
        Enum(const std::vector<EnumValue>& values);

        explicit
        Enum(const std::vector<std::string>& values);

        explicit
        Enum(const std::unordered_map<std::string, unsigned int>& values);

        void AddValue(const EnumValue& value);

        unsigned int GetIndex(const std::string& stringValue) const;

        std::string GetString(unsigned int index) const;

        std::vector<std::string> GetStrings() const;

        std::vector<unsigned int> GetIndices() const;

        bool Set(const std::string& stringValue);

        bool Set(unsigned int indexValue);

        std::string GetCurrentString() const;

        unsigned int GetCurrentIndex() const;

        operator unsigned int() const;

        operator std::string() const;

        Enum& operator=(const Enum& other);

        bool operator==(const Enum& other) const;
    };

    class TypeValueRegistry;

    TSYS_API
    void RegisterDefaultTypes(TypeValueRegistry* registry);
}

#endif //TSYS_DEFAULT_TYPES