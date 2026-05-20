
#include <iostream>

#include <gtest/gtest.h>

#include "Vec3d.hh"
#include "TetMesh.hh"

using namespace tet_weave;

//from googletest primer
TEST(CompileTest, BasicTest) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}

// =============================================================================
// Helpers
// =============================================================================

static constexpr double epsilon = 1e-12;
static constexpr double M_PI_   = 3.14159265358979323846;

/// Expect two Vec3d values to be component-wise close.
static void ExpectNear(const Vec3d& a, const Vec3d& b, double tol = epsilon) {
    EXPECT_NEAR(a.x(), b.x(), tol);
    EXPECT_NEAR(a.y(), b.y(), tol);
    EXPECT_NEAR(a.z(), b.z(), tol);
}

// =============================================================================
// Construction
// =============================================================================

TEST(Vec3dTest, DefaultConstructorExists) {
    // Must compile; values are indeterminate — just verify the object exists.
    Vec3d v;
    (void)v;
}

TEST(Vec3dTest, ComponentConstructor) {
    Vec3d v(1.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(v.x(), 1.0);
    EXPECT_DOUBLE_EQ(v.y(), 2.0);
    EXPECT_DOUBLE_EQ(v.z(), 3.0);
}

TEST(Vec3dTest, BraceInitialization) {
    Vec3d v = {4.0, 5.0, 6.0};
    EXPECT_DOUBLE_EQ(v.x(), 4.0);
    EXPECT_DOUBLE_EQ(v.y(), 5.0);
    EXPECT_DOUBLE_EQ(v.z(), 6.0);
}

TEST(Vec3dTest, CopyConstructor) {
    Vec3d a(1.0, 2.0, 3.0);
    Vec3d b(a);
    ExpectNear(a, b);
}

TEST(Vec3dTest, CopyAssignment) {
    Vec3d a(1.0, 2.0, 3.0);
    Vec3d b;
    b = a;
    ExpectNear(a, b);
}

TEST(Vec3dTest, MoveConstructor) {
    Vec3d a(7.0, 8.0, 9.0);
    Vec3d b(std::move(a));
    EXPECT_DOUBLE_EQ(b.x(), 7.0);
    EXPECT_DOUBLE_EQ(b.y(), 8.0);
    EXPECT_DOUBLE_EQ(b.z(), 9.0);
}

// =============================================================================
// Accessors
// =============================================================================

TEST(Vec3dTest, ConstAccessors) {
    const Vec3d v(1.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(v.x(), 1.0);
    EXPECT_DOUBLE_EQ(v.y(), 2.0);
    EXPECT_DOUBLE_EQ(v.z(), 3.0);
}

TEST(Vec3dTest, MutableAccessors) {
    Vec3d v(0.0, 0.0, 0.0);
    v.x() = 10.0;
    v.y() = 20.0;
    v.z() = 30.0;
    EXPECT_DOUBLE_EQ(v.x(), 10.0);
    EXPECT_DOUBLE_EQ(v.y(), 20.0);
    EXPECT_DOUBLE_EQ(v.z(), 30.0);
}

// =============================================================================
// Subscript operator
// =============================================================================

TEST(Vec3dTest, SubscriptRead) {
    const Vec3d v(1.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(v[0], 1.0);
    EXPECT_DOUBLE_EQ(v[1], 2.0);
    EXPECT_DOUBLE_EQ(v[2], 3.0);
}

TEST(Vec3dTest, SubscriptWrite) {
    Vec3d v(0.0, 0.0, 0.0);
    v[0] = 5.0;
    v[1] = 6.0;
    v[2] = 7.0;
    EXPECT_DOUBLE_EQ(v.x(), 5.0);
    EXPECT_DOUBLE_EQ(v.y(), 6.0);
    EXPECT_DOUBLE_EQ(v.z(), 7.0);
}

TEST(Vec3dTest, SubscriptMapsToCorrectComponents) {
    // Verify that layout (&x_)[i] gives the expected addresses.
    Vec3d v(1.0, 2.0, 3.0);
    EXPECT_EQ(&v[0], &v.x());
    EXPECT_EQ(&v[1], &v.y());
    EXPECT_EQ(&v[2], &v.z());
}

// =============================================================================
// Equality
// =============================================================================

TEST(Vec3dTest, EqualityTrue) {
    EXPECT_EQ(Vec3d(1.0, 2.0, 3.0), Vec3d(1.0, 2.0, 3.0));
}

TEST(Vec3dTest, EqualityFalse) {
    EXPECT_NE(Vec3d(1.0, 2.0, 3.0), Vec3d(1.0, 2.0, 4.0));
}

// =============================================================================
// Addition / subtraction
// =============================================================================

TEST(Vec3dTest, Addition) {
    ExpectNear(Vec3d(1, 2, 3) + Vec3d(4, 5, 6), Vec3d(5, 7, 9));
}

TEST(Vec3dTest, AdditionInPlace) {
    Vec3d v(1, 2, 3);
    v += Vec3d(4, 5, 6);
    ExpectNear(v, Vec3d(5, 7, 9));
}

TEST(Vec3dTest, Subtraction) {
    ExpectNear(Vec3d(5, 7, 9) - Vec3d(4, 5, 6), Vec3d(1, 2, 3));
}

TEST(Vec3dTest, SubtractionInPlace) {
    Vec3d v(5, 7, 9);
    v -= Vec3d(4, 5, 6);
    ExpectNear(v, Vec3d(1, 2, 3));
}

TEST(Vec3dTest, UnaryNegation) {
    ExpectNear(-Vec3d(1, -2, 3), Vec3d(-1, 2, -3));
}

// =============================================================================
// Scalar multiplication
// =============================================================================

TEST(Vec3dTest, ScalarMultiplyRight) {
    ExpectNear(Vec3d(1, 2, 3) * 2.0, Vec3d(2, 4, 6));
}

TEST(Vec3dTest, ScalarMultiplyLeft) {
    ExpectNear(3.0 * Vec3d(1, 2, 3), Vec3d(3, 6, 9));
}

TEST(Vec3dTest, ScalarMultiplyInPlace) {
    Vec3d v(1, 2, 3);
    v *= 4.0;
    ExpectNear(v, Vec3d(4, 8, 12));
}

TEST(Vec3dTest, ScalarMultiplyZero) {
    ExpectNear(Vec3d(1, 2, 3) * 0.0, Vec3d(0, 0, 0));
}

// =============================================================================
// Dot product
// =============================================================================

TEST(Vec3dTest, DotProductOrthogonal) {
    EXPECT_DOUBLE_EQ(Vec3d(1, 0, 0).dot(Vec3d(0, 1, 0)), 0.0);
}

TEST(Vec3dTest, DotProductParallel) {
    EXPECT_DOUBLE_EQ(Vec3d(2, 0, 0).dot(Vec3d(3, 0, 0)), 6.0);
}

TEST(Vec3dTest, DotProductGeneral) {
    // (1,2,3)·(4,5,6) = 4+10+18 = 32
    EXPECT_DOUBLE_EQ(Vec3d(1, 2, 3).dot(Vec3d(4, 5, 6)), 32.0);
}

TEST(Vec3dTest, DotProductSymmetric) {
    Vec3d a(1, 2, 3), b(4, 5, 6);
    EXPECT_DOUBLE_EQ(a.dot(b), b.dot(a));
}

// =============================================================================
// Cross product
// =============================================================================

TEST(Vec3dTest, CrossProductBasisXY) {
    // x × y = z
    ExpectNear(Vec3d(1, 0, 0).cross(Vec3d(0, 1, 0)), Vec3d(0, 0, 1));
}

TEST(Vec3dTest, CrossProductBasisYX) {
    // y × x = -z
    ExpectNear(Vec3d(0, 1, 0).cross(Vec3d(1, 0, 0)), Vec3d(0, 0, -1));
}

TEST(Vec3dTest, CrossProductAnticommutative) {
    Vec3d a(1, 2, 3), b(4, 5, 6);
    ExpectNear(a.cross(b), -(b.cross(a)));
}

TEST(Vec3dTest, CrossProductParallelVectors) {
    // Parallel vectors → zero vector.
    ExpectNear(Vec3d(2, 0, 0).cross(Vec3d(5, 0, 0)), Vec3d(0, 0, 0));
}

TEST(Vec3dTest, CrossProductGeneral) {
    // (1,2,3) × (4,5,6) = (-3, 6, -3)
    ExpectNear(Vec3d(1, 2, 3).cross(Vec3d(4, 5, 6)), Vec3d(-3, 6, -3));
}

TEST(Vec3dTest, CrossProductOrthogonalToOperands) {
    Vec3d a(1, 2, 3), b(4, 5, 6);
    Vec3d c = a.cross(b);
    EXPECT_NEAR(c.dot(a), 0.0, epsilon);
    EXPECT_NEAR(c.dot(b), 0.0, epsilon);
}

// =============================================================================
// Norm
// =============================================================================

TEST(Vec3dTest, SquaredNorm) {
    EXPECT_DOUBLE_EQ(Vec3d(1, 2, 3).squaredNorm(), 14.0);
}

TEST(Vec3dTest, NormAxisAligned) {
    EXPECT_DOUBLE_EQ(Vec3d(3, 0, 0).norm(), 3.0);
}

TEST(Vec3dTest, NormGeneral) {
    EXPECT_NEAR(Vec3d(1, 2, 3).norm(), std::sqrt(14.0), epsilon);
}

TEST(Vec3dTest, NormUnitVector) {
    EXPECT_NEAR(Vec3d(1, 0, 0).norm(), 1.0, epsilon);
}

// =============================================================================
// Normalization
// =============================================================================

TEST(Vec3dTest, NormalizedReturnsUnitLength) {
    EXPECT_NEAR(Vec3d(3, 4, 0).normalized().norm(), 1.0, epsilon);
}

TEST(Vec3dTest, NormalizedDoesNotMutate) {
    Vec3d v(3, 4, 0);
    [[maybe_unused]] Vec3d u = v.normalized();
    EXPECT_DOUBLE_EQ(v.x(), 3.0);
    EXPECT_DOUBLE_EQ(v.y(), 4.0);
}

TEST(Vec3dTest, NormalizedDirection) {
    Vec3d v(2, 0, 0);
    ExpectNear(v.normalized(), Vec3d(1, 0, 0));
}

TEST(Vec3dTest, NormalizeInPlaceReturnsUnitLength) {
    Vec3d v(0, 5, 0);
    v.normalize();
    EXPECT_NEAR(v.norm(), 1.0, epsilon);
    ExpectNear(v, Vec3d(0, 1, 0));
}

TEST(Vec3dTest, NormalizeInPlaceReturnsSelf) {
    Vec3d v(1, 2, 3);
    Vec3d& ref = v.normalize();
    EXPECT_EQ(&ref, &v);
}

TEST(Vec3dTest, NormalizedAlreadyUnit) {
    Vec3d v(1, 0, 0);
    ExpectNear(v.normalized(), Vec3d(1, 0, 0));
}

// =============================================================================
// Rotation (Rodrigues' formula)
// =============================================================================

TEST(Vec3dTest, RotateZeroAngle) {
    // Zero rotation leaves the vector unchanged.
    Vec3d v(1, 2, 3);
    ExpectNear(v.rotate(Vec3d(0, 0, 1), 0.0), v);
}

TEST(Vec3dTest, RotateFullCircle) {
    // 2π rotation returns the original vector.
    Vec3d v(1, 2, 3);
    ExpectNear(v.rotate(Vec3d(0, 0, 1), 2.0 * M_PI_), v, 1e-10);
}

TEST(Vec3dTest, RotateXAxisBy90AroundZ) {
    // x rotated 90° around z → y
    ExpectNear(Vec3d(1, 0, 0).rotate(Vec3d(0, 0, 1), M_PI_ / 2.0), Vec3d(0, 1, 0), 1e-10);
}

TEST(Vec3dTest, RotateXAxisBy180AroundZ) {
    // x rotated 180° around z → -x
    ExpectNear(Vec3d(1, 0, 0).rotate(Vec3d(0, 0, 1), M_PI_), Vec3d(-1, 0, 0), 1e-10);
}

TEST(Vec3dTest, RotateYAxisBy90AroundX) {
    // y rotated 90° around x → z (right-hand rule)
    ExpectNear(Vec3d(0, 1, 0).rotate(Vec3d(1, 0, 0), M_PI_ / 2.0), Vec3d(0, 0, 1), 1e-10);
}

TEST(Vec3dTest, RotateAxisAlignedVectorAroundItself) {
    // Rotating a vector around its own axis leaves it unchanged.
    Vec3d v(0, 0, 5);
    ExpectNear(v.rotate(Vec3d(0, 0, 1), M_PI_ / 3.0), v, 1e-10);
}

TEST(Vec3dTest, RotatePreservesNorm) {
    // Rotation is isometric — length must be preserved.
    Vec3d v(1, 2, 3);
    EXPECT_NEAR(v.rotate(Vec3d(1, 1, 0), M_PI_ / 4.0).norm(), v.norm(), 1e-10);
}

TEST(Vec3dTest, RotateWithNonUnitAxis) {
    // Passing a non-unit axis should give the same result as the normalized one.
    Vec3d v(1, 0, 0);
    Vec3d axis(0, 0, 5);  // scaled z-axis
    ExpectNear(v.rotate(axis, M_PI_ / 2.0), Vec3d(0, 1, 0), 1e-10);
}

TEST(Vec3dTest, RotateDoesNotMutate) {
    Vec3d v(1, 2, 3);
    [[maybe_unused]] Vec3d r = v.rotate(Vec3d(0, 1, 0), M_PI_ / 4.0);
    ExpectNear(v, Vec3d(1, 2, 3));
}

// =============================================================================
// Debug-only assertion tests (compiled out when NDEBUG is defined)
// =============================================================================

#ifndef NDEBUG
TEST(Vec3dTest, SubscriptAssertNegativeIndex) {
    Vec3d v(1.0, 2.0, 3.0);
    EXPECT_DEATH(v[-1], "");
}

TEST(Vec3dTest, SubscriptAssertIndexTooLarge) {
    Vec3d v(1.0, 2.0, 3.0);
    EXPECT_DEATH(v[3], "");
}

TEST(Vec3dTest, NormalizedZeroVectorAsserts) {
    EXPECT_DEATH(Vec3d(0, 0, 0).normalized(), "");
}

TEST(Vec3dTest, NormalizeInPlaceZeroVectorAsserts) {
    Vec3d v(0, 0, 0);
    EXPECT_DEATH(v.normalize(), "");
}

TEST(Vec3dTest, RotateZeroAxisAsserts) {
    EXPECT_DEATH(Vec3d(1, 0, 0).rotate(Vec3d(0, 0, 0), M_PI_ / 2.0), "");
}
#endif


TEST(TetMeshTest, DeclareMesh){
	TetMesh mesh;
}



TEST(TetMeshTest, AddVertices){
    TetMesh mesh;
    //auto v0 = mesh.add_vertex(1,2,3);
}

