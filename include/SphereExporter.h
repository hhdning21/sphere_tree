/**
 * @file SphereExporter.h
 * @brief Export sphere trees to various formats for visualization
 */

#ifndef SPHERE_EXPORTER_H
#define SPHERE_EXPORTER_H

#include "SphereTree.h"
#include <fstream>
#include <string>
#include <iomanip>
#include <cmath>

namespace spheretree {

/**
 * @brief Export spheres at a specific depth to CSV format
 * @param tree Sphere tree to export
 * @param depth Tree depth level to export
 * @param filename Output CSV filename
 * @return true if successful
 */
inline bool exportDepthToCSV(const SphereTree& tree, int depth, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "center_x,center_y,center_z,radius,error\n";
    
    auto spheres = tree.getSpheresAtDepth(depth);
    for (const auto& sphere : spheres) {
        file << std::fixed << std::setprecision(6)
             << sphere.center.x << ","
             << sphere.center.y << ","
             << sphere.center.z << ","
             << sphere.radius << ","
             << sphere.error << "\n";
    }
    
    file.close();
    std::cout << "Exported depth " << depth << " to: " << filename << std::endl;
    return true;
}

/**
 * @brief Export spheres as OBJ mesh (icosphere approximation)
 * @param tree Sphere tree to export
 * @param depth Tree depth level to export
 * @param filename Output OBJ filename
 * @param subdivisions Sphere mesh subdivision level
 * @return true if successful
 */
inline bool exportSpheresToOBJ(const SphereTree& tree, int depth, const std::string& filename, int subdivisions = 1) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    auto spheres = tree.getSpheresAtDepth(depth);
    
    file << "# Sphere tree export - depth " << depth << "\n";
    file << "# Total spheres: " << spheres.size() << "\n\n";
    
    int vertexOffset = 1;
    
    // Create icosahedron vertices for each sphere
    const double phi = (1.0 + std::sqrt(5.0)) / 2.0;
    std::vector<Point3D> icoVertices = {
        Point3D(-1,  phi, 0), Point3D( 1,  phi, 0), Point3D(-1, -phi, 0), Point3D( 1, -phi, 0),
        Point3D( 0, -1,  phi), Point3D( 0,  1,  phi), Point3D( 0, -1, -phi), Point3D( 0,  1, -phi),
        Point3D( phi, 0, -1), Point3D( phi, 0,  1), Point3D(-phi, 0, -1), Point3D(-phi, 0,  1)
    };
    
    // Normalize
    for (auto& v : icoVertices) {
        double len = v.length();
        v = v / len;
    }
    
    for (const auto& sphere : spheres) {
        // Write vertices
        for (const auto& v : icoVertices) {
            Point3D scaled = v * sphere.radius;
            Point3D vertex = sphere.center + scaled;
            file << "v " << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
        }
        
        // Write faces (icosahedron)
        std::vector<std::vector<int>> faces = {
            {0, 11, 5}, {0, 5, 1}, {0, 1, 7}, {0, 7, 10}, {0, 10, 11},
            {1, 5, 9}, {5, 11, 4}, {11, 10, 2}, {10, 7, 6}, {7, 1, 8},
            {3, 9, 4}, {3, 4, 2}, {3, 2, 6}, {3, 6, 8}, {3, 8, 9},
            {4, 9, 5}, {2, 4, 11}, {6, 2, 10}, {8, 6, 7}, {9, 8, 1}
        };
        
        for (const auto& face : faces) {
            file << "f " 
                 << (face[0] + vertexOffset) << " "
                 << (face[1] + vertexOffset) << " "
                 << (face[2] + vertexOffset) << "\n";
        }
        
        vertexOffset += icoVertices.size();
    }
    
    file.close();
    std::cout << "Exported " << spheres.size() << " spheres (depth " << depth << ") to OBJ: " << filename << std::endl;
    return true;
}

} // namespace spheretree

#endif // SPHERE_EXPORTER_H
