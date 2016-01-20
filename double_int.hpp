#ifndef INCLUDE_DOUBLE_INT
#define INCLUDE_DOUBLE_INT 1

#include <iostream>
#include <bitset>
using namespace std;

/*
 * class of int pair with extended bit operations
*/
template<class T1, class T2>
struct double_int {

	T1 first;
	T2 second;

	// constructor
	double_int(): T1(), T2() {}
	double_int(const T1& first, const T2& second):
		first(first),
		second(second) {}
	template<class U1, class U2>
	double_int(const double_int<U1, U2>& d):
		first(T1(d.first)),
		second(T2(d.second)) {}
	double_int(const T2& x):
		first(0),
		second(x) {}

	// binary bit operations
	#undef binary_operation
	#define binary_operation(op) \
	template<class U1, class U2> \
	inline friend double_int operator op (const double_int<U1, U2>& b) const { \
		return double_int<T1, T2>(first op b.first, second op b.second); \
	} \
	inline friend double_int operator op (const T2& b) const { \
		return double_int<T1, T2>(first, second op b); \
	} \
	binary_operation(|)
	binary_operation(&)
	binary_operation(^)
	#undef binary_operation

	// binary bit reassign operations
	#define binary_operation(op) \
	template<class U1, class U2> \
	inline friend double_int& operator op (const double_int<U1, U2>& b) const { \
		first op b.first; second op b.second; return *this; \
	} \
	inline friend double_int& operator op (const T2& b) const { \
		second op b; return *this; \
	} \
	binary_operation(|=)
	binary_operation(&=)
	binary_operation(^=)
	#undef binary_operation

	// access bit at index
	// offset = (1 << power)
	inline int get(int index, int power = 0) const {
		int bits = (sizeof(T2) << 3) >> power; // sizeof(T2) * 8
		if (index >= bits)
			return (first >> ((index - bits) << power)) & ((T1(1) << (1 << power)) - 1);
		else
			return (second >> (index << power)) & (T2(1) << (1 << power)) - 1;
	}

	// set bit value at index
	inline void set(int index, int value, int power = 0) {
		int bits = (sizeof(T2) << 3) >> power;
		if (index >= bits) {
			T1 mask = (T1(1) << (1 << power)) - 1;
			int shift = (index - bits) << power;
			first = (first & ~(mask << shift)) | ((mask & value) << shift);
		} else {
			T2 mask = (T2(1) << (1 << power)) - 1;
			int shift = index << power;
			second = (second & ~(mask << shift)) | ((mask & value) << shift);
		}
	}

	// shift operations
	inline double_int operator << (int shift) const {
		int bits = sizeof(T2) << 3;
		return double_int((first << shift) | T1(second >> (bits - shift)), second << shift);
	}
	inline double_int& operator << (int shift) {
		int bits = sizeof(T2) << 3;
		first = (first << shift) | T1(second >> (bits - shift));
		second <<= shift;
		return *this;
	}
	inline double_int operator >> (int shift) const {
		int bits = sizeof(T2) << 3;
		return double_int(first >> shift, (second >> shift) | T2(first & ((T1(1) << shift) - 1));
	}
	inline double_int& operator >> (int shift) {
		int bits = sizeof(T2) << 3;
		second = (second >> shift) | T2(first & ((T1(1) << shift) - 1);
		first >>= shift;
		return *this;
	}

	// overload if(*this) and while (*this)
	operator bool() const {
		return bool(first) || bool(second);
	}
	// overload if(!*this)
	inline bool operator !() const {
		return !bool(*this);
	}
	// overload unary flip
	inline double_int<T1, T2> operator~() const {
		return double_int<T1, T2>(~first, ~second);
	}
	// TODO: separate integral double_int and recursive double_int
	inline friend ostream& operator << (ostream& out, const double_int<T1, T2>& d) {
		return out << bitset<(sizeof(T1) << 3)>(d.first) << ' ' << bitset<(sizeof(T2) << 3)>(d.second);
	}
	// TODO: arithmetic operations
	template<class U1, class U2>
	inline bool operator < (const double_int<U1, U2>& b) const {
		return first < b.first || (first == b.first && second < b.second);
	}
	template<class U1, class U2>
	inline bool operator == (const double_int<U1, U2>& b) const {

	}

};

#endif /* INCLUDE_DOUBLE_INT */