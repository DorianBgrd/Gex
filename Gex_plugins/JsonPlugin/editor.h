#ifndef GEX_JSON_EDITOR_H
#define GEX_JSON_EDITOR_H
#include <functional>
#include <unordered_map>
#include <any>
#include <typeindex>

#include "jsonDefs.h"

namespace JsonPlugin
{
    typedef std::function<void(
            const std::any&,
            rapidjson::Value&,
            rapidjson::Document&
    )> Generator;

    class ValueGenerator
    {
        static std::unordered_map<std::type_index, Generator> valueGenerators;

    public:
        static void RegisterGenerator(const std::type_index& index,
                                      const Generator& generator);

        template<class T>
        static void RegisterGenerator(const Generator& generator)
        {
            RegisterGenerator(typeid(T), generator);
        }

        static bool Generate(const std::any& value, rapidjson::Value& json,
                             rapidjson::Document& jsonDoc);
    };


    class Editor
    {
    public:
        virtual JsonValuePtr EditValue(JsonDoc& json) const = 0;
    };


    class EmptyEditor: public Editor
    {
    public:
        JsonValuePtr EditValue(JsonDoc& json) const override;
    };


    class IntEditor: public Editor
    {
        int value;

    public:
        IntEditor(int v);

        JsonValuePtr EditValue(JsonDoc& json) const override;
    };


    class FloatEditor: public Editor
    {
        float value;

    public:
        FloatEditor(float v);

        JsonValuePtr EditValue(JsonDoc& json) const override;
    };


    class DoubleEditor: public Editor
    {
        double value;

    public:
        DoubleEditor(double v);

        JsonValuePtr EditValue(JsonDoc& json) const override;
    };


    class StringEditor: public Editor
    {
        std::string value;

    public:
        StringEditor(const std::string& v);

        JsonValuePtr EditValue(JsonDoc& json) const override;
    };


    typedef std::shared_ptr<Editor> EditorPtr;
    typedef std::unordered_map<std::string, EditorPtr> EditorMap;
    typedef std::vector<EditorPtr> EditorList;


    class DictEditor: public Editor
    {
        EditorMap editors;

    public:
        DictEditor() = default;

        DictEditor(const EditorMap& values);

        void AddEditor(const std::string& key,
                       const EditorPtr& value);

        void RemoveEditor(const std::string& key);

        JsonValuePtr EditValue(JsonDoc& json) const override;
    };


    class ListEditor: public Editor
    {
        EditorList editors;

    public:
        ListEditor() = default;

        ListEditor(const EditorList& values);

        void AddEditor(const EditorPtr& value);

        void RemoveEditor(unsigned int index);

        JsonValuePtr EditValue(JsonDoc& json) const override;
    };
}

#endif //GEX_json_EDITOR_H
