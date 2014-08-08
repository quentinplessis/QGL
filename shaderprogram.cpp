#include "shaderprogram.h"

using namespace qgl;
using namespace std;
using namespace qtools;

const char* glTypeToString(GLenum type) {
  switch (type) {
    case GL_FLOAT:
      return "GL_FLOAT";
    case GL_FLOAT_VEC2:
      return "GL_FLOAT_VEC2";
    case GL_FLOAT_VEC3:
      return "GL_FLOAT_VEC3";
    case GL_FLOAT_VEC4:
      return "GL_FLOAT_VEC4";
    case GL_FLOAT_MAT2:
      return "GL_FLOAT_MAT2";
    case GL_FLOAT_MAT3:
      return "GL_FLOAT_MAT3";
    case GL_FLOAT_MAT4:
      return "GL_FLOAT_MAT4";
    case GL_INT:
      return "GL_INT";
    case GL_BOOL:
      return "GL_BOOL";
    case GL_SAMPLER_2D:
      return "GL_SAMPLER_2D";
    case GL_SAMPLER_3D:
      return "GL_SAMPLER_3D";
    case GL_SAMPLER_CUBE:
      return "GL_SAMPLER_CUBE";
    case GL_SAMPLER_2D_SHADOW:
      return "GL_SAMPLER_2D_SHADOW";
  }
  return "OTHER";
}

ShaderProgram::ShaderProgram() {
  index = glCreateProgram();
  logInfo = false;
  logger = NULL;
}

ShaderProgram::ShaderProgram(qtools::Logger* logger) {
  index = glCreateProgram();
  setLogger(logger);
}

void ShaderProgram::setLogger(qtools::Logger* logger) {
  this->logger = logger;
  logInfo = true;
}

void ShaderProgram::attachShader(const Shader& shader) const {
  glAttachShader(index, shader.getIndex());
}

bool ShaderProgram::loadShader(GLenum shaderType, const string& shaderFile) {
  Shader shader(shaderType, logger);
  if (!shader.sourceFromFile(shaderFile)) {
    *logger << "Error when loading the shader." << Logger::ERROR << Logger::FILE;
    return false;
  }
  attachShader(shader);
  return true;
}

bool ShaderProgram::link() const {
  glLinkProgram(index);
  int params = -1;
  glGetProgramiv(index, GL_LINK_STATUS, &params);
  if (GL_TRUE != params) {
     if (logInfo)
      *logger << "ERROR: could not link shader program index " << index << "." << Logger::ERROR << Logger::FILE;
    else
      cerr << "ERROR: could not link shader program index " << index << "." << endl;
    printInfoLog();
    return false;
  }
  return false;
}

bool ShaderProgram::isValid() const {
  glValidateProgram(index);
  int params = -1;
  glGetProgramiv(index, GL_VALIDATE_STATUS, &params);
  if (GL_TRUE != params) {
    if (logInfo)
      *logger << "Shader program " << index << " GL_VALIDATE_STATUS = GL_FALSE" << Logger::ERROR << Logger::FILE;
    else
      cerr << "Shader program " << index << " GL_VALIDATE_STATUS = GL_FALSE" << endl;
    printInfoLog();
    return false;
  }
  if (logInfo)
      *logger << "Shader program " << index << " GL_VALIDATE_STATUS = GL_TRUE" << Logger::INFO << Logger::FILE;
    else
      cout << "Shader program " << index << " GL_VALIDATE_STATUS = GL_TRUE" << endl;
  return true;
}

void ShaderProgram::printInfoLog() const {
  int maxLength = 2048;
  int actualLength = 0;
  char logMessage[2048];
  glGetProgramInfoLog(index, maxLength, &actualLength, logMessage);
  if (logInfo)
    *logger << logMessage << Logger::INFO << Logger::FILE;
  else
    cout << logMessage << endl;
}

void ShaderProgram::printAll() const {
  if (!logInfo)
    return;

  *logger << "---------------------\nShader program " << index << " info:" << Logger::INFO;

  int params = -1;
  glGetProgramiv(index, GL_LINK_STATUS, &params);
  *logger << "GL_LINK_STATUS = " << (GL_TRUE == params ? "GL_TRUE" : "GL_FALSE") << Logger::INFO;

  glGetProgramiv(index, GL_ATTACHED_SHADERS, &params);
  *logger << "GL_ATTACHED_SHADERS = " << params << Logger::INFO;

  glGetProgramiv(index, GL_ACTIVE_ATTRIBUTES, &params);
  *logger << "GL_ACTIVE_ATTRIBUTES = " << params << Logger::INFO;

  for (int i = 0 ; i < params ; i++) {
    char name[64];
    int maxLength = 64;
    int actualLength = 0;
    int size = 0;
    GLenum type;
    glGetActiveAttrib(index, i, maxLength, &actualLength, &size, &type, name);
    if (size > 1) {
      for (int j = 0 ; j < size ; j++) {
        char longName[64];
        sprintf(longName, "%s[%i]", name, j);
        int location = glGetAttribLocation(index, longName);
        *logger << "  " << i << ")";
        *logger << " type: " << glTypeToString(type);
        *logger << ", name: " << longName;
        *logger << ", location: " << location << Logger::INFO;
      }
    }
    else {
      int location = glGetAttribLocation(index, name);
      *logger << "  " << i << ")";
      *logger << " type: " << glTypeToString(type);
      *logger << ", name: " << name;
      *logger << ", location: " << location << Logger::INFO;
    }
  }

  glGetProgramiv(index, GL_ACTIVE_UNIFORMS, &params);
  *logger << "GL_ACTIVE_UNIFORMS = " << params << Logger::INFO;
  for (int i = 0 ; i < params; i++) {
    char name[64];
    int maxLength = 64;
    int actualLength = 0;
    int size = 0;
    GLenum type;
    glGetActiveUniform(index, i, maxLength, &actualLength, &size, &type, name);
    if (size > 1) {
      for (int j = 0 ; j < size ; j++) {
        char longName[64];
        sprintf(longName, "%s[%i]", name, j);
        int location = glGetUniformLocation(index, longName);
        *logger << "  " << i << ")";
        *logger << " type: " << glTypeToString(type);
        *logger << ", name: " << longName;
        *logger << ", location: " << location << Logger::INFO;
      }
    }
    else {
      int location = glGetUniformLocation(index, name);
      *logger << "  " << i << ")";
      *logger << " type: " << glTypeToString(type);
      *logger << ", name: " << name;
      *logger << ", location: " << location << Logger::INFO;
    }
  }
  *logger << "---------------------" << Logger::INFO;
  *logger << Logger::FILE;
  printInfoLog();
}

void ShaderProgram::useUniform(char* uniform) {
  uniformLocations.insert(pair<string, int>(string(uniform), glGetUniformLocation(index, uniform)));
}

void ShaderProgram::use() {
  glUseProgram(index);
}

void ShaderProgram::setUniform1i(char* uniform, int i) {
  int location = uniformLocations.find(string(uniform))->second;
  glUniform1i(location, i);
}

void ShaderProgram::setUniform1f(char* uniform, float f) {
  int location = uniformLocations.find(string(uniform))->second;
  glUniform1i(location, f);
}

void ShaderProgram::setUniformMat4f(char* uniform, qm::Mat4f& matrix) {
  int location = uniformLocations.find(string(uniform))->second;
  glUniformMatrix4fv(location, 1, GL_FALSE, matrix.getArray());
}

void ShaderProgram::setUniformMat3f(char* uniform, qm::Mat3f& matrix) {
  int location = uniformLocations.find(string(uniform))->second;
  glUniformMatrix3fv(location, 1, GL_FALSE, matrix.getArray());
}

void ShaderProgram::setUniformVec3f(char* uniform, qm::Vec3f& vec) {
  int location = uniformLocations.find(string(uniform))->second;
  glUniform3f(location, vec[0], vec[1], vec[2]);
}

void ShaderProgram::setUniformVec3f(char* uniform, float x, float y, float z) {
  int location = uniformLocations.find(string(uniform))->second;
  glUniform3f(location, x, y, z);
}
void ShaderProgram::setUniformTextureIndex(char* uniform, int index) {
  int location = uniformLocations.find(string(uniform))->second;
  textureIndexes.insert(pair<string, int>(string(uniform), index));
  glUniform1i(location, index);
}

void ShaderProgram::setUniformsFromMaterial(Material& material) {
  map<string, int>::iterator it;
  map<string, int>::iterator it2;
  // Color
  if ((it = uniformLocations.find(string("diffuseColor"))) != uniformLocations.end())
    glUniform3f(it->second, material.diffuseColor[0], material.diffuseColor[1], material.diffuseColor[2]);
  if ((it = uniformLocations.find(string("ambientColor"))) != uniformLocations.end())
    glUniform3f(it->second, material.ambientColor[0], material.ambientColor[1], material.ambientColor[2]);
  if ((it = uniformLocations.find(string("specularColor"))) != uniformLocations.end())
    glUniform3f(it->second, material.specularColor[0], material.specularColor[1], material.specularColor[2]);
  if ((it = uniformLocations.find(string("transparency"))) != uniformLocations.end())
    glUniform1f(it->second, material.d);

  // Textures
  setUniform1i("useDiffuseMap", 0);
  setUniform1i("useSpecularMap", 0);
  if ((it = uniformLocations.find(string("diffuseMap"))) != uniformLocations.end()) {
    if (material.diffuseTexture != 0) {
      int textureIndex = -1;
      if ((it2 = textureIndexes.find(string("diffuseMap"))) != textureIndexes.end()) {
        textureIndex = it2->second;
        if (textureIndex == 0)
          glActiveTexture(GL_TEXTURE0);
        else if (textureIndex == 1)
          glActiveTexture(GL_TEXTURE1);
        else if (textureIndex == 2)
          glActiveTexture(GL_TEXTURE2);
        else if (textureIndex == 3)
          glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, material.diffuseTexture);
        setUniform1i("useDiffuseMap", 1);
      }
    }
  }
  if ((it = uniformLocations.find(string("specularMap"))) != uniformLocations.end()) {
    if (material.specularTexture != 0) {
      int textureIndex = -1;
      if ((it2 = textureIndexes.find(string("specularMap"))) != textureIndexes.end()) {
        textureIndex = it2->second;
        if (textureIndex == 0)
          glActiveTexture(GL_TEXTURE0);
        else if (textureIndex == 1)
          glActiveTexture(GL_TEXTURE1);
        else if (textureIndex == 2)
          glActiveTexture(GL_TEXTURE2);
        else if (textureIndex == 3)
          glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, material.specularTexture);
        setUniform1i("useSpecularMap", 1);
      }
    }
  }
}
