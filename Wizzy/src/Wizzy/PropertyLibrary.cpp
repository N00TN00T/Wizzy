#include "wzpch.h"

#include <imgui.h>

#include "Wizzy/PropertyLibrary.h"
#include "Wizzy/WizzyExceptions.h"

#define ITEM_INDEX_NAME		(0)
#define ITEM_INDEX_VALUE	(1)
#define ITEM_INDEX_FLAGS	(2)

namespace Wizzy
{
	enum : u8
	{
		PROP_FLAG_SLIDER = 1,
		PROP_FLAG_DRAG = 2,
		PROP_FLAG_COMBO = 3,

		PROP_FLAG_COUNT
	};

	PropertyTable::PropertyTable(const PropertyTable& src)
	{
		auto& dst = *this;
		for (const auto& [name, srcProp] : src.m_properties)
		{
			this->SetProperty(name, srcProp.value);

			auto& dstProp = dst.m_properties[name];

			dstProp.flagBits = srcProp.flagBits;

			for (const auto& [flag, dataPair] : srcProp.flagData)
			{
				const auto& srcDataSize = dataPair.first;
				const auto& srcData = dataPair.second;

				auto& dstDataSize = dstProp.flagData[flag].first;
				auto& dstData = dstProp.flagData[flag].second;

				dstDataSize = srcDataSize;
				dstData = malloc(dstDataSize);
				memcpy(dstData, srcData, dstDataSize);
			}
		}
	}

	PropertyTable::~PropertyTable()
	{
		Clear();
	
	}

	void PropertyTable::DeleteProperty(string name)
	{
		WZ_CORE_ASSERT(m_properties.find(name) != m_properties.end(), "Tried deleting non-existent property");
		for (auto& [flag, dataPair] : m_properties[name].flagData)
		{
			auto& data = dataPair.second;
			free(data);
			data = nullptr;
		}

		m_properties.erase(name);
		m_propKeys.erase(std::find(m_propKeys.begin(), m_propKeys.end(), name));
	}

	void PropertyTable::Clear()
	{
		for (int i = m_propKeys.size() - 1; i >= 0; i--)
		{
			DeleteProperty(m_propKeys[i]);
		}

		m_propKeys.clear();
		m_properties.clear();
	}

	string PropertyTable::ToString(const string& propKey) const
	{
		const auto& property = m_properties.at(propKey);
		
		if (property.value.index() == ToTypeIndex<float>())			return std::to_string(std::get<float>(property.value));
		if (property.value.index() == ToTypeIndex<int32>())			return std::to_string(std::get<int32>(property.value));
		if (property.value.index() == ToTypeIndex<string>())		return std::get<string>(property.value);
		if (property.value.index() == ToTypeIndex<bool>())			return std::to_string(std::get<bool>(property.value));
		if (property.value.index() == ToTypeIndex<PropertyTable>())	return std::get<PropertyTable>(property.value).Serialize();

		WZ_THROW(Exception, "Type index out of range");
		return "";
	}

	void PropertyTable::MakeInputField(const string& key)
	{
		m_properties[key].flagBits = 0;
	}

	void PropertyTable::MakeSlider(const string& key, const float& min, const float& max)
	{
		m_properties[key].flagBits |= BIT(PROP_FLAG_SLIDER);
		m_properties[key].flagData[PROP_FLAG_SLIDER].first = sizeof(float) * 2;
		m_properties[key].flagData[PROP_FLAG_SLIDER].second = (void*) new float[2]{ min, max };
	}
	void PropertyTable::MakeDrag(const string& key, const float& vSpeed)
	{
		m_properties[key].flagBits |= BIT(PROP_FLAG_DRAG);
		m_properties[key].flagData[PROP_FLAG_DRAG].first = sizeof(float);
		m_properties[key].flagData[PROP_FLAG_DRAG].second = new float(vSpeed);
	}
	void PropertyTable::MakeCombo(const string& key, const std::vector<string>& options)
	{
		/*
			First 4 bytes is an integer that describes how many options there are
			
			The following 4*optionCount bytes are integers that tell how long the option strings are

			The rest of the bytes are the strings
		*/
		m_properties[key].flagBits |= BIT(PROP_FLAG_COMBO);
		std::vector<byte> bytes;

		bytes.resize(sizeof(int32) + sizeof(int32) * options.size());

		int32 optionsSize = options.size();

		for (int32 i = 0; i < sizeof(int32); i++) bytes[i] = ((byte*)(&optionsSize))[i];

		size_t byteOffset = 0;
		for (int32 i = 0; i < options.size(); i++)
		{
			const auto& option = options[i];

			for (const auto& byte : option)
			{
				byteOffset++;
				bytes.push_back(byte);
			}
			for (int32 j = 0; j < sizeof(int32); j++)
				bytes[sizeof(int32) + sizeof(int32) * i + j] = ((byte*)(&byteOffset))[j];
		}

		m_properties[key].flagData[PROP_FLAG_COMBO].first = bytes.size();
		m_properties[key].flagData[PROP_FLAG_COMBO].second = malloc(bytes.size());
		memcpy(m_properties[key].flagData[PROP_FLAG_COMBO].second, bytes.data(), bytes.size());

	}
	void PropertyTable::OnGUI()
	{
		for (const auto& key : m_propKeys)
		{
			if (IsProperty<float>(key))
			{ // decimal
				DoDecimalGUI(key);
			}
			else if (IsProperty<int32>(key))
			{ // int
				DoIntGUI(key);
			}
			else if (IsProperty<string>(key))
			{ // string
				DoStringGUI(key);
			}
			else if (IsProperty<bool>(key))
			{ // bool
				DoBoolGUI(key);
			}
			else if (IsProperty<PropertyTable>(key))
			{ // bool
				DoTableGUI(key);
			}
		}
	}
	void put_byte(string& str, byte b)
	{
		str.push_back((char)b);
	}
	string PropertyTable::Serialize() const
	{
		const static byte TYPE_BYTE[4] = 
		{ 
			WZ_BYTE_TOKEN_BEGIN_FLOAT, 
			WZ_BYTE_TOKEN_BEGIN_INT, 
			WZ_BYTE_TOKEN_BEGIN_STRING, 
			WZ_BYTE_TOKEN_BEGIN_BOOL
		};
		string serialized = WZ_TOKEN_HEADER;

		for (auto [key, prop] : m_properties)
		{
			put_byte(serialized, TYPE_BYTE[prop.value.index()]);
			serialized += key;

			put_byte(serialized, WZ_BYTE_TOKEN_NEXT_ITEM);

			serialized += this->ToString(key);

			if (prop.flagBits != 0)
			{
				put_byte(serialized, WZ_BYTE_TOKEN_NEXT_ITEM);

				for (u8 flag = 1; flag < PROP_FLAG_COUNT; flag++)
				{
					if (prop.flagBits & BIT(flag))
					{
						serialized += std::to_string(flag);
						put_byte(serialized, WZ_BYTE_TOKEN_DATA_SEPARATOR);
						for (int32 i = 0; i < prop.flagData[flag].first; i++)
						{
							// NOTE:	This puts the bytes of the data in text format, NOT BINARY!
							//			4-byte data CORRECT:	072 073 089 065
							//			4-byte data INCORRECT:	H	I	Y	A

							char* buf = new char[4];
							snprintf(buf, 4, "%03d", ((byte*)prop.flagData[flag].second)[i]);
							serialized += buf;
							delete[] buf;
						}
						put_byte(serialized, WZ_BYTE_TOKEN_FLAG_SEPARATOR);
					}
				}
				if (serialized[serialized.size() - 1] == (sbyte)WZ_BYTE_TOKEN_FLAG_SEPARATOR)
				{
					serialized.pop_back();
				}
			}

			put_byte(serialized, WZ_BYTE_TOKEN_END);
		}

		serialized += WZ_TOKEN_FOOTER;

		return serialized;
	}
	bool is_power_of_2(int32 n)
	{
		return n != 0 && (n & (n - 1)) == 0;
	}
	void PropertyTable::Deserialize(string data)
	{
		bool open = false;

		if (data.find(WZ_TOKEN_HEADER) == string::npos)
		{
			WZ_THROW(Exception, "Missing wizzy header in data");
		}

		const static std::unordered_map<byte, size_t> TYPE_INDEX = 
		{
			{ WZ_BYTE_TOKEN_BEGIN_FLOAT,	ToTypeIndex<float>() },
			{ WZ_BYTE_TOKEN_BEGIN_INT,		ToTypeIndex<int32>() },
			{ WZ_BYTE_TOKEN_BEGIN_STRING,	ToTypeIndex<string>() },
			{ WZ_BYTE_TOKEN_BEGIN_BOOL,		ToTypeIndex<bool>() },
			{ WZ_BYTE_TOKEN_BEGIN_TABLE,	ToTypeIndex<PropertyTable>()}
		};

		const static int8 MODE_NONE			= -1;
		const static int8 MODE_NAME			= 0;
		const static int8 MODE_VALUE		= 1;
		const static int8 MODE_FLAG			= 2;
		const static int8 MODE_FLAG_DATA	= 3;
		const static int8 MODE_MAX			= 3;

		/*const static byte TYPE_SIZE[4] =
		{
			sizeof(float),
			sizeof(int32),
			sizeof(string),
			sizeof(bool)
		};*/

		int8 typeIndex = -1;
		int8 mode = MODE_NONE;
		int8 lastMode = mode;
		string pointer = "";
		Property prop;
		string propName = "";
		PropertyFlag currentFlag = 0;

		for (int i = data.find(WZ_TOKEN_HEADER); i < data.size(); i++)
		{
			if (i == data.find(WZ_TOKEN_FOOTER)) break;

			auto b = data[i];

			
			if (WZ_IS_TYPE_BYTE(b))						
				mode = MODE_NAME; // Type specified; new data line
			else if (b == WZ_BYTE_TOKEN_NEXT_ITEM)		mode++;
			else if (b == WZ_BYTE_TOKEN_FLAG_SEPARATOR) mode = MODE_FLAG;
			else if (b == WZ_BYTE_TOKEN_DATA_SEPARATOR) mode = MODE_FLAG_DATA;
			else if (b == WZ_BYTE_TOKEN_END)			mode = MODE_NONE;
			else if (!WZ_IS_BYTE_TOKEN(b))				pointer += (char)b;
			else
			{
				WZ_THROW(Exception, "Unhandled byte token (d:" + std::to_string(b) + ")");
			}

#define MODE_TRANSITION(m1, m2) (lastMode == m1 && mode == m2)
			if (mode != lastMode) // Mode has changed
			{
				if (std::abs(mode - lastMode) != 1 && mode != MODE_NONE)
				{
					WZ_THROW(Exception, "Invalid mode transition");
				}

				if (MODE_TRANSITION(MODE_NONE, MODE_NAME))
				{
					typeIndex = TYPE_INDEX.at(b);
				}

				if (MODE_TRANSITION(MODE_NAME, MODE_VALUE))
				{
					propName = pointer;
				}

				if (MODE_TRANSITION(MODE_VALUE, MODE_FLAG)) 
				{
					this->SetValue(typeIndex, prop, pointer);
				}

				if (MODE_TRANSITION(MODE_FLAG, MODE_FLAG_DATA)) // Flag bit end, flag data start
				{
					PropertyFlag flag = std::stoi(pointer);
					prop.flagBits |= BIT(flag);
					currentFlag = flag;
				}

				if (mode == MODE_NONE)
				{
					m_properties[propName] = prop;
					prop = Property();
					propName = "";
					typeIndex = -1;
				}

				if (lastMode == MODE_FLAG_DATA)
				{
					if (currentFlag != 0)
					{
						if (pointer.size() % 3 != 0)
						{
							WZ_THROW(Exception, "Invalid flag data format");
						}
						std::vector<byte> bytes;
						
						for (int j = 0; j < pointer.size(); j+=3)
						{
							string byteStr = pointer.substr(j, 3);
							bytes.push_back(std::stoi(byteStr));
						}

						auto& pFlagData = prop.flagData[currentFlag];
						pFlagData.first = bytes.size();
						pFlagData.second = malloc(bytes.size());
						memcpy(pFlagData.second, &bytes[0], bytes.size());
					}
					currentFlag = 0;
				}

				pointer = "";
			}

			lastMode = mode;
			
			if (mode > MODE_MAX)
			{
				WZ_THROW(Exception, "Invalid mode; too many items in data line");
			}
			if (!is_power_of_2(BIT(currentFlag)) && currentFlag != 0)
			{
				WZ_THROW(Exception, "Invalid flag bit in flag data");
			}

			if (i == data.size() - 1) // Last byte in data reached
			{
				WZ_CORE_WARN("No Wizzy footer was found in wizzy file");
			}
		}
	}
	void PropertyTable::DoDecimalGUI(string key)
	{
		PropertyFlag flags = m_properties[key].flagBits;
		auto& property = m_properties[key];
		void* data = m_properties[key].flagData[PROP_FLAG_SLIDER].second;

		string lbl = key + "###" + std::to_string(reinterpret_cast<uintptr_t>(&GetProperty<float>(key)));

		if (flags & BIT(PROP_FLAG_SLIDER))
		{
			float min = ((float*)data)[0];
			float max = ((float*)data)[1];

			
			ImGui::SliderFloat(lbl.c_str(), &GetProperty<float>(key), min, max);

		}
		else if (flags & BIT(PROP_FLAG_DRAG))
		{
			float stepSpeed = *((float*)data);
			ImGui::DragFloat(lbl.c_str(), &GetProperty<float>(key));

		}
		else
		{
			ImGui::InputFloat(lbl.c_str(), &GetProperty<float>(key));
		}
	}
	void PropertyTable::DoIntGUI(string key)
	{
		PropertyFlag flags = m_properties[key].flagBits;
		auto& property = m_properties[key];
		void* data = m_properties[key].flagData[PROP_FLAG_SLIDER].second;

		string lbl = key + "###" + std::to_string(reinterpret_cast<uintptr_t>(&GetProperty<int32>(key)));

		if (flags & BIT(PROP_FLAG_SLIDER))
		{
			int32 min = ((int32*)data)[0];
			int32 max = ((int32*)data)[1];

			ImGui::SliderInt(lbl.c_str(), &GetProperty<int32>(key), min, max);

		}
		else if (flags & BIT(PROP_FLAG_DRAG))
		{
			int32 stepSpeed = *((int32*)data);
			ImGui::DragInt(lbl.c_str(), &GetProperty<int32>(key));

		}
		else
		{
			ImGui::InputInt(lbl.c_str(), &GetProperty<int32>(key));
		}
	}
	void PropertyTable::DoStringGUI(string key)
	{
		PropertyFlag flags = m_properties[key].flagBits;
		auto& property = m_properties[key];
		void* data = m_properties[key].flagData[PROP_FLAG_SLIDER].second;

		string lbl = key + "###" + std::to_string(reinterpret_cast<uintptr_t>(&GetProperty<string>(key)));

		if (flags & BIT(PROP_FLAG_COMBO))
		{
			auto& options = *(std::vector<string>*)data;

			ImGui::BeginCombo(lbl.c_str(), GetProperty<string>(key).c_str());
			for (const auto& option : options)
			{
				if (ImGui::Selectable(option.c_str(), option == GetProperty<string>(key)))
				{
					SetProperty(key, option);
				}
			}
			ImGui::EndCombo();

		}
		else
		{
			ImGui::InputText(lbl.c_str(), GetProperty<string>(key).data(), GetProperty<string>(key).size());
		}
	}
	void PropertyTable::DoBoolGUI(string key)
	{
		string lbl = key + "###" + std::to_string(reinterpret_cast<uintptr_t>(&GetProperty<bool>(key)));
		ImGui::Checkbox(lbl.c_str(), &GetProperty<bool>(key));
	}
	void PropertyTable::DoTableGUI(string key)
	{
		string lbl = key + "###" + std::to_string(reinterpret_cast<uintptr_t>(&GetProperty<PropertyTable>(key)));
		if (ImGui::TreeNode(lbl.c_str()))
		{
			GetProperty<PropertyTable>(key).OnGUI();
			ImGui::TreePop();
		}
		
	}
	string PropertyTable::SerializeSlider(void* data)
	{
		string serialized = "";
		byte* bytes = (byte*)data;

		for (int i = 0; i < sizeof(float) * 2; i++) serialized += std::to_string((int32)bytes[i]) + " ";

		serialized.pop_back();

		return serialized;
	}
	string PropertyTable::SerializeDrag(void* data)
	{
		string serialized = "";

		byte* bytes = (byte*)data;

		for (int i = 0; i < sizeof(float); i++) serialized += std::to_string((int32)bytes[i]) + " ";

		serialized.pop_back();

		return serialized;
	}
	string PropertyTable::SerializeCombo(void* data)
	{
		string serialized = "";
		byte* bytes = (byte*)data;

		int32 optionsCount = ((int32*)bytes)[0];
		serialized += std::to_string(optionsCount) + " ";

		bytes += sizeof(int32);

		// Take the last offset value, as it points to the end of the last option; the end of the memory block
		int32 optionsByteSize = ((int32*)bytes)[optionsCount - 1];

		for (int32 i = 0; i < optionsCount * sizeof(int32); i++)
		{
			serialized += std::to_string((int32)bytes[i]) + " ";
		}

		bytes += optionsCount * sizeof(int32);

		for (int i = 0; i < optionsByteSize; i++)
		{
			serialized += std::to_string((int32)bytes[i]) + " ";
		}

		if (serialized[serialized.size() - 1] == ' ') serialized.pop_back();

		return serialized;
	}
	void PropertyTable::SetValue(u8 typeIndex, Property& prop, const string& value)
	{
		if (typeIndex == ToTypeIndex<float>())				prop.value = std::stof(value);
		else if (typeIndex == ToTypeIndex<int32>())			prop.value = (int32)std::stoi(value);
		else if (typeIndex == ToTypeIndex<string>())		prop.value = value;
		else if (typeIndex == ToTypeIndex<bool>())			prop.value = (bool)std::stoi(value);
		else if (typeIndex == ToTypeIndex<PropertyTable>())
		{
			prop.value = PropertyTable();
			std::get<PropertyTable>(prop.value).Deserialize(value);
		}

		if (typeIndex >= 4)
		{
			WZ_THROW(Exception, "Type index out of range");
		}
	}
}