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
int demon = 500;

int getId(vec3 v, int dim)
{
    return  int(v.z) * dim * dim + int(v.y) * dim + int(v.x);
}
vec4 voxels(int id)
{
    return texelFetch(VertexSampler0, id);
}

vec3 getNext(vec3 origin, vec3 dir, float step)
{
    vec3 ray = normalize(dir)*0.5f * step;
    return origin + ray;
}
vec3 voxel_traversal_closest(vec3 origin, vec3 dir, int steps)
{
    vec3 ray = origin;
    dir = normalize(dir);

    float step = 0.03f;
    int lastId=-1;
    for (int i=0;i<steps;i++)
    {
        ray += dir * step;
        vec3 shift = vec3(round(ray.x+0.5f),round(ray.y+0.5f),round(ray.z));

        if(shift.x>demon || shift.x<0 ||shift.y>demon || shift.y<0 ||shift.z>demon || shift.z<0)
            vec3(-1,-1,-1);
        if(i>200)
            step +=0.001f;
        if(voxels(getId(shift, demon)).w!=0 && intersect(Ray(origin-shift, dir), vec3(-1,-1,-1), vec3(1, 1, 1)))
        {
            return ray;
        }
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
    vec3 shift = vec3(10,10,10);
    vec3 res = voxel_traversal_closest(camera.position, ray, 1000);
    if(res.x!=-1)
        return vec3(1,1,1);

    return vec3(0,0,0);
}
void main() 
{
    vec3 ray = camera.front + camera.right * float(texcoord.x*camera.width - camera.width / 2) * camera.scale + camera.up * float(texcoord.y*camera.height - camera.height / 2) * camera.scale;

    
    FragColor = vec4(traverse(ray), 1.0f);
   
}
