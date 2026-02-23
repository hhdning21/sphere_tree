# Sphere Tree Visualization Guide

## 🎯 Overview

The sphere tree visualization tool provides an interactive OpenGL viewer to explore how sphere trees approximate 3D models at different levels of detail.

## 🚀 Quick Start

```bash
cd build
./visualize_spheres_gl path/to/model.obj
```

## 🎮 Interactive Controls

### Mouse Controls
- **Left Click + Drag**: Rotate the model
- **Right Click + Drag**: Zoom in/out

### Keyboard Controls
- **`+` / `=`**: Increase depth level (show more detail)
- **`-` / `_`**: Decrease depth level (show less detail)
- **`H` / `h`**: Toggle hierarchy mode (show all levels at once)
- **`V` / `v`**: Toggle vertex display (show original mesh points)
- **`ESC`**: Exit the visualizer

## 🌳 Understanding the Visualization

### Color Coding
- **Green spheres**: Low approximation error (tight fit to surface)
- **Yellow spheres**: Medium approximation error
- **Red spheres**: High approximation error (loose fit)
- **White points**: Original mesh vertices (when vertex display is on)

### Depth Levels
- **Depth 0**: Single sphere covering entire model
- **Depth 1**: ~8 spheres subdividing the model
- **Depth 2**: ~64 spheres (8² with octree)
- **Depth 6**: Thousands of small spheres providing tight approximation

### Display Modes
- **Normal Mode**: Shows only spheres at current depth level
- **Hierarchy Mode** (press `H`): Shows all spheres from root down to current depth
  - Useful for understanding the tree structure
  - Shows parent-child relationships

## 📊 Algorithm Visualization

### What You're Seeing

The sphere tree is built using the **Bradshaw & O'Sullivan** adaptive medial-axis approximation algorithm:

1. **Root sphere** (depth 0): Computed using Welzl's minimal enclosing sphere
2. **Subdivision**: Points are grouped using one of:
   - **Octree**: Spatial subdivision (8 octants) - fastest
   - **K-means**: Clustering-based subdivision - better for irregular shapes
   - **Principal Axis**: Split along longest dimension
3. **Recursive construction**: Each child sphere is built from its point subset
4. **Error metric**: Green = tight fit, Red = loose fit

### Performance Example

For a dog model with 49,714 vertices:
- **Depth 0**: 1 sphere
- **Depth 1**: 8 spheres
- **Depth 2**: 64 spheres
- **Depth 6**: 3,814 spheres
- **Build time**: ~64ms

## 🔧 Advanced Usage

### Custom Configuration

Edit the configuration in the source code before building:

```cpp
// In visualize_spheres_gl.cpp, line ~200
config.maxDepth = 8;                                 // More detail
config.subdivisionStrategy = OCTREE;                 // Fast
// Or: K_MEANS for better quality
// Or: PRINCIPAL_AXIS for elongated objects
config.fittingMethod = WELZL;                        // Optimal spheres
```

### Building Trees from Code

```cpp
#include "SphereTreeAPI.h"

// Easy API
auto tree = buildTreeFromOBJ("model.obj", TreeQuality::HIGH_DETAIL);

// Custom configuration
SphereTreeConfig config;
config.maxDepth = 6;
config.subdivisionStrategy = K_MEANS;
config.fittingMethod = WELZL;
auto customTree = buildTreeFromOBJ("model.obj", config);

// Export
exportToOBJ(*tree, "spheres", 3);  // Export depth 3 to OBJ
exportToCSV(*tree, "tree.csv", 6); // Export depth 6 to CSV
```

## 🎨 Example Session

```bash
# Build the project
cd /private/tmp/sphere_tree
mkdir -p build && cd build
cmake ..
cmake --build .

# Visualize a simple cube
./visualize_spheres_gl ../test_cube.obj

# Interactive exploration:
# 1. Press '+' repeatedly to see increasing detail
# 2. Press 'H' to see the full hierarchy
# 3. Press 'V' to toggle vertex display
# 4. Drag mouse to rotate and inspect from all angles
# 5. Right-drag to zoom in on interesting areas
```

## 📈 Comparison: Quality vs Performance

### TreeQuality::FAST
- Max depth: 4
- Build time: Very fast (<10ms for small models)
- Spheres: ~512 (octree: 8⁴)
- Use case: Real-time collision detection

### TreeQuality::BALANCED
- Max depth: 6
- Build time: Fast (~50ms for medium models)
- Spheres: ~3,800
- Use case: General-purpose approximation

### TreeQuality::HIGH_DETAIL
- Max depth: 8
- Build time: Slower (~500ms for large models)
- Spheres: ~260,000
- Use case: High-fidelity rendering, detailed collision

## 🐛 Troubleshooting

### Window Doesn't Open
- Ensure you have OpenGL/GLUT installed (macOS has it by default)
- Try running from the terminal, not from Finder

### "Deprecation Warnings"
- These are cosmetic warnings from macOS
- The visualizer still works correctly
- To suppress: Add `#define GL_SILENCE_DEPRECATION` to the source

### Model Appears Too Small/Large
- Use right-click drag to zoom
- Or adjust `g_zoom` initial value in the source (line ~30)

### Crashes or No Spheres Shown
- Check that the OBJ file is valid
- Ensure it contains vertices (`v` lines)
- The visualizer only reads vertices, faces are ignored

## 📚 References

**Original Paper:**
Bradshaw, G., & O'Sullivan, C. (2004). *Adaptive medial-axis approximation for sphere-tree construction.* ACM Transactions on Graphics (TOG), 23(1), 1-26.

**Key Concepts:**
- Medial axis approximation
- Hierarchical sphere trees
- Error-driven refinement
- Collision detection optimization

---

**Happy Visualizing!** 🎉

For more information, see the main [README.md](README.md) or the source code documentation.
