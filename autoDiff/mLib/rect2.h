
#ifndef CORE_GRAPHICS_rect2_H_
#define CORE_GRAPHICS_rect2_H_

template<class FloatType>
class rect2
{
public:

    rect2(void) {
        reset();
    }

    rect2(FloatType _minX, FloatType _minY, FloatType _maxX, FloatType _maxY)
    {
        minB = vec2<FloatType>(_minX, _minY);
        maxB = vec2<FloatType>(_maxX, _maxY);
    }

    rect2(const rect2 &r)
    {
        minB = r.minB;
        maxB = r.maxB;
    }

    void operator = (const rect2 &r)
    {
        minB = r.minB;
        maxB = r.maxB;
    }

    rect2(const vec2<FloatType>& minBound, const vec2<FloatType>& maxBound) {
        reset();
        minB = minBound;
        maxB = maxBound;
    }

    void reset() {
        minX = minY = std::numeric_limits<FloatType>::max();
        maxX = maxY = -std::numeric_limits<FloatType>::max();
    }

    void include(const rect2 &other) {
        if (other.minX < minX)	minX = other.minX;
        if (other.minY < minY)	minY = other.minY;

        if (other.maxX > maxX)	maxX = other.maxX;
        if (other.maxY > maxY)	maxY = other.maxY;
    }

    void include(const vec2<FloatType> &v) {
        if (v.x < minX)	minX = v.x;
        if (v.y < minY)	minY = v.y;

        if (v.x > maxX)	maxX = v.x;
        if (v.y > maxY)	maxY = v.y;
    }

    void include(const std::vector<vec2<FloatType>> &v) {
        for (const auto &p : v)
            include(p);
    }

    bool isInitialized() const {
        return (minX != std::numeric_limits<FloatType>::max());
    }

    bool isValid() const {
        return (minX <= maxX && minY <= maxY);
    }

    //! point collision
    bool intersects(const vec2<FloatType>& p) const {
        if (p.x >= minX && p.x <= maxX &&
            p.y >= minY && p.y <= maxY) return true;
        else  return false;
    }

    //! bounding box collision
    bool intersects(const rect2<FloatType>& other) const {
        return
            minX <= other.maxX && other.minX <= maxX &&
            minY <= other.maxY && other.minY <= maxY;
    }

    FloatType maxExtent() const {
        FloatType d0 = maxX - minX;
        FloatType d1 = maxY - minY;
        return std::max(d0, d1);
    }

    FloatType extentX() const {
        return maxX - minX;
    }

    FloatType extentY() const {
        return maxY - minY;
    }

    vec2<FloatType> extent() const {
        return vec2<FloatType>(maxX - minX, maxY - minY);
    }

    vec2<FloatType> min() const {
        return vec2<FloatType>(minX, minY);
    }

    vec2<FloatType> max() const {
        return vec2<FloatType>(maxX, maxY);
    }

    vec2<FloatType> center() const {
        vec2<FloatType> center = min() + max();
        center *= (FloatType)0.5;
        return center;
    }

    void setMin(const vec2<FloatType>& minValue) {
        minX = minValue.x;
        minY = minValue.y;
    }

    void setMax(const vec2<FloatType>& maxValue) {
        maxX = maxValue.x;
        maxY = maxValue.y;
    }

    void setMinX(FloatType v) { minX = v; }
    void setMinY(FloatType v) { minY = v; }
    void setMaxX(FloatType v) { maxX = v; }
    void setMaxY(FloatType v) { maxY = v; }

    //! scales the bounding box by the factor t (for t=1 the bb remains unchanged)
    void scale(FloatType x, FloatType y) {

        float scale[] = { x, y };
        for (unsigned int i = 0; i < 2; i++) {
            FloatType center = (FloatType)0.5 * (parameters[i] + parameters[i + 2]);
            FloatType diff = parameters[i + 2] - parameters[i];
            diff *= scale[i];
            diff *= (FloatType)0.5;
            parameters[i] = center - diff;
            parameters[i + 2] = center + diff;
        }
    }

    //! scales the bounding box by the factor t (for t=1 the bb remains unchanged)
    void scale(FloatType t) {
        for (unsigned int i = 0; i < 2; i++) {
            FloatType center = (FloatType)0.5 * (parameters[i] + parameters[i + 2]);
            FloatType diff = parameters[i + 2] - parameters[i];
            diff *= t;
            diff *= (FloatType)0.5;
            parameters[i] = center - diff;
            parameters[i + 2] = center + diff;
        }
    }

    void translate(const vec2<FloatType>& t) {
        minB += t;
        maxB += t;
    }

    //! scales the bounding box (see scale)
    rect2<FloatType> operator*(FloatType t) const {
        rect2<FloatType> res = *this;
        res.scale(t);
        return res;
    }

protected:
    union {
        struct {
            vec2<FloatType> minB;
            vec2<FloatType> maxB;
        };
        struct {
            FloatType minX, minY;
            FloatType maxX, maxY;
        };
        FloatType parameters[4];
    };
};

template<class FloatType>
std::ostream& operator<< (std::ostream& s, const rect2<FloatType>& bb) {
    s << bb.min() << std::endl << bb.max() << std::endl;
    return s;
}

typedef rect2<float> rect2f;
typedef rect2<double> rect2d;
typedef rect2<int> rect2i;

typedef rect2<float> bbox2f;
typedef rect2<double> bbox2d;
typedef rect2<int> bbox2i;

#endif  // CORE_GRAPHICS_rect2_H_
