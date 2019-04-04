#pragma once

template <typename T>
bool vector_contains(std::vector<T> vector, T value) {
	return std::find(vector.begin(), vector.end(), value) != vector.end();
}

template <typename T>
bool set_contains(std::set<T> set, T value) {
	return std::find(set.begin(), set.end(), value) != set.end();
}
