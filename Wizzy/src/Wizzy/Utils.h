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


}
