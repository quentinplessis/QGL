#ifndef OBJECT_H
#define OBJECT_H

#include "shaderprogram.h"
#include <GLFW/glfw3.h>
#include <iostream>

#include <mesh.h>
#include <vec4.h>
#include <quat.h>

#include "material.h"


namespace qgl {

class Object {

  public:
    Object();
    ~Object();

    bool loadOBJ(const std::string& geometryFile, const std::string& materialFile = "");
    void setMesh(Mesh& newMesh);
    void setMaterial(Material& newMaterial);

    Material& getMaterial() { return material; }

    void computeVertices();

    unsigned int verticesNumber() const { return mesh.trianglesNumber() * 3; }
    unsigned int trianglesNumber() const { return mesh.trianglesNumber(); }

    float* getPositions() const { return positions; }
    float* getNormals() const { return normals; }
    float* getUVs() const { return uvs; }

    void createVAO();
    unsigned int getVAO() { return VAO; }
    void updateVAO();
    void updatePositionsVBO();
    void updateNormalsVBO();
    void updateUVsVBO();

    void setPosition(qm::Vec3f& position);
    qm::Vec3f& getPosition() { return position; }
    void translate(qm::Vec3f& translation);
    void translate(qm::Vec3f translation);

    void setRotation(qm::Quat& rotation);
    void setRotation(float a, float x, float y, float z);
    void rotate(qm::Quat& rotation);
    void rotate(float a, float x, float y, float z);

    void setScale(qm::Vec3f& scale);
    void setScale(float x, float y, float z);
    void reScale(qm::Vec3f scale);
    void reScale(float x, float y, float z);

    void computeModelMatrix();
    qm::Mat4f& retrieveModelMatrix();


  private:
    q3ds::Mesh mesh;

    bool withNormals;
    bool withUVs;

    float* positions;
    float* normals;
    float* uvs;

    unsigned int positionsVBO;
    unsigned int normalsVBO;
    unsigned int uvsVBO;
    unsigned int VAO;

    qm::Vec3f position;
    qm::Quat rotation;
    qm::Vec3f scale;
    bool modelMatrixChanged;
    qm::Mat4f modelMatrix;

    Material material;

};

}

#endif // OBJECT_H
