#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include "hw2_types.h"
#include "hw2_math_ops.h"
#include "hw2_file_ops.h"
#include <iostream>
#include <vector>

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

Matrix_4_4 calculate_M_Model() {
    Matrix_4_4 M_model;
    M_model.makeIdentity();

    for (int m = 0; m < numberOfModels; m++) {
        Model & model = models[m];

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
                std::cerr << "ERROR: Unexpected transformation." << std::endl;
                exit(1);
            }

            M_model = transformation.multiplyBy(M_model);
        }
    }

    return M_model;
}

Matrix_4_4 calculate_M_Cam(const Camera & cam) {
    Matrix_4_4 M;
    M.makeFrom3Vec3(cam.u, cam.v, cam.w);

    Matrix_4_4 T;
    T.makeTranslation(Translation{-cam.pos.x, -cam.pos.y, -cam.pos.z});

    return M.multiplyBy(T);
}

Matrix_4_4 calculate_M_per(const Camera & cam) {
    Matrix_4_4 M_per;

    M_per.data[0][0] = 2 * cam.n / (cam.r - cam.l);
    M_per.data[0][1] = 0;
    M_per.data[0][2] = (cam.r + cam.l) / (cam.r - cam.l);
    M_per.data[0][3] = 0;

    M_per.data[1][0] = 0;
    M_per.data[1][1] = 2 * cam.n / (cam.t - cam.b);
    M_per.data[1][2] = (cam.t + cam.b) / (cam.t - cam.b);
    M_per.data[1][3] = 0;

    M_per.data[2][0] = 0;
    M_per.data[2][1] = 0;
    M_per.data[2][2] = -(cam.f + cam.n) / (cam.f - cam.n);
    M_per.data[2][3] = -2 * cam.f * cam.n / (cam.f - cam.n);

    M_per.data[3][0] = 0;
    M_per.data[3][1] = 0;
    M_per.data[3][2] = -1;
    M_per.data[3][3] = 0;

    return M_per;
}

// M_vp is actually a 3 x 4 matrix. Hence last row will be zeros for multiplication
Matrix_4_4 calculate_M_vp(const Camera & cam) {
    Matrix_4_4 M_vp;

    M_vp.makeZeros();

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
void forwardRenderingPipeline(Camera & cam, const Matrix_4_4 & M_model, std::vector<Vec4> & v_vertices) {
    // TODO: IMPLEMENT HERE

    // Calculate camera transformations - M_cam
    Matrix_4_4 M_cam = calculate_M_Cam(cam);

    // Calculate perspective projection - M_per
    Matrix_4_4 M_per = calculate_M_per(cam);

    Matrix_4_4 M_accumulation = M_per.multiplyBy(M_cam.multiplyBy(M_model));

    // Apply first part of matrix transformations
    for (Vec4 & vertex : v_vertices) {
        vertex = M_accumulation.multiplyBy(vertex);
        // Apply perspective divide
        vertex.make_homogenous();
    }

    // Make camera origin centered and align its vectors etc.
    cam.bringtToOrigin();

    // Calculate viewport transformations - M_vp
    Matrix_4_4 M_vp = calculate_M_vp(cam);

    // Apply backface test and apply viewport transformation on triangle's vertices if triangle passes test
    for (int m = 0; m < numberOfModels; m++) {
        Model & model = models[m];

        for (int t = 0; t < model.numberOfTriangles; t++) {
            Triangle & triangle = model.triangles[t];

            const Vec3 & vertex0_v3 = (Vec3) v_vertices[triangle.vertexIds[0]];
            const Vec3 & vertex1_v3 = (Vec3) v_vertices[triangle.vertexIds[1]];

            Vec3 normal = crossProductVec3(vertex0_v3, vertex1_v3);

            double dot = dotProductVec3(vertex0_v3, normal);

            // TODO care precision
            triangle.backface_passed = !(dot > 0 && backfaceCullingSetting != 0);

            // Apply viewport transformation
            if (true == triangle.backface_passed) {
                Vec4 & vertex0_v4 = v_vertices[triangle.vertexIds[0]];
                Vec4 & vertex1_v4 = v_vertices[triangle.vertexIds[1]];
                Vec4 & vertex2_v4 = v_vertices[triangle.vertexIds[2]];

                vertex0_v4 = M_vp.multiplyBy(vertex0_v4);
                vertex1_v4 = M_vp.multiplyBy(vertex1_v4);
                vertex2_v4 = M_vp.multiplyBy(vertex2_v4);
            }
        }
    }

    // Apply rasterization
    // TODO
}

int main(int argc, char **argv) {

    if (argc < 2) {
        std::cout << "Usage: ./rasterizer <scene file> <camera file>" << std::endl;
        return 1;
    }

    // read camera and scene files
    readSceneFile(argv[1]);
    readCameraFile(argv[2]);

    // Calculate M_model
    Matrix_4_4 M_model = calculate_M_Model();

    image = nullptr;

    for (int i = 0; i < numberOfCameras; i++) {

        // allocate memory for image
        if (image) {
			for (int j = 0; j < cameras[i].sizeX; j++) {
		        delete image[j];
		    }

			delete[] image;
		}

        image = new Color*[cameras[i].sizeX];

        if (image == nullptr) {
            std::cerr << "ERROR: Cannot allocate memory for image." << std::endl;
            exit(1);
        }

        for (int j = 0; j < cameras[i].sizeX; j++) {
            image[j] = new Color[cameras[i].sizeY];
            if (image[j] == nullptr) {
                std::cerr << "ERROR: Cannot allocate memory for image." << std::endl;
                exit(1);
            }
        }

        // initialize image with basic values
        initializeImage(cameras[i]);

        // copy a_vertices to v_vertices as they are global and they will be modified separately for each camera
        std::vector<Vec4> v_vertices;
        v_vertices.reserve(numberOfVertices);

        for (int v = 0; v < numberOfVertices; v++) {
            v_vertices.emplace_back(Vec4(a_vertices[i]));
        }

        /* do forward rendering pipeline operations */
        forwardRenderingPipeline(cameras[i], M_model, v_vertices);

        // generate PPM file
        writeImageToPPMFile(cameras[i]);

        // Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
        // Notice that os_type is not given as 1 (Ubuntu) or 2 (Windows), below call doesn't do conversion.
        // Change os_type to 1 or 2, after being sure that you have ImageMagick installed.
        convertPPMToPNG(cameras[i].outputFileName, 99);
    }

    return 0;
}
