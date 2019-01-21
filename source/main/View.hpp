#pragma once
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
#include <vector>

// Include GLM
#include <glm/glm.hpp>
using namespace glm;
#include "common/objloader.hpp"
#include "common/vboindexer.hpp"
#include <glm/gtx/transform.hpp>
#include "common/shader.hpp"
#include "common/controls.hpp"

class View {
private:
    
    GLFWwindow* window;
    
    GLuint programID;
    
    GLuint VertexArrayID;

    GLuint vertexbuffer, vertexbuffer1, vertexbuffer2, vertexbuffer3;
    
    std::vector<glm::vec3> vertices, vertices1, vertices2, vertices3;
    std::vector<glm::vec2> uvs, uvs1, uvs2, uvs3; // Won't be used at the moment.
    std::vector<glm::vec3> normals, normals1, normals2, normals3; // Won't be used at the moment.
    
    std::vector<glm::vec2> fVertices;
    std::vector<glm::vec2> fUVs;

    GLuint colorbuffer;
    
    GLfloat min_x, max_x,
            min_y, max_y,
            min_z, max_z;
    
    //screen size
    int width;
    int height;
  
    
    
    
    
public:
    View(int widtht, int heightt);
    ~View();
    int initialise();
    void update();

    
};

