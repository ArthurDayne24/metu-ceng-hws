#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include "hw2_types.h"
#include "hw2_math_ops.h"
#include "hw2_file_ops.h"
#include <iostream>

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

Vec3 vertices[100000];
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

/*
	Transformations, culling, rasterization are done here.
	You can define helper functions inside this file (rasterizer.cpp) only.
	Using types in "hw2_types.h" and functions in "hw2_math_ops.cpp" will speed you up while working.
*/
void forwardRenderingPipeline(const Camera & cam, const Matrix_4_4 & M_model) {
    // TODO: IMPLEMENT HERE

    // Step - 2: Calculate camera transformations - M_cam
    Matrix_4_4 M_cam = calculate_M_Cam(cam);

}

int main(int argc, char **argv) {

    if (argc < 2) {
        std::cout << "Usage: ./rasterizer <scene file> <camera file>" << std::endl;
        return 1;
    }

    // read camera and scene files
    readSceneFile(argv[1]);
    readCameraFile(argv[2]);

    image = 0;

    for (int i = 0; i < numberOfCameras; i++) {

        // allocate memory for image
        if (image) {
			for (int j = 0; j < cameras[i].sizeX; j++) {
		        delete image[j];
		    }

			delete[] image;
		}

        image = new Color*[cameras[i].sizeX];

        if (image == NULL) {
            std::cout << "ERROR: Cannot allocate memory for image." << std::endl;
            exit(1);
        }

        for (int j = 0; j < cameras[i].sizeX; j++) {
            image[j] = new Color[cameras[i].sizeY];
            if (image[j] == NULL) {
                std::cout << "ERROR: Cannot allocate memory for image." << std::endl;
                exit(1);
            }
        }

        // initialize image with basic values
        initializeImage(cameras[i]);

        /* do forward rendering pipeline operations */

        // Step - 1 : Calculate M_model
        Matrix_4_4 M_model = calculate_M_Model();

        // Other steps are here
        forwardRenderingPipeline(cameras[i], M_model);

        // generate PPM file
        writeImageToPPMFile(cameras[i]);

        // Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
        // Notice that os_type is not given as 1 (Ubuntu) or 2 (Windows), below call doesn't do conversion.
        // Change os_type to 1 or 2, after being sure that you have ImageMagick installed.
        convertPPMToPNG(cameras[i].outputFileName, 99);
    }

    return 0;

}
