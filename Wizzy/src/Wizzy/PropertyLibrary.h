#pragma once

namespace Wizzy
{
	typedef u8 PropertyFlag;

	class PropertyLibrary
	{
	private:
		struct Property
		{
			Property() {}
			Property(std::variant<float, int32, string, bool> v) : value(v) {}
			std::variant<float, int32, string, bool> value;
			PropertyFlag flagBits = 0;
			std::unordered_map<PropertyFlag, std::pair<size_t, void*>> flagData;
		};
	public:
		PropertyLibrary() {}
		PropertyLibrary(const PropertyLibrary& src);
		~PropertyLibrary();

		template <typename T>
		inline void SetProperty(const string& key, T value) 
		{ 
			if (m_properties.find(key) == m_properties.end())
			{
				auto prop = Property(value);
				m_properties[key] = prop;
			}
			else
			{
				m_properties[key].value = value;
			}
		}
		template <typename T>
		inline T& GetProperty(const string& key) const
		{
			WZ_CORE_ASSERT(m_properties.find(key) != m_properties.end(), "Tried getting a non-existent property");

#define __TRY_GET(tt) auto value = std::get_if<tt>(&m_properties.at(key).value); \
			WZ_CORE_ASSERT(value != nullptr, "Type mismatch when getting property. Requested '{0}'", typestr(T)); \
			return *(T*)(value);

			if (IS_DECIMAL(T)) {
				__TRY_GET(float);
			} else if (IS_INT(T)) {
				__TRY_GET(int32);
			} else if (IS_BOOL(T)) {
				__TRY_GET(bool);
			} else if (IS_STRING(T)) {
				__TRY_GET(string);
			}
		}
		template <typename T>
		inline bool IsProperty(const string& key) const
		{
			return	m_properties.find(key) != m_properties.end()
					&& ToTypeIndex<T>() == m_properties.at(key).value.index();
		}

		string ToString(const string& propKey) const;

		void MakeInputField(const string& key);
		void MakeSlider(const string& key, const float& min, const float& max);
		void MakeDrag(const string& key, const float& vSpeed = 1.0);
		void MakeCombo(const string& key, const std::vector<string>& options);

		void OnGUI();

		string Serialize();
		void Deserialize(string data);

	private:
		void DoDecimalGUI(string key);
		void DoIntGUI(string key);
		void DoStringGUI(string key);
		void DoBoolGUI(string key);

		string SerializeSlider(void* data);
		string SerializeDrag(void* data);
		string SerializeCombo(void* data);

		template <typename T>
		inline u8 ToTypeIndex() const
		{
			if (typeid(T) == typeid(float))			return 0;
			else if(typeid(T) == typeid(int32))		return 1;
			else if (typeid(T) == typeid(string))	return 2;
			else if (typeid(T) == typeid(bool))		return 3;

			WZ_CORE_ASSERT(false, "Type '{0}' is not a valid property type", typeid(T).name());
			return -1;
		}

		void SetValue(u8 typeIndex, Property& prop, const string& value);

	private:
		std::unordered_map<string, Property> m_properties;
	};
}