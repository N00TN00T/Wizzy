#include "wzpch.h"

#include <imgui.h>

#include "Wizzy/PropertyTable.h"
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


	PropertyTable::~PropertyTable()
	{
		ClearAll();
	}

	bool PropertyTable::Exists(const string& key)
	{
		return m_properties.find(key) != m_properties.end();
	}

	void PropertyTable::Delete(string name)
	{
		WZ_CORE_ASSERT(m_properties.find(name) != m_properties.end(), "Tried deleting non-existent property");
		m_properties.erase(name);
		m_propKeys.erase(std::find(m_propKeys.begin(), m_propKeys.end(), name));
	}

	void PropertyTable::ClearAll()
	{
		m_propKeys.clear();
		m_properties.clear();
	}

	string PropertyTable::ToString(const string& propKey) const
	{
		const auto& property = m_properties.at(propKey);
		
		if (property.value.index() == ToTypeIndex<PropFloat>())			return std::to_string(std::get<PropFloat>(property.value));
		if (property.value.index() == ToTypeIndex<PropInt>())			return std::to_string(std::get<PropInt>(property.value));
		if (property.value.index() == ToTypeIndex<PropString>())		return std::get<PropString>(property.value);
		if (property.value.index() == ToTypeIndex<PropBool>())			return std::to_string(std::get<PropBool>(property.value));
		if (property.value.index() == ToTypeIndex<PropertyTable>())	return std::get<PropertyTable>(property.value).Serialize();

		WZ_THROW(Exception, "Type index out of range");
		return "";
	}

	string PropertyTable::Serialize() const
	{
		return Serialize(0);
	}
	void PropertyTable::Deserialize(string data)
	{
		DeserializeOffset(data, 0, -1);
	}
	void PropertyTable::Add(const PropertyTable& other, bool overwrite)
	{
		for (auto& k : other.GetKeys())
		{
			if (overwrite || !this->Exists(k))
			{
				this->m_propKeys.emplace(k);
				this->m_properties[k] = other.m_properties.at(k);
			}
		}
	}
	void PropertyTable::SetValue(u8 typeIndex, Property& prop, const string& value)
	{
		if (typeIndex == ToTypeIndex<PropFloat>())				prop.value = std::stod(value);
		else if (typeIndex == ToTypeIndex<PropInt>())			prop.value = std::stoll(value);
		else if (typeIndex == ToTypeIndex<PropString>())		prop.value = value;
		else if (typeIndex == ToTypeIndex<PropBool>())			prop.value = value == "true";
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
	string PropertyTable::Serialize(int32 indents) const
	{
		std::stringstream stream;
		string indent = string(4ULL * (size_t)indents, ' ');
		stream << "[" << m_propKeys.size() << "]:\n";

		for (auto& k : m_propKeys)
		{
			stream << indent << "\"" << k << "\"";
			if (Is<PropInt>(k))				stream << ":" << Get<PropInt>(k) << "\n";
			else if (Is<PropFloat>(k))		stream << ":" << std::fixed << std::setprecision(10) << Get<PropFloat>(k) << "\n";
			else if (Is<PropString>(k))		stream << ":" << EncodeString(Get<PropString>(k)) << "\n";
			else if (Is<PropBool>(k))		stream << ":" << (Get<PropBool>(k) ? "true" : "false") << "\n";
			else if (Is<PropertyTable>(k))	stream << Get<PropertyTable>(k).Serialize(indents + 1);
		}

		return stream.str();
	}

	int32 PropertyTable::DeserializeOffset(string data, size_t offset, int32 fieldCount)
	{
		if (offset > 0) data = data.substr(offset);

		std::stringstream ss(data);
		string line;
		int32 nLine = 1;
		int32 skipLines = 0;
		int32 handledFields = 0;
		int32 nParentSkips = fieldCount;
		while (std::getline(ss, line))
		{
			if (line == "") continue;
			if (skipLines-- > 0) continue;

			while (line.find_first_of(' ') == 0) line.erase(0, 1);

			auto assignIndex = FindAssignToken(line);
			if (assignIndex == string::npos)
			{
				WZ_THROW(TableParseErrorException, "No assign (:) operator found", nLine, line);
			}

			string key = line.substr(0, assignIndex);

			if (IsString(key))
			{
				key = ProcessString(key);
				string value = line.substr((size_t)assignIndex + 1);

				if (IsInteger(value))
				{
					Set<PropInt>(key, std::stoll(value));
				}
				else if (IsNumber(value))
				{
					Set<PropFloat>(key, std::stod(value));
				}
				else if (IsBoolean(value))
				{
					Set<PropFloat>(key, value == "true");
				}
				else if (IsString(value))
				{
					Set<PropString>(key, ProcessString(value));
				}
				else
				{
					WZ_THROW(TableParseErrorException, "Invalid value", nLine, value);
				}
			}
			else
			{
				auto iOpenBracket = key.find_first_of('[');
				auto iCloseBracket = key.find_last_of(']');

				if (iOpenBracket == string::npos || iCloseBracket == string::npos || iCloseBracket < iOpenBracket)
				{
					WZ_THROW(TableParseErrorException, "Invalid key", nLine, key);
				}
				
				string sizeStr = key.substr(iOpenBracket + 1, iCloseBracket - (iOpenBracket + 1));
				if (IsInteger(sizeStr))
				{
					string tName = key.substr(0, iOpenBracket);
					if (tName == "")
					{
						
					}
					else
					{
						if (!IsString(tName))
						{
							WZ_THROW(TableParseErrorException, "Invalid table key", nLine, tName);
						}
						tName = ProcessString(tName);
						int32 size = std::stoi(sizeStr);
						if (size > 0)
						{
							PropertyTable sub;
							skipLines = sub.DeserializeOffset(data, data.find(line) + line.size(), size);
							nParentSkips += skipLines;
							Set<PropertyTable>(tName, sub);
						}
					}
				}
				else
				{
					WZ_THROW(TableParseErrorException, "Invalid table size value ([X])", nLine, sizeStr);
				}
			}
			nLine++;
			handledFields++;

			if (handledFields >= fieldCount && fieldCount != -1) return nParentSkips;
		}
	}

	bool PropertyTable::IsInteger(const string& value)
	{
		if (value == "") return false;
		for (auto c : value)
		{
			if (!isdigit(c)) return false;
		}
		return true;
	}
	bool PropertyTable::IsNumber(const string& value)
	{
		if (value == "") return false;
		int32 nPeriods = 0;
		for (auto c : value)
		{
			if ((!isdigit(c) && c != '.') || (c == '.' && ++nPeriods > 1)) return false;
		}
		return true;
	}
	bool PropertyTable::IsBoolean(const string& value)
	{
		return value == "true" || value == "false";
	}
	bool PropertyTable::IsString(const string& value)
	{
		return value[0] == '"' && value[value.size() - 1] == '"';
	}
	int32 PropertyTable::FindAssignToken(const string& line)
	{
		int32 cQuotes = 0;

		char last = 0;
		for (int i = 0; i < line.size(); i++)
		{
			char c = line[i];

			if (c == '\"' && last != '\\') cQuotes++;

			if (c == ':' && (cQuotes >= 2 || cQuotes == 0)) return i;

			last = c;
		}

		return 0;
	}
	string PropertyTable::EncodeString(string str) const
	{
		for (int32 i = str.size() - 1; i >= 0; i--)
		{
			char c = str[i];

			if (c == '\"') str.insert(str.begin() + i, '\\');
			if (c == '\n') { str.insert(str.begin() + i, '\\'); str.insert(str.begin() + i, 'n'); }
		}

		str = "\"" + str + "\"";

		return str;
	}
	string PropertyTable::ProcessString(string str)
	{
		str = str.substr(1, str.size() - 2);

		char last = 0;
		for (int32 i = 0; i < str.size(); i++)
		{
			char c = str[i];

			if (c == '\"' && last == '\\') str.erase(i--);
			if (c == 'n' && last == '\\') { str[i] = '\n'; str.erase(i--); }

			last = c;
		}

		return str;
	}
}