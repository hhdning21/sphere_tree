/**
 * @file SphereTree.h
 * @brief Hierarchical sphere tree for 3D point cloud approximation
 * 
 * Implementation based on "Adaptive Medial-Axis Approximation for Sphere-Tree Construction"
 * by Bradshaw & O'Sullivan (ACM Transactions on Graphics, 2004)
 */

#ifndef SPHERE_TREE_H
#define SPHERE_TREE_H

#include <vector>
#include <memory>
#include <cmath>

namespace spheretree {

/**
 * @brief 3D point representation
 */
struct Point3D {
    double x, y, z;
    
    Point3D() : x(0), y(0), z(0) {}
    Point3D(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}
    
    Point3D operator+(const Point3D& other) const {
        return Point3D(x + other.x, y + other.y, z + other.z);
    }
    
    Point3D operator-(const Point3D& other) const {
        return Point3D(x - other.x, y - other.y, z - other.z);
    }
    
    Point3D operator*(double scalar) const {
        return Point3D(x * scalar, y * scalar, z * scalar);
    }
    
    Point3D operator/(double scalar) const {
        return Point3D(x / scalar, y / scalar, z / scalar);
    }
    
    double dot(const Point3D& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    
    double length() const {
        return std::sqrt(x * x + y * y + z * z);
    }
    
    double distanceTo(const Point3D& other) const {
        return (*this - other).length();
    }
};

/**
 * @brief Sphere representation with center and radius
 */
struct Sphere {
    Point3D center;
    double radius;
    double error;  // Error metric from paper: e = r - ||q - v||
    
    Sphere() : center(), radius(0), error(0) {}
    Sphere(const Point3D& c, double r) : center(c), radius(r), error(0) {}
    
    bool contains(const Point3D& point) const;
    bool intersects(const Sphere& other) const;
    double volume() const;
};

/**
 * @brief Configuration for sphere tree construction
 */
struct SphereTreeConfig {
    // Maximum number of spheres per node (branching factor)
    int maxSpheresPerNode = 8;
    
    // Maximum depth of the tree
    int maxDepth = 10;
    
    // Minimum number of points to subdivide further
    int minPointsToSubdivide = 10;
    
    // Sphere fitting method
    enum class FittingMethod {
        RITTER,           // Ritter's bounding sphere algorithm (fast)
        WELZL,            // Welzl's minimal enclosing sphere (optimal but slower)
        ITERATIVE_FIT     // Iterative refinement
    } fittingMethod = FittingMethod::WELZL;  // Paper uses Welzl for root sphere
    
    // Subdivision strategy
    enum class SubdivisionStrategy {
        K_MEANS,          // K-means clustering
        OCTREE,           // Octree-based spatial subdivision (simple method from paper)
        PRINCIPAL_AXIS,   // Split along principal axis (Quinlan's method)
        MEDIAL_AXIS       // Adaptive medial axis approximation (Bradshaw & O'Sullivan)
    } subdivisionStrategy = SubdivisionStrategy::MEDIAL_AXIS;
    
    // Sphere generation/reduction method (for medial axis approach)
    enum class SphereGenerationMethod {
        MERGE,            // Merge algorithm - combine pairs of spheres
        BURST,            // Burst algorithm - remove sphere and expand neighbors
        EXPAND_SELECT     // Expand & Select (MaxElim) - uniform error distribution
    } sphereGenerationMethod = SphereGenerationMethod::EXPAND_SELECT;
    
    // Medial axis approximation parameters
    int medialAxisSampleMultiplier = 10;  // Generate 10x target spheres for selection
    double errorReductionFactor = 0.5;     // Child sphere error = parent error * 0.5 (or 0.25)
};

/**
 * @brief A node in the sphere tree
 */
class SphereTreeNode {
public:
    Sphere boundingSphere;
    std::vector<std::shared_ptr<SphereTreeNode>> children;
    std::vector<Point3D> points;  // Points contained in leaf nodes
    
    SphereTreeNode(const Sphere& sphere);
    
    bool isLeaf() const;
    int getNodeCount() const;
    int getSphereCount() const;
    int getMaxDepth() const;
};

/**
 * @brief Main sphere tree class
 */
class SphereTree {
public:
    SphereTree();
    SphereTree(const std::vector<Point3D>& points, 
               const SphereTreeConfig& config = SphereTreeConfig());
    
    void build(const std::vector<Point3D>& points);
    void setConfig(const SphereTreeConfig& config);
    
    // Collision detection
    bool collidesWith(const Point3D& point) const;
    bool collidesWith(const Sphere& sphere) const;
    
    // Query operations
    const Sphere& getRootSphere() const;
    std::vector<Sphere> getAllSpheres() const;
    std::vector<Sphere> getSpheresAtDepth(int depth) const;
    
    int getNodeCount() const;
    int getSphereCount() const;
    int getMaxDepth() const;
    
    void clear();
    bool isEmpty() const;
    
private:
    std::shared_ptr<SphereTreeNode> root;
    SphereTreeConfig config;
    
    void buildNode(std::shared_ptr<SphereTreeNode> node,
                   const std::vector<Point3D>& points,
                   int depth);
    
    void getAllSpheresRecursive(const std::shared_ptr<SphereTreeNode>& node,
                                std::vector<Sphere>& spheres) const;
    
    void getSpheresAtDepthRecursive(const std::shared_ptr<SphereTreeNode>& node,
                                    int targetDepth,
                                    int currentDepth,
                                    std::vector<Sphere>& spheres) const;
};

// Helper functions
Sphere welzl(const std::vector<Point3D>& points);
Sphere ritterBoundingSphere(const std::vector<Point3D>& points);

} // namespace spheretree

#endif // SPHERE_TREE_H
