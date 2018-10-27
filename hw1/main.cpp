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
    unsigned char image[height * width * 3];

    // ***********************************************************************
    // Store intersection points and "RGB" values for corresponding pixel
    // ***********************************************************************
    
    std::vector<std::vector<parser::CameraRay>> cameraRays;
    cameraRays.reserve(height);
    
    // Calculate "parser::CameraRay"s for each pixel
    for (int y = 0; y < height; ++y) {

        cameraRays.emplace_back(std::vector<parser::CameraRay>());
        std::vector<parser::CameraRay> & cameraRaysCurHeight = cameraRays.back();
        cameraRaysCurHeight.reserve(width);

        for (int x = 0; x < width; ++x) {

            cameraRaysCurHeight.emplace_back(parser::CameraRay(camera, x, y));
            parser::CameraRay & cameraRay = cameraRaysCurHeight.back();
            
            float min_distance = std::numeric_limits<float>::max(), f_distance;
            parser::Vec3f min_dist_intersection, f_intersection;
            int min_dist_mat_id;
            parser::Vec3f min_dist_normal, f_normal;

            for (const parser::Mesh & o: scene.meshes) {
                if (cameraRay.intersects(o, scene.vertex_data, f_intersection,
                            f_distance, f_normal)) {
                    if (f_distance < min_distance) {
                        min_distance = f_distance;
                        min_dist_intersection = f_intersection;
                        min_dist_mat_id = o.material_id;
                        min_dist_normal = f_normal;
                    }
                }
            }
            for (const parser::Triangle & o: scene.triangles) {
                if (cameraRay.intersects(o, scene.vertex_data, f_intersection,
                            f_distance, f_normal)) {
                    if (f_distance < min_distance) {
                        min_distance = f_distance;
                        min_dist_intersection = f_intersection;
                        min_dist_mat_id = o.material_id;
                        min_dist_normal = f_normal;
                    }
                }
            }
            for (const parser::Sphere & o: scene.spheres) {
                if (cameraRay.intersects(o, scene.vertex_data, f_intersection,
                            f_distance, f_normal)) {
                    if (f_distance < min_distance) {
                        min_distance = f_distance;
                        min_dist_intersection = f_intersection;
                        min_dist_mat_id = o.material_id;
                        min_dist_normal = f_normal;
                    }
                }
            }

            if (min_distance != std::numeric_limits<float>::max()) {
                cameraRay.intersection_exists = 1;
                cameraRay.register_intersection(min_dist_intersection, min_distance,
                        min_dist_mat_id, min_dist_normal);
            }
            else {
                cameraRay.intersection_exists = 0;
                cameraRay.RGB.x = scene.background_color.x;
                cameraRay.RGB.y = scene.background_color.y;
                cameraRay.RGB.z = scene.background_color.z;
            }
        }
    }




















    // Last step, write pixels from cameraRays to image
    
    int imageIt = 0;

    auto cameraRaysCurHeightIt = cameraRays.begin();
    
    for (int y = 0; y < height; ++y, std::advance(cameraRaysCurHeightIt, 1)) {
        auto cameraRayIt = (*cameraRaysCurHeightIt).begin();

        for (int x = 0; x < width; ++x, std::advance(cameraRayIt, 1)) {
            image[imageIt++] = (*cameraRayIt).RGB.x;
            image[imageIt++] = (*cameraRayIt).RGB.y;
            image[imageIt++] = (*cameraRayIt).RGB.z;
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

