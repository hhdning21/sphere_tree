/**
 * @file SphereTreeAPI.cpp
 * @brief Implementation of high-level API for sphere tree construction
 */

#include "SphereTreeAPI.h"
#include <chrono>
#include <iostream>

namespace spheretree {

// ============================================================================
// Helper: Get configuration for quality preset
// ============================================================================

SphereTreeConfig getQualityConfig(TreeQuality quality) {
    SphereTreeConfig config;
    
    switch (quality) {
        case TreeQuality::FAST:
            config.maxDepth = 4;
            config.fittingMethod = SphereTreeConfig::FittingMethod::RITTER;
            config.subdivisionStrategy = SphereTreeConfig::SubdivisionStrategy::OCTREE;
            break;
            
        case TreeQuality::BALANCED:
            config.maxDepth = 6;
            config.fittingMethod = SphereTreeConfig::FittingMethod::WELZL;
            config.subdivisionStrategy = SphereTreeConfig::SubdivisionStrategy::MEDIAL_AXIS;
            break;
            
        case TreeQuality::HIGH_DETAIL:
            config.maxDepth = 8;
            config.fittingMethod = SphereTreeConfig::FittingMethod::WELZL;
            config.subdivisionStrategy = SphereTreeConfig::SubdivisionStrategy::MEDIAL_AXIS;
            config.medialAxisSampleMultiplier = 15;
            break;
    }
    
    return config;
}

// ============================================================================
// Factory Functions - Build sphere trees
// ============================================================================

std::shared_ptr<SphereTree> buildTreeFromOBJ(const std::string& filename,
                                             TreeQuality quality) {
    SphereTreeConfig config = getQualityConfig(quality);
    std::vector<Point3D> points = loadObjFile(filename);
    
    if (points.empty()) {
        std::cerr << "Error: Failed to load OBJ file: " << filename << std::endl;
        return nullptr;
    }
    
    std::cout << "Loaded " << points.size() << " vertices from " << filename << std::endl;
    return buildTree(points, config);
}

std::shared_ptr<SphereTree> buildTree(const std::vector<Point3D>& points,
                                     TreeQuality quality) {
    return buildTree(points, getQualityConfig(quality));
}

std::shared_ptr<SphereTree> buildTree(const std::vector<Point3D>& points,
                                     const SphereTreeConfig& config) {
    if (points.empty()) {
        std::cerr << "Error: Empty point cloud" << std::endl;
        return nullptr;
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    auto tree = std::make_shared<SphereTree>(points, config);
    auto end = std::chrono::high_resolution_clock::now();
    
    double buildTime = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << "Built sphere tree in " << buildTime << " ms" << std::endl;
    std::cout << "Tree depth: " << tree->getMaxDepth() << std::endl;
    std::cout << "Total spheres: " << tree->getSphereCount() << std::endl;
    
    return tree;
}

// ============================================================================
// Export Functions
// ============================================================================

void exportToJSON(const SphereTree& tree, const std::string& filename) {
    // Export tree structure to JSON - using CSV exporter as base
    // Note: SphereExporter doesn't have JSON export, just CSV
    std::cerr << "Warning: JSON export not yet implemented, using CSV format" << std::endl;
    spheretree::exportDepthToCSV(tree, 0, filename);
}

void exportToOBJ(const SphereTree& tree, 
                 const std::string& prefix, 
                 int maxDepth) {
    int depth = (maxDepth < 0) ? tree.getMaxDepth() : std::min(maxDepth, tree.getMaxDepth());
    
    for (int d = 0; d <= depth; d++) {
        std::string filename = prefix + "_depth_" + std::to_string(d) + ".obj";
        spheretree::exportSpheresToOBJ(tree, d, filename);
    }
}

void exportToCSV(const SphereTree& tree,
                 int depth,
                 const std::string& filename) {
    spheretree::exportDepthToCSV(tree, depth, filename);
}

} // namespace spheretree
