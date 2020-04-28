#pragma once

namespace Wizzy
{
	typedef u8 PropertyFlag;
	struct Property;

	class PropertyTable
	{
	private:
		
		
	public:
		PropertyTable() {}
		~PropertyTable();

		template <typename T>
		inline void Set(const string& key, T value) 
		{ 
			if (m_properties.find(key) == m_properties.end())
			{
				auto prop = Property(value);
				m_properties[key] = prop;
				m_propKeys.emplace(key);
			}
			else
			{
				m_properties[key].value = value;
			}
		}
		template <typename T>
		inline T& Get(const string& key) const
		{
			WZ_CORE_ASSERT(m_properties.find(key) != m_properties.end(), "Tried getting a non-existent property");

#define __TRY_GET(tt) auto value = std::get_if<tt>(&m_properties.at(key).value); \
			WZ_CORE_ASSERT(value != nullptr, "Type mismatch when getting property. Requested " + typestr(T)); \
			return *(T*)(value);

			if (IS_DECIMAL(T)) 
			{
				__TRY_GET(float);
			} 
			else if (IS_INT(T)) 
			{
				__TRY_GET(int32);
			} 
			else if (IS_BOOL(T)) 
			{
				__TRY_GET(bool);
			} 
			else if (IS_STRING(T)) 
			{
				__TRY_GET(string);
			}
			else if (typestr(T) == typestr(PropertyTable))
			{
				__TRY_GET(PropertyTable);
			}
		}
		template <typename T>
		inline bool Is(const string& key) const
		{
			return	m_properties.find(key) != m_properties.end()
					&& ToTypeIndex<T>() == m_properties.at(key).value.index();
		}

		bool Exists(const string& key);

		void Delete(string name);

		void ClearAll();

		string ToString(const string& propKey) const;

		string Serialize() const;
		void Deserialize(string data);

		inline const std::unordered_set<string>& GetKeys() const
		{
			return m_propKeys;
		}

		void Add(const PropertyTable& other, bool overwrite = true);

	private:
		template <typename T>
		inline u8 ToTypeIndex() const
		{
			if (typeid(T) == typeid(float))					return 0;
			else if(typeid(T) == typeid(int32))				return 1;
			else if (typeid(T) == typeid(string))			return 2;
			else if (typeid(T) == typeid(bool))				return 3;
			else if (typeid(T) == typeid(PropertyTable))	return 4;

			WZ_CORE_ASSERT(false, "Type '{0}' is not a valid property type", typeid(T).name());
			return -1;
		}

		void SetValue(u8 typeIndex, Property& prop, const string& value);

		string Serialize(int32 indents) const;
		int32 DeserializeOffset(string data, size_t offset, int32 fieldCount);

		bool IsInteger(const string& value);
		bool IsNumber(const string& value);
		bool IsBoolean(const string& value);
		bool IsString(const string& value);
		int32 FindAssignToken(const string& line);
		string EncodeString(string str) const;
		string ProcessString(string str);

	private:
		std::unordered_set<string> m_propKeys;
		std::unordered_map<string, Property> m_properties;
	};

	struct Property
	{
		Property() {}
		Property(std::variant<float, int32, string, bool, PropertyTable> v) : value(v) {}
		std::variant<float, int32, string, bool, PropertyTable> value;
	};
}