#include "material.h"

using namespace qgl;
using namespace std;

void Material::loadTextures() {
  if (!diffuseMap.empty()) {
    if (diffuseTexture == 0)
      glGenTextures(1, &diffuseTexture);

    int width, height, n;
    int forceChannels = 4;
    delete diffuseTextureData;
    diffuseTextureData = stbi_load(diffuseMap.c_str(), &width, &height, &n, forceChannels);
    if (!diffuseTextureData)
      cerr << "Cannot load the texture " << diffuseMap << endl;
    else {
      glBindTexture(GL_TEXTURE_2D, diffuseTexture);
      glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA,
        width, height,
        0, GL_RGBA, GL_UNSIGNED_BYTE,
        diffuseTextureData
      );
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
  }

  if (!specularMap.empty()) {
    if (specularTexture == 0)
      glGenTextures(1, &specularTexture);

    int width, height, n;
    int forceChannels = 4;
    delete specularTextureData;
    specularTextureData = stbi_load(specularMap.c_str(), &width, &height, &n, forceChannels);
    if (!specularTextureData)
      cerr << "Cannot load the texture " << specularMap << endl;
    else {
      glBindTexture(GL_TEXTURE_2D, specularTexture);
      glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA,
        width, height,
        0, GL_RGBA, GL_UNSIGNED_BYTE,
        specularTextureData
      );
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
  }
}

void Material::setDiffuseTextureData(int width, int height, unsigned char* data, GLenum format) {
  if (diffuseTexture == 0)
    glGenTextures(1, &diffuseTexture);
  if (data != NULL) {
    glBindTexture(GL_TEXTURE_2D, diffuseTexture);
    glTexImage2D(
      GL_TEXTURE_2D, 0, GL_RGBA,
      width, height,
      0, format, GL_UNSIGNED_BYTE,
      data
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }
}
