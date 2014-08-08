QGL
=====

Simple library to easily load/manipulate 3D objects and manage shaders.

Dependencies:
- OpenGL
- GLEW
- GLFW
- QMath
- Q3DS
- QTools
- stb_image

Be careful with the libraries order : (glew static)
LIBS += -lglew32s -lglfw3 -lopengl32 -lglu32 -lgdi32