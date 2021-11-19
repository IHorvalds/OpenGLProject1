#pragma once

#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Vertex.h"
#include "Shader.h"
#include "SOIL.h"

#include "glm/gtc/type_ptr.hpp"

class Object
{
private:
    GLuint VAO, VBO, EBO, textureID;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    glm::mat4 transform = glm::mat4(1.f);
    glm::vec3 current_position = glm::vec3(0.f);
    glm::vec3 force = glm::vec3(0.f);
    glm::vec3 rotationCenter = glm::vec3(0.f);
    float rotation = 0.f;
    float speed = 0.2f;

public:
    Object() {};
    Object(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, const char* texture_name);
    ~Object();

    void Draw(glm::mat4& resizeMatrix, Shader* shader, void (*callback)(glm::mat4&, glm::vec3&, glm::vec3&, glm::vec3&, float&, float&));
    void Reset() {
        this->rotation = 0.f;
        this->speed = 0.2f;
        this->transform = glm::mat4(1.f);
        this->current_position = glm::vec3(0.f);
        this->force = glm::vec3(0.f);
        
        glm::vec3 center(0.f);

        for (auto& v : this->vertices)
        {
            center.x += v.position.x;
            center.y += v.position.y;
        }

        center.x /= (float)this->vertices.size();
        center.y /= (float)this->vertices.size();
        this->rotationCenter = center;
        this->rotationCenter.x -= 35.f;
    }

    glm::mat4& GetTransform()
    {
        return this->transform;
    }

    static void LoadTexture(const char* texture_name, GLuint& texture)
    {

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        int width, height;
        unsigned char* image = SOIL_load_image(texture_name, &width, &height, 0, SOIL_LOAD_AUTO);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);

        SOIL_free_image_data(image);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};

