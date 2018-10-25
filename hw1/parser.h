#ifndef __HW1__PARSER__
#define __HW1__PARSER__

#include <string>
#include <vector>

namespace parser
{
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

    inline float distance(const Vec3f & vec1,const Vec3f & vec2);
    inline float norm(const Vec3f & vec);
    inline float dot(const Vec3f & vec1,const Vec3f & vec2);
    inline Vec3f scale(const Vec3f & vec, float k);
    inline Vec3f cross_product(const Vec3f & vec1, const Vec3f & vec2);

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
    } Ray;

    // For each pixel, there exist a CameraRay
    // It stores intersection point and """"RGB value represented by that pixel""""
    // So calculations for each pixel will be stored in CameraRay objects' "RGB vector"
    // CameraRay intersects an object, else intersection_exists is set to 0
    typedef struct CameraRay: public Ray
    {
        CameraRay(const Camera & camera, float pixeli, float pixelj);

        Vec3f RGB;
        bool intersection_exists = 0;
        // These are set in main/render_image by compairing all objects !
        Vec3f intersection;
        Vec3f intersection_distance;

        // Object-wise intersection
        // These functions also fill "f_intersection" with intersecting point and
        // and f_distance and return True if intersection exists, else just return False

        // For sphere
        bool intersects(const Sphere & sphere, const std::vector<Vec3f> & vertex_data,
                Vec3f & f_intersection, float & f_distance);
        // For triangle
        bool intersects(const Triangle & triangle, const std::vector<Vec3f> & vertex_data,
                Vec3f & f_intersection, float & f_distance);
        // For mesh
        bool intersects(const Mesh & mesh, const std::vector<Vec3f> & vertex_data,
                Vec3f & f_intersection, float & f_distance);

        private:
        // For face
        bool intersects(const Face & face, const std::vector<Vec3f> & vertex_data,
                Vec3f & f_intersection, float & f_distance);

    } CameraRay;

    // TODO to be determined later
    // Used to achieve precision loss due to floating points
    const float EqualityEpsilon = 1e-4;

    typedef struct LightRay: public Ray
    {
        Vec3f intensity;
    
        LightRay(const PointLight & point_ligt, const Vec3f & target_point);

        // Point wise intersection up to an error of EqualityEpsilon
        bool intersects(const Vec3f & point);

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

