#include "object.h"
#include "objloader.h"

using namespace qgl;
using namespace std;

Object::Object() {
  positions = NULL;
  normals = NULL;
  uvs = NULL;
  withNormals = false;
  withUVs = false;
  positionsVBO = 0;
  normalsVBO = 0;
  uvsVBO = 0;
  VAO = 0;
  modelMatrixChanged = true;
  rotation.init(0.f, 0.f, 1.0f, 0.f);
  scale = qm::Vec3f(1.f, 1.f, 1.f);
}

Object::~Object() {
  delete[] positions;
  delete[] normals;
  delete[] uvs;
}

bool Object::loadOBJ(const string& geometryFile, const string& materialFile) {
  OBJLoader objLoader;
  bool loadMaterial = materialFile.compare("") != 0;

  cout << "Load object geometry." << endl;
  if (objLoader.loadGeometry(geometryFile, mesh)) {
    cout << "Object mesh successfully loaded." << endl;
    withNormals = mesh.normalsNumber() > 0;
    withUVs = mesh.uvsNumber() > 0;
    if (!withNormals)
      cout << "Loaded object does not have any normals.";
    if (!withUVs)
      cout << "Loaded object does not have any texture coordinates.";

    if (loadMaterial) {
      cout << "Load object material." << endl;
      objLoader.loadMaterial(materialFile);
    }
  }
}

void Object::setMesh(q3ds::Mesh& newMesh) {
  mesh = newMesh;
  withNormals = mesh.normalsNumber() > 0;
  withUVs = mesh.uvsNumber() > 0;
}

void Object::setMaterial(Material& newMaterial) {
  material = newMaterial;
}

void Object::computeVertices() {
  cout << "Compute object vertices" << endl;

  delete[] positions;
  delete[] normals;
  delete[] uvs;

  positions = new float[mesh.trianglesNumber() * 3 * 3];
  if (withNormals)
    normals = new float[mesh.trianglesNumber() * 3 * 3];
  if (withUVs)
    uvs = new float[mesh.trianglesNumber() * 3 * 2];

  mesh.computeVertices(positions, normals, uvs);
}

void Object::updateVAO() {
  updatePositionsVBO();
  updateNormalsVBO();
  updateUVsVBO();
}

void Object::updatePositionsVBO() {
  glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
  glBufferData(GL_ARRAY_BUFFER, mesh.trianglesNumber() * 3 * 3 * sizeof (float), positions, GL_STATIC_DRAW);
}

void Object::updateNormalsVBO() {
  if (withNormals) {
    glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.trianglesNumber() * 3 * 3 * sizeof (float), normals, GL_STATIC_DRAW);
  }
}

void Object::updateUVsVBO() {
  if (withUVs) {
    glBindBuffer(GL_ARRAY_BUFFER, uvsVBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.trianglesNumber() * 3 * 2 * sizeof (float), uvs, GL_STATIC_DRAW);
  }
}

void Object::createVAO() {
  glGenBuffers(1, &positionsVBO);
  updatePositionsVBO();

  if (withNormals) {
    glGenBuffers(1, &normalsVBO);
    updateNormalsVBO();
  }

  if (withUVs) {
    glGenBuffers(1, &uvsVBO);
    updateUVsVBO();
  }

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  if (withNormals) {
    glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  }
  if (withUVs) {
    glBindBuffer(GL_ARRAY_BUFFER, uvsVBO);
    glVertexAttribPointer(withNormals ? 2 : 1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  }
  glEnableVertexAttribArray(0);
  if (withNormals || withUVs)
    glEnableVertexAttribArray(1);
  if (withNormals && withUVs)
    glEnableVertexAttribArray(2);
}

void Object::setPosition(qm::Vec3f& position) {
  this->position = position;
  modelMatrixChanged = true;
}

void Object::translate(qm::Vec3f translation) {
  position += translation;
  modelMatrixChanged = true;
}

void Object::translate(qm::Vec3f& translation) {
  position += translation;
  modelMatrixChanged = true;
}

void Object::setRotation(qm::Quat& rotation) {
  this->rotation = rotation;
  modelMatrixChanged = true;
}

void Object::setRotation(float a, float x, float y, float z) {
  this->rotation = qm::Quat(a, x, y, z);
  modelMatrixChanged = true;
}

void Object::rotate(qm::Quat& rotation) {
  this->rotation = rotation * this->rotation;
  modelMatrixChanged = true;
}

void Object::rotate(float a, float x, float y, float z) {
  this->rotation = qm::Quat(a, x, y, z) * this->rotation;
  modelMatrixChanged = true;
}

void Object::setScale(qm::Vec3f& scale) {
  this->scale = scale;
  modelMatrixChanged = true;
}

void Object::setScale(float x, float y, float z) {
  this->scale = qm::Vec3f(x, y, z);
  modelMatrixChanged = true;
}

void Object::reScale(qm::Vec3f scale) {
  this->scale *= scale;
  modelMatrixChanged = true;
}

void Object::reScale(float x, float y, float z) {
  reScale(qm::Vec3f(x, y, z));
}

void Object::computeModelMatrix() {
  modelMatrix = qm::Mat4f::translationMatrix(position) * rotation.toMatrix() * qm::Mat4f::scaleMatrix(scale);
  modelMatrixChanged = false;
}

qm::Mat4f& Object::retrieveModelMatrix() {
  if (modelMatrixChanged)
    computeModelMatrix();
  return modelMatrix;
}

