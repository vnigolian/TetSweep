#pragma once
#include <includes.hh>

namespace tet_weave {

class Vec3d {
public:
    // -------------------------------------------------------------------------
    // Constructors
    // -------------------------------------------------------------------------

    /// Default constructor — leaves components uninitialized (matches Eigen).
    Vec3d() = default;

    /// Component constructor.
    Vec3d(double x, double y, double z) : x_(x), y_(y), z_(z) {}

    /// Aggregate / brace-init: Vec3d v = {1.0, 2.0, 3.0};
    /// Handled automatically via the (x,y,z) constructor when using brace
    /// initialization, but we also provide an explicit initializer_list ctor for
    /// clarity.
    // (The (double,double,double) ctor already supports Vec3d v{x,y,z} and
    //  Vec3d v = {x,y,z} in C++11 and later.)

    /// Standard copy constructor.
    Vec3d(const Vec3d&) = default;

    /// Standard move constructor.
    Vec3d(Vec3d&&) = default;

    // -------------------------------------------------------------------------
    // Assignment operators
    // -------------------------------------------------------------------------

    Vec3d& operator=(const Vec3d&) = default;
    Vec3d& operator=(Vec3d&&)      = default;

    // -------------------------------------------------------------------------
    // Accessors (Eigen-style: x(), y(), z())
    // -------------------------------------------------------------------------

    double  x() const { return x_; }
    double  y() const { return y_; }
    double  z() const { return z_; }

    double& x() { return x_; }
    double& y() { return y_; }
    double& z() { return z_; }

    // -------------------------------------------------------------------------
    // Subscript operator
    // -------------------------------------------------------------------------

    double operator[](int i) const {
#ifndef NDEBUG
        assert(i >= 0 && i < 3 && "Vec3d: index out of range");
#endif
        return (&x_)[i];
    }

    double& operator[](int i) {
#ifndef NDEBUG
        assert(i >= 0 && i < 3 && "Vec3d: index out of range");
#endif
        return (&x_)[i];
    }

    // -------------------------------------------------------------------------
    // Arithmetic — in-place (Eigen-style: +=, -=, *=)
    // -------------------------------------------------------------------------

    Vec3d& operator+=(const Vec3d& rhs) {
        x_ += rhs.x_; y_ += rhs.y_; z_ += rhs.z_;
        return *this;
    }

    Vec3d& operator-=(const Vec3d& rhs) {
        x_ -= rhs.x_; y_ -= rhs.y_; z_ -= rhs.z_;
        return *this;
    }

    Vec3d& operator*=(double s) {
        x_ *= s; y_ *= s; z_ *= s;
        return *this;
    }

    // -------------------------------------------------------------------------
    // Arithmetic — creating new vectors
    // -------------------------------------------------------------------------

    Vec3d operator+(const Vec3d& rhs) const { return {x_ + rhs.x_, y_ + rhs.y_, z_ + rhs.z_}; }
    Vec3d operator-(const Vec3d& rhs) const { return {x_ - rhs.x_, y_ - rhs.y_, z_ - rhs.z_}; }
    Vec3d operator-()                 const { return {-x_, -y_, -z_}; }
    Vec3d operator*(double s)         const { return {x_ * s, y_ * s, z_ * s}; }

    // -------------------------------------------------------------------------
    // Dot product (Eigen: a.dot(b))
    // -------------------------------------------------------------------------

    double dot(const Vec3d& rhs) const {
        return x_ * rhs.x_ + y_ * rhs.y_ + z_ * rhs.z_;
    }

    // -------------------------------------------------------------------------
    // Cross product (Eigen: a.cross(b))
    // -------------------------------------------------------------------------

    Vec3d cross(const Vec3d& rhs) const {
        return {
            y_ * rhs.z_ - z_ * rhs.y_,
            z_ * rhs.x_ - x_ * rhs.z_,
            x_ * rhs.y_ - y_ * rhs.x_
        };
    }

    // -------------------------------------------------------------------------
    // Norm / normalization (Eigen: norm(), normalized(), normalize())
    // -------------------------------------------------------------------------

    /// Squared L2 norm.
    double squaredNorm() const { return dot(*this); }

    /// L2 norm.
    double norm() const { return std::sqrt(squaredNorm()); }

    /// Returns a unit-length copy without modifying this vector.
    Vec3d normalized() const {
        const double n = norm();
        assert(n > 0.0 && "Vec3d::normalized() called on zero vector");
        return (*this) * (1.0 / n);
    }

    /// Normalizes this vector in-place and returns a reference to it.
    Vec3d& normalize() {
        const double n = norm();
        assert(n > 0.0 && "Vec3d::normalize() called on zero vector");
        return (*this) *= (1.0 / n);
    }

    // -------------------------------------------------------------------------
    // Equality (exact, for tests — use with care on computed values)
    // -------------------------------------------------------------------------

    bool operator==(const Vec3d& rhs) const {
        return x_ == rhs.x_ && y_ == rhs.y_ && z_ == rhs.z_;
    }

    bool operator!=(const Vec3d& rhs) const { return !(*this == rhs); }

    // -------------------------------------------------------------------------
    // Data members
    // -------------------------------------------------------------------------

    double x_, y_, z_;
};

// -----------------------------------------------------------------------------
// Non-member scalar multiplication: s * v
// -----------------------------------------------------------------------------
inline Vec3d operator*(double s, const Vec3d& v) { return v * s; }

} // namespace tetgen