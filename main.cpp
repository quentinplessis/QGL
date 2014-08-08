
#include "shaderprogram.h"
#include <GLFW/glfw3.h>
#include <logger.h>

#include <cmath>
#include <iomanip>
#include <stdlib.h>

#include <vec3.h>
#include <vec4.h>
#include <mat3.h>
#include <mat4.h>
#include <quat.h>
#include <stbi_image_write.h>

#include "pointlight.h"
#include "object.h"
#include "objloader.h"


#define ONE_DEG_IN_RAD (2.0 * M_PI) / 360.0 // 0.017444444

using namespace std;


string IMAGES_FOLDER, VIDEOS_FOLDER, DATA_FOLDER, PROJECT_FOLDER, OUTPUT_FOLDER;
string SHADERS_FOLDER, MODELS_FODLER, LOG_FILE;

bool initFromConfigFile(const std::string& filename) {
  ifstream file(filename.c_str());
  if (!file) {
    cerr << "Could not load the config file. " << endl;
    return false;
  }
  string line, head;
  while (getline(file, line, '\n')) {
    stringstream lineStream(line);
    lineStream >> head;
    if (head.compare("IMAGES") == 0)
      lineStream >> IMAGES_FOLDER;
    else if (head.compare("VIDEOS") == 0)
      lineStream >> VIDEOS_FOLDER;
    else if (head.compare("DATA") == 0)
      lineStream >> DATA_FOLDER;
    else if (head.compare("PROJECT") == 0)
      lineStream >> PROJECT_FOLDER;
    else if (head.compare("OUTPUT") == 0)
      lineStream >> OUTPUT_FOLDER;
    else if (head.compare("SHADERS") == 0)
      lineStream >> SHADERS_FOLDER;
    else if (head.compare("LOG") == 0)
      lineStream >> LOG_FILE;
    else if (head.compare("MODELS") == 0)
      lineStream >> MODELS_FOLDER;
  }
  file.close();
  return true;
}


Logger logger;

int windowWidth = 640;
int windowHeight = 480;


void glfwErrorCallback(int error, const char* description) {
  logger << "GLFW ERROR: code " << error << ", message: " << description;
  logger.flushError();
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
}


void glfwWindowSizeCallback(GLFWwindow* window, int width, int height) {
  windowWidth = width;
  windowHeight = height;
  /* update any perspective matrices used here */
}

void logGLParams() {
  GLenum params[] = {
    GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
    GL_MAX_CUBE_MAP_TEXTURE_SIZE,
    GL_MAX_DRAW_BUFFERS,
    GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
    GL_MAX_TEXTURE_IMAGE_UNITS,
    GL_MAX_TEXTURE_SIZE,
    GL_MAX_VARYING_FLOATS,
    GL_MAX_VERTEX_ATTRIBS,
    GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
    GL_MAX_VERTEX_UNIFORM_COMPONENTS,
    GL_MAX_VIEWPORT_DIMS,
    GL_STEREO,
  };
  const char* names[] = {
    "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
    "GL_MAX_CUBE_MAP_TEXTURE_SIZE",
    "GL_MAX_DRAW_BUFFERS",
    "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
    "GL_MAX_TEXTURE_IMAGE_UNITS",
    "GL_MAX_TEXTURE_SIZE",
    "GL_MAX_VARYING_FLOATS",
    "GL_MAX_VERTEX_ATTRIBS",
    "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
    "GL_MAX_VERTEX_UNIFORM_COMPONENTS",
    "GL_MAX_VIEWPORT_DIMS",
    "GL_STEREO",
  };

  logger << "GL context params:\n";

  char msg[256];
  // integers - only works if the order is 0-10 integer return types
  for (int i = 0; i < 10; i++) {
    int v = 0;
    glGetIntegerv(params[i], &v);
    logger << names[i] << " " << v << "\n";
  }
  // others
  int v[2];
  v[0] = v[1] = 0;
  glGetIntegerv(params[10], v);
  logger << names[10] << " " << v[0] << " " << v[1] << "\n";
  sprintf (msg, "%s %i %i\n", names[10], v[0], v[1]);
  unsigned char s = 0;
  glGetBooleanv(params[11], &s);
  logger << names[11] << " " << ((unsigned int) s) << "\n";
  sprintf (msg, "%s %i\n", names[11], (unsigned int)s);
  logger << "----------------------------";
  logger.flush();
}

void updateFPSCounter(GLFWwindow* window) {
  static double previousSeconds = glfwGetTime();
  static int frameCount;
  double currentSeconds = glfwGetTime();
  double elapsedSeconds = currentSeconds - previousSeconds;
  if (elapsedSeconds > 0.25) {
    previousSeconds = currentSeconds;
    double fps = (double) frameCount / elapsedSeconds;
    char tmp[128];
    sprintf(tmp, "OpenGL - FPS: %.2f", fps);
    glfwSetWindowTitle(window, tmp);
    frameCount = 0;
  }
  frameCount++;
}

void initGLWF() {
  logger.start(GL_LOG_FILE);

  // Start GLFW
  (logger << "GLFW version: " << glfwGetVersionString()).flush();
  glfwSetErrorCallback(glfwErrorCallback);
  if (!glfwInit()) {
    (logger << "ERROR: Could not load GLFW.").flushError();
    exit(0);
  }

  // GLFW hints: compatibility...
  /*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/
    // Multi-sample multi-aliasing
  //glfwWindowHint(GLFW_SAMPLES, 4);
}

GLFWwindow* createWindow() {
  // Create a window
  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
  GLFWwindow* window = glfwCreateWindow(
    //videoMode->width, videoMode->height,
    640, 480,
    "OpenGL", NULL, NULL
  );
  if (!window) {
    (logger << "Could not create the window.").flushError();
    glfwTerminate();
    exit(0);
  }
  (logger << "Window created.").flush();

  return window;
}

void initGLContext(GLFWwindow* window) {
  // OpenGL context
  glfwMakeContextCurrent(window);
  const GLubyte* renderer = glGetString(GL_RENDERER);
  const GLubyte* version = glGetString(GL_VERSION);
  logger << "Renderer: " << renderer << "\n";
  logger << "OpenGL supported version: " << version;
  logger.flush();
  logGLParams();

  // start GLEW extension handler
  glewExperimental = GL_TRUE;
  glewInit();
}

qm::Mat4f lookAt(const qm::Vec3f& cameraPos, const qm::Vec3f& targetPos, qm::Vec3f& up, qm::Vec3f& forward, qm::Vec3f& right) {
  qm::Mat4f translationMatrix = qm::Mat4f::translationMatrix(-cameraPos);

  forward = targetPos - cameraPos;
  forward.normalize();
  right = qm::Vec3f::crossProduct(forward, up);
  right.normalize();
  up = qm::Vec3f::crossProduct(right, forward);
  up.normalize();

   qm::Mat4f rotationMatrix(
    right[0], up[0], -forward[0], 0.0f,
    right[1], up[1], -forward[1], 0.0f,
    right[2], up[2], -forward[2], 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  );

  return rotationMatrix * translationMatrix;
}

qm::Mat4f lookAt(const qm::Vec3f& cameraPos, const qm::Vec3f& targetPos, const qm::Vec3f& cameraUp) {
  qm::Vec3f forward, right, up = cameraUp;
  return lookAt(cameraPos, targetPos, up, forward, right);
}


qm::Mat4f perspective(float fovY, float aspect, float near, float far) {
  float fovRad = fovY * ONE_DEG_IN_RAD;
  float range = tan(fovRad / 2.0f) * near;
  float sx = (2.0f * near) / (range * aspect + range * aspect);
  float sy = near / range;
  float sz = -(far + near) / (far - near);
  float pz = -(2.0f * far * near) / (far - near);
  qm::Mat4f matrix = qm::Mat4f::zeroMatrix();
  matrix[0] = sx;
  matrix[5] = sy;
  matrix[10] = sz;
  matrix[14] = pz;
  matrix[11] = -1.0f;

  return matrix;
}


int main() {
  initGLWF();
  GLFWwindow* window = createWindow();
  //glfwSetKeyCallback(window, keyCallback);
  initGLContext(window);

  float camSpeed = 1.0f; // 1 unit per second
  float camYawSpeed = 30.0f; // 10 degrees per second
  float camYaw = 0.0f;

  // Camera
  qm::Vec3f cameraPosition(0.0f, 0.5f, 1.0f);
  qm::Vec3f viewTarget(0.0f, 0.0f, 0.0f);
  qm::Vec3f up(0.f, 1.f, 0.f);
  qm::Vec3f forward, right;

  // Lights
  qm::Vec3f ambientColor(0.2f, 0.2f, 0.2f);
  PointLight light(qm::Vec3f(1.0f, 1.0f, 1.0f), qm::Vec3f(0.9f, 0.9f, 0.9f), qm::Vec3f(1.0f, 1.0f, 1.0f), ambientColor);

  qm::Vec3f lightPos = light.getPosition();
  qm::Vec3f lightDiffuse = light.getDiffuseColor();
  qm::Vec3f lightSpecular = light.getSpecularColor();
  qm::Vec3f lightAmbient = light.getAmbientColor();

  qm::Mat4f viewMatrix;
  viewMatrix = lookAt(cameraPosition, viewTarget, up, forward, right);
  qm::Quat quat(camYaw, 0.0f, 1.0f, 0.0f);
  qm::Mat4f rotationMatrix = quat.toMatrix();
  viewMatrix = rotationMatrix * viewMatrix;

  qm::Mat4f projectionMatrix = perspective(67.f, (float) windowWidth / (float) windowHeight, 0.1f, 100.f);


  float objectAngle = 0.f;
  float objectSpeed = 10.f;



  /*float points[] = {
    0.0f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    0.0f, -1.5f, 0.0f
  };
  float normals[] = {
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f
  };

  unsigned int pointsVbo = 0;
  glGenBuffers(1, &pointsVbo);
  glBindBuffer(GL_ARRAY_BUFFER, pointsVbo);
  glBufferData(GL_ARRAY_BUFFER, 12 * sizeof (float), points, GL_STATIC_DRAW);

  unsigned int normalsVbo = 0;
  glGenBuffers(1, &normalsVbo);
  glBindBuffer(GL_ARRAY_BUFFER, normalsVbo);
  glBufferData(GL_ARRAY_BUFFER, 12 * sizeof (float), normals, GL_STATIC_DRAW);

  unsigned int vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, pointsVbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glBindBuffer(GL_ARRAY_BUFFER, normalsVbo);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);*/


  // Textures init
  glActiveTexture(GL_TEXTURE0);
  glActiveTexture(GL_TEXTURE1);

  // Dragon
  Object dragon;
  /*dragon.loadOBJ(MODELS + "obj\\Dragon\\dragon3.obj");
  dragon.computeVertices();
  dragon.createVAO();
  dragon.getMaterial().diffuseColor = qm::Vec3f(0.627, 0.105, 0.049);
*/

  // Shader program
  ShaderProgram dragonShaderProgram(&logger);
  dragonShaderProgram.loadShader(GL_VERTEX_SHADER, SHADERS + "customMatrixes_vs.glsl");
  dragonShaderProgram.loadShader(GL_FRAGMENT_SHADER, SHADERS + "phong_fs.glsl");
  dragonShaderProgram.link();
  dragonShaderProgram.printAll();

  // Use uniforms
  dragonShaderProgram.useUniform("view");
  dragonShaderProgram.useUniform("proj");
  dragonShaderProgram.useUniform("model");

  dragonShaderProgram.useUniform("lightPosition_world");
  dragonShaderProgram.useUniform("lightDiffuse");
  dragonShaderProgram.useUniform("lightSpecular");
  dragonShaderProgram.useUniform("lightAmbient");

  dragonShaderProgram.useUniform("ambientColor");
  dragonShaderProgram.useUniform("diffuseColor");
  dragonShaderProgram.useUniform("specularColor");

  dragonShaderProgram.useUniform("useDiffuseMap");
  dragonShaderProgram.useUniform("useSpecularMap");
  dragonShaderProgram.useUniform("diffuseMap");
  dragonShaderProgram.useUniform("specularMap");

  // Init uniform values
  dragonShaderProgram.use();

  dragonShaderProgram.setUniformMat4f("view", viewMatrix);
  dragonShaderProgram.setUniformMat4f("proj", projectionMatrix);

  dragonShaderProgram.setUniformVec3f("lightPosition_world", lightPos);
  dragonShaderProgram.setUniformVec3f("lightDiffuse", lightDiffuse);
  dragonShaderProgram.setUniformVec3f("lightSpecular", lightSpecular);
  dragonShaderProgram.setUniformVec3f("lightAmbient", lightAmbient);

  dragonShaderProgram.setUniform1i("useDiffuseMap", 1);
  dragonShaderProgram.setUniform1i("useSpecularMap", 1);
  dragonShaderProgram.setUniformTextureIndex("diffuseMap", 0);
  dragonShaderProgram.setUniformTextureIndex("specularMap", 1);

  /*
  int viewLocation = glGetUniformLocation(dragonShaderProgram.getIndex(), "view");
  int projLocation = glGetUniformLocation(dragonShaderProgram.getIndex(), "proj");
  int modelLocation = glGetUniformLocation(dragonShaderProgram.getIndex(), "model");

  int lightPosLocation = glGetUniformLocation(dragonShaderProgram.getIndex(), "lightPosition_world");
  int lightDiffuseLocation = glGetUniformLocation(dragonShaderProgram.getIndex(), "lightDiffuse");
  int lightSpecularLocation = glGetUniformLocation(dragonShaderProgram.getIndex(), "lightSpecular");
  int lightAmbientLocation = glGetUniformLocation(dragonShaderProgram.getIndex(), "lightAmbient");

  glUseProgram(dragonShaderProgram.getIndex());
  glUniformMatrix4fv(viewLocation, 1, GL_FALSE, viewMatrix.getArray());
  glUniformMatrix4fv(projLocation, 1, GL_FALSE, projectionMatrix.getArray());

  glUniform3f(lightPosLocation, lightPos[0], lightPos[1], lightPos[2]);
  glUniform3f(lightDiffuseLocation, lightDiffuse[0], lightDiffuse[1], lightDiffuse[2]);
  glUniform3f(lightSpecularLocation, lightSpecular[0], lightSpecular[1], lightSpecular[2]);
  glUniform3f(lightAmbientLocation, lightAmbient[0], lightAmbient[1], lightAmbient[2]);
*/

  ShaderProgram shaderProgram2(&logger);
  shaderProgram2.loadShader(GL_VERTEX_SHADER, SHADERS + "default_vs.glsl");
  shaderProgram2.loadShader(GL_FRAGMENT_SHADER, SHADERS + "uniform_fs.glsl");
  shaderProgram2.link();
  shaderProgram2.printAll();

  /*ShaderProgram shaderProgram1(&logger);
  shaderProgram1.attachShader(fragmentShader);
  shaderProgram1.attachShader(vertexShader);
  // OpenGL 3.2 compatibility: do not use the layout keyword in the shader, instead:
  glBindAttribLocation(shader_programme, 0, "vertexPosition");
  glBindAttribLocation(shader_programme, 1, "vertexColour");
  shaderProgram1.link();*/


  // Test
  OBJLoader objLoader;
  vector<Object> dragonObjects;
  objLoader.loadObjects(MODELS + "obj\\newDragon\\dragon_objects1.obj", dragonObjects, MODELS + "obj\\newDragon\\dragon.mtl");
  for (unsigned int i = 0 ; i < dragonObjects.size() ; i++) {
    dragonObjects[i].computeVertices();
    dragonObjects[i].createVAO();
  }

  glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE); // cull face
  glCullFace(GL_BACK); // cull back face
  glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

  bool saveToImages = false;
  long frameNumber = 0;

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    updateFPSCounter(window);
    frameNumber++;

    // add a timer for doing animation
    static double previousSeconds = glfwGetTime();
    double currentSeconds = glfwGetTime();
    double elapsedSeconds = currentSeconds - previousSeconds;
    previousSeconds = currentSeconds;


    // control keys
    bool cameraMoved = false;
    if (glfwGetKey(window, GLFW_KEY_A)) {
      cameraPosition[0] -= camSpeed * elapsedSeconds;
      viewTarget[0] -= camSpeed * elapsedSeconds;
      cameraMoved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_R)) {
      saveToImages = !saveToImages;
    }
    if (glfwGetKey(window, GLFW_KEY_D)) {
      cameraPosition[0] += camSpeed * elapsedSeconds;
      viewTarget[0] += camSpeed * elapsedSeconds;
      cameraMoved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP)) {
      cameraPosition[1] += camSpeed * elapsedSeconds;
      viewTarget[1] += camSpeed * elapsedSeconds;
      cameraMoved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN)) {
      cameraPosition[1] -= camSpeed * elapsedSeconds;
      viewTarget[1] -= camSpeed * elapsedSeconds;
      cameraMoved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_W)) {
      cameraPosition[2] -= camSpeed * elapsedSeconds;
      viewTarget[2] -= camSpeed * elapsedSeconds;
      cameraMoved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S)) {
      cameraPosition[2] += camSpeed * elapsedSeconds;
      viewTarget[2] += camSpeed * elapsedSeconds;
      cameraMoved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT)) {
      camYaw += camYawSpeed * elapsedSeconds;
      cameraMoved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
      camYaw -= camYawSpeed * elapsedSeconds;
      cameraMoved = true;
    }

    //glUseProgram(dragonShaderProgram.getIndex());
    dragonShaderProgram.use();
    dragonShaderProgram.setUniformMat4f("proj", projectionMatrix);
    //glUniformMatrix4fv(projLocation, 1, GL_FALSE, projectionMatrix.getArray());

    // update view matrix
    if (cameraMoved) {
      qm::Mat4f viewMatrix2 = lookAt(cameraPosition, viewTarget, up, forward, right);
      //qm::Mat4f translationMatrix = qm::Mat4f::translationMatrix(-cameraPosition);
      //qm::Mat4f rotationMatrix = qm::Mat4f::identityMatrix().rotateY(-camYaw);
      qm::Quat quat2(camYaw, up[0], up[1], up[2]);
      //qm::Quat quat2(camYaw, forward[0], forward[1], forward[2]);
      //qm::Mat4f rotationMatrix2 = quat2.toMatrix();
      //viewMatrix2 = rotationMatrix2 * viewMatrix2;
      dragonShaderProgram.setUniformMat4f("view", viewMatrix2);
      //glUniformMatrix4fv(viewLocation, 1, GL_FALSE, viewMatrix2.getArray());
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, windowWidth, windowHeight);


    // Dragon

    //objectAngle += objectSpeed * elapsedSeconds;

    //cube
    //objectTranslationMatrix = qm::Mat4f::translationMatrix(qm::Vec3f(-0.2f, 0.f, 0.f));
    // dragon
    //objectTranslationMatrix = qm::Mat4f::translationMatrix(qm::Vec3f(0.7f, -2.f, -0.f));
    //objectTranslationMatrix = qm::Mat4f::translationMatrix(qm::Vec3f(-1.f, 0.f, -1.f));

   // qm::Mat4f objectMatrix = objectTranslationMatrix * objectRotationMatrix;

    //objectMatrix = objectRotationMatrix;


    for (unsigned int i = 0 ; i < dragonObjects.size() ; i++) {
      dragonObjects[i].rotate(objectSpeed * elapsedSeconds, 0.f, 1.f, 0.f);
      dragonShaderProgram.setUniformMat4f("model", dragonObjects[i].retrieveModelMatrix());
      dragonShaderProgram.setUniformsFromMaterial(dragonObjects[i].getMaterial());
      //cout << dragonObjects[i].getMaterial().diffuseColor << endl;
      //glUniformMatrix4fv(modelLocation, 1, GL_FALSE, dragonObjects[i].retrieveModelMatrix().getArray());
      glBindVertexArray(dragonObjects[i].getVAO());
      glDrawArrays(GL_TRIANGLES, 0, dragonObjects[i].verticesNumber()); // number of vertices
    }

    dragon.rotate(objectSpeed * elapsedSeconds, 0.f, 1.f, 0.f);
    dragonShaderProgram.setUniformsFromMaterial(dragon.getMaterial());
    dragonShaderProgram.setUniformMat4f("model", dragon.retrieveModelMatrix());
    //glUniformMatrix4fv(modelLocation, 1, GL_FALSE, dragon.retrieveModelMatrix().getArray());


    /*glBindVertexArray(dragon.getVAO());
    glDrawArrays(GL_TRIANGLES, 0, dragon.verticesNumber()); // number of vertices
*/

    if (saveToImages) {
      unsigned char* buffer = (unsigned char*)malloc (windowWidth * windowHeight * 3);
      glReadPixels (0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, buffer);
      char name[1024];
      sprintf (name, "C:\\Users\\Quentin\\Documents\\Workspace\\OpenGLTest\\output\\frame-%ld.png", frameNumber);
      unsigned char* last_row = buffer + (windowWidth * 3 * (windowHeight - 1));
      if (!stbi_write_png (
        name, windowWidth, windowHeight, 3, last_row, -3 * windowWidth)) {
        fprintf (stderr, "ERROR: could not write screenshot file %s\n", name);
      }
      free (buffer);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }


  // Termination
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(1);
}
