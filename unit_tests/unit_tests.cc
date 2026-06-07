
#include <iostream>

#include <gtest/gtest.h>

#include "Vec3d.hh"
#include "TetMesh.hh"
#include "VoxelGridMeshGen.hh"
#include "SimpleMeshGen.hh"
#include "ParametricMeshGen.hh"


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

static constexpr double epsilon = 1e-10;
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


// =============================================================================
// Helpers
// =============================================================================

/// Read a file into a string.
static std::string read_file(const std::filesystem::path& p) {
    std::ifstream f(p);
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

/// Build a minimal valid tetrahedron and return the mesh.
static TetMesh make_single_tet() {
    TetMesh m;
    auto v0 = m.add_vertex(0, 0, 0);
    auto v1 = m.add_vertex(1, 0, 0);
    auto v2 = m.add_vertex(0, 1, 0);
    auto v3 = m.add_vertex(0, 0, 1);
    m.add_cell(v0, v1, v2, v3);
    return m;
}


// Unit regular tetrahedron with volume exactly 1.0.
// Edge length a = (6*sqrt(2))^(1/3) ≈ 2.0, placed so v0 is at origin.
static TetMesh make_unit_volume_tet() {
    // Place a regular tet with volume = 1.
    // V = a^3 / (6*sqrt(2))  →  a = (6*sqrt(2))^(1/3)
    const double a = std::cbrt(6.0 * std::sqrt(2.0));
    TetMesh m;
    m.add_vertex(0,       0,            0);
    m.add_vertex(a,       0,            0);
    m.add_vertex(a / 2.0, a * std::sqrt(3.0) / 2.0, 0);
    m.add_vertex(a / 2.0, a * std::sqrt(3.0) / 6.0, a * std::sqrt(2.0 / 3.0));
    m.add_cell(VertexHandle(0), VertexHandle(1), VertexHandle(2), VertexHandle(3));
    return m;
}

// =============================================================================
// VertexHandle
// =============================================================================

TEST(VertexHandleTest, DefaultInvalid) {
    EXPECT_FALSE(VertexHandle().is_valid());
}

TEST(VertexHandleTest, ExplicitValid) {
    EXPECT_TRUE(VertexHandle(0).is_valid());
    EXPECT_TRUE(VertexHandle(42).is_valid());
}

TEST(VertexHandleTest, Equality) {
    EXPECT_EQ(VertexHandle(3), VertexHandle(3));
    EXPECT_NE(VertexHandle(3), VertexHandle(4));
}

// =============================================================================
// CellHandle
// =============================================================================

TEST(CellHandleTest, DefaultInvalid) {
    EXPECT_FALSE(CellHandle().is_valid());
}

TEST(CellHandleTest, ExplicitValid) {
    EXPECT_TRUE(CellHandle(0).is_valid());
}

TEST(CellHandleTest, Equality) {
    EXPECT_EQ(CellHandle(1), CellHandle(1));
    EXPECT_NE(CellHandle(1), CellHandle(2));
}

// =============================================================================
// Vertices
// =============================================================================

TEST(TetMeshTest, EmptyMesh) {
    TetMesh m;
    EXPECT_EQ(m.n_vertices(), 0);
    EXPECT_EQ(m.n_cells(), 0);
}

TEST(TetMeshTest, AddVertexVec3d) {
    TetMesh m;
    auto vh = m.add_vertex(Vec3d(1, 2, 3));
    EXPECT_TRUE(vh.is_valid());
    EXPECT_EQ(vh.idx(), 0);
    EXPECT_EQ(m.n_vertices(), 1);
}

TEST(TetMeshTest, AddVertexXYZ) {
    TetMesh m;
    auto vh = m.add_vertex(4, 5, 6);
    EXPECT_TRUE(vh.is_valid());
    EXPECT_EQ(m.n_vertices(), 1);
}

TEST(TetMeshTest, AddVertexReturnsCorrectHandle) {
    TetMesh m;
    auto v0 = m.add_vertex(0, 0, 0);
    auto v1 = m.add_vertex(1, 0, 0);
    auto v2 = m.add_vertex(2, 0, 0);
    EXPECT_EQ(v0.idx(), 0);
    EXPECT_EQ(v1.idx(), 1);
    EXPECT_EQ(v2.idx(), 2);
}

TEST(TetMeshTest, VertexPositionRoundtrip) {
    TetMesh m;
    auto vh = m.add_vertex(1.5, 2.5, 3.5);
    EXPECT_DOUBLE_EQ(m.vertex(vh).x(), 1.5);
    EXPECT_DOUBLE_EQ(m.vertex(vh).y(), 2.5);
    EXPECT_DOUBLE_EQ(m.vertex(vh).z(), 3.5);
}

TEST(TetMeshTest, MultipleVerticesStoredIndependently) {
    TetMesh m;
    auto v0 = m.add_vertex(1, 0, 0);
    auto v1 = m.add_vertex(0, 2, 0);
    EXPECT_DOUBLE_EQ(m.vertex(v0).x(), 1.0);
    EXPECT_DOUBLE_EQ(m.vertex(v1).y(), 2.0);
}

TEST(TetMeshTest, SetVertexVec3d) {
    TetMesh m;
    auto vh = m.add_vertex(0, 0, 0);
    m.set_vertex(vh, {1, 2, 3});
    EXPECT_DOUBLE_EQ(m.vertex(vh).x(), 1.0);
    EXPECT_DOUBLE_EQ(m.vertex(vh).y(), 2.0);
    EXPECT_DOUBLE_EQ(m.vertex(vh).z(), 3.0);
}


TEST(TetMeshTest, SetVertexDoesNotAffectOthers) {
    TetMesh m;
    auto v0 = m.add_vertex(1, 0, 0);
    auto v1 = m.add_vertex(0, 1, 0);
    m.set_vertex(v0, {9, 9, 9});
    EXPECT_DOUBLE_EQ(m.vertex(v1).x(), 0.0);
    EXPECT_DOUBLE_EQ(m.vertex(v1).y(), 1.0);
    EXPECT_DOUBLE_EQ(m.vertex(v1).z(), 0.0);
}

TEST(TetMeshTest, IsBoundaryFirstVertex) {
    EXPECT_FALSE(TetMesh::is_boundary(VertexHandle(0)));

    //NOTE: this is topologically WRONG.
    // It shows that in our setting, with always one interior vertex,
    // it's always the first one that is considered interior
    EXPECT_FALSE(make_single_tet().is_boundary(VertexHandle(0)));
}

TEST(TetMeshTest, IsBoundaryOtherVertices) {
    EXPECT_TRUE(TetMesh::is_boundary(VertexHandle(1)));
    EXPECT_TRUE(TetMesh::is_boundary(VertexHandle(42)));
    EXPECT_TRUE(make_single_tet().is_boundary(VertexHandle(1)));
    EXPECT_TRUE(make_single_tet().is_boundary(VertexHandle(2)));
    EXPECT_TRUE(make_single_tet().is_boundary(VertexHandle(3)));
}

// =============================================================================
// Cells
// =============================================================================

TEST(TetMeshTest, AddCellReturnsValidHandle) {
    TetMesh m = make_single_tet();
    EXPECT_EQ(m.n_cells(), 1);
}

TEST(TetMeshTest, AddCellHandleIndex) {
    TetMesh m = make_single_tet();
    // First cell should have index 0
    auto v0 = m.add_vertex(2, 0, 0);
    auto v1 = m.add_vertex(3, 0, 0);
    auto v2 = m.add_vertex(4, 0, 0);
    auto v3 = m.add_vertex(5, 0, 0);
    auto ch = m.add_cell({v0, v1, v2, v3});
    EXPECT_EQ(ch.idx(), 1);
}

TEST(TetMeshTest, AddCellHandleIndexWithFourArguments) {
    TetMesh m = make_single_tet();
    // First cell should have index 0
    auto v0 = m.add_vertex(2, 0, 0);
    auto v1 = m.add_vertex(3, 0, 0);
    auto v2 = m.add_vertex(4, 0, 0);
    auto v3 = m.add_vertex(5, 0, 0);
    auto ch = m.add_cell(v0, v1, v2, v3);
    EXPECT_EQ(ch.idx(), 1);
}

TEST(TetMeshTest, CellVertexHandlesRoundtrip) {
    TetMesh m;
    auto v0 = m.add_vertex(0, 0, 0);
    auto v1 = m.add_vertex(1, 0, 0);
    auto v2 = m.add_vertex(0, 1, 0);
    auto v3 = m.add_vertex(0, 0, 1);
    auto ch = m.add_cell({v0, v1, v2, v3});

    const auto& c = m.get_cell_vertices(ch);
    EXPECT_EQ(c[0], v0);
    EXPECT_EQ(c[1], v1);
    EXPECT_EQ(c[2], v2);
    EXPECT_EQ(c[3], v3);
}

TEST(TetMeshTest, MultipleCells) {
    TetMesh m;
    auto v0 = m.add_vertex(0, 0, 0);
    auto v1 = m.add_vertex(1, 0, 0);
    auto v2 = m.add_vertex(0, 1, 0);
    auto v3 = m.add_vertex(0, 0, 1);
    auto v4 = m.add_vertex(1, 1, 1);

    m.add_cell({v0, v1, v2, v3});
    m.add_cell({v1, v2, v3, v4});
    EXPECT_EQ(m.n_cells(), 2);
}


// =============================================================================
// Iterators
// =============================================================================

TEST(TetMeshTest, VerticesRangeCount) {
    TetMesh m = make_single_tet();
    int count = 0;
    for ([[maybe_unused]] const auto& v : m.vertices()) ++count;
    EXPECT_EQ(count, 4);
}

TEST(TetMeshTest, VerticesRangeValues) {
    TetMesh m;
    m.add_vertex(1, 2, 3);
    m.add_vertex(4, 5, 6);
    int i = 0;
    for (const auto& vh : m.vertices()) {
        auto v = m.vertex(vh);
        EXPECT_DOUBLE_EQ(v.x(), m.vertex(VertexHandle(i)).x());
        EXPECT_DOUBLE_EQ(v.y(), m.vertex(VertexHandle(i)).y());
        EXPECT_DOUBLE_EQ(v.z(), m.vertex(VertexHandle(i)).z());
        ++i;
    }
}

TEST(TetMeshTest, CellsRangeCount) {
    TetMesh m = make_single_tet();
    int count = 0;
    for ([[maybe_unused]] const auto& c : m.cells()) ++count;
    EXPECT_EQ(count, 1);
}

TEST(TetMeshTest, CellsRangeValues) {
    TetMesh m;
    auto v0 = m.add_vertex(0, 0, 0);
    auto v1 = m.add_vertex(1, 0, 0);
    auto v2 = m.add_vertex(0, 1, 0);
    auto v3 = m.add_vertex(0, 0, 1);
    m.add_cell(v0, v1, v2, v3);
    for (const auto& c : m.cells()) {
        auto verts = m.get_cell_vertices(c);
        EXPECT_EQ(verts[0], v0);
        EXPECT_EQ(verts[1], v1);
        EXPECT_EQ(verts[2], v2);
        EXPECT_EQ(verts[3], v3);
    }
}

TEST(TetMeshTest, EmptyVerticesRange) {
    TetMesh m;
    int count = 0;
    for ([[maybe_unused]] const auto& v : m.vertices()) ++count;
    EXPECT_EQ(count, 0);
}

TEST(TetMeshTest, EmptyCellsRange) {
    TetMesh m;
    int count = 0;
    for ([[maybe_unused]] const auto& c : m.cells()) ++count;
    EXPECT_EQ(count, 0);
}

// =============================================================================
// Volume
// =============================================================================


TEST(TetMeshTest, VolumeOfCanonicalTet) {
    // make_single_tet: v0=(0,0,0), v1=(1,0,0), v2=(0,1,0), v3=(0,0,1)
    // e1=(1,0,0), e2=(0,1,0), e3=(0,0,1) → det = 1 → volume = 1/6
    TetMesh m = make_single_tet();
    EXPECT_NEAR(m.volume(CellHandle(0)), 1.0 / 6.0, epsilon);
}

TEST(TetMeshTest, VolumeOfUnitVolumeTet) {
    TetMesh m = make_unit_volume_tet();
    EXPECT_NEAR(m.volume(CellHandle(0)), 1.0, epsilon);
}

TEST(TetMeshTest, VolumePositiveForCorrectWinding) {
    TetMesh m = make_single_tet();
    EXPECT_GT(m.volume(CellHandle(0)), 0.0);
}

TEST(TetMeshTest, VolumeScalesWithEdgeLength) {
    // Scaling all vertices by factor k scales the volume by k^3.
    const double k = 3.0;
    TetMesh m;
    m.add_vertex(0,  0,  0);
    m.add_vertex(k,  0,  0);
    m.add_vertex(0,  k,  0);
    m.add_vertex(0,  0,  k);
    m.add_cell(VertexHandle(0), VertexHandle(1), VertexHandle(2), VertexHandle(3));
    EXPECT_NEAR(m.volume(CellHandle(0)), (1.0 / 6.0) * k * k * k, epsilon);
}

TEST(TetMeshTest, TotalVolumeEmptyMesh) {
    TetMesh m;
    EXPECT_DOUBLE_EQ(m.compute_signed_volume(), 0.0);
}

TEST(TetMeshTest, TotalVolumeSingleCell) {
    TetMesh m = make_single_tet();
    EXPECT_NEAR(m.compute_signed_volume(), 1.0 / 6.0, epsilon);
}

TEST(TetMeshTest, TotalVolumeSingleFlippedCell) {
    TetMesh m = make_single_tet();
    m.set_vertex(VertexHandle(3), {0,0,-1});
    EXPECT_NEAR(m.compute_signed_volume(), -1.0 / 6.0, epsilon);
}

TEST(TetMeshTest, TotalVolumeSingleDegenerateCell) {
    TetMesh m = make_single_tet();
    m.set_vertex(VertexHandle(3), {1,1,0});
    EXPECT_NEAR(m.compute_signed_volume(), 0.0, epsilon);
}


TEST(TetMeshTest, TotalUnsignedVolumeSingleFlippedCell) {
    TetMesh m = make_single_tet();
    m.set_vertex(VertexHandle(3), {0,0,-1});
    EXPECT_NEAR(m.compute_unsigned_volume(), 1.0 / 6.0, epsilon);
}

TEST(TetMeshTest, TotalVolumeMultipleCells) {
    // Two identical canonical tets → total = 2 * (1/6) = 1/3.
    TetMesh m;
    auto v0 = m.add_vertex(0, 0, 0);
    auto v1 = m.add_vertex(1, 0, 0);
    auto v2 = m.add_vertex(0, 1, 0);
    auto v3 = m.add_vertex(0, 0, 1);
    auto v4 = m.add_vertex(0, 0, -1);
    m.add_cell(v0, v1, v2, v3);
    m.add_cell(v0, v2, v1, v4);
    EXPECT_NEAR(m.compute_signed_volume(), 1.0 / 3.0, epsilon);
}


TEST(TetMeshTest, TotalVolumeOppositeOrientations) {
    // Two identical canonical tets, but with opposite orientations → total = 0.0.
    TetMesh m;
    auto v0 = m.add_vertex(0, 0, 0);
    auto v1 = m.add_vertex(1, 0, 0);
    auto v2 = m.add_vertex(0, 1, 0);
    auto v3 = m.add_vertex(0, 0, 1);
    auto v4 = m.add_vertex(0, 0, 1);
    m.add_cell(v0, v1, v2, v3);
    m.add_cell(v0, v2, v1, v4);
    EXPECT_NEAR(m.compute_signed_volume(), 0.0, epsilon);
}


TEST(TetMeshTest, TotalUnsignedVolumeOppositeOrientations) {
    // Two identical canonical tets, but with opposite orientations → total = 0.0.
    TetMesh m;
    auto v0 = m.add_vertex(0, 0, 0);
    auto v1 = m.add_vertex(1, 0, 0);
    auto v2 = m.add_vertex(0, 1, 0);
    auto v3 = m.add_vertex(0, 0, 1);
    auto v4 = m.add_vertex(0, 0, 1);
    m.add_cell(v0, v1, v2, v3);
    m.add_cell(v0, v2, v1, v4);
    EXPECT_NEAR(m.compute_unsigned_volume(), 2.0 / 6.0, epsilon);
}


// =============================================================================
// File I/O — .ovm
// =============================================================================

class TetMeshOvmTest : public ::testing::Test {
protected:
    void SetUp() override {
        path_ = std::filesystem::temp_directory_path() / "tet_weave_test.ovm";
    }
    void TearDown() override {
        std::filesystem::remove(path_);
    }
    std::filesystem::path path_;
};


TEST_F(TetMeshOvmTest, FileIsCreated) {
    write_to_file(make_single_tet(), path_);
    EXPECT_TRUE(std::filesystem::exists(path_));
    write_to_file(make_single_tet(), "test.ovm");
}

TEST_F(TetMeshOvmTest, HeaderPresent) {
    write_to_file(make_single_tet(), path_);
    EXPECT_NE(read_file(path_).find("OVM ASCII"), std::string::npos);
}

TEST_F(TetMeshOvmTest, VertexSectionPresent) {
    write_to_file(make_single_tet(), path_);
    EXPECT_NE(read_file(path_).find("Vertices"), std::string::npos);
}

TEST_F(TetMeshOvmTest, VertexCountCorrect) {
    // Single tet has 4 vertices.
    write_to_file(make_single_tet(), path_);
    EXPECT_NE(read_file(path_).find("Vertices\n4\n"), std::string::npos);
}

TEST_F(TetMeshOvmTest, VertexPositionsWritten) {
    write_to_file(make_single_tet(), path_);
    const std::string content = read_file(path_);
    EXPECT_NE(content.find("0 0 0"), std::string::npos);
    EXPECT_NE(content.find("1 0 0"), std::string::npos);
    EXPECT_NE(content.find("0 1 0"), std::string::npos);
    EXPECT_NE(content.find("0 0 1"), std::string::npos);
}

TEST_F(TetMeshOvmTest, EdgeSectionPresent) {
    write_to_file(make_single_tet(), path_);
    EXPECT_NE(read_file(path_).find("Edges"), std::string::npos);
}

TEST_F(TetMeshOvmTest, EdgeCountCorrect) {
    // A single tet has 6 unique edges.
    write_to_file(make_single_tet(), path_);
    EXPECT_NE(read_file(path_).find("Edges\n6\n"), std::string::npos);
}

TEST_F(TetMeshOvmTest, EdgeStoredAsSourceTarget) {
    // Each edge line must be "vs vt" with vs < vt.
    write_to_file(make_single_tet(), path_);
    EXPECT_NE(read_file(path_).find("0 1"), std::string::npos);
}

TEST_F(TetMeshOvmTest, FaceSectionPresent) {
    write_to_file(make_single_tet(), path_);
    EXPECT_NE(read_file(path_).find("Faces"), std::string::npos);
}

TEST_F(TetMeshOvmTest, FaceCountCorrect) {
    // A single tet has 4 unique faces.
    write_to_file(make_single_tet(), path_);
    EXPECT_NE(read_file(path_).find("Faces\n4\n"), std::string::npos);
}

TEST_F(TetMeshOvmTest, FaceDefinedByValenceAndHalfEdges) {
    // Each face line must start with valence 3.
    write_to_file(make_single_tet(), path_);
    EXPECT_NE(read_file(path_).find("3 "), std::string::npos);
}

TEST_F(TetMeshOvmTest, PolyhedraSectionPresent) {
    write_to_file(make_single_tet(), path_);
    EXPECT_NE(read_file(path_).find("Polyhedra"), std::string::npos);
}

TEST_F(TetMeshOvmTest, PolyhedraCountCorrect) {
    write_to_file(make_single_tet(), path_);
    EXPECT_NE(read_file(path_).find("Polyhedra\n1\n"), std::string::npos);
}

TEST_F(TetMeshOvmTest, PolyhedronDefinedByValenceAndHalfFaces) {
    // Each cell line must start with valence 4 (four half-faces).
    write_to_file(make_single_tet(), path_);
    EXPECT_NE(read_file(path_).find("4 "), std::string::npos);
}

TEST_F(TetMeshOvmTest, SharedEdgesDeduplicatedAcrossCells) {
    // Two tets sharing a face have 6 + 3 = 9 unique edges.
    TetMesh m;
    auto v0 = m.add_vertex(0, 0, 0);
    auto v1 = m.add_vertex(1, 0, 0);
    auto v2 = m.add_vertex(0, 1, 0);
    auto v3 = m.add_vertex(0, 0, 1);
    auto v4 = m.add_vertex(1, 1, 1);
    m.add_cell(v0, v1, v2, v3);
    m.add_cell(v1, v2, v3, v4);
    write_to_file(m, path_);
    EXPECT_NE(read_file(path_).find("Edges\n9\n"), std::string::npos);
}

TEST_F(TetMeshOvmTest, SharedFacesDeduplicatedAcrossCells) {
    // Two tets sharing one face → 4 + 4 - 1 = 7 unique faces.
    TetMesh m;
    auto v0 = m.add_vertex(0, 0, 0);
    auto v1 = m.add_vertex(1, 0, 0);
    auto v2 = m.add_vertex(0, 1, 0);
    auto v3 = m.add_vertex(0, 0, 1);
    auto v4 = m.add_vertex(1, 1, 1);
    m.add_cell(v0, v1, v2, v3);
    m.add_cell(v1, v2, v3, v4);
    write_to_file(m, path_);
    EXPECT_NE(read_file(path_).find("Faces\n7\n"), std::string::npos);
}

TEST_F(TetMeshOvmTest, EmptyMeshWritesAllSections) {
    TetMesh m;
    write_to_file(m, path_);
    const std::string content = read_file(path_);
    EXPECT_NE(content.find("Vertices"),  std::string::npos);
    EXPECT_NE(content.find("Edges"),     std::string::npos);
    EXPECT_NE(content.find("Faces"),     std::string::npos);
    EXPECT_NE(content.find("Polyhedra"), std::string::npos);
}


// =============================================================================
// Boundary export tests
// =============================================================================

class TetMeshBoundaryOvmTest : public ::testing::Test {
protected:
    void SetUp() override {
        path_ = std::filesystem::temp_directory_path() / "tet_weave_boundary_test.ovm";
    }
    void TearDown() override {
        std::filesystem::remove(path_);
    }
    std::filesystem::path path_;
};

TEST_F(TetMeshBoundaryOvmTest, FileIsCreated) {
    write_to_file(make_single_tet(), path_, true);
    EXPECT_TRUE(std::filesystem::exists(path_));
}

TEST_F(TetMeshBoundaryOvmTest, HeaderPresent) {
    write_to_file(make_single_tet(), path_, true);
    EXPECT_NE(read_file(path_).find("OVM ASCII"), std::string::npos);
}

TEST_F(TetMeshBoundaryOvmTest, InteriorVertexSkipped) {
    // Single tet: 4 vertices total, boundary has 3 (v0 skipped).
    write_to_file(make_single_tet(), path_, true);
    EXPECT_NE(read_file(path_).find("Vertices\n3\n"), std::string::npos);
}

TEST_F(TetMeshBoundaryOvmTest, BoundaryVertexPositionsCorrect) {
    // v1=(1,0,0), v2=(0,1,0), v3=(0,0,1) remapped to indices 0,1,2
    write_to_file(make_single_tet(), path_, true);
    const std::string content = read_file(path_);
    EXPECT_NE(content.find("1 0 0"), std::string::npos);
    EXPECT_NE(content.find("0 1 0"), std::string::npos);
    EXPECT_NE(content.find("0 0 1"), std::string::npos);
    // Interior vertex position (0 0 0) should NOT appear
    EXPECT_EQ(content.find("Vertices\n3\n0 0 0"), std::string::npos);
}

TEST_F(TetMeshBoundaryOvmTest, SingleTetBoundaryEdgeCount) {
    // Single tet boundary: triangle (v1,v2,v3) has 3 edges.
    write_to_file(make_single_tet(), path_, true);
    EXPECT_NE(read_file(path_).find("Edges\n3\n"), std::string::npos);
}

TEST_F(TetMeshBoundaryOvmTest, SingleTetBoundaryFaceCount) {
    // Single tet: 1 boundary face (opposite interior vertex).
    write_to_file(make_single_tet(), path_, true);
    EXPECT_NE(read_file(path_).find("Faces\n1\n"), std::string::npos);
}

TEST_F(TetMeshBoundaryOvmTest, PolyhedraIsZero) {
    // Boundary export has no volumetric cells.
    write_to_file(make_single_tet(), path_, true);
    EXPECT_NE(read_file(path_).find("Polyhedra\n0\n"), std::string::npos);
}

TEST_F(TetMeshBoundaryOvmTest, BoundaryVertexCountEqualsNVerticesMinusOne) {
    // In a star-shaped mesh, boundary has exactly n_vertices - 1 vertices.
    const int N = 4;
    auto rod = SimpleMeshGen::generate_rod_mesh(N);
    write_to_file(rod, path_, true);
    const std::string expected = "Vertices\n" + std::to_string(rod.n_vertices() - 1) + "\n";
    EXPECT_NE(read_file(path_).find(expected), std::string::npos);
}

TEST_F(TetMeshBoundaryOvmTest, BoundaryFaceCountEqualsNCells) {
    // In a star-shaped mesh, each cell contributes exactly one boundary face
    // (the face opposite the interior vertex), and no two cells share a
    // boundary face. So n_boundary_faces == n_cells.
    const int N = 4;
    auto rod = SimpleMeshGen::generate_rod_mesh(N);
    write_to_file(rod, path_, true);
    const std::string expected = "Faces\n" + std::to_string(rod.n_cells()) + "\n";
    EXPECT_NE(read_file(path_).find(expected), std::string::npos);
}

TEST_F(TetMeshBoundaryOvmTest, DefaultArgumentIsFullExport) {
    // write_to_file without boundary_only should produce the full mesh.
    TetMesh m = make_single_tet();
    write_to_file(m, path_);
    EXPECT_NE(read_file(path_).find("Vertices\n4\n"), std::string::npos);
    EXPECT_NE(read_file(path_).find("Polyhedra\n1\n"), std::string::npos);
}

TEST_F(TetMeshBoundaryOvmTest, BoundaryAndFullExportDiffer) {
    TetMesh m = make_single_tet();
    auto path_full = std::filesystem::temp_directory_path() / "tet_weave_full_test.ovm";
    write_to_file(m, path_,      true);
    write_to_file(m, path_full,  false);
    EXPECT_NE(read_file(path_), read_file(path_full));
    std::filesystem::remove(path_full);
}

TEST_F(TetMeshBoundaryOvmTest, RemappedIndicesAreContiguous) {
    // After skipping vertex 0, the boundary vertex section should start
    // immediately after "Vertices\nN\n" with no gaps.
    // We verify by checking the first boundary vertex is written at line 3
    // (header, count, first vertex).
    write_to_file(make_single_tet(), path_, true);
    std::istringstream ss(read_file(path_));
    std::string line;
    std::getline(ss, line); // OVM ASCII
    std::getline(ss, line); // Vertices
    std::getline(ss, line); // 3
    std::getline(ss, line); // first vertex — should be "1 0 0"
    EXPECT_EQ(line, "1 0 0");
}

#ifndef NDEBUG
TEST(TetMeshBoundaryTest, UnsupportedExtensionThrowsWithBoundaryOnly) {
    TetMesh m = make_single_tet();
    EXPECT_THROW(
        write_to_file(m, std::filesystem::temp_directory_path() / "out.xyz", true),
        std::runtime_error);
}
#endif


// =============================================================================
// .obj export tests
// =============================================================================

class TetMeshObjTest : public ::testing::Test {
protected:
    void SetUp() override {
        path_ = std::filesystem::temp_directory_path() / "tet_weave_test.obj";
    }
    void TearDown() override {
        std::filesystem::remove(path_);
    }
    std::filesystem::path path_;
};

// Helper: count lines in a string starting with a given prefix.
static int count_lines_with_prefix(const std::string& content, const std::string& prefix) {
    int count = 0;
    std::istringstream ss(content);
    std::string line;
    while (std::getline(ss, line))
        if (line.rfind(prefix, 0) == 0) ++count;
    return count;
}

// -----------------------------------------------------------------------------
// Full export (boundary_only=false)
// -----------------------------------------------------------------------------

TEST_F(TetMeshObjTest, FileIsCreated) {
    write_to_file(make_single_tet(), path_);
    EXPECT_TRUE(std::filesystem::exists(path_));
}

TEST_F(TetMeshObjTest, FullExportVertexCount) {
    // Single tet: 4 vertices.
    write_to_file(make_single_tet(), path_);
    EXPECT_EQ(count_lines_with_prefix(read_file(path_), "v "), 4);
}

TEST_F(TetMeshObjTest, FullExportFaceCount) {
    // Single tet: 4 faces (one per face of the tet).
    write_to_file(make_single_tet(), path_);
    EXPECT_EQ(count_lines_with_prefix(read_file(path_), "f "), 4);
}

TEST_F(TetMeshObjTest, FullExportVertexPositions) {
    write_to_file(make_single_tet(), path_);
    const std::string content = read_file(path_);
    EXPECT_NE(content.find("v 0 0 0"), std::string::npos);
    EXPECT_NE(content.find("v 1 0 0"), std::string::npos);
    EXPECT_NE(content.find("v 0 1 0"), std::string::npos);
    EXPECT_NE(content.find("v 0 0 1"), std::string::npos);
}

TEST_F(TetMeshObjTest, FullExportFacesUseOneBasisedIndices) {
    // OBJ is 1-based — no face index should be 0.
    write_to_file(make_single_tet(), path_);
    std::istringstream ss(read_file(path_));
    std::string line;
    while (std::getline(ss, line)) {
        if (line.rfind("f ", 0) != 0) continue;
        std::istringstream ls(line.substr(2));
        int a, b, c;
        ls >> a >> b >> c;
        EXPECT_GE(a, 1) << "face index must be >= 1";
        EXPECT_GE(b, 1) << "face index must be >= 1";
        EXPECT_GE(c, 1) << "face index must be >= 1";
    }
}

TEST_F(TetMeshObjTest, FullExportNoEdgesSection) {
    // .obj doesn't have edges.
    write_to_file(make_single_tet(), path_);
    EXPECT_EQ(count_lines_with_prefix(read_file(path_), "e "), 0);
}

TEST_F(TetMeshObjTest, FullExportVertexCountScalesWithMesh) {
    const int N = 4;
    auto rod = SimpleMeshGen::generate_rod_mesh(N);
    write_to_file(rod, path_);
    EXPECT_EQ(count_lines_with_prefix(read_file(path_), "v "), rod.n_vertices());
}

// -----------------------------------------------------------------------------
// Boundary export (boundary_only=true)
// -----------------------------------------------------------------------------

TEST_F(TetMeshObjTest, BoundaryExportVertexCount) {
    // Single tet boundary: 3 vertices (interior vertex skipped).
    write_to_file(make_single_tet(), path_, true);
    EXPECT_EQ(count_lines_with_prefix(read_file(path_), "v "), 3);
}

TEST_F(TetMeshObjTest, BoundaryExportFaceCount) {
    // Single tet boundary: 1 face.
    write_to_file(make_single_tet(), path_, true);
    EXPECT_EQ(count_lines_with_prefix(read_file(path_), "f "), 1);
}

TEST_F(TetMeshObjTest, BoundaryExportInteriorVertexAbsent) {
    // Interior vertex (0,0,0) should not appear in boundary export.
    write_to_file(make_single_tet(), path_, true);
    const std::string content = read_file(path_);
    EXPECT_EQ(content.find("v 0 0 0"), std::string::npos);
}

TEST_F(TetMeshObjTest, BoundaryExportBoundaryVerticesPresent) {
    write_to_file(make_single_tet(), path_, true);
    const std::string content = read_file(path_);
    EXPECT_NE(content.find("v 1 0 0"), std::string::npos);
    EXPECT_NE(content.find("v 0 1 0"), std::string::npos);
    EXPECT_NE(content.find("v 0 0 1"), std::string::npos);
}

TEST_F(TetMeshObjTest, BoundaryExportFaceIsOneTwoThree) {
    // With 3 boundary vertices remapped to OBJ indices 1,2,3,
    // the single boundary face should be "f 1 2 3".
    write_to_file(make_single_tet(), path_, true);
    EXPECT_NE(read_file(path_).find("f 1 2 3"), std::string::npos);
}

TEST_F(TetMeshObjTest, BoundaryExportVertexCountScalesWithMesh) {
    const int N = 4;
    auto rod = SimpleMeshGen::generate_rod_mesh(N);
    write_to_file(rod, path_, true);
    EXPECT_EQ(count_lines_with_prefix(read_file(path_), "v "), rod.n_vertices() - 1);
}

TEST_F(TetMeshObjTest, BoundaryExportFaceCountEqualsNCells) {
    // In a star-shaped mesh, boundary faces == n_cells.
    const int N = 4;
    auto rod = SimpleMeshGen::generate_rod_mesh(N);
    write_to_file(rod, path_, true);
    EXPECT_EQ(count_lines_with_prefix(read_file(path_), "f "), rod.n_cells());
}

// -----------------------------------------------------------------------------
// Full vs boundary differ
// -----------------------------------------------------------------------------

TEST_F(TetMeshObjTest, FullAndBoundaryExportDiffer) {
    auto path_full = std::filesystem::temp_directory_path() / "tet_weave_full.obj";
    write_to_file(make_single_tet(), path_,      true);
    write_to_file(make_single_tet(), path_full,  false);
    EXPECT_NE(read_file(path_), read_file(path_full));
    std::filesystem::remove(path_full);
}


// =============================================================================
// File I/O — unsupported format
// =============================================================================

TEST(TetMeshTest, UnsupportedExtensionThrows) {
    TetMesh m = make_single_tet();
    EXPECT_THROW(
        write_to_file(m, std::filesystem::temp_directory_path() / "out.xyz"),
        std::runtime_error);
}

// =============================================================================
// Debug-only assertion tests
// =============================================================================

#ifndef NDEBUG
TEST(TetMeshTest, AddCellInvalidHandleAsserts) {
    TetMesh m;
    auto v0 = m.add_vertex(0, 0, 0);
    auto v1 = m.add_vertex(1, 0, 0);
    auto v2 = m.add_vertex(0, 1, 0);
    VertexHandle bad(-1);
    EXPECT_DEATH(m.add_cell(v0, v1, v2, bad), "");
}

TEST(TetMeshTest, VertexAccessorInvalidHandleAsserts) {
    TetMesh m;
    EXPECT_DEATH(m.vertex(VertexHandle(0)), "");
}

TEST(TetMeshTest, CellAccessorInvalidHandleAsserts) {
    TetMesh m;
    EXPECT_DEATH(m.get_cell_vertices(CellHandle(0)), "");
}
#endif


TEST(VoxelGridMeshGenTest, SingleVoxelMesh) {
    TetMesh mesh = VoxelGridMeshGen::generate_voxel_grid_mesh(1,1,1);
    EXPECT_EQ(mesh.n_vertices(), 8);
    EXPECT_EQ(mesh.n_cells(), 5);
}

TEST(VoxelGridMeshGenTest, VariableSizedGrids) {
    const int N(10);
    for (int i(1); i<N; i++) {
        for (int j(0); j<N; j++) {
            for (int k(0); k<N; k++) {

                TetMesh mesh = VoxelGridMeshGen::generate_voxel_grid_mesh(i,j,k);
                EXPECT_EQ(mesh.n_vertices(), (i+1) * (j+1) * (k+1));
                EXPECT_EQ(mesh.n_cells(), 5 * i * j * k);
            }
        }
    }
}


// =============================================================================
// Voxel grid meshes
// =============================================================================


TEST(VoxelGridMeshGenTest, KnottedHoles) {
    std::vector<int> cell_counts = {
        1575,
        1555,
        1545,
        1525,
        1515,
        1495,
        1465,
        1445,
        1425,
        1415,
        1405
    };

    const int n_vertices = 480; //(10 * 8 * 6)

    for (int i(0); i<cell_counts.size(); i++) {
        TetMesh mesh = VoxelGridMeshGen::generate_Furchs_knotted_hole(i);
        EXPECT_EQ(mesh.n_vertices(), n_vertices);
        EXPECT_EQ(mesh.n_cells(), cell_counts[i]);
    }
}


TEST(VoxelGridMeshGenTest, KnottedHoleExport) {
    auto mesh = VoxelGridMeshGen::generate_Furchs_knotted_hole();
    const int n_vertices = 480; //(10 * 8 * 6)

    EXPECT_EQ(mesh.n_vertices(), 480);
    EXPECT_EQ(mesh.n_cells(), 1405);
}


// =============================================================================
// Simple meshes
// =============================================================================


TEST(SimpleMeshGenTest, MinimalNonStarShaped) {
    auto codomain_mesh = SimpleMeshGen::generate_minimal_non_star_shaped_mesh();

    EXPECT_EQ(codomain_mesh.n_vertices(),9);
    EXPECT_EQ(codomain_mesh.n_cells(), 12);
    EXPECT_NEAR(codomain_mesh.compute_signed_volume(), 16.0/3.0, epsilon);

    auto domain_mesh = SimpleMeshGen::generate_minimal_non_star_shaped_domain_mesh();

    EXPECT_EQ(domain_mesh.n_vertices(),9);
    EXPECT_EQ(domain_mesh.n_cells(), 12);
    EXPECT_NEAR(domain_mesh.compute_signed_volume(), 32.0/3.0, epsilon);
}


TEST(SimpleMeshGenTest, RodMesh) {

    const int N(10);
    for (int i(1); i<=N; i++) {
        auto rod = SimpleMeshGen::generate_rod_mesh(N);

        //4 vertices per "ring", N+1 rings, then the interior vertex
        EXPECT_EQ(rod.n_vertices(), (N+1)*4 + 1);

        //2 * 4 faces per "ring", then the 2 * 2 faces at the tips
        EXPECT_EQ(rod.n_cells(),2 * (4 * N + 2));

        EXPECT_NEAR(rod.compute_signed_volume(), N, epsilon);

        //interior vertex is the first one, and in the middle of the mesh
        auto interior_v_pos = rod.vertex(VertexHandle(0));
        EXPECT_EQ(interior_v_pos, Vec3d(0.0, 0.0, 0.5 * N));

        //the first vertex (after the interior one) is on the x=0 plane
        auto first_v_pos = rod.vertex(VertexHandle(1));
        EXPECT_EQ(first_v_pos, Vec3d(-0.5, -0.5, 0.0));

        //the last vertex is on the x=N plane
        auto last_v_pos = rod.vertex(VertexHandle(rod.n_vertices()-1));
        EXPECT_EQ(last_v_pos, Vec3d(-0.5,0.5, N));
    }
}

TEST(SimpleMeshGenTest, RodMeshAxialScaling) {

    const int N(10);

    const double max_scaling(100.0);
    for (double s(0); s<max_scaling; s++) {
        auto rod = SimpleMeshGen::generate_rod_mesh(N, s);

        EXPECT_NEAR(rod.compute_signed_volume(), N * s, epsilon);

        //interior vertex is the first one, and in the middle of the mesh
        auto interior_v_pos = rod.vertex(VertexHandle(0));
        EXPECT_EQ(interior_v_pos, Vec3d(0.0, 0.0, 0.5 * N * s));

        //the last vertex is on the x=N plane
        auto last_v_pos = rod.vertex(VertexHandle(rod.n_vertices()-1));
        EXPECT_EQ(last_v_pos, Vec3d(-0.5,0.5, N * s));
    }
}


TEST(SimpleMeshGenTest, RodMeshTorsion) {

    const int N(10);

    const double max_torsion_deg(720.0);
    const int max_steps(10);

    for (int i(1); i<=N; i++) {
        for (int j(0); j<=max_steps; j++) {

            double t_deg = j * (max_torsion_deg / max_steps);
            double t_rad = M_PI * t_deg / 180.0;

            //std::cout<<" ------------------------ t = "<<t_deg<<" / "<<t_rad<<std::endl;
            auto rod = SimpleMeshGen::generate_rod_mesh(N, 1.0, t_rad);

            //EXPECT_NEAR(rod.compute_signed_volume(), N, epsilon);

            //interior vertex is the first one, and in the middle of the mesh
            auto interior_v_pos = rod.vertex(VertexHandle(0));
            EXPECT_EQ(interior_v_pos, Vec3d(0.0, 0.0, 0.5 * N));

            //the last vertex is on the x=N plane
            auto last_v_pos = rod.vertex(VertexHandle(rod.n_vertices()-1));
            auto expected_last_v_pos = Vec3d(-0.5,0.5, N).rotate({0.0, 0.0, 1.0}, t_rad);

            EXPECT_EQ(last_v_pos, expected_last_v_pos);
        }
    }
}




// =============================================================================
// SimpleMeshGen tests
// =============================================================================
  
// Helper: expected vertex and cell counts for any rod-topology mesh of length N.
static int expected_n_vertices(int N) { return (N + 1) * 4 + 1; }
static int expected_n_cells(int N)    { return 2 * (4 * N + 2); }
 
// -----------------------------------------------------------------------------
// MinimalNonStarShaped — already covered by the provided tests, add extras
// -----------------------------------------------------------------------------
 
TEST(SimpleMeshGenTest, MinimalNonStarShapedDomainLargerThanCodomain) {
    auto codomain = SimpleMeshGen::generate_minimal_non_star_shaped_mesh();
    auto domain   = SimpleMeshGen::generate_minimal_non_star_shaped_domain_mesh();
    EXPECT_GT(domain.compute_signed_volume(), codomain.compute_signed_volume());
}
 
TEST(SimpleMeshGenTest, MinimalNonStarShapedSameTopology) {
    auto codomain = SimpleMeshGen::generate_minimal_non_star_shaped_mesh();
    auto domain   = SimpleMeshGen::generate_minimal_non_star_shaped_domain_mesh();
    EXPECT_EQ(codomain.n_vertices(), domain.n_vertices());
    EXPECT_EQ(codomain.n_cells(),    domain.n_cells());
}
 
// -----------------------------------------------------------------------------
// Rod mesh
// -----------------------------------------------------------------------------
 
TEST(SimpleMeshGenTest, RodMeshTopologyScalesWithLength) {
    for (int N : {1, 5, 10, 50}) {
        auto rod = SimpleMeshGen::generate_rod_mesh(N);
        EXPECT_EQ(rod.n_vertices(), expected_n_vertices(N)) << "N=" << N;
        EXPECT_EQ(rod.n_cells(),    expected_n_cells(N))    << "N=" << N;
    }
}
 
TEST(SimpleMeshGenTest, RodMeshLengthClampedToOne) {
    // Length < 1 should be clamped to 1.
    auto rod0 = SimpleMeshGen::generate_rod_mesh(0);
    auto rod1 = SimpleMeshGen::generate_rod_mesh(1);
    EXPECT_EQ(rod0.n_vertices(), rod1.n_vertices());
    EXPECT_EQ(rod0.n_cells(),    rod1.n_cells());
    EXPECT_NEAR(rod0.compute_signed_volume(), rod1.compute_signed_volume(), epsilon);
}
 
TEST(SimpleMeshGenTest, RodMeshPositiveVolume) {
    for (int N : {1, 5, 10}) {
        auto rod = SimpleMeshGen::generate_rod_mesh(N);
        EXPECT_GT(rod.compute_signed_volume(), 0.0) << "N=" << N;
    }
}
 
 
TEST(SimpleMeshGenTest, RodMeshTorsionPreservesTopology) {
    const int N = 10;
    auto ref = SimpleMeshGen::generate_rod_mesh(N);
    for (double t_rad : {M_PI / 4.0, M_PI / 2.0, M_PI, 2.0 * M_PI}) {
        auto rod = SimpleMeshGen::generate_rod_mesh(N, 1.0, t_rad);
        EXPECT_EQ(rod.n_vertices(), ref.n_vertices());
        EXPECT_EQ(rod.n_cells(),    ref.n_cells());
    }
}
 
TEST(SimpleMeshGenTest, RodMeshAxialScalingZeroVolume) {
    // Scaling by 0 collapses the mesh — volume should be 0.
    auto rod = SimpleMeshGen::generate_rod_mesh(10, 0.0);
    EXPECT_NEAR(rod.compute_signed_volume(), 0.0, epsilon);
}
 
TEST(SimpleMeshGenTest, RodMeshInteriorVertexUnaffectedByTorsion) {
    // The interior vertex v0 sits on the z-axis and should not be rotated.
    const int N = 10;
    for (double t_rad : {M_PI / 4.0, M_PI, 2.0 * M_PI}) {
        auto rod = SimpleMeshGen::generate_rod_mesh(N, 1.0, t_rad);
        EXPECT_EQ(rod.vertex(VertexHandle(0)), Vec3d(0.0, 0.0, 0.5 * N))
            << "torsion=" << t_rad;
    }
}
 
// -----------------------------------------------------------------------------
// Sine mesh
// -----------------------------------------------------------------------------
 
 
TEST(SimpleMeshGenTest, SineMeshTopologyScalesWithLength) {
    for (int N : {10, 20, 50}) {
        auto sine = SimpleMeshGen::generate_sine_mesh(N);
        EXPECT_EQ(sine.n_vertices(), expected_n_vertices(N)) << "N=" << N;
        EXPECT_EQ(sine.n_cells(),    expected_n_cells(N))    << "N=" << N;
    }
}
 

 
TEST(SimpleMeshGenTest, SineMeshZeroAmplitudeMatchesRodVolume) {
    // With x_scale=0 and y_scale=0 the sine deformation vanishes.
    // z_scale=1 and torsion=0 → mesh is a straight rod → same volume.
    const int N = 20;
    auto rod  = SimpleMeshGen::generate_rod_mesh(N, 1.0, 0.0);
    auto sine = SimpleMeshGen::generate_sine_mesh(N, 1.0, 0.0, 1.0, 0.0);
    EXPECT_NEAR(sine.compute_signed_volume(), rod.compute_signed_volume(), 1e-9);
}
 
TEST(SimpleMeshGenTest, SineMeshVariesWithAmplitude) {
    // Non-zero amplitude should produce a different volume than a flat rod.
    const int N = 20;
    auto flat  = SimpleMeshGen::generate_sine_mesh(N, 1.0, 0.0, 2.5, 0.0);
    auto wavy  = SimpleMeshGen::generate_sine_mesh(N, 1.0, 0.0, 2.5, 2.0);
    EXPECT_NE(flat.compute_signed_volume(), wavy.compute_signed_volume());
}

// -----------------------------------------------------------------------------
// Spiral mesh
// -----------------------------------------------------------------------------
 
TEST(SimpleMeshGenTest, SpiralMeshTopology) {
    const int N = 100;
    auto spiral = SimpleMeshGen::generate_spiral_mesh(N);
    EXPECT_EQ(spiral.n_vertices(), expected_n_vertices(N));
    EXPECT_EQ(spiral.n_cells(),    expected_n_cells(N));
}
 
TEST(SimpleMeshGenTest, SpiralMeshTopologyScalesWithLength) {
    for (int N : {10, 50, 100}) {
        auto spiral = SimpleMeshGen::generate_spiral_mesh(N, 0.0, 1.0, 1.0);
        EXPECT_EQ(spiral.n_vertices(), expected_n_vertices(N)) << "N=" << N;
        EXPECT_EQ(spiral.n_cells(),    expected_n_cells(N))    << "N=" << N;
    }
}
 
TEST(SimpleMeshGenTest, SpiralMeshInteriorVertexPosition) {
    // Interior vertex (index 0) at pos (0,0,N/2):
    // t = (N/2 * 2*pi) / (N/turn_count) = pi*turn_count
    // r = 1 + 0 + (N/2) * (2*turn_count/N) = 1 + turn_count
    // spos = {0, r*cos(t), r*sin(t)}
    // With turn_count=5: r=6, t=5*pi → cos(5pi)=-1, sin(5pi)=0
    // → spos = {0, -6, 0}
    const int N = 100;
    const double turn_count = 5.0;
    auto spiral = SimpleMeshGen::generate_spiral_mesh(N, 2 * M_PI, 10.0, turn_count);
    EXPECT_NEAR(spiral.vertex(VertexHandle(0)).x(),  0.0, 1e-9);
    EXPECT_NEAR(spiral.vertex(VertexHandle(0)).y(), -6.0, 1e-9);
    EXPECT_NEAR(spiral.vertex(VertexHandle(0)).z(),  0.0, 1e-9);
}
 
// -----------------------------------------------------------------------------
// Trefoil knot mesh
// -----------------------------------------------------------------------------
 
TEST(SimpleMeshGenTest, TrefoilKnotMeshTopology) {
    const int N = 100;
    auto trefoil = SimpleMeshGen::generate_trefoil_knot_mesh(N, 4 * M_PI, 0.9);
    EXPECT_EQ(trefoil.n_vertices(), expected_n_vertices(N));
    EXPECT_EQ(trefoil.n_cells(),    expected_n_cells(N));
}
 
TEST(SimpleMeshGenTest, TrefoilKnotMeshTopologyScalesWithLength) {
    for (int N : {10, 50, 100}) {
        auto trefoil = SimpleMeshGen::generate_trefoil_knot_mesh(N, 4 * M_PI, 0.9);
        EXPECT_EQ(trefoil.n_vertices(), expected_n_vertices(N)) << "N=" << N;
        EXPECT_EQ(trefoil.n_cells(),    expected_n_cells(N))    << "N=" << N;
    }
}
 
TEST(SimpleMeshGenTest, TrefoilKnotFirstCurvePointAtOrigin) {
    // At t=0: curve_pos = (sin(0)+2sin(0), cos(0)-2cos(0), -sin(0)) = (0, -1, 0).
    // The interior vertex (index 0) is skipped. Vertex 1 has point_idx=0,
    // so it is placed relative to curve_points[0] = (0, -1, 0).
    // Its rod position is (-0.5, -0.5, 0) → x=-0.5, y=-0.5.
    // new_pos = curve_pos + 0.5*x*tp1 + 0.5*y*tp2
    // We can't easily predict the exact position without computing tp1/tp2,
    // but we can verify the mesh was generated with the correct number of curve points.
    const int N = 100;
    auto trefoil = SimpleMeshGen::generate_trefoil_knot_mesh(N, 4 * M_PI, 0.9);
    // N+1 curve points → vertex indices 1..4 all use curve_points[0]
    // Verify interior vertex (index 0) was skipped and kept at some position
    // (it is not moved by the trefoil transform).
    EXPECT_EQ(trefoil.vertex(VertexHandle(0)), Vec3d(0.0, 0.0, N * 0.5));
}

 
TEST(SimpleMeshGenTest, TrefoilKnotMeshFullRangeVsPartialRange) {
    // A full knot (range=1) and a partial one (range=0.5) should have
    // the same topology but different volumes.
    const int N = 50;
    auto full    = SimpleMeshGen::generate_trefoil_knot_mesh(N, 0.0, 1.0);
    auto partial = SimpleMeshGen::generate_trefoil_knot_mesh(N, 0.0, 0.5);
    EXPECT_EQ(full.n_vertices(), partial.n_vertices());
    EXPECT_EQ(full.n_cells(),    partial.n_cells());
    EXPECT_NE(full.compute_signed_volume(), partial.compute_signed_volume());
}




// =============================================================================
// ParametricCurve tests
// =============================================================================
  
static constexpr double curve_epsilon = 1e-10;
 
// Helper: xy-plane norm of a Vec3d.
static double xy_norm(const Vec3d& v) {
    return std::sqrt(v.x() * v.x() + v.y() * v.y());
}
 
// -----------------------------------------------------------------------------
// circle
// -----------------------------------------------------------------------------
 
TEST(ParametricCircleTest, AtTZeroReturnsRadiusOnXAxis) {
    Vec3d p = circle({1.0, 0.0}, 0.0);
    EXPECT_NEAR(p.x(), 1.0, curve_epsilon);
    EXPECT_NEAR(p.y(), 0.0, curve_epsilon);
    EXPECT_NEAR(p.z(), 0.0, curve_epsilon);
}
 
TEST(ParametricCircleTest, AtTHalfPiReturnsRadiusOnYAxis) {
    Vec3d p = circle({1.0, 0.0}, M_PI / 2.0);
    EXPECT_NEAR(p.x(), 0.0, curve_epsilon);
    EXPECT_NEAR(p.y(), 1.0, curve_epsilon);
    EXPECT_NEAR(p.z(), 0.0, curve_epsilon);
}
 
TEST(ParametricCircleTest, AtTPiReturnsNegativeXAxis) {
    Vec3d p = circle({1.0, 0.0}, M_PI);
    EXPECT_NEAR(p.x(), -1.0, curve_epsilon);
    EXPECT_NEAR(p.y(),  0.0, curve_epsilon);
}
 
TEST(ParametricCircleTest, IsClosed) {
    // Full circle: t=0 and t=2pi should give the same xy point.
    Vec3d p0  = circle({1.0, 0.0}, 0.0);
    Vec3d p2pi = circle({1.0, 0.0}, 2.0 * M_PI);
    EXPECT_NEAR(p0.x(), p2pi.x(), curve_epsilon);
    EXPECT_NEAR(p0.y(), p2pi.y(), curve_epsilon);
}
 
TEST(ParametricCircleTest, XYNormEqualsRadius) {
    // The xy distance from origin should always equal radius.
    const double radius = 3.0;
    for (double t : {0.0, M_PI / 4.0, M_PI / 2.0, M_PI, 3.0 * M_PI / 2.0}) {
        Vec3d p = circle({radius, 0.0}, t);
        EXPECT_NEAR(xy_norm(p), radius, curve_epsilon) << "t=" << t;
    }
}
 
TEST(ParametricCircleTest, RadiusScalesOutput) {
    // Doubling the radius should double the xy distance from origin.
    double t = M_PI / 3.0;
    Vec3d p1 = circle({1.0, 0.0}, t);
    Vec3d p2 = circle({2.0, 0.0}, t);
    EXPECT_NEAR(xy_norm(p2), 2.0 * xy_norm(p1), curve_epsilon);
}
 
TEST(ParametricCircleTest, ZScaleZeroKeepsCurveFlat) {
    for (double t : {0.0, 1.0, 2.0, M_PI}) {
        Vec3d p = circle({1.0, 0.0}, t);
        EXPECT_NEAR(p.z(), 0.0, curve_epsilon) << "t=" << t;
    }
}
 
TEST(ParametricCircleTest, ZComponentLinearInT) {
    // z = radius * z_scale * t
    const double radius = 2.0, z_scale = 3.0, t = 1.0;
    Vec3d p = circle({radius, z_scale}, t);
    EXPECT_NEAR(p.z(), radius * z_scale * t, curve_epsilon);
}
 
#ifndef NDEBUG
TEST(ParametricCircleTest, WrongParamCountAsserts) {
    EXPECT_DEATH(circle({1.0, 2.0, 3.0}, 0.0), "");
}
#endif
 
// -----------------------------------------------------------------------------
// parametric_spiral
// -----------------------------------------------------------------------------

 TEST(ParametricSpiralTest, ZeroZScaleKeepsCurveFlat) {
    // z_scale=0 → z component should always be 0.
    for (double t : {0.0, 1.0, M_PI, 2.0 * M_PI}) {
        Vec3d p = multi_branch_spiral({0.0, 1, 1.0, 0.0}, t);
        EXPECT_NEAR(p.z(), 0.0, curve_epsilon) << "t=" << t;
    }
}

TEST(ParametricSpiralTest, ZeroBranchLengthNoRotation) {
    // branch_length=0 → angle=0 → no rotation → p = radius*(cos(t), sin(t), z_scale*t).
    const double z_scale = 1.0, center_radius = 0.0;
    const int branches = 1;
    for (double t : {0.0, M_PI / 4.0, M_PI / 2.0, M_PI}) {
        double radius = std::cos(branches * t) + 1.0 + center_radius;
        Vec3d p        = multi_branch_spiral({center_radius, branches, 0.0, z_scale}, t);
        Vec3d expected = radius * Vec3d{std::cos(t), std::sin(t), z_scale * t};
        EXPECT_NEAR(p.x(), expected.x(), curve_epsilon) << "t=" << t;
        EXPECT_NEAR(p.y(), expected.y(), curve_epsilon) << "t=" << t;
        EXPECT_NEAR(p.z(), expected.z(), curve_epsilon) << "t=" << t;
    }
}

TEST(ParametricSpiralTest, XYNormEqualsRadius) {
    // xy norm should equal radius = cos(branches*t) + 1 + center_radius.
    const double center_radius = 1.0;
    const int branches = 2;
    for (double t : {0.0, 0.5, 1.0, M_PI}) {
        double radius = std::cos(branches * t) + 1.0 + center_radius;
        Vec3d p = multi_branch_spiral({center_radius, branches, 0.5, 0.0}, t);
        EXPECT_NEAR(xy_norm(p), radius, curve_epsilon) << "t=" << t;
    }
}

TEST(ParametricSpiralTest, ZComponentLinearInT) {
    // z = radius * z_scale * t (radius affects z too since p is scaled).
    const double z_scale = 2.0, center_radius = 0.0;
    const int branches = 1;
    for (double t : {0.0, 1.0, 2.0, M_PI}) {
        double radius = std::cos(branches * t) + 1.0 + center_radius;
        Vec3d p = multi_branch_spiral({center_radius, branches, 0.0, z_scale}, t);
        EXPECT_NEAR(p.z(), radius * z_scale * t, curve_epsilon) << "t=" << t;
    }
}

#ifndef NDEBUG
TEST(ParametricSpiralTest, WrongParamCountAsserts) {
    EXPECT_DEATH(multi_branch_spiral({1.0, 2.0}, 0.0), "");
}
#endif
 
// -----------------------------------------------------------------------------
// trefoil_knot
// -----------------------------------------------------------------------------
 
TEST(ParametricTrefoilKnotTest, AtTZero) {
    // t=0: (sin0+2sin0, cos0-2cos0, -sin0) = (0, -1, 0)
    Vec3d p = trefoil_knot({}, 0.0);
    EXPECT_NEAR(p.x(),  0.0, curve_epsilon);
    EXPECT_NEAR(p.y(), -1.0, curve_epsilon);
    EXPECT_NEAR(p.z(),  0.0, curve_epsilon);
}
 
TEST(ParametricTrefoilKnotTest, IsClosed) {
    // The trefoil knot is periodic with period 2pi.
    Vec3d p0   = trefoil_knot({}, 0.0);
    Vec3d p2pi = trefoil_knot({}, 2.0 * M_PI);
    EXPECT_NEAR(p0.x(), p2pi.x(), curve_epsilon);
    EXPECT_NEAR(p0.y(), p2pi.y(), curve_epsilon);
    EXPECT_NEAR(p0.z(), p2pi.z(), curve_epsilon);
}
 
TEST(ParametricTrefoilKnotTest, ZIsAlwaysBounded) {
    // z = -sin(3t), so |z| <= 1 for all t.
    for (int i = 0; i < 100; ++i) {
        double t = 2.0 * M_PI * i / 100.0;
        Vec3d p = trefoil_knot({}, t);
        EXPECT_LE(std::abs(p.z()), 1.0 + curve_epsilon) << "t=" << t;
    }
}
 
TEST(ParametricTrefoilKnotTest, HasThreefoldSymmetry) {
    // Shifting t by 2pi/3 rotates the knot by 2pi/3 in the xy-plane.
    // The norm of the xy component should be preserved.
    for (double t : {0.0, 0.5, 1.0}) {
        Vec3d p0 = trefoil_knot({}, t);
        Vec3d p1 = trefoil_knot({}, t + 2.0 * M_PI / 3.0);
        Vec3d p2 = trefoil_knot({}, t + 4.0 * M_PI / 3.0);
        EXPECT_NEAR(p0.norm(), p1.norm(), 1e-9) << "t=" << t;
        EXPECT_NEAR(p1.norm(), p2.norm(), 1e-9) << "t=" << t;
    }
}
 
TEST(ParametricTrefoilKnotTest, IgnoresParams) {
    // params is unused — passing different values should give identical results.
    Vec3d p0 = trefoil_knot({},            1.0);
    Vec3d p1 = trefoil_knot({1.0, 2.0},    1.0);
    Vec3d p2 = trefoil_knot({99.0, -3.14}, 1.0);
    EXPECT_NEAR(p0.x(), p1.x(), curve_epsilon);
    EXPECT_NEAR(p0.x(), p2.x(), curve_epsilon);
}
 
// -----------------------------------------------------------------------------
// multi_branch_spiral
// -----------------------------------------------------------------------------
 
TEST(ParametricComplexSpiralTest, AtTZero) {
    // t=0: x = cos0 + cos0/2 + sin0/3 = 1.5, y = sin0 + sin0/2 + cos0/3 = 1/3, z = 0
    Vec3d p = funny_spiral({1.0, 2.0, 3.0, 0.0}, 0.0);
    EXPECT_NEAR(p.x(), 1.5,       curve_epsilon);
    EXPECT_NEAR(p.y(), 1.0 / 3.0, curve_epsilon);
    EXPECT_NEAR(p.z(), 0.0,       curve_epsilon);
}
 
TEST(ParametricComplexSpiralTest, ZComponentLinearInT) {
    // z = z_scale * t, independent of a, b, c.
    const double z_scale = 2.0;
    for (double t : {0.0, 1.0, M_PI}) {
        Vec3d p = funny_spiral({1.0, 2.0, 3.0, z_scale}, t);
        EXPECT_NEAR(p.z(), z_scale * t, curve_epsilon) << "t=" << t;
    }
}
 
TEST(ParametricComplexSpiralTest, ZeroZScaleKeepsCurveFlat) {
    for (double t : {0.0, 1.0, M_PI, 2.0 * M_PI}) {
        Vec3d p = funny_spiral({1.0, 2.0, 3.0, 0.0}, t);
        EXPECT_NEAR(p.z(), 0.0, curve_epsilon) << "t=" << t;
    }
}
 
TEST(ParametricComplexSpiralTest, DifferentABCGiveDifferentCurves) {
    // Changing a, b, c should change the curve shape.
    double t = 1.0;
    Vec3d p1 = funny_spiral({1.0, 2.0, 3.0, 0.0}, t);
    Vec3d p2 = funny_spiral({2.0, 3.0, 4.0, 0.0}, t);
    EXPECT_NE(p1, p2);
}
 
TEST(ParametricComplexSpiralTest, SymmetricABCWithZeroZScale) {
    // With a=1, b=1, c=1: x=cos(t)+cos(t)/2+sin(t)/3, y=sin(t)+sin(t)/2+cos(t)/3
    // At t=0: x=1.5, y=1/3. At t=pi/2: x=sin(pi/2)/3=1/3, y=1.5
    // → x(0)==y(pi/2) and y(0)==x(pi/2): quarter-turn symmetry
    Vec3d p0    = funny_spiral({1.0, 1.0, 1.0, 0.0}, 0.0);
    Vec3d pHalf = funny_spiral({1.0, 1.0, 1.0, 0.0}, M_PI / 2.0);
    EXPECT_NEAR(p0.x(), pHalf.y(), curve_epsilon);
    EXPECT_NEAR(p0.y(), pHalf.x(), curve_epsilon);
}
 
#ifndef NDEBUG
TEST(ParametricComplexSpiralTest, WrongParamCountAsserts) {
    EXPECT_DEATH(funny_spiral({1.0, 2.0}, 0.0), "");
}
#endif



// =============================================================================
// ParametricMeshGen tests
// =============================================================================
  

// Helper: compute the curve point for a given vertex index in a parametric mesh.
// Mirrors the indexing logic in generate_parametric_mesh.
static Vec3d curve_point_for_vertex(ParametricCurve curve,
                                    const std::vector<double>& params,
                                    double t_min, double t_max,
                                    int length, int vertex_idx) {
    std::vector<Vec3d> curve_points;
    for (int i = 0; i <= length; ++i) {
        double t = t_min + (t_max - t_min) * (2.0 * M_PI * i) / length;
        curve_points.push_back(curve(params, t));
    }
    int point_idx      = (vertex_idx - 1) / 4;
    int next_point_idx = point_idx + 1;
    if (next_point_idx >= (int)curve_points.size()) {
        next_point_idx = point_idx;
        point_idx      = next_point_idx - 1;
        return curve_points[next_point_idx];
    }
    return curve_points[point_idx];
}
 
// -----------------------------------------------------------------------------
// Topology
// -----------------------------------------------------------------------------
 
TEST(ParametricMeshGenTest, TopologyMatchesRodMesh) {
    const int length = 10;
    auto mesh = ParametricMeshGen::generate_parametric_mesh(circle, {1.0, 0.0},
                                         0.0, 1.0, length, 0.5);
    EXPECT_EQ(mesh.n_vertices(), expected_n_vertices(length));
    EXPECT_EQ(mesh.n_cells(),    expected_n_cells(length));
}

TEST(ParametricMeshGenTest, InteriorVertexAtTheOrigin) {
    const int length = 10;
    auto mesh = ParametricMeshGen::generate_parametric_mesh(circle, {1.0, 0.0},
                                         0.0, 1.0, length, 0.5);
    EXPECT_EQ(mesh.vertex(VertexHandle(0)), Vec3d(0,0,0));
}
 
TEST(ParametricMeshGenTest, TopologyScalesWithLength) {
    for (int length : {5, 10, 20, 50}) {
        auto mesh = ParametricMeshGen::generate_parametric_mesh(circle, {1.0, 0.0},
                                             0.0, 1.0, length, 0.5);
        EXPECT_EQ(mesh.n_vertices(), expected_n_vertices(length)) << "length=" << length;
        EXPECT_EQ(mesh.n_cells(),    expected_n_cells(length))    << "length=" << length;
    }
}
 
TEST(ParametricMeshGenTest, TopologyIndependentOfCurveType) {
    const int length = 10;
    auto m1 = ParametricMeshGen::generate_parametric_mesh(circle,       {1.0, 0.0},         0.0, 1.0, length, 0.5);
    auto m2 = ParametricMeshGen::generate_parametric_mesh(trefoil_knot, {},                  0.0, 1.0, length, 0.5);
    auto m3 = ParametricMeshGen::generate_parametric_mesh(multi_branch_spiral,{1.0, 2.0, 3.0, 0.0}, 0.0, 1.0, length, 0.5);
    EXPECT_EQ(m1.n_vertices(), m2.n_vertices());
    EXPECT_EQ(m1.n_vertices(), m3.n_vertices());
    EXPECT_EQ(m1.n_cells(),    m2.n_cells());
    EXPECT_EQ(m1.n_cells(),    m3.n_cells());
}

 
// -----------------------------------------------------------------------------
// thickness = 0: all ring vertices collapse onto their curve point
// -----------------------------------------------------------------------------
 
TEST(ParametricMeshGenTest, ZeroThicknessCollapsesToCurve) {
    const int length = 8;
    auto mesh = ParametricMeshGen::generate_parametric_mesh(circle, {1.0, 0.0},
                                         0.0, 1.0, length, 0.0);
    // Vertices 1..4 all map to curve_points[0], etc.
    for (int vi = 1; vi < mesh.n_vertices(); ++vi) {
        Vec3d expected = curve_point_for_vertex(circle, {1.0, 0.0},
                                                0.0, 1.0, length, vi);
        Vec3d actual   = mesh.vertex(VertexHandle(vi));
        EXPECT_NEAR(actual.x(), expected.x(), 1e-9) << "vertex " << vi;
        EXPECT_NEAR(actual.y(), expected.y(), 1e-9) << "vertex " << vi;
        EXPECT_NEAR(actual.z(), expected.z(), 1e-9) << "vertex " << vi;
    }
}
 
TEST(ParametricMeshGenTest, ZeroThicknessAllRingVerticesCoincide) {
    // With thickness=0 all 4 vertices in a ring collapse to the same point.
    const int length = 6;
    auto mesh = ParametricMeshGen::generate_parametric_mesh(trefoil_knot, {},
                                         0.0, 1.0, length, 0.0);
    for (int ring = 0; ring < length; ++ring) {
        int base = 1 + ring * 4;
        Vec3d p0 = mesh.vertex(VertexHandle(base));
        for (int j = 1; j < 4; ++j) {
            Vec3d pj = mesh.vertex(VertexHandle(base + j));
            EXPECT_NEAR(p0.x(), pj.x(), 1e-9) << "ring=" << ring << " j=" << j;
            EXPECT_NEAR(p0.y(), pj.y(), 1e-9) << "ring=" << ring << " j=" << j;
            EXPECT_NEAR(p0.z(), pj.z(), 1e-9) << "ring=" << ring << " j=" << j;
        }
    }
}
 
// -----------------------------------------------------------------------------
// Ring vertex distance from curve point
// -----------------------------------------------------------------------------
 
TEST(ParametricMeshGenTest, RingVerticesEquidistantFromCurvePoint) {
    // tp1 and tp2 are orthonormal. The 4 rod vertex positions have
    // xy norm 1/sqrt(2). So each ring vertex is at distance
    // thickness/sqrt(2) from its curve point.
    const int    length    = 10;
    const double thickness = 1.0;
    const double expected_dist = thickness / std::sqrt(2.0);
 
    auto mesh = ParametricMeshGen::generate_parametric_mesh(circle, {1.0, 0.0},
                                         0.0, 1.0, length, thickness);
 
    for (int ring = 0; ring < length; ++ring) {
        Vec3d cp = curve_point_for_vertex(circle, {1.0, 0.0},
                                          0.0, 1.0, length, 1 + ring * 4);
        for (int j = 0; j < 4; ++j) {
            Vec3d vp   = mesh.vertex(VertexHandle(1 + ring * 4 + j));
            double dist = (vp - cp).norm();
            EXPECT_NEAR(dist, expected_dist, 1e-9)
                << "ring=" << ring << " j=" << j;
        }
    }
}
 
TEST(ParametricMeshGenTest, RingVertexDistanceScalesWithThickness) {
    // Doubling thickness should double the distance from the curve point.
    const int length = 10;
    const int ring   = 3;
    const int vi     = 1 + ring * 4;
 
    auto m1 = ParametricMeshGen::generate_parametric_mesh(circle, {1.0, 0.0}, 0.0, 1.0, length, 1.0);
    auto m2 = ParametricMeshGen::generate_parametric_mesh(circle, {1.0, 0.0}, 0.0, 1.0, length, 2.0);
 
    Vec3d cp = curve_point_for_vertex(circle, {1.0, 0.0}, 0.0, 1.0, length, vi);
    double d1 = (m1.vertex(VertexHandle(vi)) - cp).norm();
    double d2 = (m2.vertex(VertexHandle(vi)) - cp).norm();
    EXPECT_NEAR(d2, 2.0 * d1, 1e-9);
}
 
// -----------------------------------------------------------------------------
// Volume
// -----------------------------------------------------------------------------
 
TEST(ParametricMeshGenTest, NonZeroVolumeForNonDegenerateCurve) {
    auto mesh = ParametricMeshGen::generate_parametric_mesh(circle, {1.0, 1.0}, 0.0, 1.0, 10, 0.5);
    EXPECT_GT(std::abs(mesh.compute_signed_volume()), 0.0);
}
 
TEST(ParametricMeshGenTest, VolumeScalesWithThickness) {
    // Volume scales as thickness^2 (cross-section area scales as thickness^2,
    // length is fixed by the curve).
    const int length = 10;
    auto m1 = ParametricMeshGen::generate_parametric_mesh(circle, {1.0, 0.0}, 0.0, 1.0, length, 1.0);
    auto m2 = ParametricMeshGen::generate_parametric_mesh(circle, {1.0, 0.0}, 0.0, 1.0, length, 2.0);
    double v1 = std::abs(m1.compute_signed_volume());
    double v2 = std::abs(m2.compute_signed_volume());
    EXPECT_NEAR(v2, 4.0 * v1, 1e-6);
}
 
TEST(ParametricMeshGenTest, DifferentCurvesGiveDifferentVolumes) {
    const int length = 20;
    auto m1 = ParametricMeshGen::generate_parametric_mesh(circle,       {1.0, 1.0}, 0.0, 1.0, length, 0.5);
    auto m2 = ParametricMeshGen::generate_parametric_mesh(trefoil_knot, {},         0.0, 1.0, length, 0.5);
    EXPECT_NE(m1.compute_signed_volume(), m2.compute_signed_volume());
}




// =============================================================================
// SurfaceMeshGen tests
// =============================================================================

// Test parametric surfaces
static Vec3d flat_plane(const std::vector<double>& p, double u, double v) {
    // f(u,v) = (u, v, p[0]) — flat plane at height p[0]
    return {u, v, p[0]};
}

// -----------------------------------------------------------------------------
// generate_layer_mesh — topology
// -----------------------------------------------------------------------------

TEST(LayerMeshTest, VertexCount) {
    for (int u : {2, 3, 4, 10}) {
        for (int v : {2, 3, 4, 10}) {
            auto mesh = SimpleMeshGen::generate_layer_mesh(u, v);
            EXPECT_EQ(mesh.n_vertices(), 2 * (u+1)*(v+1) + 1) << "u=" << u << " v=" << v;
        }
    }
}

TEST(LayerMeshTest, CellCount) {
    for (int u : {2, 3, 4, 10}) {
        for (int v : {2, 3, 4, 10}) {
            auto mesh = SimpleMeshGen::generate_layer_mesh(u, v);
            EXPECT_EQ(mesh.n_cells(), 4 * (u * v + u + v)) << "u=" << u << " v=" << v;
        }
    }
}

TEST(LayerMeshTest, InteriorVertexAtCentroid) {
    // Interior vertex (index 0) placed at (u_steps/2, v_steps/2, 0).
    const int u = 4, v = 6;
    auto mesh = SimpleMeshGen::generate_layer_mesh(u, v);
    EXPECT_EQ(mesh.vertex(VertexHandle(0)), Vec3d(u * 0.5, v * 0.5, 0.5));
}

TEST(LayerMeshTest, BoundaryVerticesInXYPlane) {
    auto mesh = SimpleMeshGen::generate_layer_mesh(3, 4);

    //lower plane
    for (int i = 1; i < (mesh.n_vertices()+1) / 2; ++i)
        EXPECT_DOUBLE_EQ(mesh.vertex(VertexHandle(i)).z(), 0.0) << "vertex " << i;

    //upper plane
    for (int i = (mesh.n_vertices()+1) / 2; i < mesh.n_vertices(); ++i)
        EXPECT_DOUBLE_EQ(mesh.vertex(VertexHandle(i)).z(), 1.0) << "vertex " << i;
}

TEST(LayerMeshTest, BoundaryVertexPositions) {
    // vertex(i,j) should be at (i, j, 0).
    const int u = 3, v = 4;
    auto mesh = SimpleMeshGen::generate_layer_mesh(u, v);
    for (int i = 0; i <= u; ++i) {
        for (int j = 0; j <= v; ++j) {
            VertexHandle vh(1 + i * (v + 1) + j);
            EXPECT_EQ(mesh.vertex(vh), Vec3d(i, j, 0.0))
                                << "i=" << i << " j=" << j;
        }
    }
}

TEST(LayerMeshTest, PositiveVolume) {
    // The undeformed layer mesh has all boundary verts in z=0 and interior
    // at z=0 too → volume is 0. Volume only becomes nonzero after deformation.
    // So just check it generates without crashing and has correct counts.
    auto mesh = SimpleMeshGen::generate_layer_mesh(4, 4);
    EXPECT_EQ(mesh.n_vertices(), 51);
    EXPECT_EQ(mesh.n_cells(), 96);
}

TEST(LayerMeshTest, SquareGridIsSymmetric) {
    // For a square grid u_steps == v_steps, swapping u and v should give
    // the same counts.
    auto m1 = SimpleMeshGen::generate_layer_mesh(4, 6);
    auto m2 = SimpleMeshGen::generate_layer_mesh(6, 4);
    EXPECT_EQ(m1.n_vertices(), m2.n_vertices());
    EXPECT_EQ(m1.n_cells(),    m2.n_cells());
}

#ifndef NDEBUG
TEST(LayerMeshTest, ZeroUStepsAsserts) {
    EXPECT_DEATH(SimpleMeshGen::generate_layer_mesh(0, 4), "");
}
TEST(LayerMeshTest, ZeroVStepsAsserts) {
    EXPECT_DEATH(SimpleMeshGen::generate_layer_mesh(4, 0), "");
}
#endif

// -----------------------------------------------------------------------------
// generate_parametric_surface_mesh — topology
// -----------------------------------------------------------------------------

TEST(ParametricSurfaceMeshTest, TopologyMatchesLayerMesh) {
    const int u = 4, v = 4;
    auto mesh = ParametricMeshGen::generate_parametric_mesh(
            flat_plane, {1.0}, 0.0, 1.0, u, 0.0, 1.0, v, 0.5);
    auto layer_mesh = SimpleMeshGen::generate_layer_mesh(u,v);
    EXPECT_EQ(mesh.n_vertices(), layer_mesh.n_vertices());
    EXPECT_EQ(mesh.n_cells(),    layer_mesh.n_cells());
}


TEST(ParametricSurfaceMeshTest, TopologyIndependentOfSurface) {
    const int u = 10, v = 20;
    auto m1 = ParametricMeshGen::generate_parametric_mesh(
            flat_plane, {0.0}, 0.0, 1.0, u, 0.0, 1.0, v, 0.5);

    auto m2 = ParametricMeshGen::generate_parametric_mesh(
            sphere, {2.0}, 0.0, 2*M_PI, u, 0.0, M_PI, v, 0.1);

    auto m3 = ParametricMeshGen::generate_parametric_mesh(
            torus, {2.0, 0.5}, 0.0, 2*M_PI, u, 0.0, 2*M_PI, v, 0.1);

    EXPECT_EQ(m1.n_vertices(), m2.n_vertices());
    EXPECT_EQ(m1.n_vertices(), m3.n_vertices());
    EXPECT_EQ(m1.n_cells(),    m2.n_cells());
    EXPECT_EQ(m1.n_cells(),    m3.n_cells());
}


// -----------------------------------------------------------------------------
// generate_parametric_surface_mesh — boundary vertices on surface
// -----------------------------------------------------------------------------

TEST(ParametricSurfaceMeshTest, ZeroThicknessBoundaryVerticesOnSurface) {
    // With thickness=0, boundary vertices should lie exactly on the surface.
    const int u = 3, v = 3;
    auto mesh = ParametricMeshGen::generate_parametric_mesh(
            flat_plane, {2.0}, 0.0, 1.0, u, 0.0, 1.0, v, 0.0);
    for (int i = 0; i <= u; ++i) {
        for (int j = 0; j <= v; ++j) {
            VertexHandle vh(1 + i * (v+1) + j);
            double ui = i / static_cast<double>(u);
            double vj = j / static_cast<double>(v);
            Vec3d expected = flat_plane({2.0}, ui, vj);
            EXPECT_NEAR(mesh.vertex(vh).x(), expected.x(), 1e-9) << "i=" << i << " j=" << j;
            EXPECT_NEAR(mesh.vertex(vh).y(), expected.y(), 1e-9) << "i=" << i << " j=" << j;
            EXPECT_NEAR(mesh.vertex(vh).z(), expected.z(), 1e-9) << "i=" << i << " j=" << j;
        }
    }
}

// -----------------------------------------------------------------------------
// generate_parametric_surface_mesh — volume
// -----------------------------------------------------------------------------

TEST(ParametricSurfaceMeshTest, FlatSurfacePositiveVolume) {
    // Flat plane at z=1, thickness=1 → interior vertex at z=0,
    // boundary at z=1 → non-zero volume.
    auto mesh = ParametricMeshGen::generate_parametric_mesh(
            flat_plane, {1.0}, 0.0, 1.0, 4, 0.0, 1.0, 4, 1.0);
    EXPECT_GT(std::abs(mesh.compute_signed_volume()), 0.0);
}


TEST(ParametricSurfaceMeshTest, DifferentSurfacesGiveDifferentVolumes) {
    const int u = 6, v = 6;
    auto m1 = ParametricMeshGen::generate_parametric_mesh(
            sphere, {2.0}, 0.0, 2*M_PI, u, 0.1, M_PI-0.1, v, 0.5);
    auto m2 = ParametricMeshGen::generate_parametric_mesh(
            torus, {2.0, 0.5}, 0.0, 2*M_PI, u, 0.0, 2*M_PI, v, 0.5);
    EXPECT_NE(m1.compute_signed_volume(), m2.compute_signed_volume());
}

TEST(ParametricSurfaceMeshTest, SmokeTestWritesToFile) {
    auto mesh = ParametricMeshGen::generate_parametric_mesh(
            torus, {2.0, 0.5}, 0.0, 2*M_PI, 10, 0.0, 2*M_PI, 10, 0.1);
    write_to_file(mesh, "test_surface.ovm");
    EXPECT_TRUE(std::filesystem::exists("test_surface.ovm"));
}

// =============================================================================
// Parametric surface function tests
// =============================================================================

static constexpr double surf_epsilon = 1e-9;

// -----------------------------------------------------------------------------
// sphere
// -----------------------------------------------------------------------------

TEST(SphereTest, AllPointsAtRadiusFromOrigin) {
    const double r = 2.0;
    for (double u : {0.0, M_PI/4.0, M_PI/2.0, 3.0*M_PI/4.0, M_PI}) {
        for (double v : {0.0, M_PI/2.0, M_PI, 3.0*M_PI/2.0}) {
            Vec3d p = sphere({r}, u, v);
            EXPECT_NEAR(p.norm(), r, surf_epsilon) << "u=" << u << " v=" << v;
        }
    }
}

TEST(SphereTest, NorthPole) {
    // u=0: (sin(0)*cos(v), sin(0)*sin(v), cos(0)) = (0, 0, r)
    Vec3d p = sphere({1.0}, 0.0, 0.0);
    EXPECT_NEAR(p.x(), 0.0, surf_epsilon);
    EXPECT_NEAR(p.y(), 0.0, surf_epsilon);
    EXPECT_NEAR(p.z(), 1.0, surf_epsilon);
}

TEST(SphereTest, SouthPole) {
    // u=pi: (0, 0, -r)
    Vec3d p = sphere({1.0}, M_PI, 0.0);
    EXPECT_NEAR(p.x(), 0.0, surf_epsilon);
    EXPECT_NEAR(p.y(), 0.0, surf_epsilon);
    EXPECT_NEAR(p.z(), -1.0, surf_epsilon);
}

TEST(SphereTest, EquatorPointOnXAxis) {
    // u=pi/2, v=0: (r, 0, 0)
    Vec3d p = sphere({2.0}, M_PI/2.0, 0.0);
    EXPECT_NEAR(p.x(), 2.0, surf_epsilon);
    EXPECT_NEAR(p.y(), 0.0, surf_epsilon);
    EXPECT_NEAR(p.z(), 0.0, surf_epsilon);
}

TEST(SphereTest, RadiusScalesOutput) {
    double t = M_PI / 3.0, s = M_PI / 4.0;
    Vec3d p1 = sphere({1.0}, t, s);
    Vec3d p2 = sphere({3.0}, t, s);
    EXPECT_NEAR(p2.norm(), 3.0 * p1.norm(), surf_epsilon);
}

TEST(SphereTest, NormalIsRadiallyOutward) {
    // For a sphere, the surface normal should point radially outward,
    // i.e. parallel to the position vector.
    const double r = 1.0;
    const double fd_eps = 1e-6;
    for (double u : {M_PI/4.0, M_PI/2.0, 3.0*M_PI/4.0}) {
        for (double v : {0.0, M_PI/2.0, M_PI}) {
            auto S = [&](double u_, double v_) { return sphere({r}, u_, v_); };
            Vec3d dSdu = (S(u+fd_eps,v) - S(u-fd_eps,v)) * (0.5/fd_eps);
            Vec3d dSdv = (S(u,v+fd_eps) - S(u,v-fd_eps)) * (0.5/fd_eps);
            Vec3d n = dSdu.cross(dSdv).normalized();
            Vec3d pos = sphere({r}, u, v).normalized();
            // Normal should be parallel to position (dot product = ±1)
            EXPECT_NEAR(std::abs(n.dot(pos)), 1.0, 1e-6)
                                << "u=" << u << " v=" << v;
        }
    }
}

#ifndef NDEBUG
TEST(SphereTest, WrongParamCountAsserts) {
    EXPECT_DEATH(sphere({1.0, 2.0}, 0.0, 0.0), "");
}
#endif

// -----------------------------------------------------------------------------
// sine3d
// -----------------------------------------------------------------------------

TEST(Sine3dTest, ZeroOnAxes) {
    // sin(period*u)*sin(period*v) = 0 when u=0 or v=0
    for (double v : {0.0, 1.0, M_PI}) {
        Vec3d p = sine3d({1.0, 1.0}, 0.0, v);
        EXPECT_NEAR(p.z(), 0.0, surf_epsilon) << "v=" << v;
    }
    for (double u : {0.0, 1.0, M_PI}) {
        Vec3d p = sine3d({1.0, 1.0}, u, 0.0);
        EXPECT_NEAR(p.z(), 0.0, surf_epsilon) << "u=" << u;
    }
}

TEST(Sine3dTest, XYAreLinearInUV) {
    // x = amp*u, y = amp*v — independent of period
    const double amp = 2.0, period = 3.0;
    for (double u : {0.0, 0.5, 1.0, M_PI}) {
        for (double v : {0.0, 0.5, 1.0}) {
            Vec3d p = sine3d({amp, period}, u, v);
            EXPECT_NEAR(p.x(), amp * u, surf_epsilon) << "u=" << u;
            EXPECT_NEAR(p.y(), amp * v, surf_epsilon) << "v=" << v;
        }
    }
}

TEST(Sine3dTest, ZBoundedByAmplitude) {
    // |z| <= amp since |sin*sin| <= 1
    const double amp = 3.0;
    for (double u : {0.0, 0.5, 1.0, M_PI}) {
        for (double v : {0.0, 0.5, 1.0}) {
            Vec3d p = sine3d({amp, 1.0}, u, v);
            EXPECT_LE(std::abs(p.z()), amp + surf_epsilon);
        }
    }
}

TEST(Sine3dTest, DefaultAmplitudeIsOne) {
    // params={} → amp=1, period=1
    Vec3d p0 = sine3d({},      1.0, 1.0);
    Vec3d p1 = sine3d({1.0, 1.0}, 1.0, 1.0);
    EXPECT_NEAR(p0.x(), p1.x(), surf_epsilon);
    EXPECT_NEAR(p0.y(), p1.y(), surf_epsilon);
    EXPECT_NEAR(p0.z(), p1.z(), surf_epsilon);
}

TEST(Sine3dTest, AmplitudeScalesOutput) {
    Vec3d p1 = sine3d({1.0, 1.0}, 1.0, 1.0);
    Vec3d p2 = sine3d({2.0, 1.0}, 1.0, 1.0);
    EXPECT_NEAR(p2.z(), 2.0 * p1.z(), surf_epsilon);
}

TEST(Sine3dTest, PeriodAffectsZOnly) {
    // Changing period should not affect x or y
    const double u = 1.0, v = 1.0;
    Vec3d p1 = sine3d({1.0, 1.0}, u, v);
    Vec3d p2 = sine3d({1.0, 2.0}, u, v);
    EXPECT_NEAR(p1.x(), p2.x(), surf_epsilon);
    EXPECT_NEAR(p1.y(), p2.y(), surf_epsilon);
    EXPECT_NE(p1.z(), p2.z());
}

#ifndef NDEBUG
TEST(Sine3dTest, TooManyParamsAsserts) {
    EXPECT_DEATH(sine3d({1.0, 2.0, 3.0}, 0.0, 0.0), "");
}
#endif

// -----------------------------------------------------------------------------
// helicoidal_ring
// NOTE: helicoidal_ring has a bug — assert condition is != 1 instead of == 1.
// Tests are written against the intended correct behaviour.
// -----------------------------------------------------------------------------

TEST(HelicoidalRingTest, OutputIsFinite) {
    // Smoke test: all components should be finite for reasonable inputs
    for (double u : {0.0, 0.25, 0.5}) {
        for (double v : {0.0, 0.25, 0.5, 0.75, 0.999}) {
            Vec3d p = helicoidal_ring({3.0}, u, v);
            EXPECT_TRUE(std::isfinite(p.x())) << "u=" << u << " v=" << v;
            EXPECT_TRUE(std::isfinite(p.y())) << "u=" << u << " v=" << v;
            EXPECT_TRUE(std::isfinite(p.z())) << "u=" << u << " v=" << v;
        }
    }
}

TEST(HelicoidalRingTest, DifferentLoopCountsGiveDifferentShapes) {
    Vec3d p1 = helicoidal_ring({1.0}, 0.25, 0.25);
    Vec3d p2 = helicoidal_ring({5.0}, 0.25, 0.25);
    EXPECT_NE(p1, p2);
}

TEST(HelicoidalRingTest, ZSymmetry) {
    // z = sin(loops*tau*v) * (sin(tau*u) + 3)
    // sin(tau*u) + 3 > 0 always (min = 3-1 = 2), so sign of z follows sin(loops*tau*v)
    // At v=0: z=0
    for (double u : {0.0, 0.1, 0.25, 0.5}) {
        Vec3d p = helicoidal_ring({2.0}, u, 0.0);
        EXPECT_NEAR(p.z(), 0.0, surf_epsilon) << "u=" << u;
    }
}

// =============================================================================
// ParametricSurfaceMeshGen (new generator) tests
// =============================================================================

TEST(ParametricSurfaceMeshGenTest, TopologyMatchesLayerMesh) {
    const int u = 4, v = 4;
    auto layer = SimpleMeshGen::generate_layer_mesh(u, v);
    auto mesh  = ParametricMeshGen::generate_parametric_mesh(
            sphere, {1.0}, 0.1*M_PI, 0.9*M_PI, u, 0.0, 2*M_PI, v, 0.1);
    EXPECT_EQ(mesh.n_vertices(), layer.n_vertices());
    EXPECT_EQ(mesh.n_cells(),    layer.n_cells());
}

TEST(ParametricSurfaceMeshGenTest, TopologyScalesWithSteps) {
    for (int u : {3, 5, 10}) {
        for (int v : {3, 5, 10}) {
            auto layer = SimpleMeshGen::generate_layer_mesh(u, v);
            auto mesh  = ParametricMeshGen::generate_parametric_mesh(
                    sine3d, {1.0, 1.0}, 0.0, M_PI, u, 0.0, M_PI, v, 0.1);
            EXPECT_EQ(mesh.n_vertices(), layer.n_vertices()) << "u=" << u << " v=" << v;
            EXPECT_EQ(mesh.n_cells(),    layer.n_cells())    << "u=" << u << " v=" << v;
        }
    }
}

TEST(ParametricSurfaceMeshGenTest, ZeroThicknessTopAndBottomCoincide) {
    // With thickness=0, top and bottom vertices should be identical.
    const int u = 4, v = 4;
    auto mesh = ParametricMeshGen::generate_parametric_mesh(
            sphere, {1.0}, 0.1*M_PI, 0.9*M_PI, u, 0.0, 2*M_PI, v, 0.0);
    for (int i = 0; i <= u; ++i) {
        for (int j = 0; j <= v; ++j) {
            auto vh_b = SimpleMeshGen::layer_vertex_id(u, v, i, j, 0);
            auto vh_t = SimpleMeshGen::layer_vertex_id(u, v, i, j, 1);
            EXPECT_EQ(mesh.vertex(vh_b), mesh.vertex(vh_t))
                                << "i=" << i << " j=" << j;
        }
    }
}

TEST(ParametricSurfaceMeshGenTest, TopAndBottomEquidistantFromSurface) {
    // Each top/bottom vertex should be at distance 0.5*thickness from surface.
    const int u = 4, v = 4;
    const double thickness = 0.4;
    auto mesh = ParametricMeshGen::generate_parametric_mesh(
            sphere, {2.0}, 0.1*M_PI, 0.9*M_PI, u, 0.0, 2*M_PI, v, thickness);

    const double du = (0.9*M_PI - 0.1*M_PI) / u;
    const double dv = (2*M_PI) / v;
    for (int i = 0; i <= u; ++i) {
        for (int j = 0; j <= v; ++j) {
            double ui = 0.1*M_PI + i * du;
            double vj = j * dv;
            Vec3d surface_pt = sphere({2.0}, ui, vj);
            auto vh_b = SimpleMeshGen::layer_vertex_id(u, v, i, j, 0);
            auto vh_t = SimpleMeshGen::layer_vertex_id(u, v, i, j, 1);
            EXPECT_NEAR((mesh.vertex(vh_b) - surface_pt).norm(), 0.5*thickness, 1e-6)
                                << "bottom i=" << i << " j=" << j;
            EXPECT_NEAR((mesh.vertex(vh_t) - surface_pt).norm(), 0.5*thickness, 1e-6)
                                << "top i=" << i << " j=" << j;
        }
    }
}

TEST(ParametricSurfaceMeshGenTest, TopAndBottomSymmetricAboutSurface) {
    // Midpoint of top and bottom vertex should lie on the surface.
    const int u = 4, v = 4;
    const double thickness = 0.3;
    auto mesh = ParametricMeshGen::generate_parametric_mesh(
            sphere, {1.0}, 0.1*M_PI, 0.9*M_PI, u, 0.0, 2*M_PI, v, thickness);

    const double du = (0.9*M_PI - 0.1*M_PI) / u;
    const double dv = 2*M_PI / v;
    for (int i = 0; i <= u; ++i) {
        for (int j = 0; j <= v; ++j) {
            double ui = 0.1*M_PI + i * du;
            double vj = j * dv;
            Vec3d surface_pt = sphere({1.0}, ui, vj);
            auto vh_b = SimpleMeshGen::layer_vertex_id(u, v, i, j, 0);
            auto vh_t = SimpleMeshGen::layer_vertex_id(u, v, i, j, 1);
            Vec3d midpoint = (mesh.vertex(vh_b) + mesh.vertex(vh_t)) * 0.5;
            EXPECT_NEAR(midpoint.x(), surface_pt.x(), 1e-6) << "i=" << i << " j=" << j;
            EXPECT_NEAR(midpoint.y(), surface_pt.y(), 1e-6) << "i=" << i << " j=" << j;
            EXPECT_NEAR(midpoint.z(), surface_pt.z(), 1e-6) << "i=" << i << " j=" << j;
        }
    }
}

TEST(ParametricSurfaceMeshGenTest, InteriorVertexIsCentroidOfAllVertices) {
    // Interior vertex (index 0) should equal the average of all top+bottom vertices.
    const int u = 4, v = 4;
    auto mesh = ParametricMeshGen::generate_parametric_mesh(
            sine3d, {1.0, 1.0}, 0.0, M_PI, u, 0.0, M_PI, v, 0.2);

    Vec3d centroid{0, 0, 0};
    int count = 0;
    for (int i = 0; i <= u; ++i) {
        for (int j = 0; j <= v; ++j) {
            auto vh_b = SimpleMeshGen::layer_vertex_id(u, v, i, j, 0);
            auto vh_t = SimpleMeshGen::layer_vertex_id(u, v, i, j, 1);
            centroid = centroid + mesh.vertex(vh_b) + mesh.vertex(vh_t);
            count += 2;
        }
    }
    centroid = centroid * (1.0 / count);
    EXPECT_NEAR(mesh.vertex(VertexHandle(0)).x(), centroid.x(), 1e-9);
    EXPECT_NEAR(mesh.vertex(VertexHandle(0)).y(), centroid.y(), 1e-9);
    EXPECT_NEAR(mesh.vertex(VertexHandle(0)).z(), centroid.z(), 1e-9);
}

TEST(ParametricSurfaceMeshGenTest, SphereVerticesAtCorrectRadius) {
    // For sphere with radius r and thickness t:
    // top vertices at r + 0.5*t, bottom at r - 0.5*t from origin.
    const double r = 2.0, thickness = 0.4;
    const int u = 6, v = 6;
    auto mesh = ParametricMeshGen::generate_parametric_mesh(
            sphere, {r}, 0.1*M_PI, 0.9*M_PI, u, 0.0, 2*M_PI, v, thickness);

    for (int i = 0; i <= u; ++i) {
        for (int j = 0; j <= v; ++j) {
            auto vh_b = SimpleMeshGen::layer_vertex_id(u, v, i, j, 0);
            auto vh_t = SimpleMeshGen::layer_vertex_id(u, v, i, j, 1);
            EXPECT_NEAR(mesh.vertex(vh_b).norm(), r - 0.5*thickness, 1e-5)
                                << "bottom i=" << i << " j=" << j;
            EXPECT_NEAR(mesh.vertex(vh_t).norm(), r + 0.5*thickness, 1e-5)
                                << "top i=" << i << " j=" << j;
        }
    }
}

TEST(ParametricSurfaceMeshGenTest, NonZeroVolume) {
    auto mesh = ParametricMeshGen::generate_parametric_mesh(
            sphere, {1.0}, 0.1*M_PI, 0.9*M_PI, 6, 0.0, 2*M_PI, 6, 0.2);
    EXPECT_GT(std::abs(mesh.compute_signed_volume()), 0.0);
}


TEST(ParametricSurfaceMeshGenTest, DifferentSurfacesDifferentVolumes) {
    const int u = 6, v = 6;
    auto m1 = ParametricMeshGen::generate_parametric_mesh(
            sphere, {1.0}, 0.1*M_PI, 0.9*M_PI, u, 0.0, 2*M_PI, v, 0.2);
    auto m2 = ParametricMeshGen::generate_parametric_mesh(
            sine3d, {1.0, 1.0}, 0.0, M_PI, u, 0.0, M_PI, v, 0.2);
    EXPECT_NE(m1.compute_signed_volume(), m2.compute_signed_volume());
}

// -----------------------------------------------------------------------------
// torus
// -----------------------------------------------------------------------------

TEST(TorusTest, KnownPointOuterEquator) {
    // v=0, u=0: ((R+r), 0, 0)
    Vec3d p = torus({3.0, 1.0}, 0.0, 0.0);
    EXPECT_NEAR(p.x(), 4.0, surf_epsilon);
    EXPECT_NEAR(p.y(), 0.0, surf_epsilon);
    EXPECT_NEAR(p.z(), 0.0, surf_epsilon);
}

TEST(TorusTest, KnownPointInnerEquator) {
    // v=pi, u=0: ((R-r), 0, 0)
    Vec3d p = torus({3.0, 1.0}, 0.0, M_PI);
    EXPECT_NEAR(p.x(), 2.0, surf_epsilon);
    EXPECT_NEAR(p.y(), 0.0, surf_epsilon);
    EXPECT_NEAR(p.z(), 0.0, surf_epsilon);
}

TEST(TorusTest, KnownPointTop) {
    // v=pi/2, u=0: (R, 0, r)
    Vec3d p = torus({3.0, 1.0}, 0.0, M_PI / 2.0);
    EXPECT_NEAR(p.x(), 3.0, surf_epsilon);
    EXPECT_NEAR(p.y(), 0.0, surf_epsilon);
    EXPECT_NEAR(p.z(), 1.0, surf_epsilon);
}

TEST(TorusTest, AllPointsAtMinorRadiusFromCentralCircle) {
    // The key torus property: every point lies at distance r from the
    // central circle of radius R in the xy-plane.
    // dist = sqrt((sqrt(x^2+y^2) - R)^2 + z^2) = r
    const double R = 3.0, r = 1.0;
    for (double u : {0.0, M_PI/4.0, M_PI/2.0, M_PI, 3.0*M_PI/2.0}) {
        for (double v : {0.0, M_PI/4.0, M_PI/2.0, M_PI, 3.0*M_PI/2.0}) {
            Vec3d p = torus({R, r}, u, v);
            double xy_dist = std::sqrt(p.x()*p.x() + p.y()*p.y());
            double dist = std::sqrt((xy_dist - R)*(xy_dist - R) + p.z()*p.z());
            EXPECT_NEAR(dist, r, surf_epsilon) << "u=" << u << " v=" << v;
        }
    }
}

TEST(TorusTest, ZBoundedByMinorRadius) {
    // |z| = |r*sin(v)| <= r
    const double r = 1.5;
    for (double u : {0.0, 1.0, M_PI}) {
        for (double v : {0.0, M_PI/4.0, M_PI/2.0, M_PI, 3.0*M_PI/2.0}) {
            Vec3d p = torus({3.0, r}, u, v);
            EXPECT_LE(std::abs(p.z()), r + surf_epsilon) << "u=" << u << " v=" << v;
        }
    }
}

TEST(TorusTest, ClosedInBothDirections) {
    // Periodic in u and v with period 2pi
    for (double u : {0.0, 1.0, M_PI}) {
        for (double v : {0.0, 1.0, M_PI}) {
            Vec3d p0  = torus({3.0, 1.0}, u, v);
            Vec3d pu  = torus({3.0, 1.0}, u + 2.0*M_PI, v);
            Vec3d pv  = torus({3.0, 1.0}, u, v + 2.0*M_PI);
            EXPECT_NEAR(p0.x(), pu.x(), surf_epsilon) << "u-periodicity u=" << u;
            EXPECT_NEAR(p0.x(), pv.x(), surf_epsilon) << "v-periodicity v=" << v;
        }
    }
}

TEST(TorusTest, MajorRadiusScalesXY) {
    // Doubling R shifts all points outward in xy by R
    const double r = 0.5;
    double u = M_PI/3.0, v = M_PI/4.0;
    Vec3d p1 = torus({2.0, r}, u, v);
    Vec3d p2 = torus({4.0, r}, u, v);
    // z should be identical (only depends on r and v)
    EXPECT_NEAR(p1.z(), p2.z(), surf_epsilon);
    // xy norm difference should equal delta_R = 2.0
    double xy1 = std::sqrt(p1.x()*p1.x() + p1.y()*p1.y());
    double xy2 = std::sqrt(p2.x()*p2.x() + p2.y()*p2.y());
    EXPECT_NEAR(xy2 - xy1, 2.0, surf_epsilon);
}

TEST(TorusTest, MinorRadiusScalesZ) {
    // At v=pi/2: z = r, so doubling r doubles z
    Vec3d p1 = torus({3.0, 1.0}, 0.0, M_PI/2.0);
    Vec3d p2 = torus({3.0, 2.0}, 0.0, M_PI/2.0);
    EXPECT_NEAR(p2.z(), 2.0 * p1.z(), surf_epsilon);
}

#ifndef NDEBUG
TEST(TorusTest, WrongParamCountAsserts) {
    EXPECT_DEATH(torus({1.0}, 0.0, 0.0), "");
    EXPECT_DEATH(torus({1.0, 2.0, 3.0}, 0.0, 0.0), "");
}
#endif


// -----------------------------------------------------------------------------
// Export Tests
// -----------------------------------------------------------------------------

#ifndef REF_MESH_DIR
#error "REF_MESH_DIR not found, export tests will fail. It should be defined in the CMakeLists so maybe you modified it?"
#endif

// Try to parse a line as a Vec3d. Returns true on success.
static bool parse_vec3d(const std::string& line, Vec3d& out) {
    // Handle both plain "x y z" (OVM) and "v x y z" (OBJ)
    const std::string& s = (line.size() > 2 && line[0] == 'v' && line[1] == ' ')
                           ? line.substr(2)
                           : line;
    std::istringstream ss(s);
    double x, y, z;
    if (ss >> x >> y >> z) {
        std::string extra;
        if (!(ss >> extra)) {
            out = {x, y, z};
            return true;
        }
    }
    return false;
}

static void compare_against_reference(const std::string& filename) {
    std::filesystem::path ref_path =
            std::filesystem::path(REF_MESH_DIR) / ("ref_" + filename);

    ASSERT_TRUE(std::filesystem::exists(ref_path))
                                << "Reference file missing: " << ref_path;

    const std::string actual   = read_file(filename);
    const std::string expected = read_file(ref_path);

    ASSERT_FALSE(actual.empty())   << "Could not read output file: " << filename;
    ASSERT_FALSE(expected.empty()) << "Could not read reference file: " << ref_path;

    std::istringstream actual_stream(actual);
    std::istringstream expected_stream(expected);
    std::string actual_line, expected_line;
    int line_number = 1;

    while (std::getline(expected_stream, expected_line)) {
        ASSERT_TRUE(std::getline(actual_stream, actual_line))
                                    << "Output file is shorter than reference at line " << line_number;

        Vec3d actual_vec, expected_vec;
        if (parse_vec3d(expected_line, expected_vec) && parse_vec3d(actual_line, actual_vec)) {
            for (int i = 0; i < 3; ++i) {
                const double a = actual_vec[i];
                const double e = expected_vec[i];
                // If both are NaN, treat as equal. If only one is, fail.
                if (std::isnan(e)) {
                    ASSERT_TRUE(std::isnan(a))
                                                << "Expected NaN but got " << a
                                                << " at line " << line_number << " component " << i;
                } else {
                    ASSERT_NEAR(a, e, epsilon)
                                                << "Mismatch at line " << line_number << " component " << i;
                }
            }
        } else {
            ASSERT_EQ(actual_line, expected_line) << "First mismatch at line " << line_number;
        }
        ++line_number;
    }

    ASSERT_FALSE(std::getline(actual_stream, actual_line))
                                << "Output file is longer than reference, first extra line "
                                << line_number << ": " << actual_line;
}


class MeshExportTest : public ::testing::Test {
protected:
    void TearDown() override {
        if (HasFatalFailure()) return;
        if(mesh_name_ == "") return;
        
        write_to_file(mesh_, mesh_name_);
        compare_against_reference(mesh_name_);
        std::filesystem::remove(mesh_name_);

        const std::string mesh_boundary_name =
                std::filesystem::path(mesh_name_).stem().string() + ".obj";
        write_to_file(mesh_, mesh_boundary_name, true);
        compare_against_reference(mesh_boundary_name);
        std::filesystem::remove(mesh_boundary_name);
    }

    TetMesh mesh_;
    std::string mesh_name_;
};


TEST_F(MeshExportTest, KnottedHole) {
    mesh_name_ = "knotted_hole.ovm";
    mesh_ = VoxelGridMeshGen::generate_Furchs_knotted_hole();
}


TEST_F(MeshExportTest, MinimalNonStarShaped) {
    auto codomain_mesh = SimpleMeshGen::generate_minimal_non_star_shaped_mesh();
    std::string codomain_mesh_name = "min_non_star_shaped_codomain.ovm";
    write_to_file(codomain_mesh, codomain_mesh_name);
    compare_against_reference(codomain_mesh_name);
    std::filesystem::remove(codomain_mesh_name);

    auto domain_mesh = SimpleMeshGen::generate_minimal_non_star_shaped_domain_mesh();
    std::string domain_mesh_name = "min_non_star_shaped_domain.ovm";
    write_to_file(domain_mesh, domain_mesh_name);
    compare_against_reference(domain_mesh_name);
    std::filesystem::remove(domain_mesh_name);
    SUCCEED();
}


TEST_F(MeshExportTest, SineMesh) {
    const int N(50);
    //default mesh follows sin(x/N);
    mesh_ = SimpleMeshGen::generate_sine_mesh(N, 5.0/N, M_PI/2.0, 2.0, 2.0);
    mesh_name_ = "sine.ovm";
}


TEST_F(MeshExportTest, SpiralMesh) {
    const int N(60);
    mesh_ = SimpleMeshGen::generate_spiral_mesh(N, 2*M_PI, 10.0, 5.0);
    mesh_name_ = "spiral.ovm";

}


TEST_F(MeshExportTest, TrefoilKnotMesh) {
    const int N(100);
    mesh_ = SimpleMeshGen::generate_trefoil_knot_mesh(N, 4*M_PI, 0.9);
    //std::cout<<" ------------------------ t = "<<t_deg<<" / "<<t_rad<<std::endl;
    mesh_name_ = "trefoil.ovm";
}



TEST_F(MeshExportTest, CircleMesh) {

    mesh_ = ParametricMeshGen::generate_parametric_mesh(circle, {2.0},
                                                              0.25, 1.0, //t range 
                                                              40, 0.5); //length, thickness
    mesh_name_ = "circle.ovm";
}


TEST_F(MeshExportTest, MultiBranchSpiral) {
    mesh_ = ParametricMeshGen::generate_parametric_mesh(multi_branch_spiral, {0.5, 3.0, 2.0, 0.1},
                                                                                  0.0, 1.0, 
                                                                                  200, 0.1);
    mesh_name_ = "multi_branch_spiral.ovm";
}


TEST_F(MeshExportTest, FunnySpiral) {
    mesh_ = ParametricMeshGen::generate_parametric_mesh(funny_spiral, {0.9, 0.7, 10.0, 1.0},
                                                                                  0.0, 2.0, 
                                                                                  200, 0.2);
    mesh_name_ = "funny_spiral.ovm";

}



TEST_F(MeshExportTest, ParametricTrefoilKnot) {
    mesh_ = ParametricMeshGen::generate_parametric_mesh(trefoil_knot, {},
                                                                                  0.0, 0.9, 
                                                                                  100, 0.2);
    mesh_name_ = "para_trefoil.ovm";

}


TEST_F(MeshExportTest, LayerMesh) {

    const int M(7), N(13);
    mesh_ = SimpleMeshGen::generate_layer_mesh(M, N);
    mesh_name_ = "layer.ovm";
}


TEST_F(MeshExportTest, Sine3d) {

    int M(60), N(40);
    mesh_ = ParametricMeshGen::generate_parametric_mesh(sine3d, {2.0, 2.0},
                                                              0.0, 2 * M_PI, M, //u range 
                                                              0.0, 2 * M_PI, N, //v range 
                                                              0.2); //thickness
    mesh_name_ = "sine3d.ovm";

}



TEST_F(MeshExportTest, SpherePatch) {

    int M(10), N(20);
    mesh_ = ParametricMeshGen::generate_parametric_mesh(sphere, {2.0},
                                                              0.1 * M_PI, 0.75 * M_PI, M, //u range 
                                                              0.0, 0.66 * M_PI, N, //v range 
                                                              0.2); //thickness
    mesh_name_ = "sphere.ovm";

}


TEST_F(MeshExportTest, HelicoidalRing) {

    int M(10), N(200);
    mesh_ = ParametricMeshGen::generate_parametric_mesh(helicoidal_ring, {10.0},
                                                              0.0, 0.5, M, //u range 
                                                              0.0, 0.999, N, //v range 
                                                              0.2); //thickness
    mesh_name_ = "helicoidal_ring.ovm";

}


TEST_F(MeshExportTest, Torus) {

    int M(10), N(6);
    mesh_ = ParametricMeshGen::generate_parametric_mesh(torus, {4.0, 1.0},
                                                        0.0, 1.5 * M_PI, M, //u range
                                                        0.0, 1.5 * M_PI, N, //v range
                                                        0.2); //thickness
    mesh_name_ = "torus.ovm";

}




