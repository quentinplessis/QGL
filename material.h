#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>

#include <vec3.h>
#include <stb_image.h>

#include "shader.h"


namespace qgl {

class Material {

  public:
    Material() {
      diffuseTextureData = NULL;
      specularTextureData = NULL;
      clear();
    }

    ~Material() {
      delete diffuseTextureData;
      delete specularTextureData;
    }

    inline void clear() {
      d = 1.f;
      ns = 0.f;
      ni = 0.f;
      km = 0.f;

      ambientColor.init(0.f, 0.f, 0.f);
      diffuseColor.init(0.f, 0.f, 0.f);
      specularColor.init(0.f, 0.f, 0.f);

      diffuseMap.clear();
      specularMap.clear();

      specularTexture = 0;
      diffuseTexture = 0;

      delete diffuseTextureData;
      delete specularTextureData;
      diffuseTextureData = NULL;
      specularTextureData = NULL;
    }

    void loadTextures();
    void setDiffuseTextureData(int width, int height, unsigned char* data, GLenum format);

    // Attributes
    float d, ns, ni, km;

    qm::Vec3f ambientColor;
    qm::Vec3f diffuseColor;
    qm::Vec3f specularColor;

    std::string diffuseMap;
    GLuint diffuseTexture;
    unsigned char* diffuseTextureData;

    std::string specularMap;
    GLuint specularTexture;
    unsigned char* specularTextureData;

};

}

#endif // MATERIAL_H
