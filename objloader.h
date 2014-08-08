#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iterator>
#include <vector>
#include <stdlib.h>

#include <vec3.h>
#include <vec2.h>
#include <mesh.h>

#include "object.h"
#include "material.h"


namespace qgl {

class OBJLoader {

  public:
    OBJLoader();

    bool loadGeometry(const std::string& filename, std::vector<qm::Vec3f>& vertices, std::vector<qm::Vec2f>& uvs, std::vector<qm::Vec3f>& normals);
    bool loadGeometry(const std::string& filename, q3ds::Mesh& mesh);

    bool loadObjects(const std::string& geometryFilename, std::vector<Object>& objects, const std::string& materialFilename = "");

};

}

#endif // OBJLOADER_H
