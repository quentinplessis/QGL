#include "objloader.h"
#include <map>

using namespace qgl;
using namespace std;

OBJLoader::OBJLoader() {}

bool OBJLoader::loadGeometry(const string& filename, vector<qm::Vec3f>& vertices, vector<qm::Vec2f>& uvs, vector<qm::Vec3f>& normals) {
  ifstream file(filename.c_str());
  if (!file) {
    cerr << "Could not load the file. " << endl;
    return false;
  }
  cout << "Load model...";

  vector<qm::Vec3f> tempVertices;
  vector<qm::Vec2f> tempUVs;
  vector<qm::Vec3f> tempNormals;
  vector<int> vertexIndices, uvIndices, normalIndices;
  string line, head;
  string v("v"), vt("vt"), vn("vn"), f("f");
  qm::Vec3f vertex, normal;
  qm::Vec2f uv;
  while (getline(file, line, '\n')) {
    stringstream lineStream(line);
    lineStream >> head;
    if (head.compare(v) == 0) {
      lineStream >> vertex[0] >> vertex[1] >> vertex[2];
      tempVertices.push_back(vertex);
    }
    else if (head.compare(vt) == 0) {
      lineStream >> uv[0] >> uv[1];
      tempUVs.push_back(uv);
    }
    else if (head.compare(vn) == 0) {
      lineStream >> normal[0] >> normal[1] >> normal[2];
      tempNormals.push_back(normal);
    }
    else if (head.compare(f) == 0) {
      int index;
      string currentVertex, indexString;
      for (int i = 0 ; i < 3 ; i++) {
        lineStream >> currentVertex;
        stringstream stream(currentVertex);
        for (int j = 0 ; j < 3 ; j++) {
          getline(stream, indexString, '/');
          stringstream stream2(indexString);
          index = 0;
          stream2 >> index;
          if (index > 0) {
            if (j == 0)
              vertexIndices.push_back(index);
            else if (j == 1)
              uvIndices.push_back(index);
            else
              normalIndices.push_back(index);
          }
        }
      }
    }
  }
  file.close();
  int vertexIndex, uvIndex, normalIndex;
  for (unsigned int i = 0 ; i < vertexIndices.size() ; i++) {
    vertexIndex = vertexIndices[i];
    vertex = tempVertices[vertexIndex-1];
    vertices.push_back(vertex);
  }
  for (unsigned int i = 0 ; i < uvIndices.size() ; i++) {
    uvIndex = uvIndices[i];
    uv = tempUVs[uvIndex-1];
    uvs.push_back(uv);
  }
  for (unsigned int i = 0 ; i < normalIndices.size() ; i++) {
    normalIndex = normalIndices[i];
    normal = tempNormals[normalIndex-1];
    normals.push_back(normal);
  }
  cout << "loaded!" << endl;
  cout << "Unique Vertices: " << tempVertices.size() << endl;
  cout << "Normals: " << tempNormals.size() << endl;
  cout << "UVs: " << tempUVs.size() << endl;
  cout << "Vertices: " << vertices.size() << endl;

  return true;
}

// Load the model in one mesh
bool OBJLoader::loadGeometry(const string& filename, q3ds::Mesh& mesh) {
  ifstream file(filename.c_str());
  if (!file) {
    cerr << "Could not load the file. " << endl;
    return false;
  }
  cout << "Load model...";

  Triangle triangle;
  qm::Vec3f position, normal;
  qm::Vec2f uv;

  string v("v"), vt("vt"), vn("vn"), f("f");
  string line, head;

  while (getline(file, line, '\n')) {
    stringstream lineStream(line);
    lineStream >> head;
    if (head.compare(v) == 0) {
      lineStream >> position[0] >> position[1] >> position[2];
      mesh.addPosition(position);
    }
    else if (head.compare(vt) == 0) {
      lineStream >> uv[0] >> uv[1];
      mesh.addUV(uv);
    }
    else if (head.compare(vn) == 0) {
      lineStream >> normal[0] >> normal[1] >> normal[2];
      mesh.addNormal(normal);
    }
    else if (head.compare(f) == 0) {
      int index;
      string currentVertex, indexString;
      for (int i = 0 ; i < 3 ; i++) {
        lineStream >> currentVertex;
        stringstream stream(currentVertex);
        int positionIndex = 0, normalIndex = 0, uvIndex = 0;
        for (int j = 0 ; j < 3 ; j++) {
          getline(stream, indexString, '/');
          stringstream stream2(indexString);
          index = 0;
          stream2 >> index;
          if (index > 0) {
            if (j == 0)
              positionIndex = index;
            else if (j == 1)
              uvIndex = index;
            else
              normalIndex = index;
          }
          else
            cout << "0 index" << endl;
        }
        triangle.setVertex(i, positionIndex-1, normalIndex-1, uvIndex-1);
      }
      mesh.addTriangle(triangle);
    }
  }
  file.close();

  cout << "loaded!" << endl;
  cout << "Unique vertices: " << mesh.positionsNumber() << endl;
  cout << "Unique normals: " << mesh.normalsNumber() << endl;
  cout << "Unique uvs: " << mesh.uvsNumber() << endl;
  cout << "Triangles: " << mesh.trianglesNumber() << endl;


  return true;
}

// Load the model in objects
bool OBJLoader::loadObjects(const string& geometryFilename, vector<Object>& objects, const string& materialFilename) {
  bool loadMaterial = !materialFilename.empty();
  map<string, Material> materials;
  if (loadMaterial) {
    ifstream materialFile(materialFilename.c_str());
    if (!materialFile) {
      cerr << "Could not load the material file. " << endl;
      loadMaterial = false;
    }
    else {
      cout << "Load model material ...";
      string line, head, materialName = "", mapFile;
      Material material;
      size_t pos = materialFilename.rfind("/");
      if (pos == string::npos)
        pos = materialFilename.rfind("\\");
      string fileFolder = materialFilename.substr(0, pos);
      int materialsNumber = 0;
      while (getline(materialFile, line, '\n')) {
        stringstream lineStream(line);
        lineStream >> head;
        if (head.compare("newmtl") == 0) {
          if (materialsNumber != 0) {
            materials.insert(pair<string, Material>(materialName, material));
            material.clear();
          }
          lineStream >> materialName;
          materialsNumber++;
        }
        else if (head.compare("d") == 0) {
          lineStream >> material.d;
        }
        else if (head.compare("Ns") == 0) {
          lineStream >> material.ns;
        }
        else if (head.compare("Ni") == 0) {
          lineStream >> material.ni;
        }
        else if (head.compare("Ka") == 0) {
          lineStream >> material.ambientColor[0] >> material.ambientColor[1] >> material.ambientColor[2];
        }
        else if (head.compare("Kd") == 0) {
          lineStream >> material.diffuseColor[0] >> material.diffuseColor[1] >> material.diffuseColor[2];
        }
        else if (head.compare("Ks") == 0) {
          lineStream >> material.specularColor[0] >> material.specularColor[1] >> material.specularColor[2];
        }
        else if (head.compare("Km") == 0) {
          lineStream >> material.km;
        }
        else if (head.compare("map_Kd") == 0) {
          lineStream >> mapFile;
          material.diffuseMap = fileFolder + mapFile;
        }
        else if (head.compare("map_Ks") == 0) {
          material.specularMap = fileFolder + mapFile;
        }
      }
      if (materialsNumber != 0) {
        materials.insert(pair<string, Material>(materialName, material));
      }

      for (map<string, Material>::iterator it = materials.begin() ; it != materials.end() ; it++) {
        (it->second).loadTextures();
      }
      cout << "Loaded materials: " << materialsNumber << endl;
    }
  }

  ifstream file(geometryFilename.c_str());
  if (!file) {
    cerr << "Could not load the file. " << endl;
    return false;
  }
  cout << "Load model geometry ...";

  Object object;
  q3ds::Mesh mesh;
  q3ds::Triangle triangle;
  string materialName = "";
  qm::Vec3f position, normal;
  qm::Vec2f uv;
  int currentObject = 0;
  int lastPIndex = 0, lastObjectLastPIndex = 0;
  int lastNIndex = 0, lastObjectLastNIndex = 0;
  int lastUVIndex = 0, lastObjectLastUVIndex = 0;

  string o("o"), v("v"), vt("vt"), vn("vn"), f("f"), usemtl("usemtl");
  string line, head;

  while (getline(file, line, '\n')) {
    stringstream lineStream(line);
    lineStream >> head;
    if (head.compare(o) == 0) { // new object
      if (currentObject != 0) {
        lastObjectLastPIndex = lastPIndex;
        lastObjectLastNIndex = lastNIndex;
        lastObjectLastUVIndex = lastUVIndex;
        objects.push_back(object);
        objects.back().setMesh(mesh);
        if (loadMaterial && !materialName.empty())
          objects.back().setMaterial(materials.find(materialName)->second);
        mesh.clear();
        materialName.clear();
      }
      currentObject++;
    }
    else if (head.compare(v) == 0) {
      lineStream >> position[0] >> position[1] >> position[2];
      mesh.addPosition(position);
    }
    else if (head.compare(vt) == 0) {
      lineStream >> uv[0] >> uv[1];
      mesh.addUV(uv);
    }
    else if (head.compare(vn) == 0) {
      lineStream >> normal[0] >> normal[1] >> normal[2];
      mesh.addNormal(normal);
    }
    else if (head.compare(f) == 0) {
      int index;
      string currentVertex, indexString;
      for (int i = 0 ; i < 3 ; i++) {
        lineStream >> currentVertex;
        stringstream stream(currentVertex);
        int positionIndex = 0, normalIndex = 0, uvIndex = 0;
        for (int j = 0 ; j < 3 ; j++) {
          getline(stream, indexString, '/');
          stringstream stream2(indexString);
          index = 0;
          stream2 >> index;
          if (index > 0) {
            if (j == 0)
              positionIndex = index;
            else if (j == 1)
              uvIndex = index;
            else
              normalIndex = index;
          }
        }
        if (positionIndex > lastPIndex)
          lastPIndex = positionIndex;
        if (uvIndex > lastUVIndex)
          lastUVIndex = uvIndex;
        if (normalIndex > lastNIndex)
          lastNIndex = normalIndex;
        triangle.setVertex(i, positionIndex-lastObjectLastPIndex-1, normalIndex-lastObjectLastNIndex-1, uvIndex-lastObjectLastUVIndex-1);
      }
      mesh.addTriangle(triangle);
    }
    else if (loadMaterial && head.compare(usemtl) == 0)
      lineStream >> materialName;
  }
  if (currentObject != 0) {
    objects.push_back(object);
    objects.back().setMesh(mesh);
    if (loadMaterial && !materialName.empty())
      objects.back().setMaterial(materials.find(materialName)->second);
  }
  file.close();

  cout << "loaded!" << endl;
  cout << "Objects loaded: " << currentObject << endl;

  return true;
}


