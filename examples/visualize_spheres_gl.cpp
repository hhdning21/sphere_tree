/**
 * @file visualize_spheres_gl.cpp
 * @brief Interactive OpenGL visualization of sphere trees
 */

#include "SphereTree.h"
#include "ObjLoader.h"
#include <iostream>
#include <cmath>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

using namespace spheretree;

// Global state
SphereTree* g_tree = nullptr;
std::vector<Point3D> g_vertices;
int g_currentDepth = 0;
int g_maxDepth = 0;
bool g_showVertices = false;
bool g_showWireframe = false;
bool g_showSpheres = true;
bool g_hierarchyMode = false;

// Camera state
float g_rotationX = 30.0f;
float g_rotationY = 45.0f;
float g_zoom = -50.0f;
int g_lastMouseX = 0;
int g_lastMouseY = 0;
int g_mouseButton = -1;

// Colors for depth levels
const float DEPTH_COLORS[][3] = {
    {1.0f, 0.0f, 0.0f},  // Red
    {0.0f, 1.0f, 0.0f},  // Green
    {0.0f, 0.0f, 1.0f},  // Blue
    {1.0f, 1.0f, 0.0f},  // Yellow
    {1.0f, 0.0f, 1.0f},  // Magenta
    {0.0f, 1.0f, 1.0f},  // Cyan
    {1.0f, 0.5f, 0.0f},  // Orange
    {0.5f, 0.0f, 1.0f},  // Purple
};

void drawSphere(const Sphere& sphere, int depth) {
    glPushMatrix();
    glTranslatef(sphere.center.x, sphere.center.y, sphere.center.z);
    
    // Color based on error (green = tight fit, red = loose fit)
    float errorRatio = std::min(1.0f, static_cast<float>(sphere.error / sphere.radius));
    glColor4f(errorRatio, 1.0f - errorRatio, 0.0f, 0.3f);
    
    if (g_showWireframe) {
        glutWireSphere(sphere.radius, 16, 16);
    } else {
        glutSolidSphere(sphere.radius, 16, 16);
    }
    
    glPopMatrix();
}

void drawVertices() {
    glPointSize(2.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POINTS);
    for (const auto& v : g_vertices) {
        glVertex3f(v.x, v.y, v.z);
    }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    // Camera setup
    glTranslatef(0.0f, 0.0f, g_zoom);
    glRotatef(g_rotationX, 1.0f, 0.0f, 0.0f);
    glRotatef(g_rotationY, 0.0f, 1.0f, 0.0f);
    
    // Draw vertices
    if (g_showVertices) {
        drawVertices();
    }
    
    // Draw spheres
    if (g_showSpheres && g_tree) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        if (g_hierarchyMode) {
            // Show all levels from 0 to current depth
            for (int d = 0; d <= g_currentDepth; ++d) {
                auto spheres = g_tree->getSpheresAtDepth(d);
                for (const auto& sphere : spheres) {
                    drawSphere(sphere, d);
                }
            }
        } else {
            // Show only current depth
            auto spheres = g_tree->getSpheresAtDepth(g_currentDepth);
            for (const auto& sphere : spheres) {
                drawSphere(sphere, g_currentDepth);
            }
        }
        
        glDisable(GL_BLEND);
    }
    
    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)width / (double)height, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        g_mouseButton = button;
        g_lastMouseX = x;
        g_lastMouseY = y;
    } else {
        g_mouseButton = -1;
    }
}

void motion(int x, int y) {
    int dx = x - g_lastMouseX;
    int dy = y - g_lastMouseY;
    
    if (g_mouseButton == GLUT_LEFT_BUTTON) {
        g_rotationY += dx * 0.5f;
        g_rotationX += dy * 0.5f;
    } else if (g_mouseButton == GLUT_RIGHT_BUTTON) {
        g_zoom += dy * 0.1f;
    }
    
    g_lastMouseX = x;
    g_lastMouseY = y;
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case '+':
        case '=':
            if (g_currentDepth < g_maxDepth) {
                g_currentDepth++;
                std::cout << "Depth: " << g_currentDepth << std::endl;
                glutPostRedisplay();
            }
            break;
        case '-':
        case '_':
            if (g_currentDepth > 0) {
                g_currentDepth--;
                std::cout << "Depth: " << g_currentDepth << std::endl;
                glutPostRedisplay();
            }
            break;
        case 'h':
        case 'H':
            g_hierarchyMode = !g_hierarchyMode;
            std::cout << "Hierarchy mode: " << (g_hierarchyMode ? "ON" : "OFF") << std::endl;
            glutPostRedisplay();
            break;
        case 'v':
        case 'V':
            g_showVertices = !g_showVertices;
            glutPostRedisplay();
            break;
        case 'w':
        case 'W':
            g_showWireframe = !g_showWireframe;
            glutPostRedisplay();
            break;
        case 's':
        case 'S':
            g_showSpheres = !g_showSpheres;
            glutPostRedisplay();
            break;
        case 'r':
        case 'R':
            g_rotationX = 30.0f;
            g_rotationY = 45.0f;
            g_zoom = -50.0f;
            glutPostRedisplay();
            break;
        case 'q':
        case 'Q':
        case 27: // ESC
            exit(0);
            break;
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <obj_file>\n";
        return 1;
    }
    
    std::cout << "========================================\n";
    std::cout << "Sphere Tree OpenGL Visualization\n";
    std::cout << "========================================\n\n";
    
    // Load OBJ file
    std::cout << "Loading: " << argv[1] << "\n";
    g_vertices = loadObjFile(argv[1]);
    
    if (g_vertices.empty()) {
        std::cerr << "Error: Failed to load OBJ file!\n";
        return 1;
    }
    
    std::cout << "Loaded " << g_vertices.size() << " vertices\n\n";
    
    // Build sphere tree
    std::cout << "Building sphere tree...\n";
    SphereTreeConfig config;
    config.maxDepth = 6;
    config.maxSpheresPerNode = 8;
    
    g_tree = new SphereTree(g_vertices, config);
    g_maxDepth = g_tree->getMaxDepth();
    
    std::cout << "Tree built: " << g_tree->getSphereCount() 
              << " spheres, max depth: " << g_maxDepth << "\n\n";
    
    // Print controls
    std::cout << "========================================\n";
    std::cout << "Controls:\n";
    std::cout << "========================================\n";
    std::cout << "  Mouse left drag:  Rotate view\n";
    std::cout << "  Mouse right drag: Zoom in/out\n";
    std::cout << "  +/- keys:         Change depth level\n";
    std::cout << "  H key:            Toggle hierarchy mode (show all depths 0-current)\n";
    std::cout << "  V key:            Toggle vertices\n";
    std::cout << "  W key:            Toggle wireframe/solid\n";
    std::cout << "  S key:            Toggle spheres\n";
    std::cout << "  R key:            Reset view\n";
    std::cout << "  Q/ESC:            Quit\n";
    std::cout << "========================================\n\n";
    
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1024, 768);
    glutCreateWindow("Sphere Tree Visualization");
    
    // OpenGL setup
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    GLfloat light_pos[] = {10.0f, 10.0f, 10.0f, 1.0f};
    GLfloat light_amb[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat light_dif[] = {0.8f, 0.8f, 0.8f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_dif);
    
    // Callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);
    
    // Start
    glutMainLoop();
    
    delete g_tree;
    return 0;
}
