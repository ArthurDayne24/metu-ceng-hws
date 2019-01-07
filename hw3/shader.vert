#version 410

layout(location = 0) in vec3 vertexPosition;

// Data from CPU 
uniform mat4 MVP; // ModelViewProjection Matrix
uniform mat4 MV;  // ModelView idMVPMatrix
uniform vec4 cameraPosition;
uniform float heightFactor;

// Texture-related data
uniform sampler2D rgbTexture;
uniform int widthTexture;
uniform int heightTexture;

// These are fed to shader.frag, as I see

// Output to Fragment Shader
out vec2 textureCoordinate; // For texture-color; x,y
out vec3 vertexNormal;      // For Lighting computation to be calculated
out vec3 ToLightVector;     // Vector from Vertex to Light; differences
out vec3 ToCameraVector;    // Vector from Vertex to Camera; differences

void calculateToCameraVector() {
    vec3 cameraPositionVec3;

    cameraPositionVec3.x = cameraPosition.x / cameraPosition.w;
    cameraPositionVec3.y = cameraPosition.y / cameraPosition.w;
    cameraPositionVec3.z = cameraPosition.z / cameraPosition.w;

    ToCameraVector = cameraPositionVec3 - vertexPosition;
}

float height;

void compute_height(float x, float y) {
    // TODO: check
    vec2 l_textureCoordinate;
    l_textureCoordinate.x = x / widthTexture;
    l_textureCoordinate.y = y / heightTexture;

    // get texture value, compute height
    vec4 textureColor = texture(rgbTexture, l_textureCoordinate); // lookup!

    // global !
    height = heightFactor * (0.2126 * textureColor.r + 0.7152 * textureColor.g + 0.0722 * textureColor.b);
}

void main()
{
    //textureCoordinate ok TODO: check
    textureCoordinate.x = vertexPosition.x / widthTexture;
    textureCoordinate.y = vertexPosition.y / heightTexture;

    // get texture value, compute height
    set_height(textureCoordinate.x, textureCoordinate.y); // sets global height variable
    vertexPosition.y = height;

    // compute normal vector using also the heights of neighbor vertices
    // TODO:

    // compute toLight vector vertex coordinate in VCS
    vec3 lightPositionVec3(widthTexture / 2, widthTexture + heightTexture, heightTexture / 2);
    ToLightVector = lightPositionVec3 - vertexPosition;

    // set gl_Position variable correctly to give the transformed vertex position
    gl_Position = MVP * vec4(vertexPosition, 1);
}
