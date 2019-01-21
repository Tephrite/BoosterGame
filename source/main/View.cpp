#include "View.hpp"
#include "common/shader.hpp"

View::View(int widtht, int heightt) {
    width = widtht;
    height = heightt;
    
}
View::~View() {
    
}

int View::initialise() {
    
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
          return -1;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    
    // Open a window and create its OpenGL context
    window = glfwCreateWindow(width, height, "3D Pacman", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
         return -1;
    }
    glfwMakeContextCurrent(window);
    
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, width/ 2, height/ 2);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    
    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
           return -1;
    }
    
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    //we set the cursor at the centre so that it always start on our origin (0,0,0)
    glfwSetCursorPos(window, width / 2, height / 2);
    
    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
   
    // Read our maze.obj file
    bool res = loadOBJ("maze.obj", vertices, uvs, normals);
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    
    //bounding box
    min_x = max_x = vertices[0].x;
    min_y = max_y = vertices[0].y;
    min_z = max_z = vertices[0].z;
    
    for (int i = 0; i < vertices.size(); i++) {
        if (vertices[i].x < min_x)
            min_x = vertices[i].x;
        if (vertices[i].x > max_x)
            max_x = vertices[i].x;
        if (vertices[i].y < min_y)
            min_y = vertices[i].y;
        if (vertices[i].y > max_y)
           max_y = vertices[i].y;
        if (vertices[i].z < min_z)
            min_z = vertices[i].z;
        if (vertices[i].z > max_z)
            max_z = vertices[i].z;
    }
    
    //std::cout << min_x << " - Min_X "<< std::endl;
    //std::cout << max_x<< " - Max_X "<< std::endl;
    //std::cout << min_y<< " - Min_Y "<< std::endl;
    //std::cout << max_y<< " - Max_Y "<< std::endl;
    //std::cout << min_z<< " - Min_Z "<< std::endl;
    //std::cout << max_z<< " - Max_Z "<< std::endl;
    
    // Read our sphere.obj file
    bool res1 = loadOBJ("sphere.obj", vertices1, uvs1, normals1);
    glGenBuffers(1, &vertexbuffer1);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer1);
    glBufferData(GL_ARRAY_BUFFER, vertices1.size() * sizeof(glm::vec3), &vertices1[0], GL_STATIC_DRAW);
    
    // Read our coin.obj file
    bool res2 = loadOBJ("coin.obj", vertices2, uvs2, normals2);
    glGenBuffers(1, &vertexbuffer2);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
    glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(glm::vec3), &vertices2[0], GL_STATIC_DRAW);
    
    // Read our coin.obj file
    bool res3 = loadOBJ("coin.obj", vertices3, uvs3, normals3);
    glGenBuffers(1, &vertexbuffer3);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer3);
    glBufferData(GL_ARRAY_BUFFER, vertices3.size() * sizeof(glm::vec3), &vertices3[0], GL_STATIC_DRAW);
    
    
    programID = LoadShaders("SimpleVertexShader.hlsl", "SimpleFragmentShader.hlsl");
 
    return 1;
}

glm::mat4 getMVPMatrix(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::mat4 ModelMatrix) {
    glm::mat4 Projection = ProjectionMatrix;
    glm::mat4 View = ViewMatrix;
    glm::mat4 Model = ModelMatrix;
    glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around
    return mvp;
}

void View::update() {
    float x = 0.0f; float y = 0.0f; float z = 0.0f;
    do{
        
        // Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
        glClear( GL_COLOR_BUFFER_BIT );

        
        computeMatricesFromInputs(window, width, height);
        
        //Load MVP - Maze
        GLuint MatrixID = glGetUniformLocation(programID, "MVP");
        glm::mat4 mvp = getMVPMatrix(getProjectionMatrix(), getViewMatrix(), glm::mat4(1.0f));
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

      
        // 1st attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
                              0,                                // attribute 0. No particular reason for 0,       but must match the layout in the vertex shader.
                              3,                                // size
                              GL_FLOAT,                         // type
                              GL_FALSE,                         // normalized?
                              0,                                // stride
                              (void*)0                          // array buffer offset
                              );
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        //glDrawArrays(GL_TRIANGLES, 0, 14755); // first vertex: 0, count: 1 triangle is drawn. 1 triangle x 3 vertices = 3
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() * sizeof(glm::vec3));
        
        //glDisableVertexAttribArray(0);
        
        //Load MVP2
        GLuint MatrixID1 = glGetUniformLocation(programID, "MVP");
        
        glm::mat4 ModelMatrix(1.f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-7.f, .5f, -1.f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.f, 3.f, 3.f));
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(x, y, z));
        glm::mat4 mvp1;
        
        if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
            x+=0.01;
        }
        if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
            x-=0.01;
        }
        if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
            z-=0.01;
        }
        if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
            z+=0.01;
        }
        
        mvp1 = getMVPMatrix(getProjectionMatrix(),
                                     getViewMatrix(),
                                     ModelMatrix);
        glUniformMatrix4fv(MatrixID1, 1, GL_FALSE, &mvp1[0][0]);
        
        // 2nd attribute buffer - Sphere: vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer1);
        glVertexAttribPointer(
                              0,                                // attribute 0. No particular reason for 0,       but must match the layout in the vertex shader.
                              3,                                // size
                              GL_FLOAT,                         // type
                              GL_FALSE,                         // normalized?
                              0,                                // stride
                              (void*)0                          // array buffer offset
                              );
        
        // The following code draws a triangle using the function void glDrawArrays(     GLenum mode,      GLint first,      GLsizei count);
        
        glDrawArrays(GL_TRIANGLES, 0, vertices1.size() * sizeof(glm::vec3));
        
        //glDisableVertexAttribArray(0);
        
        //Load MVP3
        GLuint MatrixID2 = glGetUniformLocation(programID, "MVP");
        
        glm::mat4 ModelMatrix2;
        ModelMatrix2 = glm::rotate(ModelMatrix2, glm::radians(90.f), glm::vec3(0.f, 0.1f, 0.f));
        ModelMatrix2 = glm::translate(ModelMatrix2, glm::vec3(0.5f, -1.5f, -11.f));
        //ModelMatrix2 = glm::scale(ModelMatrix2, glm::vec3(3.f, 3.f, 3.f));

        glm::mat4 mvp2 = getMVPMatrix(getProjectionMatrix(),
                                      getViewMatrix(),
                                      ModelMatrix2);
        glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &mvp2[0][0]);
        
        // 3rd attribute buffer - Coin: vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
        glVertexAttribPointer(
                              0,                                // attribute 0. No particular reason for 0,       but must match the layout in the vertex shader.
                              3,                                // size
                              GL_FLOAT,                         // type
                              GL_FALSE,                         // normalized?
                              0,                                // stride
                              (void*)0                          // array buffer offset
                              );
        
        // The following code draws a triangle using the function void glDrawArrays(     GLenum mode,      GLint first,      GLsizei count);
        
        glDrawArrays(GL_TRIANGLES, 0, vertices2.size() * sizeof(glm::vec3));
        
        //Load MVP4
        GLuint MatrixID3 = glGetUniformLocation(programID, "MVP");
        
        glm::mat4 ModelMatrix3;
        ModelMatrix3 = glm::rotate(ModelMatrix3, glm::radians(90.f), glm::vec3(0.f, 0.1f, 0.f));
        ModelMatrix3 = glm::translate(ModelMatrix3, glm::vec3(5.5f, -1.5f, -6.f));
        //ModelMatrix3 = glm::scale(ModelMatrix3, glm::vec3(3.f, 3.f, 3.f));
        
        glm::mat4 mvp3 = getMVPMatrix(getProjectionMatrix(),
                                      getViewMatrix(),
                                      ModelMatrix3);
        glUniformMatrix4fv(MatrixID3, 1, GL_FALSE, &mvp3[0][0]);
        
        // 4th attribute buffer - Coin: vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer3);
        glVertexAttribPointer(
                              0,                                // attribute 0. No particular reason for 0,       but must match the layout in the vertex shader.
                              3,                                // size
                              GL_FLOAT,                         // type
                              GL_FALSE,                         // normalized?
                              0,                                // stride
                              (void*)0                          // array buffer offset
                              );
        
        // The following code draws a triangle using the function void glDrawArrays(     GLenum mode,      GLint first,      GLsizei count);
        
        glDrawArrays(GL_TRIANGLES, 0, vertices3.size() * sizeof(glm::vec3));
        
        glDisableVertexAttribArray(0);

        // Use our shader
        glUseProgram(programID);
        
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        
        
    } // Check if the ESC key was pressed or the window was closed
    
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0 );
    
    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &vertexbuffer1);
    glDeleteBuffers(1, &vertexbuffer2);
    glDeleteBuffers(1, &vertexbuffer3);
    glDeleteBuffers(1, &colorbuffer);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VertexArrayID);
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    
    
    
}

