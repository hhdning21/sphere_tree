/**
 * @file SphereTreeAPI.h
 * @brief High-level API for sphere tree construction from 3D models
 * 
 * This file provides a simple, user-friendly interface for building sphere trees
 * from OBJ files and point clouds. It abstracts away the configuration complexity
 * and provides quality presets for common use cases.
 * 
 * Features:
 * - Load OBJ files and build sphere trees
 * - Quality presets (FAST, BALANCED, HIGH_DETAIL)
 * - Custom configuration support
 * - Export to JSON, OBJ, and CSV formats
 * - Access to underlying tree structure
 * 
 * Example usage:
 * @code
 * // Build tree from OBJ file
 * auto tree = buildTreeFromOBJ("model.obj", TreeQuality::BALANCED);
 * 
 * // Export for visualization
 * exportToJSON(*tree, "tree_data.json");
 * exportToOBJ(*tree, "spheres", 3); // Export first 3 levels
 * @endcode
 */

#ifndef SPHERE_TREE_API_H
#define SPHERE_TREE_API_H

#include "SphereTree.h"
#include "ObjLoader.h"
#include "SphereExporter.h"
#include <memory>
#include <string>
#include <vector>

namespace spheretree {

/**
 * @brief Quality presets for sphere tree construction
 */
enum class TreeQuality {
    FAST,        // Quick approximation, lower accuracy (depth 4, fast fitting)
    BALANCED,    // Balanced speed and accuracy (depth 6, recommended)
    HIGH_DETAIL  // High accuracy, slower build (depth 8, detailed fitting)
};

// ============================================================================
// Factory Functions - Build sphere trees from 3D models
// ============================================================================

/**
 * @brief Build sphere tree from OBJ file with quality preset
 * @param filename Path to OBJ file
 * @param quality Quality preset (FAST, BALANCED, or HIGH_DETAIL)
 * @return Shared pointer to constructed SphereTree
 */
std::shared_ptr<SphereTree> buildTreeFromOBJ(
    const std::string& filename,
    TreeQuality quality = TreeQuality::BALANCED);

/**
 * @brief Build sphere tree from point cloud with quality preset
 * @param points Input point cloud
 * @param quality Quality preset
 * @return Shared pointer to constructed SphereTree
 */
std::shared_ptr<SphereTree> buildTree(
    const std::vector<Point3D>& points,
    TreeQuality quality = TreeQuality::BALANCED);

/**
 * @brief Build sphere tree from point cloud with custom configuration
 * @param points Input point cloud
 * @param config Custom configuration
 * @return Shared pointer to constructed SphereTree
 */
std::shared_ptr<SphereTree> buildTree(
    const std::vector<Point3D>& points,
    const SphereTreeConfig& config);

/**
 * @brief Get configuration for a quality preset
 * @param quality Quality preset
 * @return Configuration matching the preset
 */
SphereTreeConfig getQualityConfig(TreeQuality quality);

// ============================================================================
// Export Functions - Save sphere trees in various formats
// ============================================================================

/**
 * @brief Export complete tree structure to JSON
 * @param tree Tree to export
 * @param filename Output filename
 */
void exportToJSON(const SphereTree& tree, const std::string& filename);

/**
 * @brief Export spheres at each depth level to separate OBJ files
 * @param tree Tree to export
 * @param prefix Filename prefix (e.g., "spheres" -> "spheres_depth_0.obj")
 * @param maxDepth Maximum depth to export (default: all levels)
 */
void exportToOBJ(const SphereTree& tree, 
                 const std::string& prefix, 
                 int maxDepth = -1);

/**
 * @brief Export spheres at specific depth to CSV
 * @param tree Tree to export
 * @param depth Depth level to export
 * @param filename Output filename
 */
void exportToCSV(const SphereTree& tree, 
                 int depth,
                 const std::string& filename);

} // namespace spheretree

#endif // SPHERE_TREE_API_H
