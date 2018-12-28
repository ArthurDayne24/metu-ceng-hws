#version 410

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;

// Data from CPU 
uniform mat4 MVP; // ModelViewProjection Matrix
uniform mat4 MV; // ModelView idMVPMatrix
uniform vec4 cameraPosition;
uniform float heightFactor;

// Texture-related data
uniform sampler2D rgbTexture;
uniform int widthTexture;
uniform int heightTexture;

// These are fed to shader.frag, as I see

// Output to Fragment Shader
out vec2 textureCoordinate; // For texture-color
out vec3 vertexNormal; // For Lighting computation
out vec3 ToLightVector; // Vector from Vertex to Light;
out vec3 ToCameraVector; // Vector from Vertex to Camera;


void main()
{

    // get texture value, compute height
    // compute normal vector using also the heights of neighbor vertices

    // compute toLight vector vertex coordinate in VCS
   
    // set gl_Position variable correctly to give the transformed vertex position

    gl_Position = MVP * vec4(vertexPosition, 1);

}
