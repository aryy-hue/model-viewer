// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// GLAD & GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Timing variables
float deltaTime = 0.0f; 
float lastFrame = 0.0f;

float lastX = 400;
float lastY = 300;
bool firstMouse = true;

float yaw = -90.0f;
float pitch = 0.0f;

void inputProcess(GLFWwindow *window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
    float cameraSpeed = 2.5f * deltaTime;

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        cameraPos += cameraSpeed * cameraFront;
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        cameraPos -= cameraSpeed * cameraFront;
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
}

void mouse_callback(GLFWwindow*window , double xpos ,double ypos){
    if(firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f){
        pitch = 89.0f;
    }
    if(pitch < -89.0f){
        pitch = -89.0f; 
    }

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)*cos(glm::radians(pitch)));
    cameraFront = glm::normalize(front);
}

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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // OpenGL Shading Language (GLSL)
    // Vertex Shader
    const char *vertexShaderSource = 
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aNormal;\n"
        "layout (location = 2) in vec2 aTexCoord;\n"
        
        "out vec3 Normal;\n"
        "out vec3 FragPos;\n"
        "out vec2 TexCoord;\n"

        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        
        "void main()\n"
        "{\n"
        "   FragPos = vec3(model * vec4(aPos, 1.0));\n"
        "   Normal = mat3(transpose(inverse(model))) * aNormal;\n"
        "   TexCoord = aTexCoord;\n"
        "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
        "}\0";
        
    // Fragment Shader
    const char *fragmentShaderSource = 
        "#version 330 core\n"
        "in vec3 Normal;\n"
        "in vec3 FragPos;\n"
        "in vec2 TexCoord;\n"
        "out vec4 FragColor;\n"

        "uniform sampler2D texture1;\n"
        "uniform vec3 viewPos;\n"

        "void main()\n"
        "{\n"
        "   vec3 objectColor = vec3(texture(texture1, TexCoord));\n"
        "   vec3 lightColor = vec3(1.0, 1.0, 1.0);\n"
        "   vec3 lightPos = vec3(2.0, 5.0, 3.0);\n"
        // Ambient
        "   float ambientStrength = 0.2;\n"
        "   vec3 ambient = ambientStrength * lightColor;\n"
        // Diffuse
        "   vec3 norm = normalize(Normal);\n"
        "   vec3 lightDir = normalize(lightPos - FragPos);\n"
        "   float diff = max(dot(norm, lightDir), 0.0);\n"
        "   vec3 diffuse = diff * lightColor;\n"
        // Specular
        "float specularStrength = 0.5;\n"
        "vec3 viewDir = normalize(viewPos - FragPos);\n"
        "vec3 reflectDir = reflect(-lightDir, norm);\n"
        "float spec = pow(max(dot(viewDir, reflectDir),0.0), 64);\n"
        "vec3 specular = specularStrength * spec * lightColor;\n"

        "vec3 result = (ambient + diffuse + specular) * objectColor;\n"
        "FragColor = vec4(result, 1.0);\n"

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
        
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
        glEnable(GL_DEPTH_TEST); // Depth Enabling
        
        // OpenGL Mathematics (GLM)
        glm::mat4 view = glm::mat4(1.0f); // camera
        
        glm::mat4 projection = glm::mat4(1.0f); // field of view, aspect ratio, near plane, far plane
        projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);
        
        std::vector<float> objVertices; // Store vertices and normals from the OBJ file
        
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, mouse_callback);

        Assimp::Importer importer;
        std::string path = "../models/chicken.obj"; 
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    
    if(!scene || scene -> mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene -> mRootNode){
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return -1;
    }else{
        std::cout << "Model loaded successfully: " << path << std::endl;
    }
    
    for (unsigned int m =0; m < scene -> mNumMeshes; m++){
        aiMesh * mesh = scene -> mMeshes[m]; // instead of hardcoded to determined meshes
        
        for (unsigned int i = 0; i <mesh -> mNumFaces; i++){
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++){
                unsigned int index = face.mIndices[j];
                
                objVertices.push_back(mesh -> mVertices[index].x);
                objVertices.push_back(mesh -> mVertices[index].y);
                objVertices.push_back(mesh -> mVertices[index].z);
                // Check if the mesh has normals
                if (mesh -> HasNormals()){
                    objVertices.push_back(mesh -> mNormals[index].x);
                    objVertices.push_back(mesh -> mNormals[index].y);
                    objVertices.push_back(mesh -> mNormals[index].z);
                }else{
                    objVertices.push_back(0.0f);
                    objVertices.push_back(0.0f);
                    objVertices.push_back(1.0f);
                }
                // Check if the mesh has texture coordinates
                if(mesh -> mTextureCoords[0]){
                    objVertices.push_back(mesh-> mTextureCoords[0][index].x);
                    objVertices.push_back(mesh-> mTextureCoords[0][index].y);
                }else{
                    objVertices.push_back(0.0f);
                    objVertices.push_back(0.0f);
                }
            }
        }
    }

    // VBO & VAO
    // VBO example, hardcoded vertices for a cube
    // float vertices[] = { 
    //         -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  
    //         0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
    //         -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  
    //         0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
    //         -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  
    //         -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
    //         0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  
    //         0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
    //         -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  
    //         0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f,
    //         -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  
    //         0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f
    //     };
        
        unsigned int VBO, VAO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER,VBO);
        glBufferData(GL_ARRAY_BUFFER, objVertices.size()*sizeof(float) , objVertices.data(), GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6* sizeof(float)));
        glEnableVertexAttribArray(2);
        
        glBindVertexArray(0);
        
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_set_flip_vertically_on_load(true);
        int width, height, nrChannels;
        unsigned char *data = stbi_load("../models/fur.jpeg", &width, &height, &nrChannels, 0);
        if(data){
            GLenum format;
            if(nrChannels == 1) format = GL_RED;
            else if(nrChannels == 3) format = GL_RGB;
            else if(nrChannels == 4) format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }else{
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);

        while(!glfwWindowShouldClose(window)){
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            
            inputProcess(window);
            glClearColor(0.31f, 0.031f, 0.788f,1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            glm::mat4 model = glm::mat4(1.0f); // object
            model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f)); // scalling obj
            float angle = glfwGetTime() * glm::radians(50.0f); // rotate n degrees per second
            model = glm::rotate(model, angle , glm::vec3(0.0f, 0.5f, 0.0f));
            
            glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

            glUseProgram(shaderProgram);
            
            int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
            glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);

            int modelLoc = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            
            int viewLoc = glGetUniformLocation(shaderProgram, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            
            int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
            
            glBindTexture(GL_TEXTURE_2D, texture);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0 , objVertices.size()/8);
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        
        glfwTerminate();
        return 0;
    }
    
    