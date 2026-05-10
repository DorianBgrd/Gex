#ifndef GEX_JSON_READER_H
#define GEX_JSON_READER_H

#include <any>
#include <string>
#include <functional>

#include "jsonDefs.h"

namespace JsonPlugin
{
    class Reader;
    typedef std::shared_ptr<Reader> ReaderPtr;

    class Reader
    {
        ReaderPtr parent;
        rapidjson::Document document;
    public:
        void SetParent(const ReaderPtr& prnt);

        ReaderPtr GetParent() const;

        virtual rapidjson::Value& ReadValue(
                rapidjson::Value& value
                ) const = 0;

        rapidjson::Value& Read(
                rapidjson::Value& value
                ) const;

        virtual bool IsExtractable() const = 0;

        virtual std::any Extract(rapidjson::Value& value) const;

        std::any ExtractValue() const;
    };

    class DocumentReader: public Reader
    {
        rapidjson::Document& doc;
    public:
        DocumentReader(rapidjson::Document& json);

        virtual rapidjson::Value& ReadValue(
                rapidjson::Value& value
        ) const override;

        bool IsExtractable() const override;
    };

    class DictValue: public Reader
    {
        std::string key;
    public:
        DictValue(const std::string& key);

        virtual rapidjson::Value& ReadValue(
                rapidjson::Value& value
                ) const override;

        bool IsExtractable() const override;
    };

    class ListValue: public Reader
    {
        unsigned int index;
    public:
        ListValue(unsigned int index);

        virtual rapidjson::Value& ReadValue(
                rapidjson::Value& value
                ) const override;

        bool IsExtractable() const override;
    };


    class Extractor: public Reader
    {
        rapidjson::Value & ReadValue(
                rapidjson::Value &value
        ) const override;

        std::any Extract(rapidjson::Value& value) const override;
    };

    template<class T, typename... Args>
    ReaderPtr MakeReader(Args... args)
    {
        return std::make_shared<T>(args...);
    }
}

#endif //GEX_JSON_READER_H
