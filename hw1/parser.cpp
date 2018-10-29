#include "parser.h"
#include "tinyxml2.h"
#include <limits>
#include <sstream>
#include <stdexcept>
#include <iostream>

parser::Ray::Ray(const Vec3f & ray_origin) : ray_origin(ray_origin)
{
}

parser::CameraRay::CameraRay(const Camera & camera, float pixeli, float pixelj)
    : Ray(camera.position), RGB(0, 0, 0)
{
    float l = camera.near_plane.x,
          t = camera.near_plane.w;

    // pixel coordinates
    float u = l + camera.rminusl * (pixeli + 0.5) / camera.image_width,
          v = t - camera.tminusb * (pixelj + 0.5) / camera.image_height;

    ray_direction = scale(camera.cross, u) + scale(camera.up, v) +
        scale(camera.gaze, camera.near_distance);
}

parser::LightRay::LightRay(const PointLight & point_light, const Vec3f & target_point)
    : Ray(target_point), intensity(point_light.intensity)
{
    // lightrays point from intersection point to the light source for easier cosine calculation
    Vec3f diff = point_light.position - ray_origin;

    // unit vector
    ray_direction = parser::scale(diff, 1 / parser::length(diff));
}

bool parser::Ray::intersects(const Face & face,
        const std::vector<Vec3f> & vertex_data, Vec3f & f_intersection,
        float & f_distance, parser::Vec3f & f_normal)
{
    const Vec3f & vertex_a = vertex_data[face.v0_id - 1];
    const Vec3f & vertex_b = vertex_data[face.v1_id - 1];
    const Vec3f & vertex_c = vertex_data[face.v2_id - 1];

    float a = vertex_a.x - vertex_b.x,
          b = vertex_a.y - vertex_b.y,
          c = vertex_a.z - vertex_b.z,
          d = vertex_a.x - vertex_c.x,
          e = vertex_a.y - vertex_c.y,
          f = vertex_a.z - vertex_c.z,
          g = ray_direction.x,
          h = ray_direction.y,
          i = ray_direction.z,
          j = vertex_a.x - ray_origin.x,
          k = vertex_a.y - ray_origin.y,
          l = vertex_a.z - ray_origin.z;

    float ei_hf = e*i - h*f,
          gf_di = g*f - d*i,
          dh_eg = d*h - e*g,
          ak_jb = a*k - j*b,
          jc_al = j*c - a*l,
          bl_kc = b*l - k*c;

    float M = a*ei_hf + b*gf_di + c*dh_eg;

    float beta = (j*ei_hf + k*gf_di + l*dh_eg) / M,
          gamma = (i*ak_jb + h*jc_al + g*bl_kc) / M,
          t = -(f*ak_jb + e*jc_al + d*bl_kc) / M;
   
    if (gamma < 0 || gamma > 1) {
        return 0;
    }
    if (beta < 0 || beta > (1 - gamma)) {
        return 0;
    }
    if (t < 0){
        return 0;
    }

    f_intersection = ray_origin + scale(ray_direction, t);
    f_distance = parser::distance(f_intersection, ray_origin);

    // A - B x A - C
    // B - A x C - A
    f_normal = cross_product(parser::Vec3f(a, b, c), parser::Vec3f(d, e, f));
    f_normal = scale(f_normal, 1 / parser::length(f_normal));
    // TODO f_normal = -scale(f_normal, parser::length(f_normal));
    // bu mu yoksa diğeri mi ya da burda check etmemiz mi lazım emin değilim

    return 1;
}

bool parser::Ray::intersects(const Triangle & triangle,
        const std::vector<Vec3f> & vertex_data, Vec3f & f_intersection,
        float & f_distance, parser::Vec3f & f_normal)
{
    return intersects(triangle.indices, vertex_data, f_intersection, f_distance,
            f_normal);
}

bool parser::Ray::intersects(const Mesh & mesh,
        const std::vector<Vec3f> & vertex_data, Vec3f & f_intersection,
        float & f_distance, parser::Vec3f & f_normal)
{
    bool flag = 0;

    float min_distance = std::numeric_limits<float>::max();
    parser::Vec3f min_dist_intersection;
    parser::Vec3f min_dist_normal;

    for (const Face & face: mesh.faces) {
        if (intersects(face, vertex_data, f_intersection, f_distance, f_normal)) {
            if (f_distance < min_distance) {
                min_distance = f_distance;
                min_dist_intersection = f_intersection;
                min_dist_normal = f_normal;
            
                flag = 1;
            }
        }
    }

    return flag;
}

bool parser::Ray::intersects(const Sphere & sphere,
        const std::vector<Vec3f> & vertex_data, Vec3f & f_intersection,
        float & f_distance, parser::Vec3f & f_normal)
{
    const Vec3f & sphere_center = vertex_data[sphere.center_vertex_id-1];

    // e - c (ray_origin - sphere_center)
    Vec3f B = ray_origin - sphere_center;
    float C = dot(ray_direction, ray_direction);

    float discriminant = powf(dot(B, ray_direction), 2) - C * (dot(B, B) - powf(sphere.radius, 2));

    if (is_equal_epsilon(discriminant, 0)) {
        float t = -dot(ray_direction, B) / C;

        if (t < 0){
            return 0;
        }

        f_intersection = ray_origin + scale(ray_direction, t);
    }
    else if (discriminant < 0) {
        return 0;
    }
    else /* if (discriminant > 0) */ {
        float A = -dot(ray_direction, B),
              S = sqrt(discriminant);

        float t1 = (A + S) / C,
              t2 = (A - S) / C;

        if (t1 < 0){
            return 0;
        }

        Vec3f intersection1 = ray_origin + scale(ray_direction, t1),
              intersection2 = ray_origin + scale(ray_direction, t2);

        if (distance(ray_origin, intersection1) < distance(ray_origin, intersection2)) {
            f_intersection = intersection1;
        }
        else {
            f_intersection = intersection2;
        }
    }

    f_distance = parser::distance(f_intersection, ray_origin);

    f_normal = f_intersection - sphere_center;
    f_normal = scale(f_normal, 1 / parser::length(f_normal));
    
    return 1;
}

void parser::CameraRay::register_intersection(const parser::Vec3f & r_intersection,
        float r_intersection_distance, int r_material_id, const parser::Vec3f & r_normal)
{
    intersection = r_intersection;
    intersection_distance = r_intersection_distance;
    material_id = r_material_id;
    normal = r_normal;
}

/*
bool parser::LightRay::intersects(const Vec3f & point)
{
    Vec3f diff = point - ray_origin;

    float ratio_x = diff.x / point.x,
          ratio_y = diff.y / point.y,
          ratio_z = diff.z / point.z;

    return is_equal_epsilon(ratio_x, ratio_y) && is_equal_epsilon(ratio_y, ratio_z) &&
        is_equal_epsilon(ratio_x, ratio_z);
}
*/
parser::Vec3f parser::LightRay::intensity_at(const Vec3f & point)
{
    float d = distance(ray_origin, point);

    return scale(intensity, 1 / powf(d, 2));
}

parser::Vec3f::Vec3f(float x, float y, float z) : x(x), y(y), z(z)
{
}

void parser::Scene::loadFromXml(const std::string& filepath)
{
    tinyxml2::XMLDocument file;
    std::stringstream stream;

    auto res = file.LoadFile(filepath.c_str());
    if (res)
    {
        throw std::runtime_error("Error: The xml file cannot be loaded.");
    }

    auto root = file.FirstChild();
    if (!root)
    {
        throw std::runtime_error("Error: Root is not found.");
    }

    //Get BackgroundColor
    auto element = root->FirstChildElement("BackgroundColor");
    if (element)
    {
        stream << element->GetText() << std::endl;
    }
    else
    {
        stream << "0 0 0" << std::endl;
    }
    stream >> background_color.x >> background_color.y >> background_color.z;

    //Get ShadowRayEpsilon
    element = root->FirstChildElement("ShadowRayEpsilon");
    if (element)
    {
        stream << element->GetText() << std::endl;
    }
    else
    {
        stream << "0.001" << std::endl;
    }
    stream >> shadow_ray_epsilon;

    //Get MaxRecursionDepth
    element = root->FirstChildElement("MaxRecursionDepth");
    if (element)
    {
        stream << element->GetText() << std::endl;
    }
    else
    {
        stream << "0" << std::endl;
    }
    stream >> max_recursion_depth;

    //Get Cameras
    element = root->FirstChildElement("Cameras");
    element = element->FirstChildElement("Camera");
    Camera camera;
    while (element)
    {
        auto child = element->FirstChildElement("Position");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("Gaze");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("Up");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("NearPlane");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("NearDistance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("ImageResolution");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("ImageName");
        stream << child->GetText() << std::endl;

        stream >> camera.position.x >> camera.position.y >> camera.position.z;
        stream >> camera.gaze.x >> camera.gaze.y >> camera.gaze.z;
        stream >> camera.up.x >> camera.up.y >> camera.up.z;
        stream >> camera.near_plane.x >> camera.near_plane.y >> camera.near_plane.z >> camera.near_plane.w;
        stream >> camera.near_distance;
        stream >> camera.image_width >> camera.image_height;
        stream >> camera.image_name;

        // Additional initialization steps for camera
        
        camera.cross = parser::cross_product(camera.up, parser::scale(camera.gaze, -1)) ;

        camera.rminusl = camera.near_plane.y - camera.near_plane.x;
        camera.tminusb = camera.near_plane.w - camera.near_plane.z;

        cameras.push_back(camera);
        element = element->NextSiblingElement("Camera");
    }

    //Get Lights
    element = root->FirstChildElement("Lights");
    auto child = element->FirstChildElement("AmbientLight");
    stream << child->GetText() << std::endl;
    stream >> ambient_light.x >> ambient_light.y >> ambient_light.z;
    element = element->FirstChildElement("PointLight");
    PointLight point_light;
    while (element)
    {
        child = element->FirstChildElement("Position");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("Intensity");
        stream << child->GetText() << std::endl;

        stream >> point_light.position.x >> point_light.position.y >> point_light.position.z;
        stream >> point_light.intensity.x >> point_light.intensity.y >> point_light.intensity.z;

        point_lights.push_back(point_light);
        element = element->NextSiblingElement("PointLight");
    }

    //Get Materials
    element = root->FirstChildElement("Materials");
    element = element->FirstChildElement("Material");
    Material material;
    while (element)
    {
        child = element->FirstChildElement("AmbientReflectance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("DiffuseReflectance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("SpecularReflectance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("MirrorReflectance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("PhongExponent");
        stream << child->GetText() << std::endl;

        stream >> material.ambient.x >> material.ambient.y >> material.ambient.z;
        stream >> material.diffuse.x >> material.diffuse.y >> material.diffuse.z;
        stream >> material.specular.x >> material.specular.y >> material.specular.z;
        stream >> material.mirror.x >> material.mirror.y >> material.mirror.z;
        stream >> material.phong_exponent;

        materials.push_back(material);
        element = element->NextSiblingElement("Material");
    }

    //Get VertexData
    element = root->FirstChildElement("VertexData");
    stream << element->GetText() << std::endl;
    Vec3f vertex;
    while (!(stream >> vertex.x).eof())
    {
        stream >> vertex.y >> vertex.z;
        vertex_data.push_back(vertex);
    }
    stream.clear();

    //Get Meshes
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Mesh");
    Mesh mesh;
    while (element)
    {
        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;
        stream >> mesh.material_id;

        child = element->FirstChildElement("Faces");
        stream << child->GetText() << std::endl;
        Face face;
        while (!(stream >> face.v0_id).eof())
        {
            stream >> face.v1_id >> face.v2_id;
            mesh.faces.push_back(face);
        }
        stream.clear();

        meshes.push_back(mesh);
        mesh.faces.clear();
        element = element->NextSiblingElement("Mesh");
    }
    stream.clear();

    //Get Triangles
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Triangle");
    Triangle triangle;
    while (element)
    {
        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;
        stream >> triangle.material_id;

        child = element->FirstChildElement("Indices");
        stream << child->GetText() << std::endl;
        stream >> triangle.indices.v0_id >> triangle.indices.v1_id >> triangle.indices.v2_id;

        triangles.push_back(triangle);
        element = element->NextSiblingElement("Triangle");
    }

    //Get Spheres
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Sphere");
    Sphere sphere;
    while (element)
    {
        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;
        stream >> sphere.material_id;

        child = element->FirstChildElement("Center");
        stream << child->GetText() << std::endl;
        stream >> sphere.center_vertex_id;

        child = element->FirstChildElement("Radius");
        stream << child->GetText() << std::endl;
        stream >> sphere.radius;

        spheres.push_back(sphere);
        element = element->NextSiblingElement("Sphere");
    }
}
