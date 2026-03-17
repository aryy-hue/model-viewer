#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main(){
    glfwInit(); // initialize
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800,600, "Model Viewer", NULL, NULL);
    if(window == NULL){
        std::cout << "Failed to create Window" << std::endl;
        glfwTerminate();
        return -1;
    }else{
        glfwMakeContextCurrent(window);
    }

    // GLAD
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // OpenGL Shading Language (GLSL)
    // Vertex Shader
    const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   // Projection * View * Model * Posisi\n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
    "}\0";

    // Fragment Shader
    const char *fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f); // Warna oranye\n"
        "}\n\0";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);


    // VBO & VAO
    float vertices[] = { // VBO
        -0.5f, -0.5f, 0.0f, 
         0.5f, -0.5f, 0.0f, 
         0.0f,  0.5f, 0.0f  
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0,3 , GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // OpenGL Mathematics (GLM)
    glm::mat4 view = glm::mat4(1.0f); // camera
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    
    glm::mat4 projection = glm::mat4(1.0f); // field of view, aspect ratio, near plane, far plane
    projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);
    
    while(!glfwWindowShouldClose(window)){
        glClearColor(0.2f,0.3f,0.3f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glm::mat4 model = glm::mat4(1.0f); // model
        float angle = glfwGetTime() * glm::radians(50.0f); // Rotate 50 degrees per second
        model = glm::rotate(model, angle , glm::vec3(0.5f, 1.0f, 0.0f));

        glUseProgram(shaderProgram);

        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0 , 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}

