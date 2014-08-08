#version 400

// Geometry
in vec3 position_eye, normal_eye;

// Lights
uniform vec3 lightPosition_world;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;
uniform vec3 lightAmbient;

uniform mat4 view;

// Object material
uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;

uniform sampler2D diffuseMap;
uniform sampler2D specularMap;

vec3 diffuse = vec3(0.627, 0.105, 0.049);
vec3 specular = vec3(1.0, 1.0, 1.0);
vec3 ambient = vec3(1.0, 1.0, 1.0);
float specularExponent = 100.0;

out vec4 frag_colour;

void main() {
  vec3 ambientIntensity = lightAmbient * ambientColor;

  vec3 lightPosition_eye = vec3(view * vec4(lightPosition_world, 1.0));
  vec3 distanceToLight_eye = lightPosition_eye - position_eye;
  vec3 directionToLight_eye = normalize(distanceToLight_eye);
  float dotProduct = dot(directionToLight_eye, normal_eye);
  dotProduct = max(dotProduct, 0.0);

  vec3 diffuseIntensity = lightDiffuse * diffuseColor * dotProduct;

  vec3 surfaceToViewer_eye = normalize(-position_eye);
  //vec3 reflection_eye = reflect(-directionToLight_eye, normal_eye);
  //float specularDotProduct = dot(reflection_eye, surfaceToViewer_eye);
  // blinn-phong : do not use the expensive reflect method
  vec3 halfWay_eye = normalize(surfaceToViewer_eye + directionToLight_eye);
  float specularDotProduct = dot(halfWay_eye, normal_eye);
  specularDotProduct = max(specularDotProduct, 0.0);
  float specularFactor = pow(specularDotProduct, specularExponent);

  vec3 specularIntensity = lightSpecular * specularColor * specularFactor;

  frag_colour = vec4(ambientIntensity + diffuseIntensity + specularIntensity, 1.0);
}
