#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>

class Shader
{
public:
    GLuint ID;
    Shader(const char* vertexShader, const char* fragmentShader);

    void Activate();
    void Delete();

private:
    static std::string get_file_content(const char* file_path);
};

