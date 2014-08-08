#include "shader.h"

using namespace qgl;
using namespace std;
using namespace qtools;

Shader::Shader() {
  logInfo = false;
  logger = NULL;
}

Shader::Shader(GLenum shaderType) {
  index = glCreateShader(shaderType);
  logInfo = false;
  logger = NULL;
}

Shader::Shader(GLenum shaderType, qtools::Logger* logger) {
  index = glCreateShader(shaderType);
  setLogger(logger);
}

void Shader::setLogger(qtools::Logger* logger) {
  this->logger = logger;
  logInfo = true;
}

bool Shader::sourceFromFile(const string& filename) const {
  ifstream file;
  file.open(filename.c_str());

  if (!file) {
    if (logInfo)
      *logger << "ERROR: Shader file not found." << Logger::ERROR << Logger::FILE;
    else
      cerr << "ERROR: Shader file not found." << endl;
    return false;
  }

  stringstream stream;
  stream << file.rdbuf();
  file.close();

  string shaderString = stream.str();

  return setSource(shaderString.c_str());
}

bool Shader::setSource(const char* shader) const {
  glShaderSource(index, 1, &shader, NULL);
  glCompileShader(index);
  int params = -1;
  glGetShaderiv(index, GL_COMPILE_STATUS, &params);
  if (GL_TRUE != params) {
    if (logInfo)
      *logger << "ERROR: GL shader index " << index << " did not compile." << Logger::ERROR << Logger::FILE;
    else
      cerr << "ERROR: GL shader index " << index << " did not compile." << endl;
    printInfoLog();
    return false;
  }
  return true;
}

void Shader::printInfoLog() const {
  int maxLength = 2048;
  int actualLength = 0;
  char logMessage[2048];
  glGetShaderInfoLog(index, maxLength, &actualLength, logMessage);
  if (logInfo)
    *logger << logMessage << Logger::INFO << Logger::FILE;
  else
    cout << logMessage << endl;
}
