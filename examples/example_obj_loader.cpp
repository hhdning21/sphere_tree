/**
 * @file example_obj_loader.cpp
 * @brief Example demonstrating OBJ file loading and sphere tree construction
 */

#include "SphereTree.h"
#include "ObjLoader.h"
#include "SphereExporter.h"
#include <iostream>
#include <chrono>

using namespace spheretree;

int main(int argc, char** argv) {
    const char* filename = (argc > 1) ? argv[1] : "../dog_v2/10680_Dog_v2.obj";
    
    std::cout << "========================================\n";
    std::cout << "Sphere Tree - OBJ Loader Example\n";
    std::cout << "========================================\n\n";
    
    // Load OBJ file
    std::cout << "Loading OBJ file: " << filename << "\n";
    auto vertices = loadObjFile(filename);
    
    if (vertices.empty()) {
        std::cerr << "Error: Failed to load OBJ file!\n";
        return 1;
    }
    
    std::cout << "Loaded " << vertices.size() << " vertices\n\n";
    
    // Build sphere tree
    std::cout << "Building sphere tree...\n";
    
    SphereTreeConfig config;
    config.maxDepth = 6;
    config.maxSpheresPerNode = 8;
    config.subdivisionStrategy = SphereTreeConfig::SubdivisionStrategy::OCTREE;
    
    auto start = std::chrono::high_resolution_clock::now();
    SphereTree tree(vertices, config);
    auto end = std::chrono::high_resolution_clock::now();
    
    double buildTime = std::chrono::duration<double, std::milli>(end - start).count();
    
    std::cout << "Tree built in " << buildTime << " ms\n\n";
    
    // Print statistics
    std::cout << "Tree Statistics:\n";
    std::cout << "  Total nodes: " << tree.getNodeCount() << "\n";
    std::cout << "  Total spheres: " << tree.getSphereCount() << "\n";
    std::cout << "  Max depth: " << tree.getMaxDepth() << "\n";
    std::cout << "  Root sphere radius: " << tree.getRootSphere().radius << "\n\n";
    
    // Print sphere count per depth
    std::cout << "Spheres per depth:\n";
    for (int d = 0; d <= tree.getMaxDepth(); ++d) {
        auto spheres = tree.getSpheresAtDepth(d);
        std::cout << "  Depth " << d << ": " << spheres.size() << " spheres\n";
    }
    
    // Export to files
    std::cout << "\nExporting to files...\n";
    exportDepthToCSV(tree, 0, "tree_root.csv");
    exportDepthToCSV(tree, tree.getMaxDepth(), "tree_leaves.csv");
    exportSpheresToOBJ(tree, 0, "spheres_root.obj");
    
    std::cout << "\nDone! Files created:\n";
    std::cout << "  tree_root.csv - Root sphere data\n";
    std::cout << "  tree_leaves.csv - Leaf sphere data\n";
    std::cout << "  spheres_root.obj - Root sphere mesh\n";
    
    return 0;
}
