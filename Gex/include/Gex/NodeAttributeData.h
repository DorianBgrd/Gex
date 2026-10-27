#ifndef GEX_NODEATTRIBUTEDATA_H
#define GEX_NODEATTRIBUTEDATA_H

#include "api.h"
#include "Attribute.h"
#include "Status.h"

#include <any>


namespace Gex
{
    class Node;
    class PythonCommandNode;

    class GEX_API NodeAttributeData
    {
        friend Attribute;
        friend Node;

    protected:
        NodePtr node;
        AttributePtr attribute;


        explicit NodeAttributeData(const NodePtr& n);


        explicit NodeAttributeData(const AttributePtr& attr);


        explicit NodeAttributeData(const AttributeWkPtr& attr);


    public:
        NodePtr GetNode() const;

        TSys::TypePtr GetAttributeType() const;

        TSys::TypedValuePtr GetAttributeValue() const;

        std::vector<TSys::TypedValuePtr> GetAttributeArrayValues() const;

        bool SetAttributeValue(const TSys::TypedValuePtr& value) const;

        bool SetAttributeIndexValue(unsigned int index, const TSys::TypedValuePtr& value) const;

        bool CreateIndexValue(unsigned int index, const TSys::TypedValuePtr& value) const;

        bool IsInput() const;

        bool IsOutput() const;

        bool IsExclusiveOutput() const;

        template<class T> T Get(Feedback* success=nullptr) const
        {
            if (auto value = GetAttributeValue())
            {
                if (success)
                    success->status = Status::Success;

                return value->Get<T>();
            }

            if (success)
                success->status = Status::Failed;
            return T();
        }


        template<class T> bool Set(T value)
        {
            if (IsExclusiveOutput())
            {
                if (auto attributeValue = GetAttributeValue())
                {
                    return attributeValue->Set<T>(value);
                }
            }

            return false;
        }

        std::vector<unsigned int> GetIndices(Feedback* status=nullptr);

        bool CreateIndex(unsigned int index)
        {
            if (!attribute->IsArray())
                return false;

            return attribute->_CreateIndex(index);
        }


        template<class T>
        bool CreateIndex(unsigned int index, T value)
        {
            if (!attribute->IsArray())
                return false;

            if (!attribute->HasIndex(index))
            {
                if (!attribute->_CreateIndex(index))
                    return false;
            }

            return SetIndexValue(index, value);
        }


        template<class T> bool SetIndexValue(unsigned int index, T value)
        {
            if (!attribute->IsArray())
                return false;

            if (!attribute->HasIndex(index))
                return false;

            AttributeWkPtr indexAttr = attribute->GetIndexAttribute(index);
            if (!indexAttr)
                return false;

            return indexAttr->Set(value);
        }


        template<class T>
        std::vector<T> GetArrayValues()
        {
            std::vector<T> array;
            if (!attribute->IsArray())
            {
                return array;
            }

            return attribute->GetArrayValues<T>();
        }


        NodeAttributeData GetAttribute(const std::string& name, Feedback* status=nullptr) const;


        NodeAttributeData GetIndex(unsigned int index, Feedback* status=nullptr) const;

    public:

        template <class T>
        T GetIndexValue(unsigned int index, Feedback* status=nullptr)
        {
            AttributeWkPtr indexAttr = attribute->GetIndexAttribute(index);
            if (!indexAttr)
            {
                if (status)
                {
                    status->status = Status::Failed;
                }

                return T();
            }

            return indexAttr->Get<T>(status);
        }


        bool ClearMultiIndices() const;


        bool HasAttribute(const std::string& name) const;


        bool HasIndex(unsigned int index) const;


        std::vector<unsigned int> Indices() const;

    };

    /*
    class AttributeValueHandle;

    typedef std::shared_ptr<AttributeValueHandle> AttributeValueHandlePtr;
    typedef std::weak_ptr<AttributeValueHandle> AttributeValueHandleWkPtr;

    class GEX_API AttributeValueHandle
    {
        const std::string name;
        const TSys::TypedValuePtr attributeValue;
        const AttributeValueHandleWkPtr source;

        std::map<std::string, AttributeValueHandlePtr> childValueHandle;

    public:
        AttributeValueHandle(const AttributePtr& attribute);

        TSys::TypedValuePtr GetTypedValue() const;

        template<class T>
        T Get() const
        {
            return GetTypedValue()->Get<T>();
        }

        template<class T>
        bool Set(T value)
        {
            return GetTypedValue()->Set<T>();
        }

        AttributeValueHandlePtr GetChildAttribute(
                const std::string& name
        ) const;
    };


    class GEX_API NodeAttributeData
    {
        std::map<std::string, AttributeValueHandlePtr> attributes;

    public:
        NodeAttributeData(const Gex::NodePtr& node);

        AttributeValueHandleWkPtr GetAttribute(
                const std::string& name
        ) const;
    };
*/
};


#endif //GEX_NODEATTRIBUTEDATA_H
