#ifndef NODELIBRARY_ATTRIBUTE
#define NODELIBRARY_ATTRIBUTE

#include <vector>
#include <string>
#include <map>
#include <any>

#include "Status.h"

#include "SafePtrs/safeptr.h"
#include "rapidjson/document.h"

#include <regex>

#include "api.h"
#include <iostream>

namespace Gex
{

    enum class GEX_API AttrValueType
    {
        Single,
        Multi,
        Holder,
        MultiHolder
    };

    enum class GEX_API AttrType
    {
        Input,
        Output,
        Static
    };

    enum class GEX_API AttributeChange
    {
        Connected,
        Disconnected,
        ValueChanged,
        IndexAdded,
        IndexRemoved,
        AttributeAdded,
        AttributeRemoved,
        ChildAttributeConnected,
        ChildAttributeDisconnected,
        ChildAttributeValueChanged,
        ChildAttributeAdded,
        ChildAttributeIndexAdded,
        ChildAttributeIndexRemoved
    };

	class Node;
	class CompoundNode;
    class NodeAttributeData;

	class GEX_API Attribute: USING_SAFE_PTR(Attribute)
	{
		friend Node;
		friend CompoundNode;
		friend NodeAttributeData;

		std::string attributeName;
		std::string attributeLongname;
		bool attributeIsUserDefined;
        bool isInternal = false;
        bool isExternal = true;
		AttrValueType attributeValueType;
		AttrType attributeType;

        std::any defaultValue;
		std::any attributeAnyValue;

		Node* attributeNode;

		Attribute* parentAttribute;
		std::map<std::string, Attribute*> subAttributes;
		std::map<unsigned int, Attribute*> multiValues;

        SafePtr<Attribute> source;
        std::vector<SafePtr<Attribute>> dests;

	public:

        /**
         * Default constructor.
         */
		Attribute();

        AttrValueType GetAttrValueType() const;

        AttrType GetAttrType() const;

    protected:
        /**
         * Signal change to current holder node.
         * @param AttributeChange change: change.
         */
        void SignalChange(AttributeChange change);

    public:
        /**
         * Copies attribute.
         * @param std::string name: copy attribute name.
         * @param Node* node: parent node ptr.
         * @param Attribute* parent: parent Attribute ptr.
         * @return Result Attribute ptr.
         */
        Attribute* Copy(std::string name, Node* node, Attribute* parent=nullptr) const;

    private:
        /**
         * Copies specified attribute as child of current
         * attribute.
         * @param Attribute* ref: attribute ptr to copy.
         * @return Attribute* resulting attribute.
         */
        Attribute* CopyAsChild(Attribute* ref);

        /**
         * Initializes default value.
         */
        void InitDefaultValue();

    public:
        /**
         * Creates a new Attribute using std::any value.
         * @param std::string name: attribute name.
         * @param std::any v: value.
         * @param ValueType valueType: attribute value type.
         * @param Type type: attribute type.
         * @param bool userDefined: is user defined.
         * @param Node* node: parent node ptr.
         * @param Attribute* parent: parent attribute ptr.
         */
        explicit Attribute(std::string name, std::any v, AttrValueType valueType = AttrValueType::Single,
			AttrType type = AttrType::Static, bool userDefined = false,
			Node* node = nullptr, Attribute* parent=nullptr);

        /**
         * Creates new attribute using specified type.
         * @tparam T type: type to use.
         * @param std::string name: attribute name.
         * @param ValueType valueType: attribute value type.
         * @param Type type: attribute type.
         * @param bool userDefined: is user defined.
         * @param Node* node: parent node ptr.
         * @param Attribute* parent: parent attribute ptr.
         */
		template<typename T>
		explicit Attribute(const std::string& name, AttrValueType valueType = AttrValueType::Single,
			AttrType type = AttrType::Static, bool userDefined = false,
			Node* node = nullptr, Attribute* parent=nullptr):
            Attribute(name, typeid(T).hash_code(), valueType,
                      type, userDefined, node, parent){}

        explicit Attribute(const std::string& name, size_t hash, AttrValueType valueType = AttrValueType::Single,
                           AttrType type = AttrType::Static, bool userDefined = false,
                           Node* node = nullptr, Attribute* parent=nullptr);

    public:
        /**
         * Creates new holder attribute.
         * @param std::string name: attribute name.
         * @param Type type: attribute type.
         * @param bool multi: is attribute multi.
         * @param bool userDefined: is attribute user defined.
         * @param Node* node: attribute parent Node ptr.
         * @param Attribute* parent: attribute parent Attribute ptr.
         */
        explicit Attribute(const std::string& name, AttrType type = AttrType::Static, bool multi=false,
			bool userDefined = false, Node* node = nullptr,
			Attribute* parent = nullptr);

        /**
         * Sets current attribute default value.
         * @param T default value.
         */
        template<class T>
        bool SetDefaultValue(T value)
        {
            return SetDefaultAnyValue(std::make_any<T>(value));
        }

        bool SetDefaultAnyValue(const std::any& value);

    public:
        /**
         * Destructor.
         */
		virtual ~Attribute();

	protected:
        /**
         * Sets parent node.
         * @param Node* node ptr.
         */
		void SetParentNode(Node* node);

	public:
        /**
         * Returns attribute parent Node.
         * @return Node* parent node ptr.
         */
		Node* Node() const;

        /**
         * Returns attribute parent.
         * @return Attribute* parent.
         */
        Attribute* ParentAttribute() const;

        /**
         * Returns attribute short name.
         * @return std::string short name.
         */
		std::string Name() const;

        /**
         * Returns attribute long name.
         * @return std::string long name.
         */
		std::string Longname() const;

	    /**
	     * Sets attribute name.
	     * @param std::string name.
	     */
		void SetName(std::string name);

        /**
         * Returns attribute type name.
         * @return std::string type name.
         */
		std::string TypeName() const;

        /**
         * Returns attribute type hash.
         * @return size_t type hash.
         */
		size_t TypeHash() const;

        /**
         * Returns attribute default value.
         * @return std::any default value.
         */
        std::any DefaultValue() const;

        /**
         * Resets attribute to its default value.
         */
        void Reset();

    private:
        /**
         * Resets attribute to its default value.
         */
        void _Reset();

        /**
         * Performs factory reset on attributes.
         * This reverts attribute value and indices.
         */
        void FactoryReset();

    public:
        /**
         * Returns whether attribute is on
         * default value.
         * @return bool.
         */
        bool IsDefault() const;

        /**
         * Returns whether attribute is user defined.
         * @return bool user defined.
         */
		bool IsUserDefined() const;

        /**
         * Returns whether attribute is input.
         * @return bool is input.
         */
		bool IsInput() const;

        /**
         * Returns whether attribute is output.
         * @return bool is output.
         */
        bool IsOutput() const;

        /**
         * Returns whether attribute is static.
         * @return bool is static.
         */
        bool IsStatic() const;

        /**
         * Returns whether attribute is multi.
         * @return bool is multi.
         */
        bool IsMulti() const;

        /**
         * Returns whether attribute is holder.
         * @return bool is holder.
         */
		bool IsHolder() const;

        /**
         * Returns whether attribute is currently editable.
         * @return bool is editable.
         */
        virtual bool IsEditable() const;

        /**
         * Returns whether attribute is a compound node
         * internal attribute.
         * An internal attribute cannot be accessed from
         * outside a compound node, but only from the
         * inside.
         * @return attribute is internal.
         */
        bool IsInternal() const;

        /**
         * Sets attribute as internal.
         * @param internal: internal  state.
         */
        bool SetInternal(bool internal);

        /**
         * Returns zhether attribute is external.
         * All attributes are external by default, and
         * may be set from the outside. A non external
         * attribute may only be set from the Evaluate
         * node method.
         * @return attribute is external.
         */
        bool IsExternal() const;

        /**
         * Sets attribute as external.
         * @param external: external  state.
         */
        bool SetExternal(bool external);

    public:
        /**
         * Returns hashed value.
         * @return size_t hash.
         */
        virtual size_t ValueHash(bool followCnx=false);

        /**
         * Returns whether attributes has child attributes.
         * @return bool has child attrs.
         */
		bool HasChildAttributes() const;

        /**
         * Returns children attribute names.
         * @return std::vector<std::string> children names.
         */
        std::vector<std::string> ChildAttributesNames() const;

        /**
         * Returns if child attribute with name exists.
         * @param std::string name: child name.
         * @return bool exists.
         */
        bool HasChildAttribute(const std::string& name) const;

        /**
         * Returns children attributes.
         * @return std::vector<Attribute*> child attributes.
         */
		std::vector<Attribute*> GetChildAttributes() const;

        /**
         * Return child attribute with specified name.
         * @param std::string name: child name.
         * @return child Attribute ptr.
         */
		Attribute* GetAttribute(std::string name) const;

        /**
         * Returns all sub attributes.
         * @return sub Attributes ptrs.
         */
        std::vector<Attribute*> GetAllAttributes() const;

	public:
        /**
         * Adds specfied attribute as child.
         * @param Attribute* child: attribute ptr.
         * @return bool success.
         */
		bool AddChildAttribute(Attribute* child);

		template<typename Type>
		Attribute* CreateChildAttribute(const std::string& name, AttrValueType vType = AttrValueType::Single,
                                        AttrType type = AttrType::Static, bool userdefined = false)
		{
			auto child = new Attribute<Type>(name, vType, type, userdefined, userdefined, attributeNode, parentAttribute);

			bool success = AddChildAttribute(child);
			if (!success)
			{
				delete child;
				return nullptr;
			}

			return child;
		}

		Attribute* CreateChildAttribute(std::string name, std::any value, AttrValueType vType = AttrValueType::Single, AttrType type = AttrType::Static, bool userdefined = false)
		{
			Attribute* child = new Attribute(name, value, vType, type, userdefined, attributeNode, parentAttribute);
			bool success = AddChildAttribute(child);
			if (!success)
			{
				delete child;
				return nullptr;
			}

			return child;
		}

	public:
		/**
		 * @brief Returns whether this attributes has source,
		 * either real or virtuals.
		 * @return bool: has source.
		*/
		bool HasSource() const;

		/**
		 * @brief Returns whether this attributes has source at index,
		 * either real or virtuals.
		 * @param unsigned int index: index.
		 * @return bool: has source.
		*/
		bool HasSource(unsigned int index);

        /**
         * @brief Returns whether attribute has source, multi or single.
         * @return bool: has any source.
         */
        bool HasSources() const;

		/**
		 * @brief Returns whether this attributes has destinations,
		 * either real or virtuals.
		 * @return bool: has destinations.
		*/
		bool HasDests() const;

		/**
		 * @brief Returns whether index exists.
		 * @param unsigned int index: index to query.
		 * @return bool: index exists.
		*/
		bool HasIndex(unsigned int index) const;


        std::vector<unsigned int> ValidIndices() const;

	public:
		/**
		 * @brief Creates new multi index.
		* This function returns false if undex already exists.
		* @param unsigned int index: index to create.
		* @return bool: success.
		*/
		bool CreateIndex(unsigned int index);

    private:
        bool _CreateIndex(unsigned int index);

    public:
        /**
         * Removes multi index.
         * @param unsigned int index: index to remove.
         * @return bool: success.
         */
        bool RemoveIndex(unsigned int index);

    private:
        bool _RemoveIndex(unsigned int index);

    public:
        /**
         * Clears all multi indices.
         * @return bool: success.
         */
        bool ClearIndices();

    private:
        bool _ClearIndices();

    public:
        /**
         * Returns attribute multi index.
         * @param sub index attribute.
         * @return index.
         */
        unsigned int AttributeIndex(Attribute* sub, Feedback* status=nullptr);

    private:
        bool _CanConnectSource(Attribute* source);

	public:
		/**
		 * @brief Checks whether two plugs can be connected.
		 * @param Attribute* source: source attribute.
		 * @return bool: can be connected.
		*/
		bool CanConnectSource(Attribute* source);

        /**
         * Checks whether specified attribute can be connected
         * to current attribute multi index.
         * @param Attribute* source: source attribute.
         * @return bool: can be connected to multi index.
         */
        bool CanConnectSourceToIndex(Attribute* source);

		/**
		 * @brief Connects source.
		 * @param Attribute* attribute: attribute. 
		 * @return bool: success.
		*/
		bool ConnectSource(Attribute* attribute);

    private:
        bool _ConnectSource(Attribute* attribute);

    public:

		/**
		 * @brief Connects source at multi index.
		 * @param unsigned int index: index.
		 * @param Attribute* attribute: attribute.
		 * @return bool: success.
		*/
		bool ConnectSource(unsigned int index, Attribute* attribute);

		/**
		 * @brief Connects as source of specified attribute.
		 * @param Attribute* attribute: destination attribute. 
		 * @return bool: success.
		*/
		bool ConnectDest(Attribute* attribute);

    private:

        bool _ConnectDest(Attribute* attribute);

    private:
        /**
         * Cleans invalids destinations.
         */
        std::vector<Attribute*> _ValidateDests();

    public:

		/**
		 * @brief Connects as source of specified attribute multi index.
		 * @param unsigned int index : multi index.
		 * @param Attribute* attribute: destination attribute.
		 * @return bool: success.
		*/
		bool ConnectDest(unsigned int index, Attribute* attribute);

		/**
		 * @brief Disconnects source.
		 * @param Attribute* attribute: attribute. 
		 * @return bool: success.
		*/
		bool DisconnectSource(Attribute* attribute);

    private:
        bool _DisconnectSource(Attribute* attribute);

    public:
		/**
		 * @brief Disconnects multi index source.
		 * @param unsigned int index: multi index.
		 * @param Attribute* attribute: attribute.
		 * @return bool: success.
		*/
		bool DisconnectSource(unsigned int index, Attribute* attribute);

		/**
		 * @brief Disconnects from destination attribute.
		 * @param Attribute* attribute: destination attribute. 
		 * @return bool: success.
		*/
		bool DisconnectDest(Attribute* attribute);

    private:
        bool _DisconnectDest(Attribute* attribute);

    public:

		/**
		 * @brief Disconnect from destination at multi index.
		 * @param unsigned int index: multi index. 
		 * @param Attribute* attribute: attribute. 
		 * @return bool: success.
		*/
		bool DisconnectDest(unsigned int index, Attribute* attribute);

	public:
		Attribute* Source() const;

		std::vector<Attribute*> Sources() const;

        std::vector<Attribute*> Dests();

    private:
        std::any Convert(std::any value, size_t destHash,
                         Feedback* success=nullptr) const;

		bool ValueSet(std::any value);

	public:
		template<class T>
		bool Set(T value)
		{
			return ValueSet(std::make_any<T>(value));
		}

    private:
        std::any ValueGet(size_t dest, Feedback* status=nullptr) const;

	public:
		template<class T>
		T Get(Feedback* status=nullptr)
		{
			std::any dest = ValueGet(typeid(T).hash_code(), status);
            if (!dest.has_value())
            {
                if (status)
                    status->status = Status::Failed;

                return T();
            }

            if (status)
                status->status = Status::Success;
			return std::any_cast<T>(attributeAnyValue);
		}

		std::vector<Attribute*> GetArray() const;

	public:
		Attribute* GetIndexAttribute(unsigned int index) const;

	public:
		template<class T>
		std::vector<T> GetArrayValues()
		{
			std::vector<T> arr;

			size_t h = typeid(T).hash_code();
			for (std::pair<unsigned int, Attribute*> p : multiValues)
			{
				if (p.second->TypeHash() != h)
				{
					continue;
				}

                Feedback status;
                T value = p.second->Get<T>(&status);
                if (!status)
                {
                    continue;
                }

				arr.push_back(value);
			}

			return arr;
		}

		template<class T>
		bool SetIndexValue(unsigned int index, T value)
		{
			if (!HasIndex(index))
			{
				CreateIndex(index);
			}

			return multiValues.at(index)->Set(value);
		}

		std::any GetAnyValue() const;

		bool SetAnyValue(std::any value);

    public:
		// Serialize / Deserializes Values.
		void Serialize(rapidjson::Value& dict, rapidjson::Document& json);

        void Deserialize(rapidjson::Value& dict);

		// Serialize / Deserializes Construction.
		void SerializeAttribute(rapidjson::Value& value, rapidjson::Document& doc);

		static Attribute* DeserializeAttribute(const std::string& name,
			rapidjson::Value& value, Gex::Node* node=nullptr,
            Attribute* parent=nullptr, bool userDefined=false);

        void SerializeConnections(rapidjson::Value& dict, rapidjson::Document& json);

    private:
        /**
         * Pull attributes sources.
         * @return bool success.
         */
        bool PullSource();

    public:
        /**
         * Pull sources and call pull on child
         * attributes (multi and holders)
         * @return bool: success.
         */
		bool Pull();

	};

}

#endif // NODELIBRARY_ATTRIBUTE





