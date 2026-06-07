# TetWeave

![CI](https://github.com/vnigolian/TetWeave/actions/workflows/ci.yml/badge.svg)

A lightweight header-only C++ library for generating tetrahedral meshes from parametric curves and surfaces, without any dependencies.

It can be used with the provided cli to generate meshes (either in `.obj` or `.ovm` formats), or integrated into other libraries (cf. `minimal_example/`).

An online viewer and generator is available [here](https://vnigolian.github.io/TetWeave/).


# TODOS
* fix boundary stuff for KH
* finalise readme (renders)


## Building

```bash
mkdir build && cd build
cmake ..
make
```

## Usage

```bash
./cli/tet_weave --mesh-type <type> [options] --output <file>
```

Run `./cli/tet_weave --help` for the full list of options.

---

## Mesh Types

### Voxel Grid Meshes

| Mesh Type | Preview | Arguments | Notes |
|---|---|---|---|
| `voxel-grid` | ![voxel grid](figs/voxel_grid.png) | `--voxel-grid-width` (default: 10)<br>`--voxel-grid-height` (default: 10)<br>`--voxel-grid-depth` (default: 10) | Each voxel is split into 5 tetrahedra |
| `knotted-hole` | ![knotted hole](figs/knotted_hole.png) | `--knotted-hole-segments` (default: 10) | Voxel grid with a knotted hole, following Furch's 1924 construction |

---

### Simple Meshes

| Mesh Type                        | Preview                                                                    | Arguments | Notes                                                                                                      |
|----------------------------------|----------------------------------------------------------------------------|---|------------------------------------------------------------------------------------------------------------|
| `minimal-non-star-shaped`        | ![minimal non-star-shaped](figs/minimal_non_star_shaped.png)               | — | Fixed mesh, no parameters                                                                                  |
| `minimal-non-star-shaped-domain` | ![minimal non star-shaped domain](figs/minimal_non_star_shaped_domain.png) | — | Fixed mesh, no parameters. Same connectivity as the minimal non-star-shaped mesh, but star-shaped.         |
| `rod`                            | ![rod](figs/rod.png)                                                       | `--length` (default: 10)<br>`--axial-scaling` (default: 1.0)<br>`--torsion-rad` (default: 0.0) | Straight rod mesh; torsion twists the cross-sections along the axis                                        |
| `sine`                           | ![sine](figs/sine.png)                                                     | `--length` (default: 10)<br>`--axial-scaling` (default: 1.0)<br>`--torsion-rad` (default: 0.0)<br>`--sine-period` (default: 1.0)<br>`--sine-x-scale` (default: 1.0)<br>`--sine-y-scale` (default: 1.0)<br>`--sine-z-scale` (default: 1.0) | Rod mesh deformed along a sine wave                                                                        |
| `spiral`                         | ![spiral](figs/spiral.png)                                                 | `--length` (default: 10)<br>`--torsion-rad` (default: 0.0)<br>`--spiral-x-scale` (default: 1.0)<br>`--spiral-turn-count` (default: 1.0) | Rod mesh wound into a spiral                                                                               |
| `trefoil-knot`                   | ![trefoil knot](figs/trefoil_knot.png)                                     | `--length` (default: 10)<br>`--torsion-rad` (default: 0.0)<br>`--trefoil-range` (default: 1.0) | Rod mesh deformed into a trefoil knot; `--trefoil-range` controls how much of the knot is generated [0..1] |
| `layer`                          | ![layer](figs/layer.png)                                                   | `--width` (default: 10)<br>`--height` (default: 10) | Flat width × height layer mesh                                                                             |

---

### Parametric Curve Meshes

A rod mesh is deformed so that its central axis follows a parametric curve `f(t) → R³`. The `--para-t` range controls the portion of the curve to generate, `--length` controls the number of segments, and `--para-thickness` controls the tube radius.

| Mesh Type | Preview | Arguments | Notes |
|---|---|---|---|
| `para-circle` | ![circle](figs/para_circle.png) | `--para-args radius z_scale`<br>`--para-t` (default: 0.0 1.0)<br>`--length` (default: 10)<br>`--para-thickness` | `z_scale=0` gives a flat circle; non-zero gives a helix |
| `para-trefoil-knot` | ![parametric trefoil](figs/para_trefoil.png) | `--para-t` (default: 0.0 1.0)<br>`--length` (default: 10)<br>`--para-thickness` | No `--para-args` needed |
| `para-multi-branch-spiral` | ![multi branch spiral](figs/para_multi_branch_spiral.png) | `--para-args center_radius branches branch_length z_scale`<br>`--para-t` (default: 0.0 1.0)<br>`--length` (default: 10)<br>`--para-thickness` | `branches` controls the number of spiral arms |
| `para-funny-spiral` | ![funny spiral](figs/para_funny_spiral.png) | `--para-args a b c z_scale`<br>`--para-t` (default: 0.0 1.0)<br>`--length` (default: 10)<br>`--para-thickness` | Lissajous-like curve; `a`, `b`, `c` control the frequency of each component |

---

### Parametric Surface Meshes

A layer mesh is deformed so that its central surface follows a parametric surface `f(u,v) → R³`. The `--para-u` and `--para-v` ranges control the parameter domain, `--width` and `--height` control the grid resolution, and `--para-thickness` controls the slab thickness.

| Mesh Type | Preview | Arguments | Notes |
|---|---|---|---|
| `para-sphere` | ![sphere](figs/para_sphere.png) | `--para-args radius`<br>`--para-u u_min u_max`<br>`--para-v v_min v_max`<br>`--width` (default: 10)<br>`--height` (default: 10)<br>`--para-thickness` | Avoid poles (u=0, u=π) for clean normals |
| `para-torus` | ![torus](figs/para_torus.png) | `--para-args major_radius minor_radius`<br>`--para-u u_min u_max`<br>`--para-v v_min v_max`<br>`--width` (default: 10)<br>`--height` (default: 10)<br>`--para-thickness` | Full torus: `--para-u 0.0 6.283 --para-v 0.0 6.283` |
| `para-sine3d` | ![sine3d](figs/para_sine3d.png) | `--para-args amplitude period`<br>`--para-u u_min u_max`<br>`--para-v v_min v_max`<br>`--width` (default: 10)<br>`--height` (default: 10)<br>`--para-thickness` | Sinusoidal surface: z = amplitude × sin(period × u) × sin(period × v) |
| `para-helicoidal-ring` | ![helicoidal ring](figs/para_helicoidal_ring.png) | `--para-args loops`<br>`--para-u 0.0 0.5`<br>`--para-v 0.0 0.999`<br>`--width` (default: 10)<br>`--height` (default: 10)<br>`--para-thickness` | Avoid v=1.0 (singularity); `loops` controls the number of helical turns |

---

## Output Formats

| Format | Flag | Notes |
|---|---|---|
| `.ovm` | — | Full volumetric mesh (OpenVolumeMesh ASCII format) |
| `.obj` | — | Surface mesh only (Wavefront OBJ, compatible with Blender) |
| Boundary only | `--boundary-only` | Exports only the outer surface, works with both `.ovm` and `.obj` |

Example — generate a torus and export both the full mesh and its boundary:

```bash
./cli/tet_weave --mesh-type para-torus --para-args 3.0 1.0 \
    --para-u 0.0 6.283 --para-v 0.0 6.283 \
    --width 20 --height 20 --para-thickness 0.2 \
    --output torus.ovm

./cli/tet_weave --mesh-type para-torus --para-args 3.0 1.0 \
    --para-u 0.0 6.283 --para-v 0.0 6.283 \
    --width 20 --height 20 --para-thickness 0.2 \
    --boundary-only --output torus_boundary.obj
```

---

## Adding Custom Parametric Functions

To add a new curve, add a function with this signature to `ParametricFunctions.hh`:

```cpp
Vec3d my_curve(const std::vector<double>& params, double t) {
    // params contains your custom parameters
    // t is the curve parameter
    return { ... };
}
```

For a surface:

```cpp
Vec3d my_surface(const std::vector<double>& params, double u, double v) {
    return { ... };
}
```

Then add it to the dispatch table in `main.cpp`.


