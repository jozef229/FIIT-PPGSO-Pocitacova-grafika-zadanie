#version 330
// A texture is expected as program attribute
uniform sampler2D Texture;

// Direction of light
uniform vec3 LightDirection;
uniform vec3 LightColor;
uniform vec3 LightDirection2;
uniform vec3 LightColor2;
uniform vec3 AmbientLightColor;

uniform vec3 MatAmbient;
uniform vec4 MatDiffuse;
uniform vec3 MatSpecular;
uniform float MatShininess;

//camera pos
uniform vec3 CameraPosition;
// (optional) Transparency
uniform float Transparency;

// (optional) Texture offset
uniform vec2 TextureOffset;

// The vertex shader will feed this input
in vec2 texCoord;
in vec3 worldPosition;
// Wordspace normal passed from vertex shader
in vec4 normal;

// The final color
out vec4 FragmentColor;

vec4 objectTextureCoordinates(){
    // Lookup the color in Texture on coordinates given by texCoord. NOTE: Texture coordinate is inverted vertically for compatibility with OBJ
  return texture(Texture, vec2(texCoord.x, 1.0 - texCoord.y));
}

float lightDiffuseFactorOnObject(vec3 direction){
  //Calculate the lambert lighting factor based on the object normal
  return max(dot(normal, vec4(normalize(direction), 1.0f)), 0.0f);
}

vec3 specularColor(vec3 direction, vec3 color){
  //Calculate the specular color
  float SpecularFactor = pow(max(dot(normalize(CameraPosition - worldPosition), reflect(-direction, normal.xyz)), 0.0), MatShininess);
  float spStr = 0.1;
  return vec3(spStr) * vec3(SpecularFactor) * color;
}

vec4 help(vec3 direction, vec3 color){
  //Finalize the light's color contribution
  vec4 LightFragmentColor = objectTextureCoordinates() * vec4(lightDiffuseFactorOnObject(direction)) * vec4(color.rgb,1.0) * MatDiffuse;
  return vec4(specularColor(direction, color),1.0) + (LightFragmentColor);
}

void main() {

  //combine all the light colors with the scene's ambient color
  FragmentColor = vec4(AmbientLightColor * MatAmbient,1.0) + help(LightDirection, LightColor);
  FragmentColor += help(LightDirection2, LightColor2);
  FragmentColor.a = Transparency;
}

