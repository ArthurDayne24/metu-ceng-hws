#ifndef __HW1__PARSER__
#define __HW1__PARSER__

#include <string>
#include <vector>
#include <cmath>

namespace parser
{
    typedef struct Vec3f Vec3f;

    // TODO to be determined later
    // Used to achieve precision loss due to floating points
    const float EqualityEpsilon = 1e-5;

    //Notice that all the structures are as simple as possible
    //so that you are not enforced to adopt any style or design.
    typedef struct Vec3f
    {
        float x, y, z;

        Vec3f(float x=0, float y=0, float z=0); 

        // Sorry but I could not move them to .cpp xd
        friend Vec3f operator+(const Vec3f & vec1,const Vec3f & vec2) {
            return Vec3f(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z);
        }

        friend Vec3f operator-(const Vec3f & vec1,const Vec3f & vec2) {
            return Vec3f(vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z);
        }

    } Vec3f;

    //inline float length(const Vec3f & vec);
    inline float length(const Vec3f & vec)
    {
        return sqrt(powf(vec.x, 2) + powf(vec.y, 2) + powf(vec.z, 2));
    }
    //inline float distance(const Vec3f & vec1,const Vec3f & vec2);
    inline float distance(const Vec3f & vec1,const Vec3f & vec2)
    {
        return length(vec1 - vec2);
    }
    inline float dot(const parser::Vec3f & vec1,const parser::Vec3f & vec2)
    {
        return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
    }
    inline Vec3f scale(const parser::Vec3f & vec, float k)
    {
        return Vec3f(vec.x * k, vec.y * k, vec.z * k);
    }
    inline Vec3f cross_product(const Vec3f & vec1, const Vec3f & vec2)
    {
        return Vec3f(vec1.y * vec2.z - vec2.y * vec1.z,
                vec2.x * vec1.z - vec1.x * vec2.z,
                vec1.x * vec2.y - vec2.x * vec1.y);
    }
    inline bool is_equal_epsilon(float a, float b)
    {
        if (a < b + parser::EqualityEpsilon && a > b - parser::EqualityEpsilon) {
            return 1;
        }
        return 0;
    }
    inline bool is_equal_epsilon(const Vec3f & vec1, const Vec3f & vec2)
    {
        return is_equal_epsilon(vec1.x, vec2.x) && is_equal_epsilon(vec1.y, vec2.y) &&
            is_equal_epsilon(vec1.z, vec2.z);
    }

    inline bool is_same_dir_epsilon(const Vec3f & vec1, const Vec3f & vec2)
    {
        return is_equal_epsilon(vec1.x / vec2.x, vec1.y / vec2.y) &&
            is_equal_epsilon(vec1.x / vec2.x, vec1.z / vec2.z) &&
            is_equal_epsilon(vec1.z / vec2.z, vec1.y / vec2.y);
    }

    typedef struct Vec3i
    {
        int x, y, z;
    } Vec3i;

    typedef struct Vec4f
    {
        float x, y, z, w;
    } Vec4f;

    typedef struct Camera
    {
        Vec3f position;
        Vec3f gaze; // -w
        Vec3f cross; // u
        Vec3f up; // v
        Vec4f near_plane; // left, right, bottom, top
        float near_distance;
        int image_width, image_height;
        std::string image_name;
        // for reuse at each ray, optimization :)
        float rminusl;
        float tminusb;

    } Camera;

    typedef struct PointLight
    {
        Vec3f position;
        Vec3f intensity;     
    } PointLight;

    typedef struct Material
    {
        Vec3f ambient;
        Vec3f diffuse;
        Vec3f specular;
        Vec3f mirror;
        float phong_exponent;
    } Material;

    typedef struct Face
    {
        int v0_id;
        int v1_id;
        int v2_id;
    } Face;

    typedef struct Mesh
    {
        int material_id;
        std::vector<Face> faces;
    } Mesh;

    typedef struct Triangle
    {
        int material_id;
        Face indices;
    } Triangle;

    typedef struct Sphere
    {
        int material_id;
        int center_vertex_id;
        float radius;
    } Sphere;

    typedef struct Ray
    {
        Vec3f ray_origin;
        Vec3f ray_direction;

        Ray(const Vec3f & ray_origin);

        // These are set in main/render_image by register_intersection!
        Vec3f intersection;
        float intersection_distance;
        int material_id;
        Vec3f normal; // unit vector

        // Object-wise intersection
        // These functions also fill "f_intersection" with intersecting point,
        //  f_distance and f_normal and return True if intersection exists,
        //  else just return False

        // For sphere
        bool intersects(const Sphere & sphere, const std::vector<Vec3f> & vertex_data,
                Vec3f & f_intersection, float & f_distance, Vec3f & f_normal);
        // For triangle
        bool intersects(const Triangle & triangle, const std::vector<Vec3f> & vertex_data,
                Vec3f & f_intersection, float & f_distance, Vec3f & f_normal);
        // For mesh
        bool intersects(const Mesh & mesh, const std::vector<Vec3f> & vertex_data,
                Vec3f & f_intersection, float & f_distance, Vec3f & f_normal);

        private:
        // For face
        bool intersects(const Face & face, const std::vector<Vec3f> & vertex_data,
                Vec3f & f_intersection, float & f_distance, Vec3f & f_normal);
    } Ray;

    // For each pixel, there exist a CameraRay
    // It stores intersection point and """"RGB value represented by that pixel""""
    // So calculations for each pixel should be stored in CameraRay objects' "RGB vector"
    //  if CameraRay intersects an object, else intersection_exists is set to 0 RGB is
    //  set to background_color
    // It also stores normal vector and material_id of intersection point for further use
    typedef struct CameraRay: public Ray
    {
        CameraRay(const Camera & camera, float pixeli, float pixelj);

        Vec3f RGB;
        bool intersection_exists = 0;

        // After intersecting object is found, this function should be called
        void register_intersection(const Vec3f & r_intersection,
                float r_intersection_distance, int material_id, const Vec3f & r_normal);
    } CameraRay;

    typedef struct LightRay: public Ray
    {
        Vec3f intensity;
    
        LightRay(const PointLight & point_light, const Vec3f & target_point);

        // Point wise intersection up to an error of EqualityEpsilon
        //bool intersects(const Vec3f & point);

        Vec3f intensity_at(const Vec3f & point);
    } LightRay;

    typedef struct Scene
    {
        //Data
        Vec3i background_color;
        float shadow_ray_epsilon;
        int max_recursion_depth;
        std::vector<Camera> cameras;
        Vec3f ambient_light;
        std::vector<PointLight> point_lights;
        std::vector<Material> materials;
        std::vector<Vec3f> vertex_data;
        std::vector<Mesh> meshes;
        std::vector<Triangle> triangles;
        std::vector<Sphere> spheres;

        //Functions
        void loadFromXml(const std::string& filepath);
    } Scene;
}

#endif

