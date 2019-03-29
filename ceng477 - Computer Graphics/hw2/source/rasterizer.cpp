#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include "hw2_types.h"
#include "hw2_math_ops.h"
#include "hw2_file_ops.h"
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

#define __DEBUG__ true

Camera cameras[100];
int numberOfCameras = 0;

Model models[1000];
int numberOfModels = 0;

Color colors[100000];
int numberOfColors = 0;

Translation translations[1000];
int numberOfTranslations = 0;

Rotation rotations[1000];
int numberOfRotations = 0;

Scaling scalings[1000];
int numberOfScalings = 0;

Vec3 a_vertices[100000];
int numberOfVertices = 0;

Color backgroundColor;

// backface culling setting, default disabled
int backfaceCullingSetting = 0;

Color **image;

Vec3 getVec3FromVec4(const Vec4 & vec) {
    Vec3 r;

    r.x = vec.x;
    r.y = vec.y;
    r.z = vec.z;

    r.colorId = vec.colorId;

    return r;
}

void debug_print(const std::string & error_message) {
    if (true == __DEBUG__) {
        std::cerr << error_message << std::endl;
    }
}

/*
	Initializes image with background color
*/
void initializeImage(Camera cam) {
    for (int i = 0; i < cam.sizeX; i++) {
        for (int j = 0; j < cam.sizeY; j++) {
            image[i][j].r = backgroundColor.r;
            image[i][j].g = backgroundColor.g;
            image[i][j].b = backgroundColor.b;
        }
    }
}

void apply_M_model(Model &model, std::map<int, std::vector<Vec4> *> *modelTriangleMap) {

    for (int t = 0; t < model.numberOfTriangles; t++) {
        Triangle & triangle = model.triangles[t];
        std::vector<Vec4> *vertexVect = new std::vector<Vec4>();

        for (int v = 0; v < 3; v++) {
            int vertexId = triangle.vertexIds[v];

            vertexVect->push_back(Vec4(a_vertices[vertexId]));
        }
    
        modelTriangleMap->insert(std::pair<int, std::vector<Vec4> *>(t, vertexVect));
    }

    Matrix_4_4 M_model;
    M_model.makeIdentity();

    for (int t = 0; t < model.numberOfTransformations ; t++) {

        Matrix_4_4 transformation;

        char transformationType = model.transformationTypes[t];
        int transformationId = model.transformationIDs[t];

        // scaling
        if ('s' == transformationType) {
            transformation.makeScale(scalings[transformationId]);
        }
            // rotating
        else if ('r' == transformationType) {
            transformation.makeRotationArbitrary(rotations[transformationId]);
        }
            // translation
        else if ('t' == transformationType) {
            transformation.makeTranslation(translations[transformationId]);
        }
        else {
            debug_print("Unexpected transformation type.");
        }

        M_model = transformation.multiplyBy(M_model);
    }

    for (std::pair <int, std::vector<Vec4> *> triangleVertexPair : *modelTriangleMap){
        for(Vec4 &vect : *triangleVertexPair.second){
            vect = M_model.multiplyBy(vect);
        }
    }
}


Matrix_4_4 calculate_M_Cam(const Camera & cam) {
    Vec3 corrected_u = crossProductVec3(cam.gaze, cam.v);
    Vec3 corrected_v = crossProductVec3(corrected_u, cam.gaze);

    Matrix_4_4 M;
    M.makeFrom3Vec3(normalizeVec3(corrected_u), normalizeVec3(corrected_v), normalizeVec3(cam.w));

    Matrix_4_4 T;
    T.makeTranslation(-cam.pos.x, -cam.pos.y, -cam.pos.z);

    return M.multiplyBy(T);
}

Matrix_4_4 calculate_M_per(const Camera & cam) {
    Matrix_4_4 M_per;

    M_per.data[0][0] = 2 * cam.n / (cam.r - cam.l);
    M_per.data[0][2] = (cam.r + cam.l) / (cam.r - cam.l);

    M_per.data[1][1] = 2 * cam.n / (cam.t - cam.b);
    M_per.data[1][2] = (cam.t + cam.b) / (cam.t - cam.b);

    M_per.data[2][2] = -(cam.f + cam.n) / (cam.f - cam.n);
    M_per.data[2][3] = -2 * cam.f * cam.n / (cam.f - cam.n);

    M_per.data[3][2] = -1;

    return M_per;
}

// M_vp is actually a 3 x 4 matrix. Hence last row will be zeros for multiplication
Matrix_4_4 calculate_M_vp(const Camera & cam) {
    Matrix_4_4 M_vp;

    M_vp.data[0][0] = (double) cam.sizeX / 2;
    M_vp.data[0][3] = ((double) cam.sizeX - 1) / 2;

    M_vp.data[1][1] = (double) cam.sizeY / 2;
    M_vp.data[1][3] = ((double) cam.sizeY - 1) / 2;

    M_vp.data[2][2] = 0.5;
    M_vp.data[2][3] = 0.5;

    return M_vp;
}

// determines the vertex order for wireframe drawing
void determineSmaller(Vec4 v0, Vec4 v1, Vec4 *smaller, Vec4 *larger, bool x){
    if (x){
        if(v0.x > v1.x){
            *smaller = v1;
            *larger = v0;
        }
        else{ 
            *smaller = v0;
            *larger = v1;
        }
    }
    else{
        if(v0.y > v1.y){
            *smaller = v1;
            *larger = v0;
        }
        else{ 
            *smaller = v0;
            *larger = v1;
        }
    }
}

// adds two colors, used for color increment in midpoint algorithm
Color addColor(Color old, Color incr){
    Color ret;
    ret.r = old.r + incr.r;
    ret.g = old.g + incr.g;
    ret.b = old.b + incr.b;
    return ret;
}

// midpoint algorithm implementation for 1 > m > -1 cases, sign determines y direction
void midpointX(Vec4 v0, Vec4 v1, bool sign){
    int y = v0.y;
    int ydif = v0.y - v1.y; // y0 - y1
    int xdif = v1.x - v0.x; // x1 - x0
    Color c = colors[v0.colorId];

    Color dc; 
    dc.r = (colors[v1.colorId].r - colors[v0.colorId].r) / xdif;
    dc.g = (colors[v1.colorId].g - colors[v0.colorId].g) / xdif;
    dc.b = (colors[v1.colorId].b - colors[v0.colorId].b) / xdif;
    
    double d = ydif + 0.5 * xdif;
    for (int i = v0.x; i <= v1.x; i++){
        // TODO: draw the pixel x,y with color c
        image[i][y].r = std::round(c.r);
        image[i][y].g = std::round(c.g);
        image[i][y].b = std::round(c.b);

        if (sign){
            if (d < 0){
                y++;
                d += ydif + xdif;
            }
            else{
                // go E
                d += ydif;
            }
        }
        else{
            if (d > 0){
                y--;
                d += ydif - xdif;
            }
            else{
                // go E
                d += ydif;
            }
        }
        
        c = addColor(c,dc);
    }
}

// midpoint algorithm implementation for m > 1 and m < -1 cases, sign determines x direction
void midpointY(Vec4 v0, Vec4 v1, bool sign){
    int x = v0.x;
    int ydif = v1.y - v0.y; // y0 - y1
    int xdif = v0.x - v1.x; // x1 - x0
    Color c = colors[v0.colorId];

    Color dc; 
    dc.r = (colors[v1.colorId].r - colors[v0.colorId].r) / ydif;
    dc.g = (colors[v1.colorId].g - colors[v0.colorId].g) / ydif;
    dc.b = (colors[v1.colorId].b - colors[v0.colorId].b) / ydif;
    
    double d = xdif + 0.5 * ydif;
    for (int i = v0.y; i <= v1.y; i++){
        // TODO: draw the pixel x,y with color c
        image[x][i].r = std::round(c.r);
        image[x][i].g = std::round(c.g);
        image[x][i].b = std::round(c.b);

        if (sign){
            if (d < 0){
                x++;
                d += ydif + xdif;
            }
            else{
                // go E
                d += xdif;
            }
        }
        else{
            if (d > 0){
                x--;
                d += xdif - ydif;
            }
            else{
                // go E
                d += xdif;
            }
        }
        c = addColor(c,dc);
    }
}

/*
	Transformations, culling, rasterization are done here.
	You can define helper functions inside this file (rasterizer.cpp) only.
	Using types in "hw2_types.h" and functions in "hw2_math_ops.cpp" will speed you up while working.
*/
void forwardRenderingPipeline(Camera & cam, std::map<int, std::map<int, std::vector<Vec4> *> > *modelMap) {

    // Calculate camera transformations - M_cam
    Matrix_4_4 M_cam = calculate_M_Cam(cam);

    // Calculate perspective projection - M_per
    Matrix_4_4 M_per = calculate_M_per(cam);

    // Calculate viewport transformations - M_vp
    Matrix_4_4 M_vp = calculate_M_vp(cam);

    // Apply backface test and, apply viewport transformation and rasterization
    //  on triangle's vertices if triangle passes test
    for (int m = 0; m < numberOfModels; m++) {
        const Model & model = models[m];

        for (int t = 0; t < model.numberOfTriangles; t++) {

            // Apply first part of matrix transformations
            Vec4 v0 = M_cam.multiplyBy(modelMap->at(m).at(t)->at(0));
            Vec4 v1 = M_cam.multiplyBy(modelMap->at(m).at(t)->at(1));
            Vec4 v2 = M_cam.multiplyBy(modelMap->at(m).at(t)->at(2));

            Vec3 v0_3 = getVec3FromVec4(v0);
            Vec3 v1_3 = getVec3FromVec4(v1);
            Vec3 v2_3 = getVec3FromVec4(v2);

            Vec3 normal = crossProductVec3(normalizeVec3(subtractVec3(v1_3, v0_3)), normalizeVec3(subtractVec3(v2_3, v0_3)));
            normal = normalizeVec3(normal);
            double dot = dotProductVec3(normalizeVec3(v2_3), normal);

            bool backface_passed = (backfaceCullingSetting == 1 && dot < 0) || backfaceCullingSetting == 0;

            // Apply viewport transformation and rasterization
            if (true == backface_passed) {

                // Apply first part of matrix transformations
                v0 = M_per.multiplyBy(v0);
                v1 = M_per.multiplyBy(v1);
                v2 = M_per.multiplyBy(v2);

                v0.make_homogenous();
                v1.make_homogenous();
                v2.make_homogenous();

                v0 = M_vp.multiplyBy(v0);
                v1 = M_vp.multiplyBy(v1);
                v2 = M_vp.multiplyBy(v2);

                // Apply rasterization
                // Now we have vertices in Vec4 form but last dimension will be ignored

                // wireframe mode
                if (model.type == 0) {
                    // get the vertices of the triangles and draw the lines between them
                    // interpolate the color between two vertices
                    // JUST DRAW THE LINES

                    Vec4 sm, la;
                    double m;
                    
                    // draw v0-v1 line
                    determineSmaller(v0, v1, &sm, &la, true); // determine which x is smaller
                    m = (la.y - sm.y) / (la.x - sm.x); // find slope 
                    if (1 > m && m >= 0){
                        // CASE 1
                        // standard case, lower half of 1st quadrant
                        midpointX(sm, la, true);
                    }
                    else if (m > 1){
                        // CASE 2
                        // upper half of 1st quadrant,switch x and y
                        determineSmaller(v0, v1, &sm, &la, false); // determine which x is smaller
                        midpointY(sm, la, true);
                    }
                    else if (-1 < m && m < 0){
                        // CASE 3
                        // upper half of 4th quadrant, inverse of case 1
                        midpointX(sm, la, false);
                    }
                    else{
                        // CASE 4
                        // lower half of 4th quadrant, inverse of case 2
                        determineSmaller(v0, v1, &sm, &la, false); // determine which x is smaller
                        midpointY(sm, la, false);
                    }

                    // draw v1-v2 line
                    determineSmaller(v1, v2, &sm, &la, true); // determine which x is smaller
                    m = (la.y - sm.y) / (la.x - sm.x); // find slope 
                    if (1 > m && m >= 0){
                        // CASE 1
                        // standard case, lower half of 1st quadrant
                        midpointX(sm, la, true);
                    }
                    else if (m > 1){
                        // CASE 2
                        // upper half of 1st quadrant,switch x and y
                        determineSmaller(v1, v2, &sm, &la, false); // determine which x is smaller
                        midpointY(sm, la, true);
                    }
                    else if (-1 < m && m < 0){
                        // CASE 3
                        // upper half of 4th quadrant, inverse of case 1
                        midpointX(sm, la, false);
                    }
                    else{
                        // CASE 4
                        // lower half of 4th quadrant, inverse of case 2
                        determineSmaller(v1, v2, &sm, &la, false); // determine which x is smaller
                        midpointY(sm, la, false);
                    }

                    // draw v2-v0 line
                    determineSmaller(v2, v0, &sm, &la, true); // determine which x is smaller
                    m = (la.y - sm.y) / (la.x - sm.x); // find slope 
                    if (1 > m && m >= 0){
                        // CASE 1
                        // standard case, lower half of 1st quadrant
                        midpointX(sm, la, true);
                    }
                    else if (m > 1){
                        // CASE 2
                        // upper half of 1st quadrant,switch x and y
                        determineSmaller(v2, v0, &sm, &la, false); // determine which x is smaller
                        midpointY(sm, la, true);
                    }
                    else if (-1 < m && m < 0){
                        // CASE 3
                        // upper half of 4th quadrant, inverse of case 1
                        midpointX(sm, la, false);
                    }
                    else{
                        // CASE 4
                        // lower half of 4th quadrant, inverse of case 2
                        determineSmaller(v2, v0, &sm, &la, false); // determine which x is smaller
                        midpointY(sm, la, false);
                    }

                }
                // solid mode
                else { // if (model.type == 1)

                    LineEquation f01(v0, v1), f12(v1, v2), f20(v2, v0);

                    double ymin = std::min(std::min(v0.y, v1.y), v2.y),
                           ymax = std::max(std::max(v0.y, v1.y), v2.y),
                           xmin = std::min(std::min(v0.x, v1.x), v2.x),
                           xmax = std::max(std::max(v0.x, v1.x), v2.x);

                    int ymin_int = std::round(ymin),
                        ymax_int = std::round(ymax),
                        xmin_int = std::round(xmin),
                        xmax_int = std::round(xmax);

                    if (ymin_int == -1) {
                        ymin_int = 0;
                    }
                    if (ymax_int == cam.sizeY) {
                        ymax_int = cam.sizeY - 1;
                    }
                    if (xmin_int == -1) {
                        xmin_int = 0;
                    }
                    if (xmax_int == cam.sizeX) {
                        xmax_int = cam.sizeX - 1;
                    }

                    for (int y = ymin_int; y <= ymax_int; y++) {
                        for (int x = xmin_int; x <= xmax_int; x++) {

                            double alpha = f12(x, y) / f12(v0.x, v0.y),
                                   beta  = f20(x, y) / f20(v1.x, v1.y),
                                   gamma = f01(x, y) / f01(v2.x, v2.y);

                            // TODO precision ?
                            if (alpha >= 0 && beta >= 0 && gamma >= 0) {

                                const Color & c0 = colors[v0.colorId];
                                const Color & c1 = colors[v1.colorId];
                                const Color & c2 = colors[v2.colorId];

                                image[x][y].r = (int)(c0.r * alpha + c1.r * beta + c2.r * gamma);
                                image[x][y].g = (int)(c0.g * alpha + c1.g * beta + c2.g * gamma);
                                image[x][y].b = (int)(c0.b * alpha + c1.b * beta + c2.b * gamma);
                            }
                        }
                    }
                }
            }
        }
    }
}


int main(int argc, char **argv) {

    if (argc < 2) {
        std::cout << "Usage: ./rasterizer <scene file> <camera file>" << std::endl;
        return 1;
    }

    // read camera and scene files
    readSceneFile(argv[1]);
    readCameraFile(argv[2]);
    // map holding the transformed values of each model's vertices
    std::map<int, std::map<int, std::vector<Vec4> *> > modelMap;

    for (int m = 0; m < numberOfModels; m++) {
        modelMap.insert(std::pair<int, std::map<int, std::vector<Vec4> *> >(m, std::map<int, std::vector<Vec4> *>()));
        std::map<int, std::vector<Vec4> *> &modelTriangleMap = modelMap[m];
        apply_M_model(models[m], &modelTriangleMap);
    }

    image = nullptr;

    for (int c = 0; c < numberOfCameras; c++) {

        // allocate memory for image
        image = new Color*[cameras[c].sizeX];

        if (image == nullptr) {
            std::cerr << "ERROR: Cannot allocate memory for image." << std::endl;
            exit(1);
        }

        for (int row = 0; row < cameras[c].sizeX; row++) {
            image[row] = new Color[cameras[c].sizeY];
            if (image[row] == nullptr) {
                std::cerr << "ERROR: Cannot allocate memory for image." << std::endl;
                exit(1);
            }
        }

        // initialize image with basic values
        initializeImage(cameras[c]);

        /* do forward rendering pipeline operations */
        forwardRenderingPipeline(cameras[c], &modelMap);

        // generate PPM file
        writeImageToPPMFile(cameras[c]);

        // Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
        // Notice that os_type is not given as 1 (Ubuntu) or 2 (Windows), below call doesn't do conversion.
        // Change os_type to 1 or 2, after being sure that you have ImageMagick installed.
        convertPPMToPNG(cameras[c].outputFileName, 99);

        // deallocate memory in the end
        if (image) {
            for (int row = 0; row < cameras[c].sizeX; row++) {
                delete image[row];
            }

            delete[] image;
        }
    }

    return 0;
}
