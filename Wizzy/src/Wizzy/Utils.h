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
}
