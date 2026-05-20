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
    // Writers — one private method per format
    // -------------------------------------------------------------------------

    void write_ovm(const std::filesystem::path& path) const {
        std::ofstream f(path);
        if (!f)
            throw std::runtime_error(
                "TetMesh::write_ovm: cannot open '" + path.string() + "'");

        // Header
        f << "OVM ASCII\n\n";

        // Vertices
        f << "Vertices\n" << n_vertices() << "\n";
        for (const auto& v : vertices_)
            f << v.x() << " " << v.y() << " " << v.z() << "\n";
        f << "\n";

        // Cells (flat: four vertex indices per line)
        f << "Cells\n" << n_cells() << "\n";
        for (const auto& c : cells_)
            f << c[0].idx() << " " << c[1].idx() << " "
              << c[2].idx() << " " << c[3].idx() << "\n";
        f << "\n";
    }

    // -------------------------------------------------------------------------
    // Data
    // -------------------------------------------------------------------------

    std::vector<Vec3d> vertices_;
    std::vector<Cell>          cells_;
};

} // namespace tet_weave