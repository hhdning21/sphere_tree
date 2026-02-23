/**
 * @file SphereTreeAPI.cpp
 * @brief Implementation of high-level API for sphere tree construction
 */

#include "SphereTreeAPI.h"
#include "SphereTree.h"
#include "ObjLoader.h"
#include "SphereExporter.h"
#include <chrono>
#include <iostream>

namespace spheretree {

// ============================================================================
// Configuration Presets
// ============================================================================

SphereTreeConfig getQualityConfig(TreeQuality quality) {
    SphereTreeConfig config;
    
    switch (quality) {
        case TreeQuality::FAST:
            config.maxSpheresPerNode = 4;
            config.maxDepth = 4;
            config.minPointsToSubdivide = 20;
            config.fittingMethod = SphereTreeConfig::FittingMethod::RITTER;
            config.subdivisionStrategy = SphereTreeConfig::SubdivisionStrategy::OCTREE;
            break;
            
        case TreeQuality::BALANCED:
            config.maxSpheresPerNode = 8;
            config.maxDepth = 6;
            config.minPointsToSubdivide = 10;
            config.fittingMethod = SphereTreeConfig::FittingMethod::WELZL;
            config.subdivisionStrategy = SphereTreeConfig::SubdivisionStrategy::K_MEANS;
            break;
            
        case TreeQuality::HIGH_DETAIL:
            config.maxSpheresPerNode = 8;
            config.maxDepth = 8;
            config.minPointsToSubdivide = 5;
            config.fittingMethod = SphereTreeConfig::FittingMethod::WELZL;
            config.subdivisionStrategy = SphereTreeConfig::SubdivisionStrategy::MEDIAL_AXIS;
            config.medialAxisSampleMultiplier = 15;
            config.errorReductionFactor = 0.25;
            break;
    }
    
    return config;
}

// ============================================================================
// Factory Functions
// ============================================================================

std::shared_ptr<SphereTree> buildTreeFromOBJ(const std::string& filename,
                                             TreeQuality quality) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::cout << "Loading OBJ file: " << filename << std::endl;
    
    // Load OBJ file
    ObjLoader loader;
    if (!loader.load(filename)) {
        std::cerr << "Failed to load OBJ file: " << filename << std::endl;
        return nullptr;
    }
    
    std::vector<Point3D> points = loader.getVertices();
    std::cout << "Loaded " << points.size() << " vertices" << std::endl;
    
    if (points.empty()) {
        std::cerr << "No vertices found in OBJ file" << std::endl;
        return nullptr;
    }
    
    // Build tree with quality config
    SphereTreeConfig config = getQualityConfig(quality);
    auto tree = std::make_shared<SphereTree>(points, config);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "Sphere tree built in " << duration.count() << " ms" << std::endl;
    std::cout << "  Nodes: " << tree->getNodeCount() << std::endl;
    std::cout << "  Spheres: " << tree->getSphereCount() << std::endl;
    std::cout << "  Max depth: " << tree->getMaxDepth() << std::endl;
    
    return tree;
}

std::shared_ptr<SphereTree> buildTree(const std::vector<Point3D>& points,
                                      TreeQuality quality) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::cout << "Building sphere tree from " << points.size() << " points" << std::endl;
    
    if (points.empty()) {
        std::cerr << "Cannot build tree from empty point set" << std::endl;
        return nullptr;
    }
    
    SphereTreeConfig config = getQualityConfig(quality);
    auto tree = std::make_shared<SphereTree>(points, config);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "Sphere tree built in " << duration.count() << " ms" << std::endl;
    std::cout << "  Nodes: " << tree->getNodeCount() << std::endl;
    std::cout << "  Spheres: " << tree->getSphereCount() << std::endl;
    std::cout << "  Max depth: " << tree->getMaxDepth() << std::endl;
    
    return tree;
}

std::shared_ptr<SphereTree> buildTree(const std::vector<Point3D>& points,
                                      const SphereTreeConfig& config) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::cout << "Building sphere tree from " << points.size() << " points (custom config)" << std::endl;
    
    if (points.empty()) {
        std::cerr << "Cannot build tree from empty point set" << std::endl;
        return nullptr;
    }
    
    auto tree = std::make_shared<SphereTree>(points, config);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "Sphere tree built in " << duration.count() << " ms" << std::endl;
    std::cout << "  Nodes: " << tree->getNodeCount() << std::endl;
    std::cout << "  Spheres: " << tree->getSphereCount() << std::endl;
    std::cout << "  Max depth: " << tree->getMaxDepth() << std::endl;
    
    return tree;
}

// ============================================================================
// Export Functions
// ============================================================================

void exportToJSON(const SphereTree& tree, const std::string& filename) {
    SphereExporter exporter;
    
    std::cout << "Exporting sphere tree to JSON: " << filename << std::endl;
    
    if (exporter.exportToJSON(tree, filename)) {
        std::cout << "Successfully exported to " << filename << std::endl;
    } else {
        std::cerr << "Failed to export to " << filename << std::endl;
    }
}

void exportToOBJ(const SphereTree& tree, 
                 const std::string& prefix,
                 int maxDepth) {
    SphereExporter exporter;
    
    int actualMaxDepth = maxDepth < 0 ? tree.getMaxDepth() : maxDepth;
    
    std::cout << "Exporting sphere tree to OBJ files (prefix: " << prefix 
              << ", max depth: " << actualMaxDepth << ")" << std::endl;
    
    for (int depth = 0; depth < actualMaxDepth; ++depth) {
        std::string filename = prefix + "_depth_" + std::to_string(depth) + ".obj";
        
        if (exporter.exportToOBJ(tree, filename, depth)) {
            auto spheres = tree.getSpheresAtDepth(depth);
            std::cout << "  Depth " << depth << ": " << spheres.size() 
                      << " spheres -> " << filename << std::endl;
        } else {
            std::cerr << "  Failed to export depth " << depth << std::endl;
        }
    }
}

void exportToCSV(const SphereTree& tree,
                 int depth,
                 const std::string& filename) {
    SphereExporter exporter;
    
    std::cout << "Exporting sphere tree depth " << depth << " to CSV: " << filename << std::endl;
    
    if (exporter.exportToCSV(tree, filename, depth)) {
        auto spheres = tree.getSpheresAtDepth(depth);
        std::cout << "Successfully exported " << spheres.size() << " spheres to " << filename << std::endl;
    } else {
        std::cerr << "Failed to export to " << filename << std::endl;
    }
}

} // namespace spheretree
