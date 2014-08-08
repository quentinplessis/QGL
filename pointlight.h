#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "light.h"

#include <vec3.h>

namespace qgl {

class PointLight : public Light {

  public:
    PointLight();
    PointLight(const qm::Vec3f& position);
    PointLight(const qm::Vec3f& position, const qm::Vec3f& diffuse);
    PointLight(const qm::Vec3f& position, const qm::Vec3f& diffuse, const qm::Vec3f& specular);
    PointLight(const qm::Vec3f& position, const qm::Vec3f& diffuse, const qm::Vec3f& specular, const qm::Vec3f& ambient);

    void setPosition(const qm::Vec3f& position);
    void setColors(const qm::Vec3f& diffuse, const qm::Vec3f& specular, const qm::Vec3f& ambient);
    void setDiffuseColor(const qm::Vec3f& diffuse);
    void setSpecularColor(const qm::Vec3f& specular);
    void setAmbientColor(const qm::Vec3f& ambient);

    qm::Vec3f& getPosition() { return position; }
    qm::Vec3f& getDiffuseColor() { return diffuse; }
    qm::Vec3f& getSpecularColor() { return specular; }
    qm::Vec3f& getAmbientColor() { return ambient; }

  protected:
    qm::Vec3f position;
    qm::Vec3f diffuse;
    qm::Vec3f specular;
    qm::Vec3f ambient;

};

}

#endif // POINTLIGHT_H
