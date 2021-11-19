#include "Shader.h"
#include <fstream>
#include <cerrno>

Shader::Shader(const char* vertexShader, const char* fragmentShader)
{
    std::string v_s = Shader::get_file_content(vertexShader);
    std::string f_s = Shader::get_file_content(fragmentShader);
    const char* p_vertex = v_s.c_str();
    const char* p_fragment = f_s.c_str();

    GLuint vertexShaderProgram = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderProgram = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertexShaderProgram, 1, &p_vertex, NULL);
    glCompileShader(vertexShaderProgram);

    glShaderSource(fragmentShaderProgram, 1, &p_fragment, NULL);
    glCompileShader(fragmentShaderProgram);

    ID = glCreateProgram();

    glAttachShader(ID, vertexShaderProgram);
    glAttachShader(ID, fragmentShaderProgram);

    glLinkProgram(ID);
}

void Shader::Activate()
{
    glUseProgram(ID);
}

void Shader::Delete()
{
    glDeleteProgram(ID);
}

std::string Shader::get_file_content(const char* file_path)
{
    std::ifstream input(file_path, std::ios_base::binary);
    if (input)
    {
        std::string content;
        input.seekg(9, std::ios::end);
        content.resize(input.tellg());
        input.seekg(0, std::ios::beg);
        input.read(&content[0], content.size());
        input.close();
        return content;
    }
    throw errno;
}
