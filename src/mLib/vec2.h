
#ifndef CORE_MATH_POINT2D_H_
#define CORE_MATH_POINT2D_H_

//! 2D vector
template <class T>
class vec2
{
public:
	explicit vec2(T v) {
		array[0] = array[1] = v;
	}

	vec2() {
		array[0] = array[1] = 0;
	}

	vec2(size_t x, size_t y) {
		array[0] = (T)x;
		array[1] = (T)y;
	}

	vec2(T x, T y) {
		array[0] = x;
		array[1] = y;
	}

	template <class U>
	vec2(const vec2<U>& other) {
		array[0] = (T)other.array[0];
		array[1] = (T)other.array[1];
	}

	vec2(const vec2& other) {
		array[0] = other.array[0];
		array[1] = other.array[1];
	}

	vec2(const T* other) {
		array[0] = other[0];
		array[1] = other[1];
	}

	inline const vec2<T>& operator=(const vec2& other) {
		array[0] = other.array[0];
		array[1] = other.array[1];
		return *this;
	}

	inline const vec2<T>& operator=(T other) {
		array[0] = other;
		array[1] = other;
		return *this;
	}

	inline vec2<T> operator-() const {
		return vec2<T>(-array[0], -array[1]);
	}

	inline vec2<T> operator+(const vec2& other) const {
		return vec2<T>(array[0]+other.array[0], array[1]+other.array[1]);
	}

	inline vec2<T> operator+(T val) const {
		return vec2<T>(array[0]+val, array[1]+val);
	}

	inline void operator+=(const vec2& other) {
		array[0] += other.array[0];
		array[1] += other.array[1];
	}

	inline void operator-=(const vec2& other) {
		array[0] -= other.array[0];
		array[1] -= other.array[1];
	}

	inline void operator+=(T val) {
		array[0] += val;
		array[1] += val;
	}

	inline void operator-=(T val) {
		array[0] -= val;
		array[1] -= val;
	}

	inline void operator*=(T val) {
		array[0] *= val;
		array[1] *= val;
	}

	inline void operator/=(T val) {
		T inv = (T)1/val;
		array[0] *= inv;
		array[1] *= inv;
	}

	inline vec2<T> operator*(T val) const {
		return vec2<T>(array[0]*val, array[1]*val);
	}

	inline vec2<T> operator/(T val) const {
		T inv = (T)1/val;
		return vec2<T>(array[0]*inv, array[1]*inv);
	}

	inline vec2<T> operator-(const vec2& other) const {
		return vec2<T>(array[0]-other.array[0], array[1]-other.array[1]);
	}

	inline vec2<T> operator-(T val) const {
		return vec2<T>(array[0]-val, array[1]-val);
	}

	inline bool operator==(const vec2& other) const {
		if ((array[0] == other.array[0]) && (array[1] == other.array[1]))
			return true;

		return false;
	}

	inline bool operator!=(const vec2& other) const {
		return !(*this == other);
	}

	//! dot product
	inline T operator|(const vec2& other) const {
		return (array[0]*other.array[0] + array[1]*other.array[1]);
	}

    static inline T dot(const vec2& l, const vec2& r) {
        return(l.array[0] * r.array[0] + l.array[1] * r.array[1]);
    }

    static inline vec2 directProduct(const vec2& l, const vec2& r) {
        return vec2(l.array[0] * r.array[0], l.array[1] * r.array[1]);
    }

	static inline vec2 directQuotient(const vec2& l, const vec2& r) {
		return vec2(l.array[0] / r.array[0], l.array[1] / r.array[1]);
	}

	inline T& operator[](unsigned int i) {
        MLIB_ASSERT(i < 2);
		return array[i];
	}

	inline const T& operator[](unsigned int i) const {
		MLIB_ASSERT(i < 2);
		return array[i];
	}

	inline bool operator<(const vec2& other) const {
		if ((x < other.x) && (y < other.y))
			return true;

		return false;
	}

	inline T lengthSq() const {
		return (array[0]*array[0] + array[1]*array[1]);
	}

	inline T length() const {
		return sqrt(array[0]*array[0] + array[1]*array[1]);
	}

	static T distSq(const vec2& v0, const vec2& v1) {
		return ((v0.array[0]-v1.array[0])*(v0.array[0]-v1.array[0]) + (v0.array[1]-v1.array[1])*(v0.array[1]-v1.array[1]));
	}

	static T dist(const vec2& v0, const vec2& v1) {
		return sqrt((v0.array[0]-v1.array[0])*(v0.array[0]-v1.array[0]) + (v0.array[1]-v1.array[1])*(v0.array[1]-v1.array[1]));
	}

    static inline void normalize(const vec2& v) {
        return v.getNormalized();
    }

	inline void normalize() {
		T val = (T)1.0 / length();
		array[0] *= val;
		array[1] *= val;
	}

	inline vec2<T> getNormalized() const {
		T val = (T)1.0 / length();
		return vec2<T>(array[0] * val, array[1] * val);
	}

	inline void print() const {
		std::cout << "(" << array[0] << " / " << array[1] << ")" << std::endl;
	}

	inline T* ptr() {
		return &array[0];
	}

    inline std::vector<T> toStdVector() const {
        std::vector<T> result(2);
        result[0] = x;
        result[1] = y;
        return result;
    }

    static const vec2<T> origin()
    {
        return vec2<T>(T(0), T(0));
    }

    static const vec2<T> eX()
    {
        return vec2<T>(T(1), T(0));
    }

    static const vec2<T> eY()
    {
        return vec2<T>(T(0), T(1));
    }

	union {
		struct {
			T x,y;      // standard names for components
		};
		T array[2];     // array access
	};
};

//! operator for scalar * vector
template <class T>
inline vec2<T> operator*(T s, const vec2<T>& v)
{
	return v * s;
}
template <class T>
inline vec2<T> operator/(T s, const vec2<T>& v)
{
	return vec2<T>(s/v.x, s/v.y);
}
template <class T>
inline vec2<T> operator+(T s, const vec2<T>& v)
{
	return v + s;
}
template <class T>
inline vec2<T> operator-(T s, const vec2<T>& v)
{
	return vec2<T>(s-v.x, s-v.y);
}

namespace math {
	template<class T>
	inline vec2<int> sign(const vec2<T>& v) {
		return vec2<int>(sign(v.x), sign(v.y));
	}
}

//! write a vec2 to a stream
template <class T> inline std::ostream& operator<<(std::ostream& s, const vec2<T>& v) {
  return (s << v[0] << " " << v[1]);
}

//! read a vec2 from a stream
template <class T> inline std::istream& operator>>(std::istream& s, vec2<T>& v) {
  return (s >> v[0] >> v[1]);
}

typedef vec2<double> vec2d;
typedef vec2<float> vec2f;
typedef vec2<int> vec2i;
typedef vec2<unsigned int> vec2ui;
typedef vec2<unsigned char> vec2uc;

namespace math {
	template<class T>
	inline vec2i round(const vec2<T>& f) {
		return vec2i(round(f.x), round(f.y));
	}
	template<class T>
	inline vec2i ceil(const vec2<T>& f) {
		return vec2i(ceil(f.x), ceil(f.y));
	}
	template<class T>
	inline vec2i floor(const vec2<T>& f) {
		return vec2i(floor(f.x), floor(f.y));
	}
	template<class T>
	inline vec2<T> abs(const vec2<T>& p) {
		return vec2<T>(abs(p.x), abs(p.y));
	}
	template<class T>
	inline vec2<T> max(const vec2<T>& p, T v) {
		return vec2<T>(std::max(p.x, v),
			std::max(p.y, v));
	}
	template<class T>
	inline vec2<T> max(const vec2<T>& p, const vec2<T>& v) {
		return vec2<T>(
			std::max(p.x, v.x),
			std::max(p.y, v.y));
	}
	template<class T>
	inline vec2<T> min(const vec2<T>& p, T v) {
		return vec2<T>(std::min(p.x, v),
			std::min(p.y, v));
	}
	template<class T>
	inline vec2<T> min(const vec2<T>& p, const vec2<T>& v) {
		return vec2<T>(
			std::min(p.x, v.x),
			std::min(p.y, v.y));
	}


}

#endif  // CORE_MATH_POINT2D_H_
