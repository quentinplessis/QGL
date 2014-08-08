#ifndef SHADER_H
#define SHADER_H

#ifndef GLEW_STATIC
  #define GLEW_STATIC
  #include <GL/glew.h>
#endif

#include <logger.h>


namespace qgl {

class Shader {

  public:
    Shader();
    Shader(GLenum shaderType);
    Shader(GLenum shaderType, qtools::Logger* log);

    bool sourceFromFile(const std::string& filename) const;
    bool setSource(const char* shader) const;
    unsigned int getIndex() const { return index; }

    void setLogger(qtools::Logger* logger);
    void printInfoLog() const;

  private:
    unsigned int index;
    bool logInfo;
    qtools::Logger *logger;

};

}

#endif // SHADER_H
