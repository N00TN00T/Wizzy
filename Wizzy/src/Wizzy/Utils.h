#pragma once

namespace Wizzy {
    template <typename T>
    bool vector_contains(std::vector<T> vector, T value) {
        return std::find(vector.begin(), vector.end(), value) != vector.end();
    }

    template <typename T>
    bool set_contains(std::set<T> set, T value) {
        return std::find(set.begin(), set.end(), value) != set.end();
    }

    double inline to_radians(double deg)  {
        return deg * 0.0174532925;
    }

    double inline to_degrees(double rad)  {
        return rad * 57.2957795;
    }

	inline std::vector<string> split_string(const string& str, char delimiter) {

		std::vector<string> _ret;

		std::stringstream _stream(str);
		string _line = "";

		while (std::getline(_stream, _line, delimiter)) {

			if (_line != "") _ret.push_back(_line);

		}

		return _ret;

	}

	template <typename T>
	inline std::vector<std::vector<T>> split_vector(const std::vector<T>& v, const T& delimiter)
	{
		std::vector<std::vector<T>> _ret;
		_ret.push_back(std::vector<T>());
		int32 vectorIndex = 0;

		for (int i = 0; i < v.size(); i++)
		{
			if (v[i] == delimiter)
			{
				_ret.push_back(std::vector<T>());
				vectorIndex++;
			}
			else
			{
				_ret[vectorIndex].push_back(v[i]);
			}
		}

		return _ret;

	}

	template <typename T>
	inline std::vector<std::vector<T>> split_vector(const std::vector<T>& v, const std::vector<T>& delimiter)
	{
		std::vector<std::vector<T>> _ret;
		if (delimiter.size() > v.size() || v.size() == 0) return _ret;
		_ret.push_back(std::vector<T>());
		int32 vectorIndex = 0;

		for (int i = 0; i < v.size() - delimiter.size(); i++)
		{
			if (memcmp(&v[i], delimiter.data(), delimiter.size()) == 0)
			{
				if (_ret[vectorIndex].size() > 0)
				{
					_ret.push_back(std::vector<T>());
					vectorIndex++;
				}
				i += delimiter.size() - 1;
			}
			else
			{
				_ret[vectorIndex].push_back(v[i]);
			}
		}

		return _ret;

	}
}
