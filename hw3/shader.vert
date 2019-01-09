#version 410

layout(location = 0) in vec3 position;

// Data from CPU
uniform mat4 MVP; // ModelViewProjection Matrix
uniform mat4 MV;  // ModelView idMVPMatrix
uniform vec4 cameraPosition;
uniform float heightFactor;

// Texture-related data
uniform sampler2D rgbTexture;
// these two are converted to float for the sake of better precision
uniform float widthTexture;
uniform float heightTexture;

// These are fed to shader.frag, as I see

// Output to Fragment Shader
out vec2 textureCoordinate; // For texture-color; x,y
out vec3 vertexNormal;      // For Lighting computation to be calculated
out vec3 ToLightVector;     // Vector from Vertex to Light; differences
out vec3 ToCameraVector;    // Vector from Vertex to Camera; differences

vec3 calculateToCameraVector(vec3 v) {
    return normalize(cameraPosition.xyz - v);
}

vec3 calculateToLightVector(vec3 v) {
    vec3 lightPositionVec3;

    lightPositionVec3.x = float(widthTexture) / 2.0f;
    lightPositionVec3.y = float(widthTexture) + float(heightTexture);
    lightPositionVec3.z = float(heightTexture) / 2.0f;

    return normalize(lightPositionVec3 - v);
}

float calculateHeight(vec3 v) {
    vec2 l_textureCoordinate;
    l_textureCoordinate.x = 1.0f - v.x/float(widthTexture);
    l_textureCoordinate.y = 1.0f - v.z/float(heightTexture);

    // get texture value, compute height
    vec4 textureColor = texture(rgbTexture, l_textureCoordinate); // lookup!

    return heightFactor * (0.2126f * textureColor.r + 0.7152f * textureColor.g + 0.0722f * textureColor.b);
}

bool isValid(vec3 v) {
    return v.x >= 0.0f && v.z >= 0.0f && v.x <= widthTexture && v.z <= heightTexture;
}

// clock-wise
vec3 calculateSurfaceNormal(vec3 v0, vec3 v1, vec3 v2) {
    return normalize(cross(v2 - v0, v1 - v0));
}

// y's are always zero, before calculation
vec3 calculateNormalVector(vec3 pos, vec3 pos_with_height) {
    vec3 dir1 = pos + vec3(1.0f, 0.0f, 0.0f);  // 0
    vec3 dir2 = pos + vec3(0.0f, 0.0f, 1.0f);  // 90
    vec3 dir3 = pos + vec3(-1.0f, 0.0f, 1.0f); // 135
    vec3 dir4 = pos + vec3(-1.0f, 0.0f, 0.0f); // 180
    vec3 dir5 = pos + vec3(0.0f, 0.0f, -1.0f); // 270
    vec3 dir6 = pos + vec3(1.0f, 0.0f, -1.0f); // 315

    bool isVal_dir1 = isValid(dir1);
    bool isVal_dir2 = isValid(dir2);
    bool isVal_dir3 = isValid(dir3);
    bool isVal_dir4 = isValid(dir4);
    bool isVal_dir5 = isValid(dir5);
    bool isVal_dir6 = isValid(dir6);

    if (isVal_dir1) {
        dir1.y = calculateHeight(dir1);
    }

    if (isVal_dir2) {
        dir2.y = calculateHeight(dir2);
    }

    if (isVal_dir3) {
        dir3.y = calculateHeight(dir3);
    }

    if (isVal_dir4) {
        dir4.y = calculateHeight(dir4);
    }

    if (isVal_dir5) {
        dir5.y = calculateHeight(dir5);
    }

    if (isVal_dir6) {
        dir6.y = calculateHeight(dir6);
    }

    vec3 normal;
    normal.x = 0.0f;
    normal.y = 0.0f;
    normal.z = 0.0f;

    if (isVal_dir1 && isVal_dir2) {
        normal += calculateSurfaceNormal(pos_with_height, dir1, dir2);
    }

    if (isVal_dir2 && isVal_dir3) {
        normal += calculateSurfaceNormal(pos_with_height, dir2, dir3);
    }

    if (isVal_dir3 && isVal_dir4) {
        normal += calculateSurfaceNormal(pos_with_height, dir3, dir4);
    }

    if (isVal_dir4 && isVal_dir5) {
        normal += calculateSurfaceNormal(pos_with_height, dir4, dir5);
    }

    if (isVal_dir5 && isVal_dir6) {
        normal += calculateSurfaceNormal(pos_with_height, dir5, dir6);
    }

    if (isVal_dir6 && isVal_dir1) {
        normal += calculateSurfaceNormal(pos_with_height, dir6, dir1);
    }


    return normalize(normal);
}

void main()
{
    vec3 current_position;
    current_position.x = position.x;
    current_position.z = position.z;

    // get texture value, compute height
    current_position.y = calculateHeight(position);

    textureCoordinate.x = 1.0f - float(current_position.x) / float(widthTexture);
    textureCoordinate.y = 1.0f - float(current_position.z) / float(heightTexture);

    // compute normal vector using also the heights of neighbor vertices
    vertexNormal = calculateNormalVector(position, current_position);

    // compute toLight vector vertex coordinate in VCS
    ToLightVector = calculateToLightVector(current_position);
    ToCameraVector = calculateToCameraVector(current_position);

    // set gl_Position variable correctly to give the transformed vertex position
    gl_Position = MVP * vec4(current_position, 1.0f);
}
