/**
 * @file SphereTree.cpp
 * @brief Implementation of hierarchical sphere tree
 */

#include "SphereTree.h"
#include <algorithm>
#include <random>
#include <limits>
#include <cmath>
#include <queue>

namespace spheretree {

// ============================================================================
// Sphere Methods
// ============================================================================

bool Sphere::contains(const Point3D& point) const {
    return center.distanceTo(point) <= radius;
}

bool Sphere::intersects(const Sphere& other) const {
    return center.distanceTo(other.center) <= (radius + other.radius);
}

double Sphere::volume() const {
    return (4.0 / 3.0) * M_PI * radius * radius * radius;
}

// ============================================================================
// SphereTreeNode Methods
// ============================================================================

SphereTreeNode::SphereTreeNode(const Sphere& sphere) 
    : boundingSphere(sphere) {
}

bool SphereTreeNode::isLeaf() const {
    return children.empty();
}

int SphereTreeNode::getNodeCount() const {
    int count = 1;
    for (const auto& child : children) {
        count += child->getNodeCount();
    }
    return count;
}

int SphereTreeNode::getSphereCount() const {
    return getNodeCount();
}

int SphereTreeNode::getMaxDepth() const {
    if (isLeaf()) {
        return 1;
    }
    int maxChildDepth = 0;
    for (const auto& child : children) {
        maxChildDepth = std::max(maxChildDepth, child->getMaxDepth());
    }
    return 1 + maxChildDepth;
}

// ============================================================================
// Helper Functions - Sphere Fitting Algorithms
// ============================================================================

// Welzl's minimal enclosing sphere algorithm (Welzl 1991)
namespace {
    Sphere welzlRecursive(std::vector<Point3D>& points, 
                          std::vector<Point3D>& boundary, 
                          size_t n) {
        // Base cases
        if (n == 0 || boundary.size() == 4) {
            if (boundary.size() == 0) {
                return Sphere(Point3D(0, 0, 0), 0);
            } else if (boundary.size() == 1) {
                return Sphere(boundary[0], 0);
            } else if (boundary.size() == 2) {
                Point3D center = (boundary[0] + boundary[1]) * 0.5;
                double radius = center.distanceTo(boundary[0]);
                return Sphere(center, radius);
            } else if (boundary.size() == 3) {
                // Circle through 3 points
                Point3D a = boundary[0];
                Point3D b = boundary[1];
                Point3D c = boundary[2];
                
                Point3D ab = b - a;
                Point3D ac = c - a;
                Point3D abXac = Point3D(
                    ab.y * ac.z - ab.z * ac.y,
                    ab.z * ac.x - ab.x * ac.z,
                    ab.x * ac.y - ab.y * ac.x
                );
                
                double denom = 2.0 * abXac.dot(abXac);
                if (std::abs(denom) < 1e-10) {
                    // Collinear points
                    Point3D center = (a + c) * 0.5;
                    double r1 = center.distanceTo(a);
                    double r2 = center.distanceTo(b);
                    double r3 = center.distanceTo(c);
                    double radius = std::max({r1, r2, r3});
                    return Sphere(center, radius);
                }
                
                Point3D acXabXac = Point3D(
                    ac.y * abXac.z - ac.z * abXac.y,
                    ac.z * abXac.x - ac.x * abXac.z,
                    ac.x * abXac.y - ac.y * abXac.x
                );
                Point3D abXacXab = Point3D(
                    abXac.y * ab.z - abXac.z * ab.y,
                    abXac.z * ab.x - abXac.x * ab.z,
                    abXac.x * ab.y - abXac.y * ab.x
                );
                
                double toCircumsphereCenter = (ab.dot(ab) * acXabXac.dot(ac) + 
                                               ac.dot(ac) * abXacXab.dot(ab)) / denom;
                Point3D center = a + abXac * toCircumsphereCenter;
                double radius = center.distanceTo(a);
                return Sphere(center, radius);
            } else {
                // 4 points - sphere through 4 points (simplified - use centroid)
                Point3D center(0, 0, 0);
                for (const auto& p : boundary) {
                    center = center + p;
                }
                center = center / static_cast<double>(boundary.size());
                double radius = 0;
                for (const auto& p : boundary) {
                    radius = std::max(radius, center.distanceTo(p));
                }
                return Sphere(center, radius);
            }
        }
        
        // Recursive case
        Point3D p = points[n - 1];
        Sphere sphere = welzlRecursive(points, boundary, n - 1);
        
        if (sphere.contains(p)) {
            return sphere;
        }
        
        boundary.push_back(p);
        sphere = welzlRecursive(points, boundary, n - 1);
        boundary.pop_back();
        
        return sphere;
    }
}

Sphere welzl(const std::vector<Point3D>& points) {
    if (points.empty()) {
        return Sphere(Point3D(0, 0, 0), 0);
    }
    
    std::vector<Point3D> pointsCopy = points;
    std::vector<Point3D> boundary;
    
    // Randomize for better average performance
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(pointsCopy.begin(), pointsCopy.end(), gen);
    
    return welzlRecursive(pointsCopy, boundary, pointsCopy.size());
}

// Ritter's bounding sphere algorithm (Ritter 1990)
Sphere ritterBoundingSphere(const std::vector<Point3D>& points) {
    if (points.empty()) {
        return Sphere(Point3D(0, 0, 0), 0);
    }
    
    if (points.size() == 1) {
        return Sphere(points[0], 0);
    }
    
    // Find two points that are far apart
    Point3D p1 = points[0];
    Point3D p2 = points[0];
    
    // Find point farthest from p1
    double maxDist = 0;
    for (const auto& p : points) {
        double dist = p1.distanceTo(p);
        if (dist > maxDist) {
            maxDist = dist;
            p2 = p;
        }
    }
    
    // Find point farthest from p2
    p1 = p2;
    maxDist = 0;
    for (const auto& p : points) {
        double dist = p2.distanceTo(p);
        if (dist > maxDist) {
            maxDist = dist;
            p1 = p;
        }
    }
    
    // Initial sphere
    Point3D center = (p1 + p2) * 0.5;
    double radius = center.distanceTo(p1);
    
    // Expand to include all points
    for (const auto& p : points) {
        double dist = center.distanceTo(p);
        if (dist > radius) {
            // Expand sphere to include this point
            double newRadius = (radius + dist) * 0.5;
            double alpha = (dist - radius) / (2.0 * dist);
            center = center + (p - center) * alpha;
            radius = newRadius;
        }
    }
    
    return Sphere(center, radius);
}

// ============================================================================
// Subdivision Methods
// ============================================================================

namespace {
    std::vector<std::vector<Point3D>> kmeansSubdivide(
        const std::vector<Point3D>& points,
        int k,
        int maxIterations = 10) {
        
        if (points.size() <= k) {
            std::vector<std::vector<Point3D>> clusters;
            for (const auto& p : points) {
                clusters.push_back({p});
            }
            return clusters;
        }
        
        std::random_device rd;
        std::mt19937 gen(rd());
        
        // Initialize centroids with random points
        std::vector<Point3D> centroids;
        std::vector<size_t> indices(points.size());
        for (size_t i = 0; i < points.size(); ++i) {
            indices[i] = i;
        }
        std::shuffle(indices.begin(), indices.end(), gen);
        
        for (int i = 0; i < k && i < static_cast<int>(points.size()); ++i) {
            centroids.push_back(points[indices[i]]);
        }
        
        std::vector<std::vector<Point3D>> clusters(k);
        
        for (int iter = 0; iter < maxIterations; ++iter) {
            // Assign points to nearest centroid
            for (auto& cluster : clusters) {
                cluster.clear();
            }
            
            for (const auto& p : points) {
                int nearest = 0;
                double minDist = centroids[0].distanceTo(p);
                
                for (int i = 1; i < k; ++i) {
                    double dist = centroids[i].distanceTo(p);
                    if (dist < minDist) {
                        minDist = dist;
                        nearest = i;
                    }
                }
                
                clusters[nearest].push_back(p);
            }
            
            // Update centroids
            bool changed = false;
            for (int i = 0; i < k; ++i) {
                if (clusters[i].empty()) continue;
                
                Point3D newCentroid(0, 0, 0);
                for (const auto& p : clusters[i]) {
                    newCentroid = newCentroid + p;
                }
                newCentroid = newCentroid / static_cast<double>(clusters[i].size());
                
                if (newCentroid.distanceTo(centroids[i]) > 1e-6) {
                    changed = true;
                    centroids[i] = newCentroid;
                }
            }
            
            if (!changed) break;
        }
        
        // Remove empty clusters
        std::vector<std::vector<Point3D>> result;
        for (auto& cluster : clusters) {
            if (!cluster.empty()) {
                result.push_back(std::move(cluster));
            }
        }
        
        return result;
    }
    
    std::vector<std::vector<Point3D>> octreeSubdivide(
        const std::vector<Point3D>& points,
        int maxChildren) {
        
        if (points.empty()) {
            return {};
        }
        
        // Find bounding box
        Point3D minBound = points[0];
        Point3D maxBound = points[0];
        
        for (const auto& p : points) {
            minBound.x = std::min(minBound.x, p.x);
            minBound.y = std::min(minBound.y, p.y);
            minBound.z = std::min(minBound.z, p.z);
            maxBound.x = std::max(maxBound.x, p.x);
            maxBound.y = std::max(maxBound.y, p.y);
            maxBound.z = std::max(maxBound.z, p.z);
        }
        
        Point3D center = (minBound + maxBound) * 0.5;
        
        // Create 8 octants
        std::vector<std::vector<Point3D>> octants(8);
        
        for (const auto& p : points) {
            int index = 0;
            if (p.x >= center.x) index |= 1;
            if (p.y >= center.y) index |= 2;
            if (p.z >= center.z) index |= 4;
            octants[index].push_back(p);
        }
        
        // Return non-empty octants
        std::vector<std::vector<Point3D>> result;
        for (auto& octant : octants) {
            if (!octant.empty()) {
                result.push_back(std::move(octant));
            }
        }
        
        // If we need to reduce to maxChildren, merge smallest octants
        while (result.size() > static_cast<size_t>(maxChildren)) {
            // Find two smallest clusters
            size_t smallest1 = 0, smallest2 = 1;
            if (result[1].size() < result[0].size()) {
                smallest1 = 1;
                smallest2 = 0;
            }
            
            for (size_t i = 2; i < result.size(); ++i) {
                if (result[i].size() < result[smallest1].size()) {
                    smallest2 = smallest1;
                    smallest1 = i;
                } else if (result[i].size() < result[smallest2].size()) {
                    smallest2 = i;
                }
            }
            
            // Merge them
            result[smallest1].insert(result[smallest1].end(),
                                    result[smallest2].begin(),
                                    result[smallest2].end());
            result.erase(result.begin() + smallest2);
        }
        
        return result;
    }
    
    std::vector<std::vector<Point3D>> principalAxisSubdivide(
        const std::vector<Point3D>& points,
        int maxChildren) {
        
        if (points.size() <= 1) {
            return {points};
        }
        
        // Compute centroid
        Point3D centroid(0, 0, 0);
        for (const auto& p : points) {
            centroid = centroid + p;
        }
        centroid = centroid / static_cast<double>(points.size());
        
        // Compute covariance matrix
        double cxx = 0, cyy = 0, czz = 0;
        double cxy = 0, cxz = 0, cyz = 0;
        
        for (const auto& p : points) {
            Point3D d = p - centroid;
            cxx += d.x * d.x;
            cyy += d.y * d.y;
            czz += d.z * d.z;
            cxy += d.x * d.y;
            cxz += d.x * d.z;
            cyz += d.y * d.z;
        }
        
        // Find axis with maximum variance
        double maxVar = cxx;
        int axis = 0; // 0=x, 1=y, 2=z
        
        if (cyy > maxVar) {
            maxVar = cyy;
            axis = 1;
        }
        if (czz > maxVar) {
            maxVar = czz;
            axis = 2;
        }
        
        // Sort points along principal axis
        std::vector<Point3D> sortedPoints = points;
        std::sort(sortedPoints.begin(), sortedPoints.end(),
            [axis](const Point3D& a, const Point3D& b) {
                if (axis == 0) return a.x < b.x;
                if (axis == 1) return a.y < b.y;
                return a.z < b.z;
            });
        
        // Divide into maxChildren groups
        std::vector<std::vector<Point3D>> result(maxChildren);
        size_t pointsPerChild = points.size() / maxChildren;
        size_t remainder = points.size() % maxChildren;
        
        size_t index = 0;
        for (int i = 0; i < maxChildren; ++i) {
            size_t count = pointsPerChild + (i < static_cast<int>(remainder) ? 1 : 0);
            for (size_t j = 0; j < count && index < sortedPoints.size(); ++j) {
                result[i].push_back(sortedPoints[index++]);
            }
        }
        
        // Remove empty groups
        result.erase(
            std::remove_if(result.begin(), result.end(),
                [](const std::vector<Point3D>& v) { return v.empty(); }),
            result.end());
        
        return result;
    }
}

// ============================================================================
// SphereTree Methods
// ============================================================================

SphereTree::SphereTree() : root(nullptr) {
}

SphereTree::SphereTree(const std::vector<Point3D>& points,
                       const SphereTreeConfig& config)
    : config(config) {
    build(points);
}

void SphereTree::build(const std::vector<Point3D>& points) {
    if (points.empty()) {
        root = nullptr;
        return;
    }
    
    // Create root sphere
    Sphere rootSphere;
    if (config.fittingMethod == SphereTreeConfig::FittingMethod::WELZL) {
        rootSphere = welzl(points);
    } else if (config.fittingMethod == SphereTreeConfig::FittingMethod::RITTER) {
        rootSphere = ritterBoundingSphere(points);
    } else {
        rootSphere = ritterBoundingSphere(points);
    }
    
    root = std::make_shared<SphereTreeNode>(rootSphere);
    buildNode(root, points, 0);
}

void SphereTree::setConfig(const SphereTreeConfig& cfg) {
    config = cfg;
}

void SphereTree::buildNode(std::shared_ptr<SphereTreeNode> node,
                           const std::vector<Point3D>& points,
                           int depth) {
    // Stopping conditions
    if (depth >= config.maxDepth ||
        points.size() <= static_cast<size_t>(config.minPointsToSubdivide)) {
        node->points = points;
        return;
    }
    
    // Subdivide points
    std::vector<std::vector<Point3D>> subdivisions;
    
    if (config.subdivisionStrategy == SphereTreeConfig::SubdivisionStrategy::K_MEANS) {
        subdivisions = kmeansSubdivide(points, config.maxSpheresPerNode);
    } else if (config.subdivisionStrategy == SphereTreeConfig::SubdivisionStrategy::OCTREE) {
        subdivisions = octreeSubdivide(points, config.maxSpheresPerNode);
    } else if (config.subdivisionStrategy == SphereTreeConfig::SubdivisionStrategy::PRINCIPAL_AXIS) {
        subdivisions = principalAxisSubdivide(points, config.maxSpheresPerNode);
    } else {
        // Default to octree for MEDIAL_AXIS (simplified implementation)
        subdivisions = octreeSubdivide(points, config.maxSpheresPerNode);
    }
    
    // Create child nodes
    for (auto& subdivision : subdivisions) {
        if (subdivision.empty()) continue;
        
        Sphere childSphere;
        if (config.fittingMethod == SphereTreeConfig::FittingMethod::WELZL) {
            childSphere = welzl(subdivision);
        } else {
            childSphere = ritterBoundingSphere(subdivision);
        }
        
        auto child = std::make_shared<SphereTreeNode>(childSphere);
        node->children.push_back(child);
        buildNode(child, subdivision, depth + 1);
    }
}

bool SphereTree::collidesWith(const Point3D& point) const {
    if (!root) return false;
    
    std::queue<std::shared_ptr<SphereTreeNode>> queue;
    queue.push(root);
    
    while (!queue.empty()) {
        auto node = queue.front();
        queue.pop();
        
        if (!node->boundingSphere.contains(point)) {
            continue;
        }
        
        if (node->isLeaf()) {
            return true;
        }
        
        for (const auto& child : node->children) {
            queue.push(child);
        }
    }
    
    return false;
}

bool SphereTree::collidesWith(const Sphere& sphere) const {
    if (!root) return false;
    
    std::queue<std::shared_ptr<SphereTreeNode>> queue;
    queue.push(root);
    
    while (!queue.empty()) {
        auto node = queue.front();
        queue.pop();
        
        if (!node->boundingSphere.intersects(sphere)) {
            continue;
        }
        
        if (node->isLeaf()) {
            return true;
        }
        
        for (const auto& child : node->children) {
            queue.push(child);
        }
    }
    
    return false;
}

const Sphere& SphereTree::getRootSphere() const {
    static Sphere emptySphere;
    if (!root) return emptySphere;
    return root->boundingSphere;
}

std::vector<Sphere> SphereTree::getAllSpheres() const {
    std::vector<Sphere> spheres;
    if (root) {
        getAllSpheresRecursive(root, spheres);
    }
    return spheres;
}

void SphereTree::getAllSpheresRecursive(const std::shared_ptr<SphereTreeNode>& node,
                                        std::vector<Sphere>& spheres) const {
    if (!node) return;
    
    spheres.push_back(node->boundingSphere);
    
    for (const auto& child : node->children) {
        getAllSpheresRecursive(child, spheres);
    }
}

std::vector<Sphere> SphereTree::getSpheresAtDepth(int depth) const {
    std::vector<Sphere> spheres;
    if (root) {
        getSpheresAtDepthRecursive(root, depth, 0, spheres);
    }
    return spheres;
}

void SphereTree::getSpheresAtDepthRecursive(const std::shared_ptr<SphereTreeNode>& node,
                                            int targetDepth,
                                            int currentDepth,
                                            std::vector<Sphere>& spheres) const {
    if (!node) return;
    
    if (currentDepth == targetDepth) {
        spheres.push_back(node->boundingSphere);
        return;
    }
    
    for (const auto& child : node->children) {
        getSpheresAtDepthRecursive(child, targetDepth, currentDepth + 1, spheres);
    }
}

int SphereTree::getNodeCount() const {
    if (!root) return 0;
    return root->getNodeCount();
}

int SphereTree::getSphereCount() const {
    if (!root) return 0;
    return root->getSphereCount();
}

int SphereTree::getMaxDepth() const {
    if (!root) return 0;
    return root->getMaxDepth();
}

void SphereTree::clear() {
    root = nullptr;
}

bool SphereTree::isEmpty() const {
    return root == nullptr;
}

} // namespace spheretree
