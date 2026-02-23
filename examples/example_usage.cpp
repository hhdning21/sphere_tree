/**
 * @file example_usage.cpp
 * @brief Basic usage examples for the sphere tree library
 */

#include "SphereTree.h"
#include <iostream>
#include <vector>

using namespace spheretree;

int main() {
    std::cout << "Sphere Tree - Basic Examples\n";
    std::cout << "=============================\n\n";
    
    // Example 1: Create a simple point cloud
    std::cout << "Example 1: Basic sphere tree from point cloud\n";
    std::cout << "---------------------------------------------\n";
    
    std::vector<Point3D> points;
    points.push_back(Point3D(0, 0, 0));
    points.push_back(Point3D(1, 0, 0));
    points.push_back(Point3D(0, 1, 0));
    points.push_back(Point3D(0, 0, 1));
    points.push_back(Point3D(1, 1, 0));
    points.push_back(Point3D(1, 0, 1));
    points.push_back(Point3D(0, 1, 1));
    points.push_back(Point3D(1, 1, 1));
    
    // Build with default configuration
    SphereTree tree(points);
    
    std::cout << "Built tree with " << tree.getSphereCount() << " spheres\n";
    std::cout << "Tree depth: " << tree.getMaxDepth() << "\n";
    std::cout << "Root sphere radius: " << tree.getRootSphere().radius << "\n\n";
    
    // Example 2: Collision detection
    std::cout << "Example 2: Collision detection\n";
    std::cout << "-------------------------------\n";
    
    Point3D testPoint1(0.5, 0.5, 0.5);
    Point3D testPoint2(5.0, 5.0, 5.0);
    
    std::cout << "Point (0.5, 0.5, 0.5) collides: " 
              << (tree.collidesWith(testPoint1) ? "YES" : "NO") << "\n";
    std::cout << "Point (5.0, 5.0, 5.0) collides: "
              << (tree.collidesWith(testPoint2) ? "YES" : "NO") << "\n\n";
    
    // Example 3: Custom configuration
    std::cout << "Example 3: Custom configuration with K-means\n";
    std::cout << "--------------------------------------------\n";
    
    SphereTreeConfig config;
    config.maxDepth = 4;
    config.maxSpheresPerNode = 4;
    config.subdivisionStrategy = SphereTreeConfig::SubdivisionStrategy::K_MEANS;
    
    SphereTree customTree(points, config);
    
    std::cout << "Built tree with custom config\n";
    std::cout << "  Spheres: " << customTree.getSphereCount() << "\n";
    std::cout << "  Depth: " << customTree.getMaxDepth() << "\n\n";
    
    std::cout << "Examples completed!\n";
    
    return 0;
}
