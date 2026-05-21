#pragma once

#include "Vec3d.hh"


namespace tet_weave {

// =============================================================================
// Handles — thin wrappers around an int index, à la OpenVolumeMesh
// =============================================================================

struct VertexHandle {
    explicit VertexHandle(int i = -1) : idx_(i) {}
    int  idx()      const { return idx_; }
    bool is_valid() const { return idx_ >= 0; }
    bool operator==(const VertexHandle& rhs) const { return idx_ == rhs.idx_; }
    bool operator!=(const VertexHandle& rhs) const { return idx_ != rhs.idx_; }
private:
    const int idx_;
};

struct CellHandle {
    explicit CellHandle(int i = -1) : idx_(i) {}
    int  idx()      const { return idx_; }
    bool is_valid() const { return idx_ >= 0; }
    bool operator==(const CellHandle& rhs) const { return idx_ == rhs.idx_; }
    bool operator!=(const CellHandle& rhs) const { return idx_ != rhs.idx_; }
private:
    const int idx_;
};

// =============================================================================
// TetMesh
// =============================================================================

class TetMesh {
public:
    using Cell = std::array<VertexHandle, 4>;

    // -------------------------------------------------------------------------
    // Vertices
    // -------------------------------------------------------------------------

    /// Append a vertex and return its handle.
    VertexHandle add_vertex(Vec3d pos) {
        vertices_.push_back(pos);
        return VertexHandle(static_cast<int>(vertices_.size()) - 1);
    }

    /// Convenience overload — construct the position in-place.
    VertexHandle add_vertex(double x, double y, double z) {
        return add_vertex(Vec3d(x, y, z));
    }

    // -------------------------------------------------------------------------
    // Cells
    // -------------------------------------------------------------------------

    /// Append a tetrahedral cell defined by four vertex handles and return its handle.
    /// The handles must refer to vertices already added to this mesh.
    CellHandle add_cell(VertexHandle v0, VertexHandle v1,
                        VertexHandle v2, VertexHandle v3) {
        assert(is_valid_vertex(v0) && "add_cell: invalid vertex handle v0");
        assert(is_valid_vertex(v1) && "add_cell: invalid vertex handle v1");
        assert(is_valid_vertex(v2) && "add_cell: invalid vertex handle v2");
        assert(is_valid_vertex(v3) && "add_cell: invalid vertex handle v3");
        cells_.push_back({v0, v1, v2, v3});
        return CellHandle(static_cast<int>(cells_.size()) - 1);
    }

    // -------------------------------------------------------------------------
    // Accessors
    // -------------------------------------------------------------------------

    const Vec3d& vertex(VertexHandle vh) const {
        assert(is_valid_vertex(vh) && "vertex: invalid handle");
        return vertices_[vh.idx()];
    }

    const Cell& cell(CellHandle ch) const {
        assert(is_valid_cell(ch) && "cell: invalid handle");
        return cells_[ch.idx()];
    }

    int n_vertices() const { return static_cast<int>(vertices_.size()); }
    int n_cells()    const { return static_cast<int>(cells_.size());    }


    /// Range over all vertex positions: for (const Vec3d& v : mesh.vertices())
    const std::vector<Vec3d>& vertices() const { return vertices_; }

    /// Range over all cells: for (const Cell& c : mesh.cells())
    const std::vector<Cell>& cells() const { return cells_; }

    // -------------------------------------------------------------------------
    // Geometry
    // -------------------------------------------------------------------------

    /// Volume of a single tet, computed as det(e1, e2, e3) / 6
    /// where e1, e2, e3 are the edge vectors from the first vertex.
    /// Returns a positive value for a right-hand-rule oriented cell.
    double volume(CellHandle ch) const {
        const Cell& c  = cell(ch);
        const Vec3d& v0 = vertices_[c[0].idx()];
        const Vec3d  e1 = vertices_[c[1].idx()] - v0;
        const Vec3d  e2 = vertices_[c[2].idx()] - v0;
        const Vec3d  e3 = vertices_[c[3].idx()] - v0;
        // det(e1, e2, e3) = e1 · (e2 × e3)
        return e1.dot(e2.cross(e3)) / 6.0;
    }

    /// Sum of volumes of all cells.
    double compute_total_volume() const {
        double total = 0.0;
        for (int i = 0; i < n_cells(); ++i)
            total += volume(CellHandle(i));
        return total;
    }

    // -------------------------------------------------------------------------
    // File I/O — format detected from path extension
    // -------------------------------------------------------------------------

    /// Write the mesh to `path`. The output format is inferred from the file
    /// extension. Currently supported: .ovm
    void write_to_file(const std::filesystem::path& path) const {
        const std::string ext = path.extension().string();

        if (ext == ".ovm") {
            write_ovm(path);
        } else {
            throw std::runtime_error(
                "TetMesh::write_to_file: unsupported format '" + ext + "'");
        }
    }

private:
    // -------------------------------------------------------------------------
    // Internal helpers
    // -------------------------------------------------------------------------

    bool is_valid_vertex(VertexHandle vh) const {
        return vh.idx() >= 0 && vh.idx() < n_vertices();
    }

    bool is_valid_cell(CellHandle ch) const {
        return ch.idx() >= 0 && ch.idx() < n_cells();
    }
// -------------------------------------------------------------------------
    // Topology extraction
    // -------------------------------------------------------------------------

    // The four faces of a tet with vertices (v0,v1,v2,v3), wound so that the
    // normal points outward (right-hand rule, away from the opposite vertex):
    //   face 0 — opposite v0: (v1, v2, v3)
    //   face 1 — opposite v1: (v0, v3, v2)
    //   face 2 — opposite v2: (v0, v1, v3)
    //   face 3 — opposite v3: (v0, v2, v1)
    static constexpr int kFaceTable[4][3] = {
        {1, 2, 3},
        {0, 3, 2},
        {0, 1, 3},
        {0, 2, 1}
    };

    // Canonical (sorted) key for a face, used for deduplication.
    static std::array<int, 3> face_key(int a, int b, int c) {
        std::array<int, 3> key = {a, b, c};
        std::sort(key.begin(), key.end());
        return key;
    }

    // -------------------------------------------------------------------------
    // Build edge and face tables for OVM output.
    //
    // Edges are stored as ordered pairs (vs, vt) — the direction is the
    // canonical one (vs < vt). Half-edge 2i goes vs→vt, 2i+1 goes vt→vs.
    //
    // Faces are stored as oriented triples following kFaceTable winding.
    // Half-face 2i has that orientation, 2i+1 is the opposite.
    // A map from sorted face key → face index enables O(log n) lookup.
    // -------------------------------------------------------------------------

    struct EdgeTable {
        // Ordered list of edges: edges[i] = {vs, vt} with vs < vt.
        std::vector<std::array<int, 2>> edges;
        // Map (vs, vt) with vs < vt → edge index.
        std::map<std::array<int, 2>, int> index;
    };

    struct FaceTable {
        // Oriented faces: faces[i] = winding as extracted from kFaceTable.
        std::vector<std::array<int, 3>> faces;
        // Map sorted key → face index.
        std::map<std::array<int, 3>, int> index;
    };

    EdgeTable build_edge_table() const {
        EdgeTable t;
        for (const auto& c : cells_) {
            for (int i = 0; i < 4; ++i) {
                for (int j = i + 1; j < 4; ++j) {
                    int a = c[i].idx(), b = c[j].idx();
                    if (a > b) std::swap(a, b);
                    std::array<int, 2> key = {a, b};
                    if (t.index.find(key) == t.index.end()) {
                        t.index[key] = static_cast<int>(t.edges.size());
                        t.edges.push_back(key);
                    }
                }
            }
        }
        return t;
    }

    FaceTable build_face_table() const {
        FaceTable t;
        for (const auto& c : cells_) {
            for (const auto& f : kFaceTable) {
                int a = c[f[0]].idx(), b = c[f[1]].idx(), cc = c[f[2]].idx();
                auto key = face_key(a, b, cc);
                if (t.index.find(key) == t.index.end()) {
                    t.index[key] = static_cast<int>(t.faces.size());
                    t.faces.push_back({a, b, cc});
                }
            }
        }
        return t;
    }

    // Return the half-edge index for a directed edge (from → to).
    // Half-edge 2i is the canonical direction (vs < vt),
    // half-edge 2i+1 is the reverse.
    static int half_edge_idx(const EdgeTable& et, int from, int to) {
        int vs = from, vt = to;
        bool flipped = vs > vt;
        if (flipped) std::swap(vs, vt);
        int i = et.index.at({vs, vt});
        return flipped ? 2 * i + 1 : 2 * i;
    }

    // Return the half-face index for the face opposite to vertex `opp` in cell `c`.
    // Half-face 2i matches the stored winding; 2i+1 is the opposite.
    static int half_face_idx(const FaceTable& ft, const Cell& c, int face_slot) {
        const auto& f   = kFaceTable[face_slot];
        int a = c[f[0]].idx(), b = c[f[1]].idx(), cc = c[f[2]].idx();
        auto key        = face_key(a, b, cc);
        int  fi         = ft.index.at(key);
        // Check whether the stored face orientation matches (a,b,c) or is flipped.
        const auto& stored = ft.faces[fi];
        bool flipped = (stored[0] != a || stored[1] != b || stored[2] != cc);
        return flipped ? 2 * fi + 1 : 2 * fi;
    }

    // -------------------------------------------------------------------------
    // Writers — one private method per format
    // -------------------------------------------------------------------------

    void write_ovm(const std::filesystem::path& path) const {
        std::ofstream f(path);
        if (!f)
            throw std::runtime_error(
                "TetMesh::write_ovm: cannot open '" + path.string() + "'");

        const EdgeTable et = build_edge_table();
        const FaceTable ft = build_face_table();

        // Header
        f << "OVM ASCII\n";

        // Vertices
        f << "Vertices\n" << n_vertices() << "\n";
        for (const auto& v : vertices_)
            f << v.x() << " " << v.y() << " " << v.z() << "\n";

        // Edges — each as "vs vt" (vs < vt; half-edge 2i = forward, 2i+1 = reverse)
        f << "Edges\n" << et.edges.size() << "\n";
        for (const auto& e : et.edges)
            f << e[0] << " " << e[1] << "\n";

        // Faces — each defined by its 3 half-edges following stored winding
        f << "Faces\n" << ft.faces.size() << "\n";
        for (const auto& face : ft.faces) {
            // Walk the three directed edges of this face: (a→b), (b→c), (c→a)
            int he0 = half_edge_idx(et, face[0], face[1]);
            int he1 = half_edge_idx(et, face[1], face[2]);
            int he2 = half_edge_idx(et, face[2], face[0]);
            f << "3 " << he0 << " " << he1 << " " << he2 << "\n";
        }

        // Polyhedra — each defined by its 4 half-faces
        f << "Polyhedra\n" << n_cells() << "\n";
        for (const auto& c : cells_) {
            f << "4";
            for (int s = 0; s < 4; ++s)
                f << " " << half_face_idx(ft, c, s);
            f << "\n";
        }
    }


    // -------------------------------------------------------------------------
    // Data
    // -------------------------------------------------------------------------

    std::vector<Vec3d> vertices_;
    std::vector<Cell>  cells_;
};

} // namespace tet_weave