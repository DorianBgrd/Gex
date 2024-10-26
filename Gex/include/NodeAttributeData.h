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


    public:
        NodePtr GetNode() const;


        std::any GetAnyValue(Feedback* success=nullptr);


        template<class T> T GetValue(Feedback* success=nullptr) const
        {
            if (!attribute)
            {
                if (success)
                    success->status = Status::Failed;
                return T();
            }

            if (success)
                success->status = Status::Success;

            return attribute->Get<T>(success);
        }


        bool SetAnyValue(std::any value);


        template<class T> bool SetValue(T value)
        {
            if (!attribute)
            {
                return false;
            }

            return SetAnyValue(std::make_any<T>(value));
        }


        std::vector<unsigned int> GetIndices(Feedback* status=nullptr);


        bool CreateIndex(unsigned int index)
        {
            if (!attribute->IsMulti())
                return false;

            return attribute->_CreateIndex(index);
        }


        template<class T>
        bool CreateIndex(unsigned int index, T value)
        {
            if (!attribute->IsMulti())
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
            if (!attribute->IsMulti())
                return false;

            if (!attribute->HasIndex(index))
                return false;

            AttributePtr indexAttr = attribute->GetIndexAttribute(index);
            if (!indexAttr)
                return false;

            return indexAttr->SetAnyValue(std::make_any<T>(value));
        }


        template<class T>
        std::vector<T> GetArrayValues()
        {
            std::vector<T> array;
            if (!attribute->IsMulti())
            {
                return array;
            }

            return attribute->GetArrayValues<T>();
        }


        NodeAttributeData GetAttribute(std::string name, Feedback* success=nullptr) const;


        NodeAttributeData GetIndex(unsigned int index, Feedback* success=nullptr) const;


        template <class T>
        T GetIndexValue(unsigned int index, Feedback* stat=nullptr)
        {
            if (!attribute->IsMulti())
            {
                if (stat)
                {
                    stat->status = Status::Failed;
                    stat->message = "Attribute \"" + attribute->Name() + "\" is not a multi attribute.";
                }
                return T();
            }

            if (!attribute->HasIndex(index))
            {
                if (stat)
                {
                    stat->status = Status::Failed;
                    stat->message = "Attribute \"" + attribute->Name() + "\" index " + std::to_string(index) +
                        "does not exist.";
                }

                return T();
            }

            AttributePtr indexAttr = attribute->GetIndexAttribute(index);
            if (!indexAttr)
            {
                if (stat)
                {
                    stat->status = Status::Failed;
                    stat->message = ("Attribute \"" + attribute->Name() + "\" - failed to retrieve index " +
                                     std::to_string(index) + "does not exist.");
                }

                return T();
            }

            return indexAttr->Get<T>(stat);
        }


        bool ClearMultiIndices() const;


        bool HasAttribute(std::string name) const;


        bool HasIndex(unsigned int index) const;


        std::vector<unsigned int> Indices() const;

    };

};


#endif //GEX_NODEATTRIBUTEDATA_H
