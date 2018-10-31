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

parser::Vec3f apply_effects(int rem_depth, parser::ViewingRay viewingRay, 
        const parser::Scene & scene)
{
    if (rem_depth < 0) {
        return parser::Vec3f(0, 0, 0);
    }

    parser::Vec3f accumulation(0, 0, 0);

    float min_distance = std::numeric_limits<float>::max(), f_distance;
    parser::Vec3f min_dist_intersection, f_intersection;
    int min_dist_mat_id;
    parser::Vec3f min_dist_normal, f_normal;

    for (const parser::Mesh & o: scene.meshes) {
        if (viewingRay.intersects(o, scene.vertex_data, f_intersection,
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
        if (viewingRay.intersects(o, scene.vertex_data, f_intersection,
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
        if (viewingRay.intersects(o, scene.vertex_data, f_intersection,
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

        viewingRay.register_intersection(min_dist_intersection, min_distance,
                min_dist_mat_id, min_dist_normal);
        
        // FROM THIS POINT ON, WE HAVE TO HANDLE THE INTERSECTION
        
        const parser::Material & material = scene.materials[viewingRay.material_id-1];
        
        // add ambient shading values 
        accumulation += parser::element_mult(scene.ambient_light, material.ambient);
        
        for (const parser::PointLight & pointLight : scene.point_lights) {
            bool inShadow = false;
            
            // create a lightray from each point light source to the intersection point
            parser::LightRay lightRay = parser::LightRay(pointLight, viewingRay.intersection);
            
            // found distance must be less than this distance for shadow occurrence
            float lightDistance = parser::distance(pointLight.position, viewingRay.intersection); 
            //f_distance = std::numeric_limits<float>::max();                  
            for (const parser::Mesh & o: scene.meshes) {
                if (inShadow){
                    break;
                } 
                if (lightRay.intersects(o, scene.vertex_data, f_intersection,
                            f_distance, f_normal)) {
                    if (fabs(f_distance - lightDistance) > scene.shadow_ray_epsilon &&
                        f_distance < lightDistance &&
                        f_distance > scene.shadow_ray_epsilon) {
                        parser::Vec3f i2i = parser::scale(viewingRay.intersection - f_intersection , 
                                                1 / parser::length(viewingRay.intersection - f_intersection));
                        parser::Vec3f i2l = parser::scale(viewingRay.ray_origin - viewingRay.intersection , 
                                                1 / parser::length(viewingRay.ray_origin - viewingRay.intersection));
                        if (parser::dot(i2i, i2l) < 0){
                            inShadow = true;
                            break;
                        }
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
                        f_distance < lightDistance &&
                        f_distance > scene.shadow_ray_epsilon) {
                        parser::Vec3f i2i = parser::scale(viewingRay.intersection - f_intersection , 
                                                1 / parser::length(viewingRay.intersection - f_intersection));
                        parser::Vec3f i2l = parser::scale(viewingRay.ray_origin - viewingRay.intersection , 
                                                1 / parser::length(viewingRay.ray_origin - viewingRay.intersection));
                        if (parser::dot(i2i, i2l) < 0){
                            inShadow = true;
                            break;
                        }
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
                        f_distance < lightDistance &&
                        f_distance > scene.shadow_ray_epsilon) {
                        parser::Vec3f i2i = parser::scale(viewingRay.intersection - f_intersection , 
                                                1 / parser::length(viewingRay.intersection - f_intersection));
                        parser::Vec3f i2l = parser::scale(viewingRay.ray_origin - viewingRay.intersection , 
                                                1 / parser::length(viewingRay.ray_origin - viewingRay.intersection));
                        if (parser::dot(i2i, i2l) < 0){
                            inShadow = true;
                            break;
                        }
                    }
                }
            }
            if (inShadow){
                // point is in shadow for this light source, 
                //  no diffuse or specular component, continue
                //std::cout << "IN SHADOW" << std::endl;
                continue;
            }
            
            // else

            parser::Vec3f l = lightRay.ray_direction,
                          n = viewingRay.normal;

            // point is not in shadow from this light source, calculate diffuse shading
            float orientationAngle = 
                fmax(0, parser::dot(lightRay.ray_direction, viewingRay.normal));
            if (orientationAngle > 0){
                accumulation += parser::element_mult(parser::scale(lightRay.intensity, 
                            orientationAngle / powf(lightDistance, 2)), 
                        material.diffuse);
            }
            
            // calculate specular shading

            // viewing ray is from eye to object (initially from camera to intersection)
            // hence reverse viewingRay and convert to unit vector here
            parser::Vec3f v = parser::scale(viewingRay.ray_direction, -1 / parser::length(viewingRay.ray_direction));

            parser::Vec3f half = v + l;
            half = parser::scale(half, 1 / parser::length(half));

            orientationAngle = fmax(0, parser::dot(n, half));
            if (orientationAngle > 0){
                accumulation += parser::scale(parser::element_mult(material.specular, 
                    lightRay.intensity), powf(fmax(0, parser::dot(n, half)), 
                        material.phong_exponent) / powf(lightDistance, 2));
            }
            
            
            // WARN: no epsilon wise equality check
            if (!parser::is_zero(material.mirror)) {
                
                // do some recursion for specular in this function
                parser::Vec3f d = parser::scale(v, -1);
                parser::Vec3f r = d - parser::scale(n, parser::dot(d, n) * 2);

                parser::ViewingRay bouncingRay(viewingRay.intersection + 
                        parser::scale(r, scene.shadow_ray_epsilon), r);

                accumulation += 
                    parser::element_mult(apply_effects(rem_depth-1, bouncingRay, scene), 
                            material.mirror);
            }

            // round float values to integer 
            accumulation.x = round(accumulation.x);
            accumulation.y = round(accumulation.y);
            accumulation.z = round(accumulation.z);
            
            // SHADING ENDS HERE
            if (accumulation.x > 255){
                accumulation.x = 255;
            }
            else if (accumulation.x < 1){
                accumulation.x = 0;
            }
            if (accumulation.y > 255){
                accumulation.y = 255;
            }
            else if (accumulation.y < 1){
                accumulation.y = 0;
            }
            if (accumulation.z > 255){
                accumulation.z = 255;
            }
            else if (accumulation.z < 1){
                accumulation.z = 0;
            }
        }
    }

    else {
        accumulation.x = scene.background_color.x;
        accumulation.y = scene.background_color.y;
        accumulation.z = scene.background_color.z;
    }

    return accumulation;
}

// Our main function for homework
void render_image(const parser::Camera & camera, const parser::Scene & scene)
{
    int width = camera.image_width, height = camera.image_height;
    unsigned char image[height * width * 3];

    // ***********************************************************************
    // Calculate intersection points and "RGB" values for corresponding pixel
    // ***********************************************************************
    
    int imageIt = 0;

    // Calculate "parser::CameraRay"s for each pixel
    for (int y = 0; y < height; ++y) {

        for (int x = 0; x < width; ++x) {
                
            parser::CameraRay cameraRay(camera, x, y);

            // initially viewingRay (pure ray) is cameraRay
            parser::ViewingRay viewingRay(cameraRay.ray_origin, cameraRay.ray_direction);

            parser::Vec3f result = apply_effects(scene.max_recursion_depth, viewingRay, scene);

            // Last step, write pixels from cameraRays to image
            
            image[imageIt++] = (int) (result.x);
            image[imageIt++] = (int) (result.y);
            image[imageIt++] = (int) (result.z);
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

