
#include "TestMeshGenerators.hh"


#if 0

void setup_minimal_non_star_shaped_mesh(TetrahedralMesh& ref_mesh,
                                        TetrahedralMesh& mesh){

    using namespace OpenVolumeMesh;

    //auto v0 = mesh.add_vertex({0,0,  0});
    auto vb = mesh.add_vertex({0,0,-1}); //here so it's still v0
    auto v1 = mesh.add_vertex({-1,-1,0});
    auto v2 = mesh.add_vertex({-1,-3,0});
    auto v3 = mesh.add_vertex({-2,1, 0});
    auto v4 = mesh.add_vertex({2,1,  0});
    auto v5 = mesh.add_vertex({1,-3, 0});
    auto v6 = mesh.add_vertex({1,-1, 0});

    auto vt = mesh.add_vertex({0,0, 1});

    mesh.add_cell({ v2, v1,vb, vt});
    mesh.add_cell({v3, v2, vb, vt});
    mesh.add_cell({v4, v3, vb, vt});
    mesh.add_cell({v5, v4, vb, vt});
    mesh.add_cell({v6, v5, vb, vt});
    mesh.add_cell({v1, v6, vb, vt});

    mesh.split_edge(mesh.edge_handle(mesh.find_halfedge(vb, vt)));
    mesh.collect_garbage();

    ref_mesh = mesh;
    //ref_mesh.set_vertex(VertexHandle(0), {0, 0.5,0});
    ref_mesh.set_vertex(v1, {-1, -3,0});
    ref_mesh.set_vertex(v2, {-2, -3,0});
    ref_mesh.set_vertex(v5, {2, -3, 0});
    ref_mesh.set_vertex(v6, {1, -3, 0});

    for(auto v: mesh.vertices()){
        std::cout<<" - vertex "<<v<<" is "<<(mesh.is_boundary(v) ? "boundary" : "interior")<<std::endl;
    }

    //mesh.split_edge(OpenVolumeMesh::EdgeHandle(7));

    std::cout<<" - generated min non-star-shaped mesh with "<<mesh.n_cells()<<" cells "<<std::endl;
}



/*
 * x--x-...-x-...-x
 * |        x     |
 * x--x-...-x-...-x
 *
 */
void setup_straight_mesh(TriangleMesh& ref_mesh,
                         TriangleMesh& mesh,
                         int length,
                         double ref_mesh_axial_scaling){

    if(length < 1){
        std::cout<<" - warning, clamped mesh length to 1"<<std::endl;
        length = 1;
    }

    auto v0 = mesh.add_vertex({length * 0.5, 0.5});
    auto v1 = mesh.add_vertex({0,1});
    auto v2 = mesh.add_vertex({0,0});

    //std::cout<<" - added vertices"<<std::endl;

    mesh.add_face({v1,v2,v0});

    //std::cout<<" - added first face"<<std::endl;
    /* a--b
     * \ /
     * v0
     * / \
     * c--d */
    OVM::VertexHandle a = v1;
    OVM::VertexHandle b(-1);
    OVM::VertexHandle c = v2;
    OVM::VertexHandle d(-1);
    for(int i(0); i < length; i++){
        double x = (i+1);
        b = mesh.add_vertex({x,1});
        d = mesh.add_vertex({x,0});

        mesh.add_face({b,a,v0});
        mesh.add_face({c,d,v0});

        a = b;
        c = d;

        //std::cout<<" - added face"<<std::endl;
    }

    mesh.add_face({c,a,v0});

    ref_mesh = mesh;

    if (ref_mesh_axial_scaling != 1.0) {
        for (auto vh: ref_mesh.vertices()) {
            auto pos = ref_mesh.vertex(vh);
            pos[0] *= ref_mesh_axial_scaling;
            ref_mesh.set_vertex(vh, pos);
        }
    }

    //std::cout<<" - added last face"<<std::endl;
}


void setup_straight_mesh(TetrahedralMesh& ref_mesh,
                         TetrahedralMesh& mesh,
                         int length,
                         double ref_mesh_axial_scaling,
                         double torsion){

    if(length < 1) {
        std::cout<<" - warning, clamped mesh length to 1"<<std::endl;
        length = 1;
    }

    //the only interior vertex
    auto v0 = mesh.add_vertex({0.0, 0.0, length * 0.5});

    //add vertices
    /* b---c
     * | / |
     * a---d /*/
    for(int i(0); i <= length; i++){
        mesh.add_vertex({-0.5, -0.5, i});
        mesh.add_vertex({ 0.5, -0.5, i});
        mesh.add_vertex({ 0.5,  0.5, i});
        mesh.add_vertex({-0.5,  0.5, i});
    }
    std::cout<<" - added "<<mesh.n_vertices()<<" vertices"<<std::endl;

    //add the first cells
    mesh.add_cell({VertexHandle(1), VertexHandle(2), VertexHandle(3), v0});
    mesh.add_cell({VertexHandle(1), VertexHandle(3), VertexHandle(4), v0});


    //std::cout<<" - added first face"<<std::endl;
    for(int i(0); i < length; i++){

        int c = i * 4;
        //right
        mesh.add_cell({VertexHandle(c + 0 + 1), VertexHandle(c + 3 + 1), VertexHandle(c + 4 + 1), v0});
        mesh.add_cell({VertexHandle(c + 4 + 1), VertexHandle(c + 3 + 1), VertexHandle(c + 7 + 1), v0});

        //top
        mesh.add_cell({VertexHandle(c + 3 + 1), VertexHandle(c + 2 + 1), VertexHandle(c + 6 + 1), v0});
        mesh.add_cell({VertexHandle(c + 3 + 1), VertexHandle(c + 6 + 1), VertexHandle(c + 7 + 1), v0});

        //left
        mesh.add_cell({VertexHandle(c + 2 + 1), VertexHandle(c + 1 + 1), VertexHandle(c + 5 + 1), v0});
        mesh.add_cell({VertexHandle(c + 2 + 1), VertexHandle(c + 5 + 1), VertexHandle(c + 6 + 1), v0});

        //top
        mesh.add_cell({VertexHandle(c + 0 + 1), VertexHandle(c + 5 + 1), VertexHandle(c + 1 + 1), v0});
        mesh.add_cell({VertexHandle(c + 0 + 1), VertexHandle(c + 4 + 1), VertexHandle(c + 5 + 1), v0});
    }

    int c = (length -1) * 4;
    std::cout<<" c = "<<c<<std::endl;
    mesh.add_cell({VertexHandle(c + 4 + 1), VertexHandle(c + 7 + 1), VertexHandle(c + 6 + 1), v0});
    mesh.add_cell({VertexHandle(c + 4 + 1), VertexHandle(c + 6 + 1), VertexHandle(c + 5 + 1), v0});

    //mesh.split_edge(EdgeHandle(3));

    ref_mesh = mesh;

    double d_rot = M_PI * (torsion / (length + 1))/180.0;

    for(int i(0); i <= length; i++){
        int start_idx = 4 * i + 1;
        double theta = d_rot * i;
        //std::cout<<" theta_"<<i<<" = "<<theta<<std::endl;
        for(int j(0); j<4; j++){
            VertexHandle v(start_idx + j);
            auto pos = mesh.vertex(v);
            Eigen::Vector3d eigen_pos = vec2vec(pos);
            Eigen::Matrix3d rot;
            rot << std::cos(theta), -std::sin(theta), 0,
                   std::sin(theta), std::cos(theta), 0,
                   0,               0,               1;
            auto rot_pos = vec2vec((rot * eigen_pos).eval());
            mesh.set_vertex(v, rot_pos);
        }
    }

    if (ref_mesh_axial_scaling != 1.0) {
        for (auto vh: ref_mesh.vertices()) {
            auto pos = ref_mesh.vertex(vh);
            pos[2] *= ref_mesh_axial_scaling;
            ref_mesh.set_vertex(vh, pos);
        }
    }

    //std::cout<<" - added last face"<<std::endl;
}

void setup_sine_mesh(TriangleMesh& ref_mesh,
                     TriangleMesh& mesh,
                     int length,
                     double period,
                     double x_scale,
                     double y_scale){

    setup_straight_mesh(ref_mesh, mesh, length, 1.0);

    x_scale = x_scale / length;

    for(auto v: mesh.vertices()){
        auto pos = mesh.vertex(v);
        //std::cout<<" - v"<<v<<" at "<<pos[0]<<std::endl;
        auto x = pos[0];
        auto y = pos[1];

        auto sin_x = 2 * M_PI * x / length;
        //std::cout<<" -- sin x: "<<sin_x<<std::endl;

        y += y_scale * std::sin(period * sin_x);

        auto scaled_x = sin_x * x_scale;
        mesh.set_vertex(v,{scaled_x, y});
        //std::cout<<" -- scaled x: "<<scaled_x<<std::endl;
    }

    std::cout<<" - setup sine mesh"<<std::endl;
}


void setup_sine_mesh(TetrahedralMesh& ref_mesh,
                     TetrahedralMesh& mesh,
                     int length,
                     double torsion,
                     double period,
                     double x_scale,
                     double y_scale,
                     double z_scale){

    setup_straight_mesh(ref_mesh, mesh, length, 1.0, torsion);

    z_scale = z_scale / length;
    double x_period = 1.0;

    for(auto v: mesh.vertices()){
        auto pos = mesh.vertex(v);
        //std::cout<<" - v"<<v<<" at "<<pos[0]<<std::endl;
        auto x = pos[0];
        auto y = pos[1];
        auto z = pos[2];

        auto sin_z = 2 * M_PI * z / length;

        //std::cout<<" - x = "<<x<<std::endl;
        y += y_scale * std::sin(period * sin_z);
        //std::cout<<" - sine(T sin_z) = "<<std::sin(period * sin_z)<<std::endl;
        x += x_scale * std::sin(x_period * sin_z);
        //std::cout<<" - x + sine(T sin_z) = "<<x<<std::endl;
        //std::cout<<" ------------------------- "<<std::endl;

        auto scaled_z = sin_z * z_scale;
        mesh.set_vertex(v, {x, y, scaled_z});
        //std::cout<<" -- scaled x: "<<scaled_x<<std::endl;
    }

    std::cout<<" - setup sine mesh"<<std::endl;
}


void setup_spiral_mesh(TriangleMesh& ref_mesh,
                       TriangleMesh& mesh,
                       int length,
                       bool compress_ref_mesh_to_cube,
                       double turn_count){

    double length_by_turn = (double)length / turn_count;

    std::cout<<" len by turn: "<<length_by_turn<<", turn count: "<<turn_count<<std::endl;

    double axial_scaling = compress_ref_mesh_to_cube ? 1.0 / (double)length : 1.0;
    setup_straight_mesh(ref_mesh, mesh, length, axial_scaling);

    double dr = 2.0 * (double)turn_count / (double)length;
    //dr = 2.0 / length_by_turn;

    for(auto v: mesh.vertices()){
        auto pos = mesh.vertex(v);
        auto x = pos[0];
        auto y = pos[1];
        double t = - (x * 2 * M_PI) / length_by_turn;
        double r = 1.0 + y + x * dr;
        OVM::Vec2d spos = {r * std::cos(t), r * std::sin(t)};
        //std::cout<<" - v"<<v<<" at "<<pos<<" moved to "<<spos<<std::endl;
        if(!y) {
            std::cout << " - x = " << x << ", r = " << r << std::endl;
        }

        mesh.set_vertex(v, spos);
    }

}

void setup_spiral_mesh(TetrahedralMesh & ref_mesh,
                       TetrahedralMesh& mesh,
                       int length,
                       bool compress_ref_mesh_to_cube,
                       double torsion,
                       double x_scale,
                       double turn_count){

    double length_by_turn = (double)length / turn_count;

    std::cout<<" len by turn: "<<length_by_turn<<", turn count: "<<turn_count<<std::endl;

    double axial_scaling = compress_ref_mesh_to_cube ? 1.0 / (double)length : 1.0;
    setup_straight_mesh(ref_mesh, mesh, length, axial_scaling, torsion);

    double dr = 2.0 * (double)turn_count / (double)length;
    //dr = 2.0 / length_by_turn;

    double dx = x_scale / mesh.n_vertices();
    int i(0);
    for(auto v: mesh.vertices()){
        auto pos = mesh.vertex(v);
        auto x = pos[0];
        auto y = pos[1];
        auto z = pos[2];

        double t =  (z * 2 * M_PI) / length_by_turn;
        double r = 1.0 + y + z * dr;
        OVM::Vec3d spos = {x + i * dx, r * std::cos(t), r * std::sin(t)};
        //std::cout<<" - v"<<v<<" at "<<pos<<" moved to "<<spos<<std::endl;
        if(!y) {
            std::cout << " - x = " << x << ", r = " << r << std::endl;
        }

        mesh.set_vertex(v, spos);
        i++;
    }

}


void setup_trefoil_knot_mesh(TetrahedralMesh & ref_mesh,
                             TetrahedralMesh& mesh,
                             int length,
                             double torsion,
                             double range){


    //double length = length_by_turn * turn_count;
    setup_straight_mesh(ref_mesh, mesh, length, 1.0, torsion);

    //double dr = 2.0 * (double)turn_count / (double)length;
    //dr = 2.0 / length_by_turn;

    //std::cout<<" - curve points: "<<std::endl;
    std::vector<OVM::Vec3d> curve_points;
    for(int i(0); i <= length; i++){

        double t = range * (2.0 * M_PI * i) / (length);

        OVM::Vec3d curve_pos = { std::sin(t) + 2 * std::sin(2 * t),
                                 std::cos(t) - 2 * std::cos(2 * t),
                                 -std::sin(3 * t)};
        //OVM::Vec3d curve_pos = {0,0,t};

        curve_points.push_back(curve_pos);
        //std::cout<<" - point "<<i<<" for t = "<<t<<" : "<<curve_pos<<std::endl;
    }

    double orthogonal_scaling(0.5);
    int i(0);
    for(auto v: mesh.vertices()){
        if(!i){
            //std::cout<<" - skipping interior vertex"<<std::endl;
            i++;
            continue;
        }
        //std::cout<<" ---------------------------------------------------------- "<<std::endl;
        //std::cout<<" - handling vertex "<<v<<std::endl;
        int point_idx = (i - 1) / 4;
        int next_point_idx = point_idx + 1;
        bool last_point(false);
        if(next_point_idx >= curve_points.size()){
            last_point = true;
            //std::cout<<" -> last point"<<std::endl;
            next_point_idx = point_idx;
            point_idx = next_point_idx - 1;
        }

        //std::cout<<" -      point idx: "<<point_idx<<std::endl;
        //std::cout<<" - next point idx: "<<next_point_idx<<std::endl;

        auto pos = mesh.vertex(v);
        auto x = pos[0];
        auto y = pos[1];
        auto z = pos[2];

        auto curve_pos = curve_points[point_idx];
        auto next_curve_pos = curve_points[next_point_idx];
        //std::cout<<" - next curve point: "<<next_curve_pos<<std::endl;

        //tangent
        auto t = -(next_curve_pos - curve_pos).normalized();
        if(last_point){
            curve_pos = next_curve_pos;
        }
        //std::cout<<" -      curve point: "<<curve_pos<<std::endl;
        //std::cout<<" - tangent: "<<t<<std::endl;

        // first perpendicular vector (with projection)
        auto t_proj = t;
        double sign = t_proj[2] > 0 ? 1 : -1;
        t_proj[2] = 0;
        auto tp1 = sign * t_proj.cross(t);
        tp1.normalize();
        //std::cout<<" - orthogonal axis 1: "<<tp1<<std::endl;

        auto tp2 = tp1.cross(t);
        tp2.normalize();
        /*std::cout<<" - orthogonal axis 2: "<<tp2<<std::endl;

        std::cout<<" - dot prod check 1: "<<tp1.dot(t)<<std::endl;
        std::cout<<" - dot prod check 2: "<<tp2.dot(t)<<std::endl;*/

        OVM::Vec3d new_pos = curve_pos + orthogonal_scaling * x * tp1 + orthogonal_scaling * y * tp2;
        //std::cout<<" - new pos: "<<new_pos<<std::endl;

        //std::cout<<" - v"<<v<<" at "<<pos<<" moved to "<<new_pos<<std::endl;

        mesh.set_vertex(v, new_pos);
        i++;
    }
    //mesh.delete_vertex(VertexHandle(0));
    //mesh.collect_garbage();

}

Eigen::Vector2d parametric_circle(argparse::ArgumentParser& parser, double t){
    return {std::cos(t), std::sin(t)};
}


Eigen::Vector2d parametric_clover(argparse::ArgumentParser& parser, double t){

    double center_radius = parser.get<double>("--clover-center-radius");
    double center_shift = parser.get<double>("--clover-shift");
    int leaves = parser.get<int>("--clover-leaves");

    if(center_shift > 2.0 * center_radius){
        center_shift = 2.0 * center_radius;
        //std::cout<<" WARNING - clamped clover shift to "<<center_shift<<std::endl;
    }

    double radius = std::sin(leaves * t) + (1 + center_radius);

    return {radius * std::cos(t) + center_shift * std::sin(t), radius * std::sin(t)};
}

Eigen::Vector2d parametric_teeth(argparse::ArgumentParser& parser, double t) {

    double teeth_count = parser.get<double>("--mouth-teeth-count");
    double opening = parser.get<double>("--mouth-opening");

    return {(std::cos(std::sin(2 * t) ) + 1.1) * sin(-t),
            (std::cos(2 *  teeth_count * t + 1) + 1 + opening) * std::cos(-t)};
}


Eigen::Vector2d parametric_spiral(argparse::ArgumentParser& parser, double t){

    double center_radius = parser.get<double>("--paraspiral-center-radius");
    int branches = parser.get<int>("--paraspiral-branches");
    double branch_length = parser.get<double>("--paraspiral-branch-len");

    double radius = (std::cos(branches * t) + 1.0 + center_radius);
    double angle = radius * branch_length;

    Eigen::Matrix2d rotation;
    rotation << std::cos(angle), -std::sin(angle),
                std::sin(angle),  std::cos(angle);
    Eigen::Vector2d v({std::cos(t), std::sin(t)});

    return radius * rotation * v;
}



Eigen::Vector3d parametric_spiral_3D(argparse::ArgumentParser& parser, double t){
    double center_radius = parser.get<double>("--paraspiral-center-radius");
    int branches = parser.get<int>("--paraspiral-branches");
    double branch_length = parser.get<double>("--paraspiral-branch-len");
    double z_scale = parser.get<double>("--parametric-z-scale");

    double radius = (std::cos(branches * t) + 1.0 + center_radius);
    double angle = radius * branch_length;

    Eigen::Matrix2d rotation;
    rotation << std::cos(angle), -std::sin(angle),
            std::sin(angle),  std::cos(angle);
    Eigen::Vector2d v({std::cos(t), std::sin(t)});

    Eigen::Vector2d point = radius * rotation * v;
    Eigen::Vector3d point_3d;
    point_3d << point[0], point[1], z_scale * t;

    return point_3d;
}

Eigen::Vector3d parametric_trefoil_knot(argparse::ArgumentParser& parser, double t){
    return { std::sin(t) + 2 * std::sin(2 * t),
             std::cos(t) - 2 * std::cos(2 * t),
             -std::sin(3 * t)};
}


Eigen::Vector3d parametric_complex_spiral(argparse::ArgumentParser& parser, double t){
    double a = parser.get<double>("--complex-spiral-a");
    double b = parser.get<double>("--complex-spiral-b");
    double c = parser.get<double>("--complex-spiral-c");
    double z_scale = parser.get<double>("--parametric-z-scale");

    return {std::cos(a * t) + std::cos(b * t) / 2.0 + std::sin(c * t) / 3.0,
            std::sin(a * t) + std::sin(b * t) / 2.0 + std::cos(c * t) / 3.0,
            z_scale * t};
}


void setup_parametric_mesh_with_ref(TriangleMesh& ref_mesh,
                                    TriangleMesh& mesh,
                                    ParametricCurve2D curve,
                                    argparse::ArgumentParser& parser){

    setup_parametric_mesh(ref_mesh, parametric_circle, parser);

    setup_parametric_mesh(mesh, curve, parser);

}


void setup_parametric_mesh(TriangleMesh& mesh,
                           ParametricCurve2D curve,
                           argparse::ArgumentParser& parser){

    int v_count = parser.get<int>("--mesh-len");

    double dt = 2.0 * M_PI / v_count;

    for(int i(0); i<v_count; i++){
        //std::cout<<" - t = "<<(i * dt)<<std::endl;
        auto pos = curve(parser, i * dt);
        auto v = mesh.add_vertex({pos[0], pos[1]});
        //std::cout<<" - added vertex "<<v<<" at "<<pos.transpose()<<std::endl;
    }

    auto center_v = mesh.add_vertex({0, 0});

    for(int i(0); i < v_count; i++){
        int a = i;
        int b = (i+1) % v_count;
        //std::cout<<" - adding face "<<a<<", "<<b<<", "<<center_v<<std::endl;
        mesh.add_face({VertexHandle(a), VertexHandle(b), center_v});
    }

    return; //below is to debug shape
    mesh.delete_vertex(center_v);
    mesh.collect_garbage();
}


void setup_parametric_mesh_with_ref(TetrahedralMesh& ref_mesh,
                                    TetrahedralMesh& mesh,
                                    ParametricCurve3D curve,
                                    argparse::ArgumentParser& parser){

    setup_straight_mesh(ref_mesh, mesh, parser.get<int>("--mesh-len"), 1.0, parser.get<double>("--torsion"));

    setup_parametric_mesh(mesh, curve, parser);

}


void setup_parametric_mesh(TetrahedralMesh& mesh,
                           ParametricCurve3D curve,
                           argparse::ArgumentParser& parser){


    int length = parser.get<int>("--mesh-len");

    double range = parser.get<double>("--parametric-range");
    //std::cout<<" - curve points: "<<std::endl;
    std::vector<Eigen::Vector3d> curve_points;
    for(int i(0); i <= length; i++){

        double t = range * (2.0 * M_PI * i) / (length);

        curve_points.push_back(curve(parser, t));
        //std::cout<<" - point "<<i<<" for t = "<<t<<" : "<<curve_pos<<std::endl;
    }

    double orthogonal_scaling = parser.get<double>("--parametric-width");
    int i(0);
    for(auto v: mesh.vertices()){
        if(!i){
            //std::cout<<" - skipping interior vertex"<<std::endl;
            i++;
            continue;
        }
        //std::cout<<" ---------------------------------------------------------- "<<std::endl;
        //std::cout<<" - handling vertex "<<v<<std::endl;
        int point_idx = (i - 1) / 4;
        int next_point_idx = point_idx + 1;
        bool last_point(false);
        if(next_point_idx >= curve_points.size()){
            last_point = true;
            //std::cout<<" -> last point"<<std::endl;
            next_point_idx = point_idx;
            point_idx = next_point_idx - 1;
        }

        //std::cout<<" -      point idx: "<<point_idx<<std::endl;
        //std::cout<<" - next point idx: "<<next_point_idx<<std::endl;

        auto pos = mesh.vertex(v);
        auto x = pos[0];
        auto y = pos[1];
        auto z = pos[2];

        Eigen::Vector3d curve_pos = curve_points[point_idx];
        Eigen::Vector3d next_curve_pos = curve_points[next_point_idx];
        //std::cout<<" - next curve point: "<<next_curve_pos<<std::endl;

        //tangent
        Eigen::Vector3d t = -(next_curve_pos - curve_pos).normalized();
        if(last_point){
            curve_pos = next_curve_pos;
        }
        //std::cout<<" -      curve point: "<<curve_pos<<std::endl;
        //std::cout<<" - tangent: "<<t<<std::endl;

        // first perpendicular vector (with projection)
        Eigen::Vector3d t_proj = t;
        double sign = t_proj[2] == 0 ? 0 :(t_proj[2] > 0 ? 1 : -1);
        if(sign){
            t_proj[2] = 0;
        }else{
            t_proj = {0,0,1};
            sign = 1;
        }
        Eigen::Vector3d tp1 = sign * t_proj.cross(t);
        tp1.normalize();
        //std::cout<<" - orthogonal axis 1: "<<tp1.transpose()<<std::endl;

        Eigen::Vector3d tp2 = tp1.cross(t);
        tp2.normalize();

        //std::cout<<" - orthogonal axis 2: "<<tp2.transpose()<<std::endl;
        //std::cout<<" - dot prod check 1: "<<tp1.dot(t)<<std::endl;
        //std::cout<<" - dot prod check 2: "<<tp2.dot(t)<<std::endl;

        Eigen::Vector3d new_pos = curve_pos + orthogonal_scaling * x * tp1 + orthogonal_scaling * y * tp2;
        //std::cout<<" - new pos: "<<new_pos<<std::endl;

        //std::cout<<" - v"<<v<<" at "<<pos<<" moved to "<<new_pos<<std::endl;

        mesh.set_vertex(v, vec2vec(new_pos));
        i++;
    }
    return; //below is to debug shape
    mesh.delete_vertex(VertexHandle(0));
    mesh.collect_garbage();
}




void setup_compression_disc_mesh(TriangleMesh& ref_mesh,
                                 TriangleMesh& mesh,
                                 argparse::ArgumentParser& parser) {

    double compression_ratio = parser.get<double>("--compression-disc-ratio");

    std::cout << " - generating compression disc mesh with ratio " << compression_ratio << std::endl;

    ref_mesh.clear();
    setup_parametric_mesh(ref_mesh, parametric_circle, parser);
    //assumes that the center vertex is the last-added one
    ref_mesh.delete_vertex(OVM::VertexHandle((int) ref_mesh.n_vertices() - 1));
    ref_mesh.collect_garbage();

    fill_boundary_with_CDT(ref_mesh, parser);

    Vec2d bbox_min(1, 1), bbox_max(1, 1);
    bbox_min *= std::numeric_limits<double>::max();
    bbox_max *= std::numeric_limits<double>::lowest();

    for (auto v: ref_mesh.vertices()) {
        auto pos = ref_mesh.vertex(v);

        for (int i(0); i < pos.size(); i++) {
            bbox_min[i] = std::min(bbox_min[i], pos[i]);
            bbox_max[i] = std::max(bbox_max[i], pos[i]);
        }
    }

    auto center = bbox_min + 0.5 * (bbox_max - bbox_min);


    mesh = ref_mesh;

    for (auto v: mesh.vertices()) {
        auto pos = mesh.vertex(v);
        auto compressed_pos = center + (pos - center) * compression_ratio;
        mesh.set_vertex(v, {compressed_pos[0], pos[1]});
    }
}



void fill_boundary_with_CDT(TriangleMesh& ref_mesh,
                                argparse::ArgumentParser& parser){


    double interior_to_boundary_v_count_ratio = parser.get<double>("--i-to-b-v-count-ratio");


    //add some random vertices
    const int random_v_count(interior_to_boundary_v_count_ratio * parser.get<int>("--mesh-len"));
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<double> r_dis(0.01, 0.9);
    std::uniform_real_distribution<double> theta_dis(0.0, 2 * M_PI);

    // Generate two random doubles

    //rings for now
    for(int i(0); i<random_v_count; i++){
        double tmp;
        double r = r_dis(gen);
        double theta = 2 * M_PI * theta_dis(gen);
        Vec2d pos(r * std::cos(theta), r * std::sin(theta));

        /*std::cout<<" ---------------- "<<std::endl;
        std::cout<<"     r = "<<r<<std::endl;
        std::cout<<" theta = "<<theta<<std::endl;
        std::cout<<"    pos: "<<pos<<std::endl;*/

        ref_mesh.add_vertex(pos);
    }

    apply_CDT(ref_mesh);
}



void convert_to_compression_mesh(TriangleMesh& ref_mesh,
                                 TriangleMesh& mesh,
                                 argparse::ArgumentParser& parser){

    std::cout<<" ---- converting mesh to compression test version"<<std::endl;

    OVM::IO::FileManager fm;
    fm.writeFile("compr_init_ref_mesh.ovm", ref_mesh);
    fm.writeFile("compr_init_mesh.ovm", mesh);

    auto interior_vertex_prop = mesh.request_vertex_property<bool>();
    for(auto e: ref_mesh.edges()){
        int val(0);
        for(auto ef_it = ref_mesh.ef_iter(e); ef_it.valid(); ef_it++){
            val++;
        }
        if(val == 1){
            interior_vertex_prop[ref_mesh.edge(e).from_vertex()] = true;
            interior_vertex_prop[ref_mesh.edge(e).to_vertex()] = true;
        }
    }
    std::vector<VertexHandle> to_delete;
    for(auto v: ref_mesh.vertices()){
        if(!interior_vertex_prop[v]){
            to_delete.push_back(v);
        }
    }
    for(auto v: to_delete){
        ref_mesh.delete_vertex(v);
        mesh.delete_vertex(v);
        std::cout<<" - deleted interior vertex "<<v<<std::endl;
    }


    ref_mesh.collect_garbage();
    mesh.collect_garbage();

    const int relaxation_iterations(10);
    for(int i(0); i<relaxation_iterations; i++) {

        //relax boundary vertices
        for(auto v: ref_mesh.vertices()){
            Vec2d neighbors_average(0,0);
            double neighbors_count(0);
            for(auto vv_it = ref_mesh.vv_iter(v); vv_it.valid(); vv_it++){
                neighbors_average += ref_mesh.vertex(*vv_it);
                neighbors_count++;
            }

            neighbors_average /= neighbors_count;
            ref_mesh.set_vertex(v, neighbors_average);
        }

        Vec2d centroid(0,0);
        for (auto v: ref_mesh.vertices()) {
            centroid += ref_mesh.vertex(v);
        }
        centroid /= ref_mesh.n_vertices();

        for (auto v: ref_mesh.vertices()) {
            auto pos = ref_mesh.vertex(v);
            pos = (pos - centroid).normalized();
            ref_mesh.set_vertex(v, pos);
        }
        //fm.writeFile("ref_mesh_it"+std::to_string(i)+".ovm", ref_mesh);
    }

    fill_boundary_with_CDT(ref_mesh, parser);

    //fm.writeFile("compr_ref_mesh.ovm", ref_mesh);

    TriangleMesh boundary_mesh = mesh;
    //fm.writeFile("compr_boundary_mesh.ovm", boundary_mesh);

    mesh.clear();
    mesh = ref_mesh;

    /*std::cout<<" - boundary mesh #verts: "<<boundary_mesh.n_vertices()<<std::endl;
    std::cout<<" -      ref mesh #verts: "<<ref_mesh.n_vertices()<<std::endl;
    std::cout<<" -          mesh #verts: "<<mesh.n_vertices()<<std::endl;

    fm.writeFile("compr_ref_copy.ovm", boundary_mesh);*/

    //set the interior
    for(auto v: mesh.vertices()){
        mesh.set_vertex(v, {0,0});
        //std::cout<<" - moved vertex "<<v<<" to origin"<<std::endl;
    }

    //fm.writeFile("compr_origin.ovm", boundary_mesh);

    //and then the boundary
    for(auto v: boundary_mesh.vertices()){
        mesh.set_vertex(v, boundary_mesh.vertex(v));
        //std::cout<<" - moved boundary vertex "<<v<<" to "<<mesh.vertex(v)<<std::endl;
    }

    //fm.writeFile("compr_boundary_set.ovm", boundary_mesh);

    //fm.writeFile("boundary_mesh.ovm", boundary_mesh);
    //fm.writeFile("temp_input_mesh.ovm", mesh);
}

#endif