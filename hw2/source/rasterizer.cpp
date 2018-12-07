#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include "hw2_types.h"
#include "hw2_math_ops.h"
#include "hw2_file_ops.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <set>

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

void apply_M_model(const Model & model, std::vector<Vec4> & v_vertices) {
    std::set<int> model_vertexIds;

    for (int t = 0; t < model.numberOfTriangles; t++) {
        const Triangle & triangle = model.triangles[t];

        for (int v = 0; v < 3; v++) {
            int vertexId = triangle.vertexIds[v];

            model_vertexIds.insert(vertexId);
        }
    }

    Matrix_4_4 M_model;
    M_model.makeIdentity();

    for (int t = 0; t < model.numberOfTransformations; t++) {

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

    for (const int & vertexId : model_vertexIds) {
        v_vertices[vertexId] = M_model.multiplyBy(v_vertices[vertexId]);
    }
}

// bugfix XXX
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

    M_vp.data[0][0] = cam.sizeX / 2;
    M_vp.data[0][3] = (cam.sizeX - 1) / 2;

    M_vp.data[1][1] = cam.sizeY / 2;
    M_vp.data[1][3] = (cam.sizeY - 1) / 2;

    M_vp.data[2][2] = 0.5;
    M_vp.data[2][3] = 0.5;

    return M_vp;
}

/*
	Transformations, culling, rasterization are done here.
	You can define helper functions inside this file (rasterizer.cpp) only.
	Using types in "hw2_types.h" and functions in "hw2_math_ops.cpp" will speed you up while working.
*/
void forwardRenderingPipeline(const Camera & cam, std::vector<Vec4> & v_vertices) {
    // TODO: IMPLEMENT HERE

    // Calculate camera transformations - M_cam
    Matrix_4_4 M_cam = calculate_M_Cam(cam);

    // Calculate perspective projection - M_per
    Matrix_4_4 M_per = calculate_M_per(cam);

    Matrix_4_4 M_accumulation = M_per.multiplyBy(M_cam);

    // Apply first part of matrix transformations
    for (unsigned int v = 1; v < v_vertices.size(); v++) {
        Vec4 & vertex = v_vertices[v];

        vertex = M_accumulation.multiplyBy(vertex);

        // Apply perspective divide
        vertex.make_homogenous();
    }

    // Calculate viewport transformations - M_vp
    Matrix_4_4 M_vp = calculate_M_vp(cam);

    // Apply backface test and, apply viewport transformation and rasterization
    //  on triangle's vertices if triangle passes test
    for (int m = 0; m < numberOfModels; m++) {
        const Model & model = models[m];

        for (int t = 0; t < model.numberOfTriangles; t++) {
            const Triangle & triangle = model.triangles[t];

            Vec3 v0_3 = (Vec3) v_vertices[triangle.vertexIds[0]];
            Vec3 v1_3 = (Vec3) v_vertices[triangle.vertexIds[1]];
            Vec3 v2_3 = (Vec3) v_vertices[triangle.vertexIds[2]];

            Vec3 normal = crossProductVec3(subtractVec3(v2_3, v0_3), subtractVec3(v1_3, v0_3));

            double dot = dotProductVec3(v0_3, normal);

            // TODO care precision
            bool backface_passed = !(dot > 0 && backfaceCullingSetting != 0);

            // Apply viewport transformation and rasterization
            if (true == backface_passed) {
                // bugfix XXX
                Vec4 v0 = v_vertices[triangle.vertexIds[0]];
                Vec4 v1 = v_vertices[triangle.vertexIds[1]];
                Vec4 v2 = v_vertices[triangle.vertexIds[2]];

                v0 = M_vp.multiplyBy(v0);
                v1 = M_vp.multiplyBy(v1);
                v2 = M_vp.multiplyBy(v2);

                // Apply rasterization
                // Now we have vertices in Vec4 form but last dimension will be ignored

                // wireframe mode
                if (model.type == 0) {
                    // TODO implement me
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

                    if (xmin < 0 || xmin > cam.sizeX-1) {
                        debug_print("xmin: " + std::to_string(xmin) + " error");
                    }
                    if (ymin < 0 || ymin > cam.sizeY-1) {
                        debug_print("ymin: " + std::to_string(ymin) + " error");
                    }
                    if (xmax < 0 || xmax > cam.sizeX-1) {
                        debug_print("xmax: " + std::to_string(xmax) + " error");
                    }
                    if (ymax < 0 || ymax > cam.sizeY-1) {
                        debug_print("ymax: " + std::to_string(ymax) + " error");
                    }

                    // TODO possible problem with pixel coordinate to image array
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

                                image[x][y].r = c0.r * alpha + c1.r * beta + c2.r * gamma;
                                image[x][y].g = c0.g * alpha + c1.g * beta + c2.g * gamma;
                                image[x][y].b = c0.b * alpha + c1.b * beta + c2.b * gamma;
                            }
                        }
                    }
                }
            }
        }
    }
}

// TODO check application wise precision things

int main(int argc, char **argv) {

    if (argc < 2) {
        std::cout << "Usage: ./rasterizer <scene file> <camera file>" << std::endl;
        return 1;
    }

    // read camera and scene files
    readSceneFile(argv[1]);
    readCameraFile(argv[2]);

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

        // copy a_vertices to v_vertices as they are global and they will be modified separately for each camera
        std::vector<Vec4> v_vertices;
        //TODO v_vertices.reserve(numberOfVertices);

        // dummy
        v_vertices.push_back(Vec4());

        for (int v = 1; v <= numberOfVertices; v++) {
            v_vertices.push_back(Vec4(a_vertices[v]));
        }


        for (int m = 0; m < numberOfModels; m++) {
            apply_M_model(models[m], v_vertices);
        }

        /* do forward rendering pipeline operations */
        forwardRenderingPipeline(cameras[c], v_vertices);

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
