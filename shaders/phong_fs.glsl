#version 400

// Geometry
in vec3 position_eye, normal_eye;
in vec2 uv;

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

uniform bool useDiffuseMap;
uniform bool useSpecularMap;
uniform sampler2D diffuseMap;
uniform sampler2D specularMap;

float specularExponent = 100.0;

out vec4 frag_colour;

void main() {
  vec2 flippedUV = vec2(uv.x, 1.0 - uv.y);
  vec3 ambientIntensity = lightAmbient * ambientColor;

  vec3 lightPosition_eye = vec3(view * vec4(lightPosition_world, 1.0));
  vec3 distanceToLight_eye = lightPosition_eye - position_eye;
  vec3 directionToLight_eye = normalize(distanceToLight_eye);

  // because of scaling
  vec3 normal = normalize(normal_eye);

  float dotProduct = dot(directionToLight_eye, normal);
  dotProduct = max(dotProduct, 0.0);

  vec3 diffuse = diffuseColor;
  if (useDiffuseMap)
    diffuse *= texture(diffuseMap, flippedUV).rgb;
  vec3 diffuseIntensity = lightDiffuse * diffuse * dotProduct;

  vec3 surfaceToViewer_eye = normalize(-position_eye);
  //vec3 reflection_eye = reflect(-directionToLight_eye, normal);
  //float specularDotProduct = dot(reflection_eye, surfaceToViewer_eye);
  // blinn-phong : do not use the expensive reflect method
  vec3 halfWay_eye = normalize(surfaceToViewer_eye + directionToLight_eye);
  float specularDotProduct = dot(halfWay_eye, normal);
  specularDotProduct = max(specularDotProduct, 0.0);
  float specularFactor = pow(specularDotProduct, specularExponent);

  vec3 specular = specularColor;
  if (useSpecularMap)
    specular *= texture(specularMap, flippedUV).rgb;
  vec3 specularIntensity = lightSpecular * specular * specularFactor;

  frag_colour = vec4(ambientIntensity + diffuseIntensity + specularIntensity, 1.0);
}
