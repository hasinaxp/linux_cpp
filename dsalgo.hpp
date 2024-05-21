#pragma once
#include <cstring>
#include <cmath>
#include <vector>
#include <execution>
#include <unordered_map>
#include <functional>
#include <random>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <cassert>

namespace sp {

    //-----------------------------constants---------------------------------------------

    const float PI = 3.14159265358979323846f;
    const float PI2 = 6.28318530717958647692f;
    const float PI_2 = 1.57079632679489661923f;
    const float PI_4 = 0.785398163397448309616f;
    const float PI_INV = 0.318309886183790671538f;

    const float E = 2.71828182845904523536f;
    const float E_INV = 0.367879441171442321595f;

    const float SQRT2 = 1.41421356237309504880f;
    const float SQRT2_INV = 0.707106781186547524401f;

    const float SQRT3 = 1.73205080756887729352f;
    const float SQRT3_INV = 0.577350269189625764509f;


    template<typename T>
        inline T clamp(const T& value, const T& min, const T& max)
        {
            return value < min ? min : value > max ? max : value;
        }
    template<typename T>
        inline T lerp(const T& a, const T& b, const float& t)
        {
            return a + (b - a) * t;
        }
    template<typename T>
        inline T smoothstep(const T& a, const T& b, const float& t)
        {
            float x = clamp((t - a) / (b - a), 0.0f, 1.0f);
            return x * x * (3 - 2 * x);
        }
    template<typename T>
        inline T smootherstep(const T& a, const T& b, const float& t)
        {
            float x = clamp((t - a) / (b - a), 0.0f, 1.0f);
            return x * x * x * (x * (x * 6 - 15) + 10);
        }
    template<typename T>
        inline T map(const T& value, const T& min1, const T& max1, const T& min2, const T& max2)
        {
            return min2 + (max2 - min2) * ((value - min1) / (max1 - min1));
        }
    template<typename T>
        inline T map(const T& value, const T& min1, const T& max1, const T& min2, const T& max2, const T& min3, const T& max3)
        {
            return min3 + (max3 - min3) * ((value - min1) / (max1 - min1)) * ((max2 - min2) / (max3 - min3));
        }
    template<typename T>
        inline T map(const T& value, const T& min1, const T& max1, const T& min2, const T& max2, const T& min3, const T& max3, const T& min4, const T& max4)
        {
            return min4 + (max4 - min4) * ((value - min1) / (max1 - min1)) * ((max2 - min2) / (max3));
        }


    template<typename T>
        T lerp(const T& a, const T& b, float t) {
            return a + t * (b - a);
        }
    template<typename T>
        float inverseLerp(const T& a, const T& b, const T& value) {
            return (value - a) / (b - a);
        }


    template<typename T>
        T step(const T& edge, const T& x) {
            return x < edge ? T(0) : T(1);
        }

    template<typename T>
        T smoothstep(const T& edge0, const T& edge1, const T& x) {
            T t = clamp((x - edge0) / (edge1 - edge0), T(0), T(1));
            return t * t * (3 - 2 * t);
        }

    template<typename T>
        T smooth(const T& t) {
            return t * t * (3.0f - 2.0f * t);
        }

    template<typename T>
        T bezierCurve(const T& p0, const T& p1, const T& p2, const T& p3, float t) {
            float inv_t = 1.0f - t;
            float inv_t2 = inv_t * inv_t;
            float t2 = t * t;
            return p0 * inv_t2 * inv_t + p1 * 3.0f * t * inv_t2 + p2 * 3.0f * inv_t * t2 + p3 * t * t2;
        }

    template<typename T>
        T hermiteSpline(const T& p0, const T& p1, const T& m0, const T& m1, float t) {
            float t2 = t * t;
            float t3 = t2 * t;
            float h1 = 2.0f * t3 - 3.0f * t2 + 1.0f;
            float h2 = -2.0f * t3 + 3.0f * t2;
            float h3 = t3 - 2.0f * t2 + t;
            float h4 = t3 - t2;
            return p0 * h1 + p1 * h2 + m0 * h3 + m1 * h4;
        }

    struct quat
    {
        float x, y, z, w;


        quat() : x(0), y(0), z(0), w(1) {}
        quat(const float& x, const float& y, const float& z, const float& w) : x(x), y(y), z(z), w(w) {}
        quat(const float *& v, const float& w) : x(v[0]), y(v[1]), z(v[2]), w(w) {}
        quat(const float *& v) : x(v[0]), y(v[1]), z(v[2]), w(v[3]) {}

        void getEularAngle(float * v) 
        {

            float sqw = w * w;
            float sqx = x * x;
            float sqy = y * y;
            float sqz = z * z;
            float unit = sqx + sqy + sqz + sqw;
            float test = x * y + z * w;
            if (test > 0.499 * unit)
            {
                v[1] = 2 * atan2(x, w);
                v[2] = PI / 2;
                v[0] = 0;
                return;
            }
            if (test < -0.499 * unit)
            {
                v[1] = -2 * atan2(x, w);
                v[2] = -PI / 2;
                v[0] = 0;
                return;
            }
            v[1] = atan2(2 * y * w - 2 * x * z, sqx - sqy - sqz + sqw);
            v[2] = asin(2 * test / unit);
            v[0] = atan2(2 * x * w - 2 * y * z, -sqx + sqy - sqz + sqw);
            return;


        }

        quat operator+(const quat& q) const
        {
            return quat(x + q.x, y + q.y, z + q.z, w + q.w);
        }

        quat operator-(const quat& q) const
        {
            return quat(x - q.x, y - q.y, z - q.z, w - q.w);
        }

        quat operator*(const quat& q) const
        {
            return quat(
                    w * q.x + x * q.w + y * q.z - z * q.y,
                    w * q.y + y * q.w + z * q.x - x * q.z,
                    w * q.z + z * q.w + x * q.y - y * q.x,
                    w * q.w - x * q.x - y * q.y - z * q.z);
        }

        quat operator*(const float& s) const
        {
            return quat(x * s, y * s, z * s, w * s);
        }

        quat operator/(const float& s) const
        {
            return quat(x / s, y / s, z / s, w / s);
        }

        quat operator-() const
        {
            return quat(-x, -y, -z, -w);
        }

        quat& operator+=(const quat& q)
        {
            x += q.x;
            y += q.y;
            z += q.z;
            w += q.w;
            return *this;
        }

        quat& operator-=(const quat& q)
        {
            x -= q.x;
            y -= q.y;
            z -= q.z;
            w -= q.w;
            return *this;
        }

        quat& operator*=(const quat& q)
        {
            *this = *this * q;
            return *this;
        }

    };

    quat operator*(const float& s, const quat& q)
    {
        return quat(q.x * s, q.y * s, q.z * s, q.w * s);
    }

    quat operator/(const float& s, const quat& q)
    {
        return quat(q.x / s, q.y / s, q.z / s, q.w / s);
    }





#ifdef STD_VECTOR_UPGRADE


    template<typename T>
        std::vector<T>& operator+=(std::vector<T>& lhs, const std::vector<T>& rhs)
        {
            assert(lhs.size() == rhs.size());
            std::transform(std::execution::par_unseq, lhs.begin(), lhs.end(), rhs.cbegin(), lhs.begin(), std::plus<double>());
            return lhs;
        }
    template<typename T>
        std::vector<T>& operator-=(std::vector<T>& lhs, const std::vector<T>& rhs)
        {
            assert(lhs.size() == rhs.size());
            std::transform(std::execution::par_unseq, lhs.begin(), lhs.end(), rhs.cbegin(), lhs.begin(), std::minus<double>());
            return lhs;
        }
    template<typename T>
        std::vector<T>& operator*=(std::vector<T>& lhs, const std::vector<T>& rhs)
        {
            assert(lhs.size() == rhs.size());
            std::transform(std::execution::par_unseq, lhs.begin(), lhs.end(), rhs.cbegin(), lhs.begin(), std::multiplies<double>());
            return lhs;
        }
    template<typename T>
        std::vector<T>& operator/=(std::vector<T>& lhs, const std::vector<T>& rhs)
        {
            assert(lhs.size() == rhs.size());
            std::transform(std::execution::par_unseq, lhs.begin(), lhs.end(), rhs.cbegin(), lhs.begin(), std::divides<double>());
            return lhs;
        }
    template<typename T>
        std::vector<T> operator-(const std::vector<T>& v) {

            std::vector<T> result(v.size());
            std::transform(std::execution::par_unseq, v.begin(), v.end(), result.begin(), std::negate<T>());
            return result;
        }

    template<typename T>
        std::vector<T> operator+(const std::vector<T>& lhs, const std::vector<T>& rhs)
        {
            assert(lhs.size() == rhs.size());
            std::vector<T> result(lhs.size());
            std::transform(std::execution::par_unseq, lhs.begin(), lhs.end(), rhs.begin(), result.begin(), std::plus<T>());
            return result;
        }

    template<typename T>
        std::vector<T> operator-(const std::vector<T>& lhs, const std::vector<T>& rhs)
        {
            assert(lhs.size() == rhs.size());
            std::vector<T> result(lhs.size());
            std::transform(std::execution::par_unseq, lhs.begin(), lhs.end(), rhs.begin(), result.begin(), std::minus<T>());
            return result;
        }

    template<typename T>
        std::vector<T> operator*(const std::vector<T>& lhs, const std::vector<T>& rhs)
        {
            assert(lhs.size() == rhs.size());
            std::vector<T> result(lhs.size());
            std::transform(std::execution::par_unseq, lhs.begin(), lhs.end(), rhs.begin(), result.begin(), std::multiplies<T>());
            return result;
        }

    template<typename T>
        std::vector<T> operator/(const std::vector<T>& lhs, const std::vector<T>& rhs)
        {
            assert(lhs.size() == rhs.size());
            std::vector<T> result(lhs.size());
            std::transform(std::execution::par_unseq, lhs.begin(), lhs.end(), rhs.begin(), result.begin(), std::divides<T>());
            return result;
        }


    template<typename T>
        std::vector<T> operator+(const T& lhs, const std::vector<T>& rhs)
        {
            std::vector<T> result(rhs.size());
            std::transform(std::execution::par_unseq, rhs.begin(), rhs.end(), result.begin(), [lhs](const T& val) { return lhs + val; });
            return result;
        }

    template<typename T>
        std::vector<T> operator+(const std::vector<T>& lhs, const T& rhs)
        {
            std::vector<T> result(lhs.size());
            std::transform(std::execution::par_unseq, lhs.begin(), lhs.end(), result.begin(), [rhs](const T& val) { return val + rhs; });
            return result;
        }

    template<typename T>
        std::vector<T> operator-(const T& lhs, const std::vector<T>& rhs)
        {
            std::vector<T> result(rhs.size());
            std::transform(std::execution::par_unseq, rhs.begin(), rhs.end(), result.begin(), [lhs](const T& val) { return lhs - val; });
            return result;
        }

    template<typename T>
        std::vector<T> operator-(const std::vector<T>& lhs, const T& rhs)
        {
            std::vector<T> result(lhs.size());
            std::transform(std::execution::par_unseq, lhs.begin(), lhs.end(), result.begin(), [rhs](const T& val) { return val - rhs; });
            return result;
        }

    template<typename T>
        std::vector<T> operator*(const T& lhs, const std::vector<T>& rhs)
        {
            std::vector<T> result(rhs.size());
            std::transform(std::execution::par_unseq, rhs.begin(), rhs.end(), result.begin(), [lhs](const T& val) { return lhs * val; });
            return result;
        }

    template<typename T>
        std::vector<T> operator*(const std::vector<T>& lhs, const T& rhs)
        {
            std::vector<T> result(lhs.size());
            std::transform(std::execution::par_unseq, lhs.begin(), lhs.end(), result.begin(), [rhs](const T& val) { return val * rhs; });
            return result;
        }

    template<typename T>
        std::vector<T> operator/(const T& lhs, const std::vector<T>& rhs)
        {
            std::vector<T> result(rhs.size());
            std::transform(std::execution::par_unseq, rhs.begin(), rhs.end(), result.begin(), [lhs](const T& val) { return lhs / val; });
            return result;
        }

    template<typename T>
        std::vector<T> operator/(const std::vector<T>& lhs, const T& rhs)
        {
            std::vector<T> result(lhs.size());
            std::transform(std::execution::par_unseq, lhs.begin(), lhs.end(), result.begin(), [rhs](const T& val) { return val / rhs; });
            return result;
        }


    template<typename T>
        bool operator==(const std::vector<T>& a, const std::vector<T>& b) {
            if (a.size() != b.size()) {
                return false;
            }
            for (std::size_t i = 0; i < a.size(); ++i) {
                if (a[i] != b[i]) {
                    return false;
                }
            }
            return true;
        }
    template<typename T>
        bool operator<(const std::vector<T>& a, const std::vector<T>& b) {
            if (a.size() < b.size()) {
                return true;
            }
            else if (a.size() > b.size()) {
                return false;
            }
            else {
                for (std::size_t i = 0; i < a.size(); ++i) {
                    if (a[i] < b[i]) {
                        return true;
                    }
                    else if (a[i] > b[i]) {
                        return false;
                    }
                }
                return false;
            }
        }

    template<typename T>
        bool operator!=(const std::vector<T>& a, const std::vector<T>& b) {
            return !(a == b);
        }

    template<typename T>
        bool operator<=(const std::vector<T>& a, const std::vector<T>& b) {
            return !(b < a);
        }

    template<typename T>
        bool operator>(const std::vector<T>& a, const std::vector<T>& b) {
            return b < a;
        }

    template<typename T>
        bool operator>=(const std::vector<T>& a, const std::vector<T>& b) {
            return !(a < b);
        }

    template<typename T>
        T sum(const std::vector<T>& vec)
        {
            return std::accumulate(vec.begin(), vec.end(), T(0));
        }

    template<typename T>
        T product(const std::vector<T>& vec)
        {
            return std::accumulate(std::execution::par_unseq, vec.begin(), vec.end(), T(1), std::multiplies<T>());
        }

    template<typename T>
        T minElem(const std::vector<T>& vec)
        {
            return *std::min_element(std::execution::par_unseq, vec.begin(), vec.end());
        }

    template<typename T>
        T maxElem(const std::vector<T>& vec)
        {
            return *std::max_element(std::execution::par_unseq, vec.begin(), vec.end());
        }

    template<typename T>
        T mean(const std::vector<T>& vec)
        {
            return sum(vec) / static_cast<T>(vec.size());
        }

    template<typename T>
        std::vector<T> concat(const std::vector<T>& v1, const std::vector<T>& v2) {
            std::vector<T> result = v1;
            result.insert(result.end(), v2.begin(), v2.end());
            return result;
        }

    template<typename T>
        void append(std::vector<T>& v1, const std::vector<T>& v2) {
            v1.insert(v1.end(), v2.begin(), v2.end());
        }

    template<typename T>
        std::unordered_map<T, size_t> histogram(const std::vector<T>& v) {
            std::unordered_map<T, size_t> counts;
            for (const auto& x : v) {
                ++counts[x];
            }
            return counts;
        }

    template<typename T>
        std::unordered_map<T, size_t> histogram(const std::vector<T>& v, const size_t bucket_count) {
            // Determine the minimum and maximum values in the vector
            auto minmax = std::minmax_element(v.begin(), v.end());
            const T min_val = *minmax.first;
            const T max_val = *minmax.second;

            // Compute the width of each histogram bin
            const T bin_width = (max_val - min_val) / static_cast<T>(bucket_count);

            // Create an unordered map to store the counts for each bin
            std::unordered_map<T, size_t> counts;
            for (const auto& x : v) {
                // Determine which bin the value belongs to
                const size_t bin_index = static_cast<size_t>((x - min_val) / bin_width);

                // Increment the count for the bin
                ++counts[min_val + bin_index * bin_width];
            }
            return counts;
        }

    template<typename T1, typename T2>
        static std::vector<T1> change(const std::vector<T2>& v)
        {
            std::vector<T1> output(v.size());
            std::transform(std::execution::par_unseq, v.cbegin(), v.cend(), output.begin(), [](const T2& t) {
                    return (T1)t;
                    });
            return output;
        }

    template<typename T>
        static void apply(std::vector<T>& v, std::function<T(const T&)> func)
        {
            std::transform(std::execution::par_unseq, v.cbegin(), v.cend(), v.begin(), [&func](const T& t) {
                    return func(t);
                    });
        }

    template<typename T>
        std::vector<T> repeat(const std::vector<T>& v, uint32_t count) {
            std::vector<T> result;
            result.reserve(v.size() * count);
            for (uint32_t i = 0; i < count; ++i) {
                result.insert(result.end(), v.begin(), v.end());
            }
            return result;
        }

    template<typename T>
        std::vector<T> slice(const std::vector<T>& v, int32_t start, int32_t end) {
            if (start < 0) {
                start = v.size() + start;
            }
            if (end < 0) {
                end = v.size() + end;
            }
            if (start >= end || start < 0 || static_cast<size_t>(end) > v.size()) {
                return {};
            }
            return std::vector<T>(v.begin() + start, v.begin() + end);
        }

    template<typename T>
        std::vector<T> unique(const std::vector<T>& v) {
            std::vector<T> result;
            std::unordered_set<T> seen;
            for (const auto& x : v) {
                if (seen.insert(x).second) {
                    result.push_back(x);
                }
            }
            return result;
        }

    template<typename T>
        std::vector<T> filter(const std::vector<T>& v, std::function<bool(const T&)> predicate) {
            std::vector<T> result;
            std::copy_if(v.begin(), v.end(), std::back_inserter(result), predicate);
            return result;
        }

    template<typename T>
        std::vector<T> sample(const std::vector<T>& v, double frac) {
            if (frac <= 0.0) {
                return std::vector<T>();
            }

            int sampleSize = static_cast<int>(v.size() * frac);
            if (sampleSize == 0) {
                sampleSize = 1;
            }

            std::vector<T> sample(v.begin(), v.end());
            std::random_device rd;
            std::mt19937 gen(rd());
            std::shuffle(sample.begin(), sample.end(), gen);
            sample.resize(sampleSize);

            return sample;
        }

    template<typename T>
        void shuffleVector(std::vector<T>& v) {
            std::random_device rd;
            std::mt19937 gen(rd());

            std::shuffle(v.begin(), v.end(), gen);
        }


    template<typename T>
        std::vector<int> findIndices(const std::vector<T>& v, bool (*predicate)(T)) {
            std::vector<int> result;
            for (int i = 0; i < v.size(); i++) {
                if (predicate(v[i])) {
                    result.push_back(i);
                }
            }
            return result;
        }

    std::vector<double> normalize(const std::vector<double>& v) {
        std::vector<double> result(v.size());
        double sum = std::accumulate(v.begin(), v.end(), 0.0);
        for (int i = 0; i < v.size(); i++) {
            result[i] = v[i] / sum;
        }
        return result;
    }

    std::vector<float> normalize(const std::vector<float>& v) {
        std::vector<float> result(v.size());
        double sum = std::accumulate(v.begin(), v.end(), 0.0f);
        for (int i = 0; i < v.size(); i++) {
            result[i] = v[i] / sum;
        }
        return result;
    }

    double standardDeviation(const std::vector<double>& v) {
        if (v.size() < 2) {
            return 0.0;
        }

        double m = mean(v);
        double variance = 0.0;
        for (double x : v) {
            variance += std::pow(x - m, 2);
        }
        variance /= v.size() - 1;
        return std::sqrt(variance);
    }
    float standardDeviation(const std::vector<float>& v) {
        if (v.size() < 2) {
            return 0.0;
        }

        double m = mean(v);
        double variance = 0.0f;
        for (double x : v) {
            variance += std::pow(x - m, 2);
        }
        variance /= v.size() - 1;
        return std::sqrt(variance);
    }

    template<typename T>
        T dot(const std::vector<T>& a, const std::vector<T>& b) {
            assert(a.size() == b.size());
            return std::inner_product(a.begin(), a.end(), b.begin(), T(0));
        } 

    template<typename T>
        T length(const std::vector<T>& v) {
            return std::sqrt(dot(v, v));
        }

    template<typename T>
        std::vector<T> cross(const std::vector<T>& a, const std::vector<T>& b) {
            assert(a.size() == 3 && b.size() == 3);
            return {
                a[1] * b[2] - a[2] * b[1],
                a[2] * b[0] - a[0] * b[2],
                a[0] * b[1] - a[1] * b[0]
            };
        }

    template<typename T>
        std::vector<T> normalize(const std::vector<T>& v) {
            T len = length(v);
            if (len == 0) {
                return v;
            }
            return v / len;
        }

    template<typename T>
        std::vector<T> reflect(const std::vector<T>& v, const std::vector<T>& n) {
            return v - 2 * dot(v, n) * n;
        }

    template<typename T>
        std::vector<T> refract(const std::vector<T>& v, const std::vector<T>& n, T eta) {
            T k = 1 - eta * eta * (1 - dot(v, n) * dot(v, n));
            if (k < 0) {
                return { 0, 0, 0 };
            }
            return eta * v - (eta * dot(v, n) + std::sqrt(k)) * n;
        }



#endif

    inline uint32_t quickHash(const char* str)
    {
        uint32_t hash = 5381;
        int c;
        while ((c = *str++))
            hash = ((hash << 5) + hash) + c;
        return hash;
    }

    inline uint32_t quickHash(const char* str, const uint32_t& length)
    {
        uint32_t hash = 5381;
        int c;
        for (uint32_t i = 0; i < length; i++)
        {
            c = *str++;
            hash = ((hash << 5) + hash) + c;
        }
        return hash;
    }

    inline uint32_t crc(char* buffer, const uint64_t& length)
    {

        uint32_t crc = 0xFFFFFFFF;
        for (uint64_t i = 0; i < length; i++)
        {
            crc = crc ^ buffer[i];
            for (uint32_t j = 0; j < 8; j++)
            {
                if (crc & 1)
                    crc = (crc >> 1) ^ 0xEDB88320;
                else
                    crc = crc >> 1;
            }
        }
        return ~crc;

    }

    inline uint64_t getTimestamp() { return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count(); }

    unsigned int fastRandom() {

        static unsigned int s = static_cast<unsigned int>(std::time(nullptr));
        s = (214013 * s + 2531011);
        return (s >> 16) & 0x7FFF;
    }

    inline float randomFloat(float min, float max) {
        return min + static_cast<float>(fastRandom()) / 0x7FFF * (max - min);
    }

    inline int randomInt(int min, int max) {
        return min + fastRandom() % (max - min + 1);
    }

    	struct base64 {

		std::string _charset;
		std::vector<int> _invSet;
		base64()
			:_charset("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"),
			_invSet({ 62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58,
					59, 60, 61, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5,
					6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
					21, 22, 23, 24, 25, -1, -1, -1, -1, -1, -1, 26, 27, 28,
					29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
					43, 44, 45, 46, 47, 48, 49, 50, 51 })
		{
		}

		std::string encode(const char* text, const uint32_t & length = 0)
		{
			std::string out;

			if (strlen(text) == 0)
				return "";
			uint64_t txtLength = length ? length : strlen(text);
			uint64_t encLength = calcEncodingSize(txtLength);
			out.resize(encLength, ' ');
			uint64_t v;
			for (uint64_t i = 0, j = 0; i < encLength; i += 3, j += 4) {
				v = text[i];
				v = i + 1 < txtLength ? v << 8 | text[i + 1] : v << 8;
				v = i + 2 < txtLength ? v << 8 | text[i + 2] : v << 8;

				out[j] = _charset[(v >> 18) & 0x3F];
				out[j + 1] = _charset[(v >> 12) & 0x3F];
				if (i + 1 < txtLength) {
					out[j + 2] = _charset[(v >> 6) & 0x3F];
				}
				else {
					out[j + 2] = '=';
				}
				if (i + 2 < txtLength) {
					out[j + 3] = _charset[v & 0x3F];
				}
				else {
					out[j + 3] = '=';
				}
			}
			return out;
		}

		std::string decode(const char* text, const uint32_t & length = 0) {
			std::string out;

			if (strlen(text) == 0)
				return "";
			uint64_t txtLength = length ? length : strlen(text);
			uint64_t nEql = 0;
			for (uint64_t i = 0; i < txtLength; i++) {
				if (!isValidChar(text[i])) {
					return "";
				}
				if (text[i] == '=') nEql++;
			}
			uint64_t encLength = calcDecodingSize(txtLength, nEql);
			out.resize(encLength, ' ');
			uint64_t v;
			for (uint64_t i = 0, j = 0; i < txtLength; i += 4, j += 3) {
				v = _invSet[text[i] - 43];
				v = (v << 6) | _invSet[text[i + 1] - 43];
				v = text[i + 2] == '=' ? v << 6 : (v << 6) | _invSet[text[i + 2] - 43];
				v = text[i + 3] == '=' ? v << 6 : (v << 6) | _invSet[text[i + 3] - 43];

				out[j] = (v >> 16) & 0xFF;
				if (text[i + 2] != '=')
					out[j + 1] = (v >> 8) & 0xFF;
				if (text[i + 3] != '=')
					out[j + 2] = v & 0xFF;
			}

			return out;
		}

		inline uint64_t calcEncodingSize(uint64_t length)
		{
			if (length % 3 != 0)
				length = ((length / 3) + 1) * 4;
			else
				length = (length / 3) * 4;

			return length;
		}
		inline uint64_t calcDecodingSize(uint64_t length,const uint64_t & nEqual) {
			length = (length / 4) * 3;
			length -= nEqual;
			return length;
		}
		bool isValidChar(const char & c) {
			if (c >= '0' && c <= '9')
				return true;
			if (c >= 'A' && c <= 'Z')
				return true;
			if (c >= 'a' && c <= 'z')
				return true;
			if (c == '+' || c == '/' || c == '=')
				return true;
			return false;
		}
	};




};  // namespace sp
