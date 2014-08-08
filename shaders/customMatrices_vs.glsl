#version 400
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 UV;

uniform mat4 view, proj, model;

out vec3 position_eye, normal_eye;
out vec2 uv;

void main () {
  uv = UV;
  position_eye = vec3(view * model * vec4(vertexPosition, 1.0));
  normal_eye = vec3(view * model * vec4(vertexNormal, 0.0));
  gl_Position = proj * vec4(position_eye, 1.0);
}
