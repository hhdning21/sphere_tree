/**
 * @file example_simple_api.cpp
 * @brief Examples demonstrating the simplified sphere tree API
 */

#include "SphereTreeAPI.h"
#include <iostream>
#include <iomanip>

using namespace spheretree;

void printSeparator(const std::string& title) {
    std::cout << "\n========================================\n";
    std::cout << title << "\n";
    std::cout << "========================================\n\n";
}

void example1_BasicUsage() {
    printSeparator("Example 1: Basic Usage");
    
    // Build tree from OBJ with default (BALANCED) quality
    auto tree = buildTreeFromOBJ("../dog_v2/10680_Dog_v2.obj");
    
    if (!tree) {
        std::cout << "Failed to load OBJ file\n";
        return;
    }
    
    // Print tree statistics
    std::cout << "\nTree Statistics:\n";
    std::cout << "  Total Spheres: " << tree->getSphereCount() << "\n";
    std::cout << "  Max Depth: " << tree->getMaxDepth() << "\n";
    std::cout << "  Root Sphere Radius: " << tree->getRootSphere().radius << "\n";
}

void example2_QualityLevels() {
    printSeparator("Example 2: Quality Levels Comparison");
    
    const char* filename = "../dog_v2/10680_Dog_v2.obj";
    
    std::cout << "Building FAST quality tree...\n";
    auto treeFast = buildTreeFromOBJ(filename, TreeQuality::FAST);
    
    std::cout << "\nBuilding BALANCED quality tree...\n";
    auto treeBalanced = buildTreeFromOBJ(filename, TreeQuality::BALANCED);
    
    std::cout << "\nBuilding HIGH_DETAIL quality tree...\n";
    auto treeDetail = buildTreeFromOBJ(filename, TreeQuality::HIGH_DETAIL);
    
    std::cout << "\n--- Comparison ---\n";
    std::cout << "FAST:        " << treeFast->getSphereCount() << " spheres\n";
    std::cout << "BALANCED:    " << treeBalanced->getSphereCount() << " spheres\n";
    std::cout << "HIGH_DETAIL: " << treeDetail->getSphereCount() << " spheres\n";
}

void example3_CustomConfiguration() {
    printSeparator("Example 3: Custom Configuration");
    
    // Load points
    auto points = loadObjFile("../dog_v2/10680_Dog_v2.obj");
    
    // Create custom configuration
    SphereTreeConfig config;
    config.maxDepth = 5;
    config.minPointsToSubdivide = 30;
    config.fittingMethod = SphereTreeConfig::FittingMethod::WELZL;
    config.subdivisionStrategy = SphereTreeConfig::SubdivisionStrategy::K_MEANS;
    
    std::cout << "Building tree with custom config:\n";
    std::cout << "  Max Depth: " << config.maxDepth << "\n";
    std::cout << "  Min Points: " << config.minPointsToSubdivide << "\n";
    std::cout << "  Fitting: WELZL\n";
    std::cout << "  Subdivision: K_MEANS\n\n";
    
    auto tree = buildTree(points, config);
    
    std::cout << "\nResult: " << tree->getSphereCount() << " spheres at depth " 
              << tree->getMaxDepth() << "\n";
}

void example4_AccessingTreeData() {
    printSeparator("Example 4: Accessing Tree Data");
    
    auto tree = buildTreeFromOBJ("../dog_v2/10680_Dog_v2.obj", TreeQuality::FAST);
    
    if (!tree) return;
    
    // Get spheres at each depth level
    std::cout << "Spheres per depth level:\n";
    for (int d = 0; d <= tree->getMaxDepth(); ++d) {
        auto spheres = tree->getSpheresAtDepth(d);
        std::cout << "  Depth " << d << ": " << spheres.size() << " spheres\n";
        
        if (!spheres.empty()) {
            std::cout << "    Example sphere: "
                      << "center=(" << spheres[0].center.x << ", "
                      << spheres[0].center.y << ", "
                      << spheres[0].center.z << "), "
                      << "radius=" << spheres[0].radius << ", "
                      << "error=" << spheres[0].error << "\n";
        }
    }
    
    // Get root bounding sphere
    const Sphere& root = tree->getRootSphere();
    std::cout << "\nRoot bounding sphere:\n";
    std::cout << "  Center: (" << root.center.x << ", " 
              << root.center.y << ", " << root.center.z << ")\n";
    std::cout << "  Radius: " << root.radius << "\n";
}

void example5_ExportForVisualization() {
    printSeparator("Example 5: Export for Visualization");
    
    auto tree = buildTreeFromOBJ("../dog_v2/10680_Dog_v2.obj");
    
    if (!tree) return;
    
    // Export full tree data to JSON
    exportToJSON(*tree, "sphere_tree.json");
    
    // Export first few depth levels as OBJ files
    exportToOBJ(*tree, "spheres", 3);  // Depths 0-3
    
    std::cout << "\nExported files:\n";
    std::cout << "  sphere_tree.json - Complete tree data\n";
    std::cout << "  spheres_depth_0.obj - Root sphere\n";
    std::cout << "  spheres_depth_1.obj - First subdivision\n";
    std::cout << "  spheres_depth_2.obj - Second subdivision\n";
    std::cout << "  spheres_depth_3.obj - Third subdivision\n";
    
    std::cout << "\nVisualize with:\n";
    std::cout << "  ./visualize_spheres_gl ../dog_v2/10680_Dog_v2.obj\n";
}

void example6_BuiltInQueries() {
    printSeparator("Example 6: Using Built-in Tree Queries");
    
    auto tree = buildTreeFromOBJ("../dog_v2/10680_Dog_v2.obj");
    
    if (!tree) return;
    
    // Test point collision
    Point3D testPoint(0, 0, 0);
    if (tree->collidesWith(testPoint)) {
        std::cout << "Point (0,0,0) is inside the model\n";
    } else {
        std::cout << "Point (0,0,0) is outside the model\n";
    }
    
    // Test sphere collision
    Sphere testSphere(Point3D(5, 5, 5), 2.0);
    if (tree->collidesWith(testSphere)) {
        std::cout << "Test sphere intersects\n";
    } else {
        std::cout << "Test sphere does not intersect\n";
    }
    
    std::cout << "\nTotal spheres in tree: " << tree->getSphereCount() << "\n";
}

int main() {
    std::cout << "\n╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  Sphere Tree API - Simple Examples                        ║\n";
    std::cout << "║  Build hierarchical sphere approximations from 3D models  ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
    
    example1_BasicUsage();
    example2_QualityLevels();
    example3_CustomConfiguration();
    example4_AccessingTreeData();
    example5_ExportForVisualization();
    example6_BuiltInQueries();
    
    printSeparator("All examples completed!");
    
    return 0;
}
