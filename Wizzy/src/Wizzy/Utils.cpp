#include "wzpch.h"

#include "Wizzy/Utils.h"

bool write_file(const string& file, const string& data) {
    std::ofstream _file(file);
    if (_file.is_open()) { _file << data; _file.close(); return true; }
    return false;
}

bool read_file(const string& file, string *out) {
    *out = "";
    string _line;
    std::ifstream _file(file, std::ios::in);
    if (_file.is_open()) {
        while (std::getline(_file, _line)) *out += _line + "\n";
        _file.close();
        return true;
    }

    if (_file.bad()) WZ_CORE_ERROR("File '{0}' is bad", file);
    if (_file.fail()) WZ_CORE_ERROR("File '{0}' fail()", file);

    return false;
}
