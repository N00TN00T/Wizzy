#pragma once

#define _STATIC_ASSERT_VALID_PROP_TYPE(t, msg) static_assert(\
						std::is_integral<t>()\
						|| std::is_floating_point<t>()\
						|| IS_STRING(t)\
						|| std::is_same<t, PropertyTable>(),\
						msg);\

#define _STATIC_ASSERT_VALID_PROP_TYPE_STRICT(t, msg) static_assert(\
						std::is_same<t, PropInt>()\
						|| std::is_same<t, PropFloat>()\
						|| std::is_same<t, PropString>()\
						|| std::is_same<t, PropBool>()\
						|| std::is_same<t, PropertyTable>(),\
						msg);\

namespace Wizzy
{
	typedef u8 PropertyFlag;
	

	typedef s64 PropInt;
	typedef f64 PropFloat;
	typedef string PropString;
	typedef bool PropBool;

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
			_STATIC_ASSERT_VALID_PROP_TYPE(T, "Invalid PropertyTable type in Set()");
			
			if (m_properties.find(key) == m_properties.end())
			{
				if constexpr (std::is_floating_point<T>()) 
					m_properties[key] = Property((PropFloat)value);
				else if constexpr (std::is_integral<T>() && !std::is_same<T, bool>()) 
					m_properties[key] = Property((PropInt)value);
				else if constexpr (std::is_same<T, bool>())
					m_properties[key] = Property((PropBool)value);
				else if constexpr (IS_STRING(T)) 
					m_properties[key] = Property(value);
				else if constexpr (std::is_same<T, PropertyTable>())
					m_properties[key] = Property(value);
			}
			else
			{
				if constexpr (std::is_floating_point<T>()) 
					m_properties[key].value = (PropFloat)value;
				else if constexpr (std::is_integral<T>() && !std::is_same<T, bool>())
					m_properties[key].value = (PropInt)value;
				else if constexpr (std::is_same<T, bool>())
					m_properties[key].value = (PropBool)value;
				else if constexpr (IS_STRING(T))
					m_properties[key].value = value;
				else if constexpr (std::is_same<T, PropertyTable>())
					m_properties[key].value = value;
			}
		}
		template <typename T>
		inline T Get(const string& key) const
		{
			_STATIC_ASSERT_VALID_PROP_TYPE(T, "Invalid ProertyTable type in Get()");

			WZ_CORE_ASSERT(m_properties.find(key) != m_properties.end(), "Tried getting a non-existent property");

#define __TRY_GET(tt) auto value = std::get_if<tt>(&m_properties.at(key).value); \
			WZ_CORE_ASSERT(value != nullptr, "Type mismatch when getting property"); \
			return *value;

			if constexpr (std::is_floating_point<T>()) 
			{
				__TRY_GET(PropFloat);
			} 
			else if constexpr (std::is_integral<T>() && !std::is_same<T, bool>())
			{
				__TRY_GET(PropInt);
			} 
			else if constexpr (std::is_same<T, bool>())
			{
				__TRY_GET(PropBool);
			} 
			else if constexpr (std::is_same<T, string>()) 
			{
				__TRY_GET(PropString);
			}
			else if constexpr (std::is_same<T, PropertyTable>())
			{
				__TRY_GET(PropertyTable);
			}

		}

		// Strict. Has to be s64, f64, string, bool or PropertyTable
		template <typename T>
		inline T& GetRef(const string& key) const
		{
			_STATIC_ASSERT_VALID_PROP_TYPE_STRICT(T, "GetRef is strict and handles only s64, f64, string, bool & PropertyTable");

			WZ_CORE_ASSERT(m_properties.find(key) != m_properties.end(), "Tried getting a non-existent property");

			if constexpr (std::is_same<T, PropFloat>()) 
			{
				__TRY_GET(PropFloat);
			} 
			else if constexpr (std::is_same<T, PropInt>())
			{
				__TRY_GET(PropInt);
			} 
			else if constexpr (std::is_same<T, PropBool>())
			{
				__TRY_GET(PropBool);
			} 
			else if constexpr (std::is_same<T, PropString>()) 
			{
				__TRY_GET(PropString);
			}
			else if constexpr (std::is_same<T, PropertyTable>())
			{
				__TRY_GET(PropertyTable);
			}

		}

		template <typename T>
		inline bool Is(const string& key) const
		{
			_STATIC_ASSERT_VALID_PROP_TYPE(T, "Invalid PropertyTable type in Is()");
			return	m_properties.find(key) != m_properties.end()
					&& ToTypeIndex<T>() == m_properties.at(key).value.index();
		}

		bool Exists(const string& key);

		void Delete(string name);

		void ClearAll();

		string ToString(const string& propKey) const;

		string Serialize() const;
		void Deserialize(string data);

		void TraverseKeys(std::function<bool(const string& k)> callback) const;

		void Add(const PropertyTable& other, bool overwrite = true);

		inline size_t GetCount() const { return m_properties.size(); }

	private:
		template <typename T>
		inline u8 ToTypeIndex() const
		{
			_STATIC_ASSERT_VALID_PROP_TYPE(T, "Invalid PropertyTable type in ToTypeIndex()");

			if constexpr (std::is_floating_point<T>())				
				return 0;
			else if constexpr (std::is_integral<T>() && !std::is_same<T, bool>())			
				return 1;
			else if constexpr (std::is_same<T, string>())			
				return 2;
			else if constexpr (std::is_same<T, bool>())
				return 3;
			else if constexpr (std::is_same<T, PropertyTable>())
				return 4;

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
		std::unordered_map<string, Property> m_properties;
	};

	struct Property
	{
		Property() {}
		Property(std::variant<PropFloat, PropInt, PropString, PropBool, PropertyTable> v)
			: value(v)
		{
		}
		std::variant<PropFloat, PropInt, PropString, PropBool, PropertyTable> value;
	};
}