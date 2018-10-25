#include <iostream>
#include <limits>
#include "parser.h"
#include "ppm.h"

typedef unsigned char RGB[3];

const RGB BAR_COLOR[8] =
{
    { 255, 255, 255 },  // 100% White
    { 255, 255,   0 },  // Yellow
    {   0, 255, 255 },  // Cyan
    {   0, 255,   0 },  // Green
    { 255,   0, 255 },  // Magenta
    { 255,   0,   0 },  // Red
    {   0,   0, 255 },  // Blue
    {   0,   0,   0 },  // Black
};

// Our main function for homework
void render_image(const parser::Camera & camera, const parser::Scene & scene)
{
        int width = camera.image_width, height = camera.image_height;
        unsigned char image[width * height * 3];

        // ***********************************************************************
        // Store intersection points and "RGB" values for corresponding pixel
        // ***********************************************************************
        
        std::vector<parser::CameraRay> cameraRays;
        cameraRays.reserve(width * height * 3);
        
        // Calculate "parser::CameraRay"s for each pixel
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {

                cameraRays.emplace_back(parser::CameraRay(camera, x, y));
                parser::CameraRay & cameraRay = cameraRays.back();
                
                float min_distance = std::numeric_limits<float>::max(), f_distance;
                parser::Vec3f min_dist_intersection, f_intersection;

                for (const parser::Mesh & o: scene.meshes) {
                    if (cameraRay.intersects(o, scene.vertex_data, f_intersection, f_distance)) {
                        if (f_distance < min_distance) {
                            min_distance = f_distance;
                            min_dist_intersection = f_intersection;
                        }
                    }
                }
                for (const parser::Triangle & o: scene.triangles) {
                    if (cameraRay.intersects(o, scene.vertex_data, f_intersection, f_distance)) {
                        if (f_distance < min_distance) {
                            min_distance = f_distance;
                            min_dist_intersection = f_intersection;
                        }
                    }
                }
                for (const parser::Sphere & o: scene.spheres) {
                    if (cameraRay.intersects(o, scene.vertex_data, f_intersection, f_distance)) {
                        if (f_distance < min_distance) {
                            min_distance = f_distance;
                            min_dist_intersection = f_intersection;
                        }
                    }
                }

                if (min_distance != std::numeric_limits<float>::max()) {
                    cameraRay.intersection = min_dist_intersection;
                    cameraRay.intersection_distance = min_distance;
                    cameraRay.intersection_exists = 1;
                }
                else {
                    cameraRay.intersection_exists = 0;
                    // TODO set RGB to background colour;
                }
            }
        }


        
        
        
        
        
        
        
        
        write_ppm(camera.image_name.c_str(), image, width, height);

}

int main(int argc, char* argv[])
{
    parser::Scene scene;

    scene.loadFromXml(argv[1]);

    for (const parser::Camera & camera: scene.cameras) {
        render_image(camera, scene);
    }

    return 0;
}

