/**
 * @file ObjLoader.h
 * @brief Simple OBJ file loader for extracting vertices
 */

#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include "SphereTree.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

namespace spheretree {

/**
 * @brief Load vertices from a Wavefront OBJ file
 * @param filename Path to OBJ file
 * @return Vector of 3D points (vertices)
 */
inline std::vector<Point3D> loadObjFile(const std::string& filename) {
    std::vector<Point3D> vertices;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open OBJ file: " << filename << std::endl;
        return vertices;
    }
    
    std::string line;
    int vertexCount = 0;
    int faceCount = 0;
    
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        
        if (prefix == "v") {
            // Vertex line: v x y z
            double x, y, z;
            if (iss >> x >> y >> z) {
                vertices.emplace_back(x, y, z);
                vertexCount++;
            }
        } else if (prefix == "f") {
            // Face line (we count but don't parse)
            faceCount++;
        }
    }
    
    file.close();
    
    std::cout << "Loaded OBJ file: " << filename << std::endl;
    std::cout << "  Vertices: " << vertexCount << std::endl;
    std::cout << "  Faces: " << faceCount << std::endl;
    
    return vertices;
}

} // namespace spheretree

#endif // OBJ_LOADER_H
