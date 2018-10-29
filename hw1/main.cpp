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
    
    std::vector<std::vector<parser::CameraRay> > cameraRays; // pixel sutunlarini tutan yapi
    cameraRays.reserve(height);
    
    // Calculate "parser::CameraRay"s for each pixel
    for (int y = 0; y < height; ++y) {

        cameraRays.emplace_back(std::vector<parser::CameraRay>());
        std::vector<parser::CameraRay> & cameraRaysCurHeight = cameraRays.back();
        cameraRaysCurHeight.reserve(width);

        for (int x = 0; x < width; ++x) {

            cameraRaysCurHeight.emplace_back(parser::CameraRay(camera, x, y)); // bir pixel satiri
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

            if (min_distance < std::numeric_limits<float>::max()) {
                
                // SHADING BEGINS HERE

                std::cout << "intersection normal: " << min_dist_normal.x << "," << 
                    min_dist_normal.y << "," << min_dist_normal.z  
                            << "intersection point: " << min_dist_intersection.x << 
                            "," << min_dist_intersection.y << "," << min_dist_intersection.z << "  ";

                cameraRay.intersection_exists = 1;
                cameraRay.register_intersection(min_dist_intersection, min_distance,
                        min_dist_mat_id, min_dist_normal);
                
                // FROM THIS POINT ON, WE HAVE TO HANDLE THE INTERSECTION

                // add ambient shading values 
                cameraRay.RGB.x = scene.ambient_light.x * 
                    scene.materials[cameraRay.material_id-1].ambient.x;
                cameraRay.RGB.y = scene.ambient_light.y * 
                    scene.materials[cameraRay.material_id-1].ambient.y;
                cameraRay.RGB.z = scene.ambient_light.z * 
                    scene.materials[cameraRay.material_id-1].ambient.z;

                // calcuate diffuse shading value(s) by tracking the reflection of the 
                //  cameraray with the intersection surface
                
                for (const parser::PointLight & l : scene.point_lights){
                    bool inShadow = false;
                    
                    // create a lightray from each point light source to the intersection point
                    parser::LightRay lightRay = parser::LightRay(l, cameraRay.intersection);
                    
                    // found distance must be less than this distance for shadow occurrence
                    float lightDistance = parser::distance(l.position, cameraRay.intersection);                    

                    for (const parser::Mesh & o: scene.meshes) {
                        if (inShadow){
                            break;
                        } 
                        if (lightRay.intersects(o, scene.vertex_data, f_intersection,
                                    f_distance, f_normal)) {
                            if (fabs(f_distance - lightDistance) > scene.shadow_ray_epsilon && 
                                parser::is_same_dir_epsilon(lightRay.ray_direction, 
                                    f_intersection - cameraRay.intersection)) {
                                inShadow = true;
                                break;
                            }
                        }
                    }
                    for (const parser::Triangle & o: scene.triangles) {
                        if (inShadow){
                            break;
                        } 
                        if (lightRay.intersects(o, scene.vertex_data, f_intersection,
                                    f_distance, f_normal)) {
                            if (fabs(f_distance - lightDistance) > scene.shadow_ray_epsilon && 
                                parser::is_same_dir_epsilon(lightRay.ray_direction, 
                                    f_intersection - cameraRay.intersection)) {
                                inShadow = true;
                                break;
                            }
                        }
                    }
                    for (const parser::Sphere & o: scene.spheres) {
                        if (inShadow){
                            break;
                        } 
                        if (lightRay.intersects(o, scene.vertex_data, f_intersection,
                                    f_distance, f_normal)) {
                            if (fabs(f_distance - lightDistance) > scene.shadow_ray_epsilon && 
                                parser::is_same_dir_epsilon(lightRay.ray_direction, 
                                    f_intersection - cameraRay.intersection)) {
                                inShadow = true;
                                break;
                            }
                        }
                    }
                    if (inShadow){
                        // point is in shadow for this light source, 
                        //  no diffuse or specular component, continue
                        continue;
                    }
                    else{
                        // point is not in shadow from this light source, 
                        //  calculate diffuse and specular shading
                        float orientationAngle = 
                            fmax(0, parser::dot(lightRay.ray_direction, cameraRay.normal));
                        if (orientationAngle > 0){
                            cameraRay.RGB.x += l.intensity.x * orientationAngle * 
                                scene.materials[cameraRay.material_id-1].diffuse.x / 
                                pow(lightDistance, 2);
                            cameraRay.RGB.y += l.intensity.y * orientationAngle * 
                                scene.materials[cameraRay.material_id-1].diffuse.y / 
                                pow(lightDistance, 2);
                            cameraRay.RGB.z += l.intensity.z * orientationAngle * 
                                scene.materials[cameraRay.material_id-1].diffuse.z / 
                                pow(lightDistance, 2);
                        }
                        
                        std::cout << "color after diffuse shading: " << 
                            cameraRay.RGB.x << "," << cameraRay.RGB.y << 
                            "," << cameraRay.RGB.z << " / material id:" << 
                            cameraRay.material_id << " / orientation angle: " << 
                            orientationAngle << std::endl;
                    }
                }

                // TODO: calculate specular shading

                // SHADING ENDS HERE
                if (cameraRay.RGB.x > 255){
                    cameraRay.RGB.x = 255;
                }
                else if (cameraRay.RGB.x < 1e-2){
                    cameraRay.RGB.x = 0;
                } 
                if (cameraRay.RGB.y > 255){
                    cameraRay.RGB.y = 255;
                }
                else if (cameraRay.RGB.y < 1e-2){
                    cameraRay.RGB.y = 0;
                } 
                if (cameraRay.RGB.z > 255){
                    cameraRay.RGB.z = 255;
                }
                else if (cameraRay.RGB.z < 1e-2){
                    cameraRay.RGB.z = 0;
                } 
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
            image[imageIt++] = (int)(*cameraRayIt).RGB.x;
            image[imageIt++] = (int)(*cameraRayIt).RGB.y;
            image[imageIt++] = (int)(*cameraRayIt).RGB.z;
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

