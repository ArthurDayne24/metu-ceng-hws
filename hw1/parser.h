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
            return parser::Vec3f(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z);
        }

        friend Vec3f operator-(const Vec3f & vec1,const Vec3f & vec2) {
            return parser::Vec3f(vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z);
        }

    } Vec3f;

    inline float distance(const Vec3f & vec1,const Vec3f & vec2);
    inline float norm(const Vec3f & vec);
    inline float dot(const Vec3f & vec1,const Vec3f & vec2);
    inline Vec3f scale(const Vec3f & vec, float k);

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
        Vec3f origin;
        Vec3f ray_direction;

        Ray(const Vec3f & origin);

        // These two functions also fill "intersection" with intersecting point and 
        // return True if intersection exists, else just return False

        // Object-wise intersection functions to find intersection point if exists
        // Can be used both for Triangle and Mesh objects
        bool intersects(const Face & face, const std::vector<Vec3f> & vertex_data,
                Vec3f & intersection);
        // For sphere
        bool intersects(const Sphere & sphere, const std::vector<Vec3f> & vertex_data,
                Vec3f & intersection);
    } Ray;

    typedef struct CameraRay: public Ray
    {
        CameraRay(const Camera & camera, float pixeli, float pixelj);
    } CameraRay;

    // TODO to be determined later
    // Used to achieve precision loss due to floating points
    const float EqualityEpsilon = 1e-6;

    typedef struct LightRay: public Ray
    {
        Vec3f intensity;
    
        LightRay(const PointLight & point_ligt, const Vec3f & target_point);

        // Point wise intersection
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

