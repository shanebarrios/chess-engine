#pragma once

#include <cassert>
#include <algorithm>
#include <array>
#include <cstring>

template <typename T, size_t M>
using Array = std::array<T, M>;

template <typename T, size_t M, size_t N>
using Array2D = std::array<std::array<T, N>, M>;

template <typename T, size_t M, size_t N, size_t P>
using Array3D = std::array<std::array<std::array<T, P>, N>, M>;

template <typename T, size_t N>
class HeapArray
{
public:
	HeapArray() { m_data = new T[N]{}; }

	explicit HeapArray(const T& val) { 
		m_data = new T[N]; 
		std::fill(m_data, m_data + N, val);
	}

	~HeapArray() { delete[] m_data; };

	HeapArray(const HeapArray& other) {
		m_data = new T[N];
		if constexpr (std::is_trivially_copyable_v<T>) {
			std::memcpy(m_data, other.m_data, N * sizeof(T));
		}
		else {
			std::copy(other.m_data, other.m_data + N, m_data);
		}
	}

	HeapArray& operator=(const HeapArray& other) {
		if (&other == this) return *this;
		delete[] m_data;
		m_data = new T[N];
		if constexpr (std::is_trivially_copyable_v<T>) {
			std::memcpy(m_data, other.m_data, N * sizeof(T));
		}
		else {
			std::copy(other.m_data, other.m_data + N, m_data);
		}
		return *this;
	}

	HeapArray(HeapArray&& other) noexcept : 
		m_data {other.m_data} {
		other.m_data = nullptr;
	}

	HeapArray& operator=(HeapArray&& other) noexcept {
		if (&other == this) return *this;

		delete[] m_data;
		m_data = other.m_data;
		other.m_data = nullptr;
		return *this;
	}

	constexpr size_t size() const { return N; }

	T& operator[](size_t i) { 
		assert(i < N); 
		return m_data[i];
	}

	const T& operator[](size_t i) const {
		assert(i < N);
		return m_data[i];
	}

	T* begin() { return m_data; }
	T* end() { return m_data + N;  }

	const T* begin() const { return m_data; }
	const T* end() const { return m_data + N; }
private:
	T* m_data = nullptr;
};

template <typename T, size_t N>
class FixedStack
{
public:
	FixedStack() = default;

	void push(const T& val) {
		m_data[m_pos++] = val;
	}

	void pop() {
		assert(m_pos > 0);
		m_pos--;
	}

	T& top() {
		assert(m_pos > 0);
		return m_data[m_pos - 1];
	}

	const T& top() const {
		assert(m_pos > 0);
		return m_data[m_pos - 1];
	}

	void clear() { m_pos = 0; }

	constexpr size_t capacity() const { return N; }

	size_t size() const { return m_pos; }

	T& operator[](size_t i) { 
		assert(i < m_pos);
		return m_data[i]; 
	}

	const T& operator[](size_t i) const {
		assert(i < m_pos);
		return m_data[i];
	}
private:
	HeapArray<T, N> m_data{};
	size_t m_pos = 0;
};
