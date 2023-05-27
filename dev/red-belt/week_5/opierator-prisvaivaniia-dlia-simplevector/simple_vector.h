//#pragma once

#include <algorithm>

template <class T>
class SimpleVector {
public:
	SimpleVector() = default;

	explicit SimpleVector(size_t size)
		:data(new T[size]), size(size), capacity(size)
	{}

	~SimpleVector() {
		delete[] data;
	}

	SimpleVector(const SimpleVector& other)
	{
		if (other.Size() == 0) {
			data = nullptr;
		}
		else {
			data = new T[other.Size()];
			copy(other.begin(), other.end(), begin());
		}

		capacity = other.Capacity();
		size = other.Size();
	}

	void operator=(const SimpleVector& other) {
		if (data != nullptr) {
			delete[] data;
		}
		if (other.Size() != 0) {
			data = new T[other.Size()];
			copy(other.begin(), other.end(), begin());
		}

		capacity = other.Capacity();
		size = other.Size();
	}

	T& operator[](size_t pos) {
		return *(data + pos);
	}

	const T& At(size_t pos) const {
		return *(data + pos);
	}

	void PushBack(const T& val) {
		if (size == capacity) {
			capacity *= 2;
			T* tmp = new T[capacity];
			copy(begin(), end(), tmp);
			if (data != nullptr) {
				delete[]data;
			}
			data = tmp;
		}
		*(data + size) = val;
		++size;
	}

	T* begin() {
		return data;
	}

	T* end() {
		return data + size;
	}

	const T* begin() const {
		return data;
	}

	const T* end() const {
		return data + size;
	}

	size_t Capacity() const {
		return capacity;
	}

	size_t Size() const {
		return size;
	}

private:
	T* data;
	size_t size;
	size_t capacity;
};