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
    const char *vertexShaderSource = 
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aNormal;\n"
        "out vec3 Normal;\n"
        "out vec3 FragPos;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main()\n"
        "{\n"
        "   FragPos = vec3(model * vec4(aPos, 1.0));\n"
        "   Normal = mat3(transpose(inverse(model))) * aNormal;\n"
        "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
        "}\0";
        
    // Fragment Shader
    const char *fragmentShaderSource = 
        "#version 330 core\n"
        "in vec3 Normal;\n"
        "in vec3 FragPos;\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   vec3 objectColor = vec3(1.0, 0.5, 0.2);\n"
        "   vec3 lightColor = vec3(1.0, 1.0, 1.0);\n"
        "   vec3 lightPos = vec3(2.0, 5.0, 3.0);\n"
            
        "   float ambientStrength = 0.2;\n"
        "   vec3 ambient = ambientStrength * lightColor;\n"
        "   vec3 norm = normalize(Normal);\n"
        "   vec3 lightDir = normalize(lightPos - FragPos);\n"
        "   float diff = max(dot(norm, lightDir), 0.0);\n"
        "   vec3 diffuse = diff * lightColor;\n"
        "   vec3 result = (ambient + diffuse) * objectColor;\n"
        "   FragColor = vec4(result, 1.0);\n"
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
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));
    
    glm::mat4 projection = glm::mat4(1.0f); // field of view, aspect ratio, near plane, far plane
    projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);
    
    std::vector<float> objVertices; // Store vertices and normals from the OBJ file
    
    Assimp::Importer importer;
    std::string path = "../models/Suzanne.obj"; 
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
            }
        }
    }

    // VBO & VAO
    // VBO example, hardcoded vertices for a cube
    // float vertices[] = { 
    //     -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  
    //     0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
    //     -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  
    //     0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
    //     -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  
    //     -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
    //     0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  
    //     0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
    //     -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  
    //     0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f,
    //     -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  
    //     0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f
    // };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER, objVertices.size()*sizeof(float) , objVertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    while(!glfwWindowShouldClose(window)){
        glClearColor(0.2f,0.231f,0.2f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 model = glm::mat4(1.0f); // object
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f)); // scalling obj
        float angle = glfwGetTime() * glm::radians(5.0f); // rotate n degrees per second
        model = glm::rotate(model, angle , glm::vec3(0.0f, 0.5f, 0.0f));

        glUseProgram(shaderProgram);

        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0 , objVertices.size()/6);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}

