#include "wzpch.h"

#include <experimental/filesystem>

#include "Wizzy/Utils.h"

namespace Wizzy {
    namespace File {
        namespace fs = ::std::experimental::filesystem::v1;
    
        bool write(const string& file, const string& data) {
            std::ofstream _file(file);
            if (_file.is_open()) { _file << data; _file.close(); return true; }
            return false;
        }

            bool read(const string& file, string *out) {
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
        
        string to_absolute(const string& path) {
            if (!is_absolute(path)) return fs::absolute(path);
            
            else return "";
        }
        
        bool is_absolute(const string& file) {
            return exists(file) && fs::path(file).is_absolute();
        }
        bool is_relative(const string& file) {
            return exists(file) && fs::path(file).is_relative();
        }
        
        bool exists(const string& file) {
            return fs::exists(file);
        }
        
        bool has_extension(const string& file) {
            return fs::path(file).has_extension();
        }   
        
        string extension_of(const string& file) {
            return fs::path(file).extension();
        }
    }
}
