#include "Object.h"

Object::Object(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, const char* texture_name)
{
    this->vertices = vertices;
    this->indices = indices;

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

    glGenVertexArrays(1, &this->VAO);
    glBindVertexArray(this->VAO);

    // vertex buffer
    glGenBuffers(1, &this->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &this->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    // Attributes
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(4 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(8 * sizeof(float)));

    Object::LoadTexture(texture_name, this->textureID);
}

Object::~Object()
{
    glDisableVertexArrayAttrib(this->VAO, 2);
    glDisableVertexArrayAttrib(this->VAO, 1);
    glDisableVertexArrayAttrib(this->VAO, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &this->VBO);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &this->VAO);
}

void Object::Draw(glm::mat4& resizeMatrix, Shader* shader, void (*callback)(glm::mat4&, glm::vec3&, glm::vec3&, glm::vec3&, float&, float&))
{
    glBindVertexArray(this->VAO);
    glBindTexture(GL_TEXTURE_2D, this->textureID);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glActiveTexture(GL_TEXTURE0);


    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "resizeMatrix"), 1, GL_FALSE, glm::value_ptr(resizeMatrix));

    callback(transform, current_position, force, rotationCenter, speed, rotation);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "translationMatrix"), 1, GL_FALSE, glm::value_ptr(this->transform));


    glUniform1i(glGetUniformLocation(shader->ID, "myTexture"), 0);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
}
