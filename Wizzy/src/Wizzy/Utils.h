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

    namespace File {
        bool write(const string& file, const string& data);

        bool read(const string& file, string *out);
        
        string to_absolute(const string& path);
        
        bool is_absolute(const string& file);
        bool is_relative(const string& file);
        
        bool exists(const string& file);
        
        bool has_extension(const string& file);
        
        string extension_of(const string& file);
    }
}



