#version 330 core
out vec4 FragColor;
in vec2 texcoord;

#define DBL_MAX 3.402823466e+38F 
struct Camera
{
    float scale;
    vec3 position;
    vec3 front;
    vec3 right;
    vec3 up;
    int width;
    int height;
};
uniform Camera camera;
uniform samplerBuffer VertexSampler0;
struct Polygon
{
    vec3 v1;
    vec3 v2;
    vec3 v3;
    vec3 normal;
};
vec3 intersectPoint(vec3 rayVector, vec3 rayPoint, vec3 planeNormal, vec3 planePoint)
{
    vec3 diff = rayPoint - planePoint;
    float prod1 = dot(diff, planeNormal);
    float prod2 = dot(rayVector, planeNormal);
    float ratio = prod1 / prod2;

    return rayPoint - rayVector * ratio;
}
bool pointInPolygon(vec3 v, Polygon p)
{
    float area = length(cross((p.v1 - p.v2), (p.v3 - p.v2))) * 0.5f;

    float a = length(cross((v - p.v2), (v - p.v3))) / (2 * area);
    float b = length(cross((v - p.v3), (v - p.v1))) / (2 * area);
    float c = length(cross((v - p.v2), (v - p.v1))) / (2 * area);

    float tresh = 1.0f + 0.0001f;
    return a <= tresh && a >= 0 && b <= tresh && b >= 0 && c <= tresh && c >= 0 && (a + b + c) <= tresh;
}
bool pointInPolygon(vec3 v, vec3 v1, vec3 v2, vec3 v3)
{
    float area = length(cross((v1 - v2), (v3 - v2))) * 0.5f;

    float a = length(cross((v - v2), (v - v3))) / (2 * area);
    float b = length(cross((v - v3), (v - v1))) / (2 * area);
    float c = length(cross((v - v2), (v - v1))) / (2 * area);

    float tresh = 1.0f + 0.0001f;
    return a <= tresh && a >= 0 && b <= tresh && b >= 0 && c <= tresh && c >= 0 && (a + b + c) <= tresh;
}
float cube[] = float[](
    -1.0f,-1.0f,-1.0f, // triangle 1 : begin
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 0.0f, 0.0f,       // triangle 1 : end
    1.0f, 1.0f,-1.0f, // triangle 2 : begin
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    0.0f, 0.0f, -1.0f, // triangle 2 : end
    1.0f,-1.0f, 1.0f,//3
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    0.0f, -1.0f, 0.0f,
    1.0f, 1.0f,-1.0f,//4
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
     0.0f, 0.0f, -1.0f,
    -1.0f,-1.0f,-1.0f,//5
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 0.0f, 0.0f,
    1.0f,-1.0f, 1.0f,//6
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    0.0f, -1.0f, 0.0f,
    -1.0f, 1.0f, 1.0f,//7
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f,//8
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f, 0.0f, 0.0f,
    1.0f,-1.0f,-1.0f,//9
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 1.0f,//10
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f,//11
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f,//12
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    0.0f, 0.0f, 1.0f
);
struct Hit 
{ vec3 pos; vec3 normal; };
struct Ray 
{
 vec3 orig; 
 vec3 dir;
};
bool intersectsCube(vec3 ray, vec3 shift)
{
    for(int i=0; i < 12; i++)
    {
        int off = i*12;
        vec3 src = camera.position - shift;
        vec3 contact = intersectPoint(ray, src, vec3(cube[9+off], cube[10+off], cube[11+off]), vec3(cube[0+off], cube[1+off], cube[2+off]));
        if(pointInPolygon(contact, vec3(cube[0+off], cube[1+off], cube[2+off]),vec3(cube[3+off], cube[4+off], cube[5+off]), vec3(cube[6+off], cube[7+off], cube[8+off])) && dot(contact-src, ray) > 0)
            return true;
    }

    return false;
}
bool intersect(Ray r, vec3 min, vec3 max) 
{ 
    float tmin = (min.x - r.orig.x) / r.dir.x; 
    float tmax = (max.x - r.orig.x) / r.dir.x; 
 
    if (tmin > tmax) 
    {
        float tmp = tmin;
        tmin = tmax;
        tmax = tmp;
    } 
 
    float tymin = (min.y - r.orig.y) / r.dir.y; 
    float tymax = (max.y - r.orig.y) / r.dir.y; 
 
    if (tymin > tymax) 
    {
        float tmp = tymin;
        tymin = tymax;
        tymax = tmp;
    }
 
    if ((tmin > tymax) || (tymin > tmax)) 
        return false; 
 
    if (tymin > tmin) 
        tmin = tymin; 
 
    if (tymax < tmax) 
        tmax = tymax; 
 
    float tzmin = (min.z - r.orig.z) / r.dir.z; 
    float tzmax = (max.z - r.orig.z) / r.dir.z; 
 
    if (tzmin > tzmax) 
    {
        float tmp = tzmin;
        tzmin = tzmax;
        tzmax = tmp;
    }
 
    if ((tmin > tzmax) || (tzmin > tmax)) 
        return false; 
 
    if (tzmin > tmin) 
        tmin = tzmin; 
 
    if (tzmax < tmax) 
        tmax = tzmax; 
 
    return true; 
} 
vec3 lightDir = normalize(vec3(-0.2f,-0.8f,-0.2f));

float dist2(vec3 v1, vec3 v2)
{
    return dot(v1-v2, v1-v2);
}
Hit intersectsCubePoint(vec3 ray, vec3 shift)
{
    Hit hit = Hit(vec3(-199, -199, -199), vec3(cube[9], cube[10], cube[11]));
    for(int i=0; i < 12; i++)
    {
        int off = i*12;
        vec3 src = camera.position - shift;
        vec3 contact = intersectPoint(ray, src, vec3(cube[9+off], cube[10+off], cube[11+off]), vec3(cube[0+off], cube[1+off], cube[2+off]));
        if(pointInPolygon(contact, vec3(cube[0+off], cube[1+off], cube[2+off]),vec3(cube[3+off], cube[4+off], cube[5+off]), vec3(cube[6+off], cube[7+off], cube[8+off])) 
            && dot(contact-src, ray) > 0 && dist2(contact, src)<dist2(src, hit.pos))
            hit = Hit(contact, vec3(cube[9+off], cube[10+off], cube[11+off]));
    }

    return hit;
}
uniform int boxes;
float _bin_size = 1;
int demon = 10;

int getId(vec3 v, int dim)
{
    return  int(v.z) * dim * dim + int(v.y) * dim + int(v.x);
}
vec4 voxels(int id)
{
    return texelFetch(VertexSampler0, id);
}
vec3 voxel_traversal_closest(vec3 ray_start, vec3 ray_end)
    {
        int id = getId(ray_start, demon);
        // This id of the first/current voxel hit by the ray.
        // Using floor (round down) is actually very important,
        // the implicit int-casting will round up for negative numbers.
        vec3 current_voxel = vec3(floor(ray_start[0] / _bin_size),
            floor(ray_start[1] / _bin_size),
            floor(ray_start[2] / _bin_size));

        // The id of the last voxel hit by the ray.
        // TODO: what happens if the end point is on a border?
        vec3 last_voxel= vec3(floor(ray_end[0] / _bin_size),
            floor(ray_end[1] / _bin_size),
            floor(ray_end[2] / _bin_size));

        // Compute normalized ray direction.
        vec3 ray = ray_end - ray_start;
        //ray.normalize();

        // In which direction the voxel ids are incremented.
        float stepX = (ray[0] >= 0) ? 1 : -1; // correct
        float stepY = (ray[1] >= 0) ? 1 : -1; // correct
        float stepZ = (ray[2] >= 0) ? 1 : -1; // correct

        // Distance along the ray to the next voxel border from the current position (tMaxX, tMaxY, tMaxZ).
        float next_voxel_boundary_x = (current_voxel[0] + stepX) * _bin_size; // correct
        float next_voxel_boundary_y = (current_voxel[1] + stepY) * _bin_size; // correct
        float next_voxel_boundary_z = (current_voxel[2] + stepZ) * _bin_size; // correct

        // tMaxX, tMaxY, tMaxZ -- distance until next intersection with voxel-border
        // the value of t at which the ray crosses the first vertical voxel boundary
        float tMaxX = (ray[0] != 0) ? (next_voxel_boundary_x - ray_start[0]) / ray[0] : DBL_MAX; //
        float tMaxY = (ray[1] != 0) ? (next_voxel_boundary_y - ray_start[1]) / ray[1] : DBL_MAX; //
        float tMaxZ = (ray[2] != 0) ? (next_voxel_boundary_z - ray_start[2]) / ray[2] : DBL_MAX; //

        // tDeltaX, tDeltaY, tDeltaZ --
        // how far along the ray we must move for the horizontal component to equal the width of a voxel
        // the direction in which we traverse the grid
        // can only be FLT_MAX if we never go in that direction
        float tDeltaX = (ray[0] != 0) ? _bin_size / ray[0] * stepX : DBL_MAX;
        float tDeltaY = (ray[1] != 0) ? _bin_size / ray[1] * stepY : DBL_MAX;
        float tDeltaZ = (ray[2] != 0) ? _bin_size / ray[2] * stepZ : DBL_MAX;

        vec3 diff = vec3(0, 0, 0);
        bool neg_ray = false;
        if (current_voxel[0] != last_voxel[0] && ray[0] < 0) { diff[0]--; neg_ray = true; }
        if (current_voxel[1] != last_voxel[1] && ray[1] < 0) { diff[1]--; neg_ray = true; }
        if (current_voxel[2] != last_voxel[2] && ray[2] < 0) { diff[2]--; neg_ray = true; }
        if(id != getId(current_voxel, demon) && voxels(getId(current_voxel, demon)).w != 0)
            return current_voxel;
        if (neg_ray) {
            current_voxel += diff;
            if (id != getId(current_voxel, demon) && voxels(getId(current_voxel, demon)).w != 0)
                return current_voxel;
        }

        while (last_voxel != current_voxel) {
            if (tMaxX < tMaxY) {
                if (tMaxX < tMaxZ) {
                    current_voxel[0] += stepX;
                    tMaxX += tDeltaX;
                }
                else {
                    current_voxel[2] += stepZ;
                    tMaxZ += tDeltaZ;
                }
            }
            else {
                if (tMaxY < tMaxZ) {
                    current_voxel[1] += stepY;
                    tMaxY += tDeltaY;
                }
                else {
                    current_voxel[2] += stepZ;
                    tMaxZ += tDeltaZ;
                }
            }
            if (id != getId(current_voxel, demon) && voxels(getId(current_voxel, demon)).w != 0)
                return current_voxel;
        }
        return vec3(-1,-1,-1);
    }

vec3 intersectsTestScene(vec3 ray)
{
    for(int i=0;i<boxes; ++i)
    {
        vec3 shift = vec3(2*i,0,0);//texelFetch(VertexSampler0, i).xyz;
        //if(texelFetch(VertexSampler0, i).w == 0)
        //    continue;
        vec3 src = camera.position - shift;
        Ray r = Ray(src, ray);
        if(intersect(r, vec3(-1,-1,-1), vec3(1,1,1)))
        {            
            Hit hit = intersectsCubePoint(ray, shift);

            
            float slope = abs(dot(hit.normal, normalize(ray)));
            return vec3(slope, slope, slope);
        }
    }
    return vec3(0,0,0);
}
vec3 traverse(vec3 ray)
{
    vec3 res = voxel_traversal_closest(camera.position, camera.position+ray*10);
    if(res.x <= -1)
        return vec3(0,0,0);
    else
    {
        return vec3(1, 1, 1);
    }
}
void main() 
{
    vec3 ray = camera.front + camera.right * float(texcoord.x*camera.width - camera.width / 2) * camera.scale + camera.up * float(texcoord.y*camera.height - camera.height / 2) * camera.scale;

    
    FragColor = vec4(traverse(ray), 1.0f);
   
}
