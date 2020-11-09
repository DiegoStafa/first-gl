#include "ShaderTools.h"

std::string loadShaderCode(const char* filename)
{
    std::string shaderCode;
    std::ifstream file(filename, std::ios::in);

    if (!file.good())
    {
        std::cout << "Can't read file " << filename << std::endl;

        exit(1);
    }

    file.seekg(0, std::ios::end);
    shaderCode.resize((unsigned int)file.tellg());

    file.seekg(0, std::ios::beg);
    file.read(&shaderCode[0], shaderCode.size());
    file.close();

    return shaderCode;
}

GLuint compileShaderObject(const GLchar* shaderCode, GLenum shaderType)
{
    GLuint shaderObject = glCreateShader(shaderType);

    if(shaderObject == 0)
    {
        std::cout << "Error creating shader type: " << shaderType << std::endl;

        exit(1);
    }

    glShaderSource(shaderObject, 1, &shaderCode, NULL);
    glCompileShader(shaderObject);

    GLint success;
    GLchar infoLog[512];

    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shaderObject, 512, NULL, infoLog);

        std::cout << "Error compiling shader type: " << shaderType << std::endl;
        std::cout << infoLog << std::endl;

        exit(1);
    }

    return shaderObject;
}

GLuint ShaderProgram(const char *vsFilename, const char *fsFilename)
{
    GLuint shaderProgram = glCreateProgram();

    std::string shaderStr = loadShaderCode(vsFilename);
    GLuint vertexShader = compileShaderObject(shaderStr.c_str(), GL_VERTEX_SHADER);

    shaderStr = loadShaderCode(fsFilename);
    GLuint fragmentShader = compileShaderObject(shaderStr.c_str(), GL_FRAGMENT_SHADER);

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    GLchar infoLog[512];

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);

        std::cout << "Link Error: " << infoLog << std::endl;

        exit(1);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

