#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <map>
#include <string>

#include <vec3.h>
#include <mat3.h>
#include <mat4.h>

#include "shader.h"
#include "material.h"


namespace qgl {

class ShaderProgram {

  public:
    ShaderProgram();
    ShaderProgram(qtools::Logger* logger);

    void attachShader(const Shader& shader) const;
    bool loadShader(GLenum shaderType, const std::string& shaderFile);
    bool link() const;
    unsigned int getIndex() const { return index; }

    void setLogger(qtools::Logger* logger);
    bool isValid() const;
    void printAll() const;
    void printInfoLog() const;

    void useUniform(char* uniform);

    void use();

    void setUniform1i(char* uniform, int i);
    void setUniform1f(char* uniform, float f);
    void setUniformMat4f(char* uniform, qm::Mat4f& matrix);
    void setUniformMat3f(char* uniform, qm::Mat3f& matrix);
    void setUniformVec3f(char* uniform, qm::Vec3f& vec);
    void setUniformVec3f(char* uniform, float x, float y, float z);
    void setUniformTextureIndex(char* uniform, int index);

    void setUniformsFromMaterial(Material& material);

  private:
    unsigned int index;
    bool logInfo;
    qtools::Logger *logger;

    std::map<std::string, int> uniformLocations;
    std::map<std::string, int> textureIndexes;

};

}

#endif // SHADERPROGRAM_H
