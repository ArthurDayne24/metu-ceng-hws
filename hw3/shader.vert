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

vec3 calculateToCameraVector(vec4 current_vertex) {
    vec3 cameraPositionVec3;

    cameraPositionVec3.x = cameraPosition.x / cameraPosition.w;
    cameraPositionVec3.y = cameraPosition.y / cameraPosition.w;
    cameraPositionVec3.z = cameraPosition.z / cameraPosition.w;

    return cameraPositionVec3 - current_vertex.xyz;
}

vec3 calculateToLightVector(vec4 current_vertex) {
    vec3 lightPositionVec3;

    lightPositionVec3.x = widthTexture / 2;
    lightPositionVec3.y = widthTexture + heightTexture;
    lightPositionVec3.z = heightTexture / 2;

    return current_vertex.xyz - lightPositionVec3;
}

float calculateHeight(vec3 v) {
    // TODO: check
    vec2 l_textureCoordinate;
    l_textureCoordinate.x = (1-v.x) / widthTexture;
    l_textureCoordinate.y = (1-v.z) / heightTexture;

    // get texture value, compute height
    vec4 textureColor = texture(rgbTexture, l_textureCoordinate); // lookup!

    return heightFactor * (0.2126 * textureColor.x + 0.7152 * textureColor.y + 0.0722 * textureColor.z);
}

bool isValid(vec3 v) {
    return v.x > 0 && v.z > 0 && v.x < widthTexture && v.z < heightTexture;
}

// clock-wise
vec3 calculateSurfaceNormal(vec3 v0, vec3 v1, vec3 v2) {
    return cross(v1 - v0, v2 - v0);
}

// y's are always zero, before calculation
vec3 calculateNormalVector(vec4 current_vertex) {
    vec3 dir1 = current_vertex.xyz + vec3(1, 0, 0);   // 0
    vec3 dir2 = current_vertex.xyz + vec3(1, 0, 1);   // 60
    vec3 dir3 = current_vertex.xyz + vec3(-1, 0, 1);  // 120
    vec3 dir4 = current_vertex.xyz + vec3(-1, 0, 0);  // 180
    vec3 dir5 = current_vertex.xyz + vec3(-1, 0, -1); // 240
    vec3 dir6 = current_vertex.xyz + vec3(1, 0, -1);  // 300

    bool isVal_dir1 = isValid(dir1);
    bool isVal_dir2 = isValid(dir2);
    bool isVal_dir3 = isValid(dir3);
    bool isVal_dir4 = isValid(dir4);
    bool isVal_dir5 = isValid(dir5);
    bool isVal_dir6 = isValid(dir6);

    if (isVal_dir1 == true) {
        dir1.y = calculateHeight(dir1);
    }

    if (isVal_dir2 == true) {
        dir2.y = calculateHeight(dir2);
    }

    if (isVal_dir3 == true) {
        dir3.y = calculateHeight(dir3);
    }

    if (isVal_dir4 == true) {
        dir4.y = calculateHeight(dir4);
    }

    if (isVal_dir5 == true) {
        dir5.y = calculateHeight(dir5);
    }

    if (isVal_dir6 == true) {
        dir6.y = calculateHeight(dir6);
    }

    // number of valid triangles
    float validCount = 0;

    vec3 normal;
    normal.x = 0;
    normal.y = 0;
    normal.z = 0;

    if (isVal_dir1 && isVal_dir2) {
        normal += calculateSurfaceNormal(current_vertex.xyz, dir1, dir2);
        validCount += 1;
    }

    if (isVal_dir2 && isVal_dir3) {
        normal += calculateSurfaceNormal(current_vertex.xyz, dir2, dir3);
        validCount += 1;
    }

    if (isVal_dir3 && isVal_dir4) {
        normal += calculateSurfaceNormal(current_vertex.xyz, dir3, dir4);
        validCount += 1;
    }

    if (isVal_dir4 && isVal_dir5) {
        normal += calculateSurfaceNormal(current_vertex.xyz, dir4, dir5);
        validCount += 1;
    }

    if (isVal_dir5 && isVal_dir6) {
        normal += calculateSurfaceNormal(current_vertex.xyz, dir5, dir6);
        validCount += 1;
    }

    if (isVal_dir6 && isVal_dir1) {
        normal += calculateSurfaceNormal(current_vertex.xyz, dir6, dir1);
        validCount += 1;
    }

    normal.x /= validCount;
    normal.y /= validCount;
    normal.z /= validCount;

    return normalize(normal);
}

void main()
{
    vec4 current_vertex = vec4(vertexPosition, 1);

    // get texture value, compute height
    current_vertex.y = calculateHeight(vertexPosition.xyz); // sets global height variable

    textureCoordinate.x = (1 - current_vertex.x) / widthTexture;
    textureCoordinate.y = (1 - current_vertex.z) / heightTexture;

    // compute normal vector using also the heights of neighbor vertices
    vertexNormal = calculateNormalVector(current_vertex);

    // compute toLight vector vertex coordinate in VCS
    ToLightVector = calculateToLightVector(current_vertex);
    ToCameraVector = calculateToCameraVector(current_vertex);

    // set gl_Position variable correctly to give the transformed vertex position
    gl_Position = MVP * current_vertex;
}
