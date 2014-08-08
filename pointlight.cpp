#include "pointlight.h"

using namespace qgl;

PointLight::PointLight() {
  position = qm::Vec3f(0.0f, 0.0f, 0.0f);
  diffuse = specular = ambient = qm::Vec3f(0.0f, 0.0f, 0.0f);
}

PointLight::PointLight(const qm::Vec3f& position, const qm::Vec3f& diffuse) {
  this->position = position;
  this->diffuse = diffuse;
  specular = ambient = qm::Vec3f(0.0f, 0.0f, 0.0f);
}

PointLight::PointLight(const qm::Vec3f& position, const qm::Vec3f& diffuse, const qm::Vec3f& specular) {
  this->position = position;
  this->diffuse = diffuse;
  this->specular = specular;
  ambient = qm::Vec3f(0.0f, 0.0f, 0.0f);
}

PointLight::PointLight(const qm::Vec3f& position, const qm::Vec3f& diffuse, const qm::Vec3f& specular, const qm::Vec3f& ambient) {
  this->position = position;
  this->diffuse = diffuse;
  this->specular = specular;
  this->ambient = ambient;
}

void PointLight::setPosition(const qm::Vec3f& position) {
  this->position = position;
}

void PointLight::setColors(const qm::Vec3f& diffuse, const qm::Vec3f& specular, const qm::Vec3f& ambient) {
  this->diffuse = diffuse;
  this->specular = specular;
  this->ambient = ambient;
}

void PointLight::setDiffuseColor(const qm::Vec3f& diffuse) {
  this->diffuse = diffuse;
}

void PointLight::setSpecularColor(const qm::Vec3f& specular) {
  this->specular = specular;
}

void PointLight::setAmbientColor(const qm::Vec3f& ambient) {
  this->ambient = ambient;
}

