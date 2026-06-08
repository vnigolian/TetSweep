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


    inline std::ostream &operator<<(std::ostream &os, const VertexHandle &vh) {
        os << vh.idx();
        return os;
    }

    inline std::ostream &operator<<(std::ostream &os, const CellHandle &ch) {
        os << ch.idx();
        return os;
    }

    // =============================================================================
    // Ranges
    // =============================================================================

    template <typename Handle>
class HandleRange {
    public:
        struct Iterator {
            int idx;
            Handle operator*()        const { return Handle(idx); }
            Iterator& operator++()          { ++idx; return *this; }
            bool operator!=(const Iterator& rhs) const { return idx != rhs.idx; }
        };

        explicit HandleRange(int n) : n_(n) {}
        Iterator begin() const { return {0};     }
        Iterator end()   const { return {n_};    }
        int      size()  const { return n_;      }
        bool     empty() const { return n_ == 0; }
    private:
        int n_;
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
            boundary_v_.push_back(true);
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
        CellHandle add_cell(const std::array<VertexHandle, 4> &verts) {
            assert(is_valid_vertex(verts[0]) && "add_cell: invalid vertex handle v0");
            assert(is_valid_vertex(verts[1]) && "add_cell: invalid vertex handle v1");
            assert(is_valid_vertex(verts[2]) && "add_cell: invalid vertex handle v2");
            assert(is_valid_vertex(verts[3]) && "add_cell: invalid vertex handle v3");
            cells_.push_back(verts);
            return CellHandle(static_cast<int>(cells_.size()) - 1);
        }

        CellHandle add_cell(VertexHandle v0, VertexHandle v1,
                            VertexHandle v2, VertexHandle v3) {
            return add_cell({v0, v1, v2, v3});
        }

        // -------------------------------------------------------------------------
        // Accessors
        // -------------------------------------------------------------------------

        const Vec3d &vertex(VertexHandle vh) const {
            assert(is_valid_vertex(vh) && "vertex: invalid handle");
            return vertices_[vh.idx()];
        }

        const Cell &get_cell_vertices(CellHandle ch) const {
            assert(is_valid_cell(ch) && "cell: invalid handle");
            return cells_[ch.idx()];
        }

        int n_vertices() const { return static_cast<int>(vertices_.size()); }

        int n_cells() const { return static_cast<int>(cells_.size()); }


        /// Range over all vertices or cells
        auto vertices() const { return HandleRange<VertexHandle>(n_vertices()); }

        auto cells() const { return HandleRange<CellHandle>(n_cells()); }

        /// Overwrite the position of an existing vertex.
        void set_vertex(VertexHandle vh, Vec3d pos) {
            assert(is_valid_vertex(vh) && "set_vertex: invalid handle");
            vertices_[vh.idx()] = pos;
        }

        //only the first vertex is interior in this simple mesh implementation
        // (dedicated for this specific type of generated meshes)
        bool is_boundary(const VertexHandle &vh) const {
            return boundary_v_[vh.idx()];
        }

        void mark_as_boundary(const VertexHandle &vh, bool boundary_or_interior){
            boundary_v_[vh.idx()] = boundary_or_interior;
        }

        // -------------------------------------------------------------------------
        // Geometry
        // -------------------------------------------------------------------------

        /// Volume of a single tet, computed as det(e1, e2, e3) / 6
        /// where e1, e2, e3 are the edge vectors from the first vertex.
        /// Returns a positive value for a right-hand-rule oriented cell.
        double volume(CellHandle ch) const {
            const Cell &c = get_cell_vertices(ch);
            const Vec3d &v0 = vertices_[c[0].idx()];
            const Vec3d e1 = vertices_[c[1].idx()] - v0;
            const Vec3d e2 = vertices_[c[2].idx()] - v0;
            const Vec3d e3 = vertices_[c[3].idx()] - v0;
            // det(e1, e2, e3) = e1 · (e2 × e3)
            return e1.dot(e2.cross(e3)) / 6.0;
        }

        /// Sum of volumes of all cells.
        double compute_signed_volume() const {
            double total = 0.0;
            for (int i = 0; i < n_cells(); ++i)
                total += volume(CellHandle(i));
            return total;
        }

        double compute_unsigned_volume() const {
            double total = 0.0;
            for (int i = 0; i < n_cells(); ++i)
                total += std::abs(volume(CellHandle(i)));
            return total;
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

            //counts the number of incident faces and cells.
            // Used to determine whether it should be exported or not
            // (for boundary-only exports)
            // actually not needed, IF meshes are manifold everywhere. I'll leave it here just in case
            //std::map<std::array<int, 2>, int> f_count;
            //std::map<std::array<int, 2>, int> c_count;
        };

        struct FaceTable {
            // Oriented faces: faces[i] = winding as extracted from kFaceTable.
            std::vector<std::array<int, 3>> faces;
            // Map sorted key → face index.
            std::map<std::array<int, 3>, int> index;
            //counts the number of incident cells.
            std::map<std::array<int, 3>, int> c_count;
        };

        EdgeTable build_edge_table(const FaceTable& ft, bool boundary_only = false) const {

            //cell-based version, doesn't take cord edges into account.
            /*EdgeTable t;
            for (const auto &c: cells_) {
                for (int i = 0; i < 4; ++i) {
                    if (boundary_only && !is_boundary(c[i])) continue;
                    for (int j = i + 1; j < 4; ++j) {
                        if (boundary_only && !is_boundary(c[j])) continue;
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
            return t;*/

            EdgeTable t;
            for (const auto &f: ft.faces) {
                //NOTE: no need to filter based on boundary, because we assume that ft was already filtered
                for (int i = 0; i < 3; ++i) {
                    for (int j = i + 1; j < 3; ++j) {
                        int a = f[i], b = f[j];
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

        FaceTable build_face_table(bool boundary_only = false) const {
            //std::cout<<" building face table"<<std::endl;
            FaceTable t;
            std::vector<std::array<int,3>> faces;
            for (const auto &c: cells_) {
                for (const auto &f: kFaceTable) {
                    /*if (boundary_only &&
                        (!is_boundary(c[f[0]]) || !is_boundary(c[f[1]]) || !is_boundary(c[f[2]])))
                        continue;*/
                    int a = c[f[0]].idx(), b = c[f[1]].idx(), cc = c[f[2]].idx();
                    auto key = face_key(a, b, cc);

                    //std::cout<<" index at key: "<<t.index[key]<<std::endl;
                    if (t.index.find(key) == t.index.end()) {
                        t.index[key] = static_cast<int>(faces.size());
                        faces.push_back({a,b,cc});
                        t.c_count[key] = 1;
                        //std::cout<<" - added face "<<key[0]<<", "<<key[1]<<", "<<key[2]<<" to map with index "<<t.index[key]<<" and count "<<t.count[key]<<std::endl;
                    }else{
                        t.c_count[key]++;
                        //std::cout<<" - updated count for face "<<key[0]<<", "<<key[1]<<", "<<key[2]<<" with index "<<t.index[key]<<" to "<<t.count[key]<<std::endl;
                    }
                }
            }
            //std::cout<<" -> found "<<faces.size()<<" faces"<<std::endl;

            //only add faces incident to a single tet if we're only exporting the boundary
            if(boundary_only){
                //std::cout<<" - filtering for boundary faces..."<<std::endl;
                for(const auto& f: faces){
                    auto key = face_key(f[0], f[1], f[2]);
                    //std::cout<<" -- count for face with index "<<t.index[key]<<": "<<t.count[key]<<std::endl;
                    if(t.c_count[key] == 1) {
                        t.faces.push_back(f);
                    }
                }
                //std::cout<<" - updated face count: "<<t.faces.size()<<std::endl;
            }else{
                t.faces = faces;
            }
            return t;
        }


        // Return the half-edge index for a directed edge (from → to).
        // Half-edge 2i is the canonical direction (vs < vt),
        // half-edge 2i+1 is the reverse.
        static int half_edge_idx(const EdgeTable &et, int from, int to) {
            int vs = from, vt = to;
            bool flipped = vs > vt;
            if (flipped) std::swap(vs, vt);
            int i = et.index.at({vs, vt});
            return flipped ? 2 * i + 1 : 2 * i;
        }

        // Return the half-face index for the face opposite to vertex `opp` in cell `c`.
        // Half-face 2i matches the stored winding; 2i+1 is the opposite.
        static int half_face_idx(const FaceTable &ft, const Cell &c, int face_slot) {
            const auto &f = kFaceTable[face_slot];
            int a = c[f[0]].idx(), b = c[f[1]].idx(), cc = c[f[2]].idx();
            auto key = face_key(a, b, cc);
            int fi = ft.index.at(key);
            // Check whether the stored face orientation matches (a,b,c) or is flipped.
            const auto &stored = ft.faces[fi];
            bool flipped = (stored[0] != a || stored[1] != b || stored[2] != cc);
            return flipped ? 2 * fi + 1 : 2 * fi;
        }


        // -------------------------------------------------------------------------
        // Data
        // -------------------------------------------------------------------------

        std::vector<Vec3d> vertices_;
        std::vector<bool> boundary_v_;
        std::vector<Cell> cells_;

    public:

        // -------------------------------------------------------------------------
        // Writers — one method per format
        // -------------------------------------------------------------------------


        void write_ovm_to_stream(std::ostream& f, bool boundary_only = false) const {


            //std::cout<<" ------------------- exporting to .ovm, boundary only: "<<boundary_only<<std::endl;
            const FaceTable ft = build_face_table(boundary_only);
            const EdgeTable et = build_edge_table(ft, boundary_only);

            f << "OVM ASCII\n";

            std::vector<int> idx_map(n_vertices());
            int vidx(0);
            // Vertices — if boundary_only, skip interior vertex (index 0)
            //const int n_verts = boundary_only ? n_vertices() - 1 : n_vertices();
            //const int v_start = boundary_only ? 1 : 0;

            //first pass for map setup
            for (int i = 0; i < n_vertices(); ++i) {
                if(!boundary_only || is_boundary(VertexHandle(i))) {
                    const Vec3d &v = vertices_[i];
                    idx_map[i] = vidx;
                    vidx++;
                }else{
                    idx_map[i] = -1;
                }
                //std::cout<<" - "<<(is_boundary(VertexHandle(i)) ? "boundary" : "interior") << " vertex "<<i<<" mapped to index "<<idx_map[i]<<std::endl;
            }
            //std::cout<<" index map size: "<<idx_map.size()<<std::endl;

            //then second pass to write to stream
            f << "Vertices\n" << vidx << "\n";
            for (int i = 0; i < n_vertices(); ++i) {
                if(idx_map[i] >= 0) {
                    const Vec3d &v = vertices_[i];
                    f << v.x() << " " << v.y() << " " << v.z() << "\n";
                }
            }

            f << "Edges\n" << et.edges.size() << "\n";
            for (const auto &e: et.edges) {
                //std::cout<<" - plain edge: "<<e[0]<<", "<<e[1]<<std::endl;
                f << idx_map[e[0]] << " " << idx_map[e[1]] << "\n";
            }
            //std::cout<<" edges ok"<<std::endl;

            //std::cout<<" -> face count in table: "<<ft.faces.size()<<std::endl;
            f << "Faces\n" << ft.faces.size() << "\n";
            for (const auto &face: ft.faces) {
                int he0 = half_edge_idx(et, face[0], face[1]);
                int he1 = half_edge_idx(et, face[1], face[2]);
                int he2 = half_edge_idx(et, face[2], face[0]);
                f << "3 " << he0 << " " << he1 << " " << he2 << "\n";
            }

            //std::cout<<" faces ok"<<std::endl;

            // Polyhedra — empty for boundary-only export
            f << "Polyhedra\n" << (boundary_only ? 0 : n_cells()) << "\n";
            if (!boundary_only) {
                for (const auto &c: cells_) {
                    f << "4";
                    for (int s = 0; s < 4; ++s)
                        f << " " << half_face_idx(ft, c, s);
                    f << "\n";
                }
            }
            //std::cout<<" cells ok"<<std::endl;
        }

        void write_obj_to_stream(std::ostream& f, bool boundary_only = false) const {

            std::vector<int> idx_map(n_vertices());
            int vidx(0);
            for (int i = 0; i < n_vertices(); ++i) {
                if(!boundary_only || is_boundary(VertexHandle(i))) {
                    const Vec3d &v = vertices_[i];
                    f << "v " << v.x() << " " << v.y() << " " << v.z() << "\n";
                    idx_map[i] = vidx;
                    vidx++;
                }
            }

            const FaceTable ft = build_face_table(boundary_only);
            for (const auto& face : ft.faces)
                f << "f " << idx_map[face[0]] + 1 << " "
                  << idx_map[face[1]] + 1 << " "
                  << idx_map[face[2]] + 1 << "\n";
        }
    };

    // -------------------------------------------------------------------------
    // File I/O — format detected from path extension
    // -------------------------------------------------------------------------

    /// Write the mesh to `path`. The output format is inferred from the file
    /// extension. Currently supported: .ovm
    inline void write_to_file(const TetMesh& mesh,
                              const std::filesystem::path& path,
                              bool boundary_only = false) {
        const std::string ext = path.extension().string();

        std::ofstream f(path);
        if (!f)
            throw std::runtime_error(
                    "TetMesh::write_to_file: cannot open '" + path.string() + "'");

        if (ext == ".ovm") {
            mesh.write_ovm_to_stream(f, boundary_only);
        } else if (ext == ".obj") {
            mesh.write_obj_to_stream(f, boundary_only);
        } else {
            throw std::runtime_error(
                    "TetMesh::write_to_file: unsupported format '" + ext + "'");
        }
    }

    inline std::string mesh_to_string(const TetMesh& mesh,
                                      const std::string& format,
                                      bool boundary_only = false) {
        std::ostringstream ss;
        if (format == "obj")
            mesh.write_obj_to_stream(ss, boundary_only);
        else if (format == "ovm")
            mesh.write_ovm_to_stream(ss, boundary_only);
        else
            throw std::runtime_error("mesh_to_string: unsupported format '" + format + "'");
        return ss.str();
    }


} // namespace tet_weave