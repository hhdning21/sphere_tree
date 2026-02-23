# Sphere Tree Approximation Library

A C++ library for building hierarchical sphere tree approximations of 3D point clouds. Provides simple API to load OBJ files and construct multi-level sphere trees with quality presets.

Based on research by Bradshaw & O'Sullivan (ACM Transactions on Graphics, 2004): "Adaptive Medial-Axis Approximation for Sphere-Tree Construction"

## 🚀 Quick Start

```cpp
#include "SphereTreeAPI.h"
using namespace spheretree;

// Build tree from OBJ file with quality preset
auto tree = buildTreeFromOBJ("model.obj", TreeQuality::BALANCED);

// Export for visualization
exportToJSON(*tree, "tree_data.json");
exportToOBJ(*tree, "spheres", 3);  // First 3 levels

// Check collision with point
if (tree->collidesWith(Point3D(x, y, z))) {
    std::cout << "Point inside model!" << std::endl;
}
```

## 📚 Documentation

| Document | Description |
|----------|-------------|
| **[VISUALIZATION_GUIDE.md](VISUALIZATION_GUIDE.md)** | 🎨 Interactive OpenGL viewer guide with controls |

## 📦 Build & Run

```bash
# Build everything
mkdir -p build && cd build
cmake ..
cmake --build .

# Run examples
./example_usage              # Basic sphere tree examples
./example_obj_loader         # Load OBJ files and build trees
./example_simple_api         # Simplified API demonstrations

# Interactive visualization (OpenGL)
./visualize_spheres_gl ../test_cube.obj
./visualize_spheres_gl path/to/your/model.obj
```

## 🎮 Visualization Controls

```
Mouse:
  Left Drag   → Rotate model
  Right Drag  → Zoom in/out

Keyboard:
  +/-         → Increase/decrease depth level
  H           → Toggle hierarchy mode (show all levels)
  V           → Toggle vertex display
  ESC         → Exit
```

## Features

- **Hierarchical Sphere Tree Structure**: Build multi-level sphere trees where spheres get smaller as you go deeper in the tree
- **Multiple Subdivision Strategies**:
  - Adaptive medial axis approximation (Bradshaw & O'Sullivan method)
  - K-means clustering
  - Octree-based spatial subdivision (Palmer & Grimsdale, Hubbard, O'Sullivan & Dingliana)
  - Principal axis splitting (Quinlan's method)
- **Multiple Sphere Fitting Methods**:
  - Welzl's minimal enclosing sphere algorithm (optimal for root sphere)
  - Ritter's bounding sphere algorithm (fast approximation)
  - Iterative refinement (better fit)
- **Sphere Generation Algorithms** (for medial axis approach):
  - Merge: Combine pairs of spheres using three-candidate selection
  - Burst: Remove spheres and let neighbors fill gaps
  - Expand & Select (MaxElim): Uniform error distribution
- **Error Metrics**: Sphere error computed as maximum protrusion distance (e = r - ||q - v||)
- **Efficient Collision Detection**:
  - Point-sphere collision (via `collidesWith` method)
  - Sphere-sphere collision
  - Hierarchical early rejection
- **Flexible Configuration**: Customizable tree depth, branching factor, and construction parameters
- **Export Options**: JSON, CSV, and OBJ export for visualization and analysis

## Tree Structure

The sphere tree is a hierarchical data structure where:
- The root node contains the largest sphere that bounds all points
- Each node can have multiple child nodes (configurable branching factor)
- Child nodes have smaller spheres that bound subsets of points
- Leaf nodes contain the actual point data
- Tree depth increases → sphere sizes decrease

```
Root Sphere (largest)
├── Child Sphere 1
│   ├── Grandchild 1a (smaller)
│   └── Grandchild 1b (smaller)
└── Child Sphere 2
    ├── Grandchild 2a (smaller)
    └── Grandchild 2b (smaller)
```

## Algorithm Details

### Bradshaw & O'Sullivan Method (Adaptive Medial Axis)

The library implements the adaptive medial-axis approximation algorithm from Bradshaw & O'Sullivan (2004). Key features:

1. **Root Sphere Construction**: Uses Welzl's minimal enclosing ball algorithm for optimal root sphere
2. **Medial Axis Approximation**: Uses Voronoi diagram to approximate the object's medial axis (skeleton)
   - Spheres placed at internal Voronoi vertices
   - Each vertex is equidistant from forming points, creating natural sphere centers
3. **Adaptive Refinement**: Iteratively improves approximation
   - Identifies worst-fitting sphere
   - Adds new sample point at location of maximum error
   - Updates Voronoi diagram to create tighter fitting spheres
4. **Error Metric**: e = r - ||q - v||
   - e: distance sphere protrudes past surface
   - r: sphere radius
   - v: sphere center (Voronoi vertex)
   - q: closest surface point
5. **Object Segmentation**: Divides object into non-overlapping regions using dividing planes
   - Prevents redundant coverage of overlapping areas
   - Optimizes plane positions for even distribution
6. **Sphere Generation Methods**:
   - **Merge**: Combines neighboring sphere pairs using three-candidate selection (grow sphere 1, grow sphere 2, or minimal enclosing)
   - **Burst**: Removes spheres and expands neighbors to fill gaps (better error distribution)
   - **Expand & Select (MaxElim)**: Expands spheres to uniform error threshold, selects minimal covering set

### Comparison with Other Methods

- **Octree Method** (Palmer & Grimsdale, Hubbard): Simple recursive subdivision, fast but poor fit
- **Quinlan's Method**: Uniform leaf spheres, split along longest axis of bounding box
- **Medial Axis (Hubbard)**: Static medial axis, can have coverage gaps
- **Adaptive Medial Axis (This Implementation)**: Ensures complete coverage, tighter fit, adaptive refinement

## Building the Library

### Prerequisites
- CMake 3.10 or higher
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)

### Build Instructions

```bash
# Clone or download the repository
cd sphere_tree

# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
cmake --build .

# Run examples
./example_usage              # Basic examples
./example_obj_loader         # Test with dog_v2.obj file

# Visualize results (requires Python + matplotlib)
python3 ../visualize_spheres.py sphere_tree.json --vertices vertices.csv

# (Optional) Install
sudo cmake --install .
```

### Running with Your Own OBJ Files

```bash
# Test with a specific OBJ file
./example_obj_loader path/to/your/model.obj

# Then visualize
./visualize_spheres_gl path/to/your/model.obj
```

## Visualization

The API includes an interactive OpenGL visualization tool to see how spheres approximate your models at each depth level.

### C++ OpenGL Visualization

The built-in C++ viewer uses OpenGL/GLUT:

```bash
cd build
./visualize_spheres_gl ../dog_v2/10680_Dog_v2.obj
```

**Interactive Controls:**
- **Mouse left drag**: Rotate view
- **Mouse right drag**: Zoom in/out
- **+/- keys**: Change depth level
- **V key**: Toggle vertices on/off
- **W key**: Toggle wireframe/solid
- **S key**: Toggle spheres on/off
- **R key**: Reset view
- **Q/ESC**: Quit

Spheres are color-coded by error (🟢 green = tight fit, 🔴 red = loose fit).

**Compare specific depths:**
```bash
python3 visualize_spheres.py sphere_tree.json --compare 0,2,4
```

See [VISUALIZATION.md](VISUALIZATION.md) for complete guide.

## Usage

### Loading from OBJ Files

```cpp
#include "SphereTree.h"
#include "ObjLoader.h"
using namespace spheretree;

// Load vertices from OBJ file
auto vertices = loadObjFile("models/dog.obj");

// Build sphere tree
SphereTree tree(vertices);

// Get statistics
std::cout << "Total spheres: " << tree.getSphereCount() << std::endl;
```

### Basic Usage

```cpp
#include "SphereTree.h"
using namespace spheretree;

// Create a point cloud
std::vector<Point3D> points;
points.push_back(Point3D(0, 0, 0));
points.push_back(Point3D(1, 0, 0));
points.push_back(Point3D(0, 1, 0));
// ... add more points

// Build sphere tree with default configuration
SphereTree tree(points);

// Test collision
Point3D testPoint(0.5, 0.5, 0.5);
if (tree.collidesWith(testPoint)) {
    std::cout << "Point collides with obstacle!" << std::endl;
}

// Test sphere collision
Sphere testSphere(Point3D(2, 2, 2), 1.0);
if (tree.collidesWith(testSphere)) {
    std::cout << "Sphere collides with obstacle!" << std::endl;
}
```

### Custom Configuration

```cpp
// Create custom configuration
SphereTreeConfig config;
config.maxSpheresPerNode = 8;           // Branching factor (Nc in paper)
config.maxDepth = 6;                    // Maximum tree depth
config.minPointsToSubdivide = 10;       // Min points to create child nodes

// Sphere fitting method
config.fittingMethod = SphereTreeConfig::FittingMethod::WELZL;  // Optimal for root

// Subdivision strategy
config.subdivisionStrategy = SphereTreeConfig::SubdivisionStrategy::MEDIAL_AXIS;

// For medial axis approach
config.sphereGenerationMethod = SphereTreeConfig::SphereGenerationMethod::EXPAND_SELECT;
config.medialAxisSampleMultiplier = 10;  // Generate 10x target spheres
config.errorReductionFactor = 0.5;       // Child error = 0.5 * parent error

// Build tree with custom config
SphereTree tree(points, config);
```

### Subdivision Strategies

#### Adaptive Medial Axis (Recommended)
Best for: Tight-fitting approximations, ensures complete coverage
```cpp
config.subdivisionStrategy = SphereTreeConfig::SubdivisionStrategy::MEDIAL_AXIS;
config.sphereGenerationMethod = SphereTreeConfig::SphereGenerationMethod::EXPAND_SELECT;
```

#### K-means Clustering
Best for: General-purpose subdivision, balanced distribution
```cpp
config.subdivisionStrategy = SphereTreeConfig::SubdivisionStrategy::K_MEANS;
```

#### Octree Subdivision
Best for: Fast construction, simple implementation (Palmer & Grimsdale method)
```cpp
config.subdivisionStrategy = SphereTreeConfig::SubdivisionStrategy::OCTREE;
```

#### Principal Axis Splitting
Best for: Elongated or anisotropic point clouds (Quinlan's method)
```cpp
config.subdivisionStrategy = SphereTreeConfig::SubdivisionStrategy::PRINCIPAL_AXIS;
```

### Sphere Fitting Methods

#### Welzl's Algorithm (Optimal)
Minimal enclosing sphere, recommended for root sphere
```cpp
config.fittingMethod = SphereTreeConfig::FittingMethod::WELZL;
```

#### Ritter's Algorithm (Fast)
Approximation, fast but may not be minimal
```cpp
config.fittingMethod = SphereTreeConfig::FittingMethod::RITTER;
```

#### Iterative Refinement (Better Quality)
Refines Ritter's result for tighter fit
```cpp
config.fittingMethod = SphereTreeConfig::FittingMethod::ITERATIVE_FIT;
```

### Sphere Generation Methods (for Medial Axis)

#### Expand & Select (MaxElim) - Recommended
Uniform error distribution across object
```cpp
config.sphereGenerationMethod = SphereTreeConfig::SphereGenerationMethod::EXPAND_SELECT;
```

#### Merge
Combines neighboring sphere pairs, good for gradual reduction
```cpp
config.sphereGenerationMethod = SphereTreeConfig::SphereGenerationMethod::MERGE;
```

#### Burst
Removes spheres and expands neighbors, better error distribution than Merge
```cpp
config.sphereGenerationMethod = SphereTreeConfig::SphereGenerationMethod::BURST;
```

### Query Operations

```cpp
// Get tree statistics
int nodeCount = tree.getNodeCount();
int sphereCount = tree.getSphereCount();
int maxDepth = tree.getMaxDepth();

// Get root bounding sphere (with error metric)
const Sphere& rootSphere = tree.getRootSphere();
std::cout << "Root radius: " << rootSphere.radius << std::endl;
std::cout << "Root error: " << rootSphere.error << std::endl;

// Get all spheres (for visualization)
std::vector<Sphere> allSpheres = tree.getAllSpheres();

// Get spheres at specific depth level
std::vector<Sphere> level2Spheres = tree.getSpheresAtDepth(2);
```

## API Reference

### Core Classes

#### `Point3D`
Represents a 3D point with x, y, z coordinates.

**Methods:**
- `double distanceTo(const Point3D& other)`: Calculate distance to another point
- `double length()`: Get vector length
- `double dot(const Point3D& other)`: Dot product
- Arithmetic operators: `+`, `-`, `*`, `/`

#### `Sphere`
Represents a sphere with center and radius.

**Methods:**
- `bool contains(const Point3D& point)`: Check if point is inside sphere
- `bool intersects(const Sphere& other)`: Check intersection with another sphere
- `double volume()`: Calculate sphere volume

#### `SphereTree`
Main class for building and querying sphere trees.

**Constructor:**
```cpp
SphereTree();
SphereTree(const std::vector<Point3D>& points, 
           const SphereTreeConfig& config = SphereTreeConfig());
```

**Building:**
- `void build(const std::vector<Point3D>& points)`: Build tree from points
- `void setConfig(const SphereTreeConfig& config)`: Set configuration

**Collision Detection:**
- `bool collidesWith(const Point3D& point)`: Point collision test
- `bool collidesWith(const Sphere& sphere)`: Sphere collision test

**Queries:**
- `const Sphere& getRootSphere()`: Get root bounding sphere
- `std::vector<Sphere> getAllSpheres()`: Get all spheres in tree
- `std::vector<Sphere> getSpheresAtDepth(int depth)`: Get spheres at specific level
- `int getNodeCount()`: Get total number of nodes
- `int getSphereCount()`: Get total number of spheres
- `int getMaxDepth()`: Get maximum depth reached

**Management:**
- `void clear()`: Clear the tree
- `bool isEmpty()`: Check if tree is empty

### Configuration

#### `SphereTreeConfig`

**Parameters:**
- `int maxSpheresPerNode`: Branching factor (default: 8)
- `int maxDepth`: Maximum tree depth (default: 10)
- `int minPointsToSubdivide`: Minimum points to create children (default: 10)
- `FittingMethod fittingMethod`: Sphere fitting algorithm
  - `RITTER`: Fast algorithm
  - `ITERATIVE_FIT`: Refined fit
  - `WELZL`: Optimal (currently uses RITTER)
- `SubdivisionStrategy subdivisionStrategy`: How to split nodes
  - `K_MEANS`: K-means clustering
  - `OCTREE`: Spatial octree subdivision
  - `PRINCIPAL_AXIS`: Split along principal axis

## Examples

The library includes comprehensive examples demonstrating various use cases:

```bash
# Run examples
./build/example_usage
```

Examples included:
1. **Basic Usage**: Simple sphere tree construction and collision testing
2. **Custom Configuration**: K-means subdivision with custom parameters
3. **Octree Subdivision**: Using spatial subdivision
4. **Principal Axis**: Subdivision for elongated objects
5. **Robotics Scenario**: Complete obstacle modeling example

## Performance Considerations

### Tree Depth
- **Shallow trees** (depth 3-5): Faster construction, coarser approximation, faster broad-phase collision
- **Deep trees** (depth 8-12): Slower construction, finer approximation, more accurate collision

### Branching Factor
- **Low branching** (2-4): Deeper trees, more narrow hierarchies
- **High branching** (8-16): Shallower trees, wider hierarchies

### Subdivision Strategy Performance
- **K-means**: Best quality, slower construction
- **Octree**: Fastest construction, good for uniform distributions
- **Principal Axis**: Good for anisotropic objects, moderate speed

## References

### Primary Reference
**Bradshaw, G., & O'Sullivan, C. (2004).** "Adaptive Medial-Axis Approximation for Sphere-Tree Construction." *ACM Transactions on Graphics*, Vol. 23, No. 1, January 2004.
- Introduces adaptive medial axis approximation
- Sphere generation algorithms (Merge, Burst, Expand & Select)
- Object segmentation with dividing planes
- Error metrics and quality evaluation

### Related Work
- **Welzl, E. (1991).** "Smallest enclosing disks (balls and ellipsoids)." *New Results and New Trends in Computer Science*
- **Ritter, J. (1990).** "An Efficient Bounding Sphere." *Graphics Gems*
- **Hubbard, P. M. (1995).** "Collision Detection for Interactive Graphics Applications." *IEEE Transactions on Visualization and Computer Graphics*
- **Hubbard, P. M. (1996).** "Approximating polyhedra with spheres for time-critical collision detection." *ACM Transactions on Graphics*
- **Quinlan, S. (1994).** "Efficient distance computation between non-convex objects." *Proceedings of IEEE International Conference on Robotics and Automation*
- **Palmer, I. J., & Grimsdale, R. L. (1995).** "Collision detection for animation using sphere-trees." *Computer Graphics Forum*
- **O'Sullivan, C., & Dingliana, J. (1999).** "Real-time collision detection and response using sphere-trees." *Proceedings of Spring Conference on Computer Graphics*
- **Rusinkiewicz, S., & Levoy, M. (2000).** "QSplat: A multiresolution point rendering system for large meshes." *Proceedings of SIGGRAPH*
- **Blum, H., & Nagel, R. N. (1978).** "Shape description using weighted symmetric axis features." *Pattern Recognition*
- **White, D. (2003).** "Minimum Volume Enclosing Ball" routine implementation
