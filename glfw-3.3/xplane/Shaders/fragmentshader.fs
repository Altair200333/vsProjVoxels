#version 330 core
out vec4 FragColor;
in vec2 texcoord;

#define M_PI 3.1415926535897932384626433832795
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
uniform int demon ;
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
-0.5f, -0.5f, -0.5f,
-0.5f, -0.5f, 0.5f,
-0.5f, 0.5f, 0.5f,
-1.0f, 0.0f, 0.0f,

0.5f, 0.5f, -0.5f,
-0.5f, -0.5f, -0.5f,
-0.5f, 0.5f, -0.5f,
0.0f, 0.0f, -1.0f,

0.5f, -0.5f, 0.5f,
-0.5f, -0.5f, -0.5f,
0.5f, -0.5f, -0.5f,
0.0f, -1.0f, 0.0f,

0.5f, 0.5f, -0.5f,
0.5f, -0.5f, -0.5f,
-0.5f, -0.5f, -0.5f,
0.0f, 0.0f, -1.0f,

-0.5f, -0.5f, -0.5f,
-0.5f, 0.5f, 0.5f,
-0.5f, 0.5f, -0.5f,
-1.0f, 0.0f, 0.0f,

0.5f, -0.5f, 0.5f,
-0.5f, -0.5f, 0.5f,
-0.5f, -0.5f, -0.5f,
0.0f, -1.0f, 0.0f,

-0.5f, 0.5f, 0.5f,
-0.5f, -0.5f, 0.5f,
0.5f, -0.5f, 0.5f,
0.0f, 0.0f, 1.0f,

0.5f, 0.5f, 0.5f,
0.5f, -0.5f, -0.5f,
0.5f, 0.5f, -0.5f,
1.0f, 0.0f, 0.0f,

0.5f, -0.5f, -0.5f,
0.5f, 0.5f, 0.5f,
0.5f, -0.5f, 0.5f,
1.0f, 0.0f, 0.0f,

0.5f, 0.5f, 0.5f,
0.5f, 0.5f, -0.5f,
-0.5f, 0.5f, -0.5f,
0.0f, 1.0f, 0.0f,

0.5f, 0.5f, 0.5f,
-0.5f, 0.5f, -0.5f,
-0.5f, 0.5f, 0.5f,
0.0f, 1.0f, 0.0f,

0.5f, 0.5f, 0.5f,
-0.5f, 0.5f, 0.5f,
0.5f, -0.5f, 0.5f,
0.0f, 0.0f, 1.0f
);
struct Hit 
{ vec3 pos; vec3 normal; bool hit;};
struct Ray 
{
 vec3 orig; 
 vec3 dir;
};
bool intersectsCube(vec3 ray, vec3 src, vec3 shift)
{
    for(int i=0; i < 12; i++)
    {
        int off = i*12;
        vec3 src = src - shift;
        vec3 contact = intersectPoint(ray, src, vec3(cube[9+off], cube[10+off], cube[11+off]), vec3(cube[0+off], cube[1+off], cube[2+off]));
        if(pointInPolygon(contact, vec3(cube[0+off], cube[1+off], cube[2+off])*0.5f,vec3(cube[3+off], cube[4+off], cube[5+off])*0.5f, vec3(cube[6+off], cube[7+off], cube[8+off])*0.5f) && dot(contact-src, ray) > 0)
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

vec3 lightDir = normalize(vec3(0.2f,-0.85f,0.4f));

float dist2(vec3 v1, vec3 v2)
{
    return dot(v1-v2, v1-v2);
}
bool pointInCube(vec3 p)
{
    return abs(p.x)<=0.5 && abs(p.y)<=0.5 && abs(p.z)<=0.5;
}
Hit intersectsCubePoint(vec3 ray, vec3 src, vec3 shift)
{
    Hit hit = Hit(vec3(-2000, -2000, -2000), vec3(cube[9], cube[10], cube[11]), false);
    src = src - shift;

    for(int i=0; i < 12; i++)
    {
        int off = i*12;
        vec3 contact = intersectPoint(ray, src, vec3(cube[9+off], cube[10+off], cube[11+off]), vec3(cube[0+off], cube[1+off], cube[2+off]));
        if(pointInPolygon(contact, vec3(cube[0+off], cube[1+off], cube[2+off]),vec3(cube[3+off], cube[4+off], cube[5+off]), vec3(cube[6+off], cube[7+off], cube[8+off])) 
            && dot(contact-src, ray) > 0 && dist2(contact, src)<dist2(src, hit.pos))
            hit = Hit(contact, vec3(cube[9+off], cube[10+off], cube[11+off]), true);
    }

    return hit;
}
uniform int boxes;


float _bin_size = 1;

int getId(vec3 v, int dim)
{
    return  int(round(v.z)) * dim * dim + int(round(v.y)) * dim + int(round(v.x));
}
int getSwapId(vec3 v, int dim)
{
	return int(v.y)* dim* dim +int(v.z)* dim + int(v.x);
}
vec4 voxels(int id)
{
    return texelFetch(VertexSampler0, id);
}

vec3 reflect(vec3 I, vec3 N)
{
    return I - N * (dot(I, N) * 2.0f);
}


struct VoxelHit 
{ vec3 pos; vec3 normal; bool hit;vec3 relPos; };

bool inBounds(vec3 point)
{
    return point.x>=0 && point.x<demon && point.y>=0 && point.y<demon && point.z>=0 && point.z<demon;
}
vec3 traverse2(vec3 v3dStart, vec3 v3dEnd)
{
    float tmp = v3dStart.y;
    v3dStart.y = v3dStart.z;
    v3dStart.z = tmp;
    tmp = v3dEnd.y;
    v3dEnd.y = v3dEnd.z;
    v3dEnd.z = tmp;

	float x1 = v3dStart.x + 0.5f;
	float y1 = v3dStart.y + 0.5f;
	float z1 = v3dStart.z + 0.5f;
	float x2 = v3dEnd.x + 0.5f;
	float y2 = v3dEnd.y + 0.5f;
	float z2 = v3dEnd.z + 0.5f;

	int i = int(floor(x1));
	int j = int(floor(y1));
	int k = int(floor(z1));

	int iend = int(floor(x2));
	int jend = int(floor(y2));
	int kend = int(floor(z2));

	int di = ((x1 < x2) ? 1 : ((x1 > x2) ? -1 : 0));
	int dj = ((y1 < y2) ? 1 : ((y1 > y2) ? -1 : 0));
	int dk = ((z1 < z2) ? 1 : ((z1 > z2) ? -1 : 0));

	float deltatx = 1.0f / abs(x2 - x1);
	float deltaty = 1.0f / abs(y2 - y1);
	float deltatz = 1.0f / abs(z2 - z1);

	float minx = floor(x1), maxx = minx + 1.0f;
	float tx = ((x1 > x2) ? (x1 - minx) : (maxx - x1))* deltatx;
	float miny = floor(y1), maxy = miny + 1.0f;
	float ty = ((y1 > y2) ? (y1 - miny) : (maxy - y1))* deltaty;
	float minz = floor(z1), maxz = minz + 1.0f;
	float tz = ((z1 > z2) ? (z1 - minz) : (maxz - z1))* deltatz;

	vec3 curPos = vec3( i,j,k );

	while(true)
	{
        if(!inBounds(curPos))
            break;
		int id = getSwapId(curPos, demon);
		if (voxels(id).w!=0)
		{
            float tmp = curPos.y;
            curPos.y = curPos.z;
            curPos.z = tmp;
			return curPos;
		}
		if (tx <= ty && tx <= tz)
		{
			if (i == iend) break;
			tx += deltatx;
			i += di;

			if (di == 1) curPos.x+=1;
			if (di == -1) curPos.x -= 1;
		}
		else if (ty <= tz)
		{
			if (j == jend) break;
			ty += deltaty;
			j += dj;

			if (dj == 1) curPos.y += 1;
			if (dj == -1) curPos.y -= 1;
		}
		else
		{
			if (k == kend) break;
			tz += deltatz;
			k += dk;

			if (dk == 1) curPos.z += 1;
			if (dk == -1) curPos.z -= 1;
		}
	}
	return vec3( -1,-1,-1 );
}
VoxelHit traceHit2(vec3 ray, vec3 src)
{
    vec3 res = traverse2(src, src+ray*200.0f);
   
    Hit hit =  Hit(vec3(-2000, -2000, -2000), vec3(1, 1, 1), false);
    
    vec3 relPos;
    if(res.x!=-1)
    {
        hit = intersectsCubePoint(ray, src, res);
        if(hit.hit)
        {
            relPos = hit.pos;
            hit.pos = res; 
        }
    }
    
    return VoxelHit(hit.pos, hit.normal, hit.hit, relPos);
}
vec3 getBackColor(vec3 ray)
{
    float theta = (atan(ray.z, ray.x) * 180 / M_PI + 180)/360.0f;
    float alpha = (atan(ray.y, sqrt(ray.x * ray.x + ray.z * ray.z)) * 180 / M_PI + 90)/180.0f;

    return vec3(alpha*0.7+0.3, 0.5, 0.2);
}
float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}
vec3 random3(vec3 c) {
	float j = 4096.0*sin(dot(c,vec3(17.0, 59.4, 15.0)));
	vec3 r;
	r.z = fract(512.0*j);
	j *= .125;
	r.x = fract(512.0*j);
	j *= .125;
	r.y = fract(512.0*j);
	return r-0.5;
}
const float F3 =  0.3333333;
const float G3 =  0.1666667;
float snoise(vec3 p) {

	vec3 s = floor(p + dot(p, vec3(F3)));
	vec3 x = p - s + dot(s, vec3(G3));
	 
	vec3 e = step(vec3(0.0), x - x.yzx);
	vec3 i1 = e*(1.0 - e.zxy);
	vec3 i2 = 1.0 - e.zxy*(1.0 - e);
	 	
	vec3 x1 = x - i1 + G3;
	vec3 x2 = x - i2 + 2.0*G3;
	vec3 x3 = x - 1.0 + 3.0*G3;
	 
	vec4 w, d;
	 
	w.x = dot(x, x);
	w.y = dot(x1, x1);
	w.z = dot(x2, x2);
	w.w = dot(x3, x3);
	 
	w = max(0.6 - w, 0.0);
	 
	d.x = dot(random3(s), x);
	d.y = dot(random3(s + i1), x1);
	d.z = dot(random3(s + i2), x2);
	d.w = dot(random3(s + 1.0), x3);
	 
	w *= w;
	w *= w;
	d *= w;
	 
	return dot(d, vec4(52.0));
}
vec3 normalNoise(vec2 _st, float _zoom, float _speed){
	vec2 v1 = _st;
	vec2 v2 = _st;
	vec2 v3 = _st;
	float expon = pow(10.0, _zoom*2.0);
	v1 /= 1.0*expon;
	v2 /= 0.62*expon;
	v3 /= 0.83*expon;
	float n = _speed;
	float nr = (snoise(vec3(v1, n)) + snoise(vec3(v2, n)) + snoise(vec3(v3, n))) / 6.0 + 0.5;
	n =  _speed + 1000.0;
	float ng = (snoise(vec3(v1, n)) + snoise(vec3(v2, n)) + snoise(vec3(v3, n))) / 6.0 + 0.5;
	return vec3(nr,ng,0.5);
}
vec3 getDiffuse(VoxelHit hit)
{
    vec4 texel = voxels(getId(hit.pos, demon));
    float type = texel.w;
    if(type == 2 || type == 1)
    {
        float slope = clamp(-(dot(hit.normal, normalize(lightDir))), 0.1, 1);
        vec3 color = texel.xyz;
        return color * slope;
    }
    else if(type == 3)
    {
        float slope = clamp(-(dot(hit.normal + normalNoise(hit.relPos.xz, 1.0, 1.0)*0.5f, normalize(lightDir))), 0.1, 1);
        vec3 color = texel.xyz;
        return color * slope;
    }
}
vec3 traceScene(vec3 ray)
{
    VoxelHit hit = traceHit2(ray, camera.position);
    if(hit.hit)
    {
        vec3 color = getDiffuse(hit);
        VoxelHit shadow = traceHit2(-lightDir, hit.pos+hit.relPos+hit.normal*0.0001f);
        if(voxels(getId(hit.pos, demon)).w == 2)
        {
            vec3 reflection = reflect(normalize(ray), hit.normal);
            VoxelHit reflectHit = traceHit2(reflection, hit.pos+hit.relPos+hit.normal*0.0001f);
            vec3 reflectedColor = getBackColor(ray);
            if(reflectHit.hit)
            {
                reflectedColor = getDiffuse(reflectHit);
                VoxelHit shadowOfReflection = traceHit2(-lightDir, reflectHit.pos+reflectHit.relPos+reflectHit.normal*0.0001f);

                if(shadowOfReflection.hit)
                    reflectedColor*=0.1f;    
            }
            color = color*0.5 + reflectedColor*0.5;
        }
        if(shadow.hit)
            return color*0.1;
        
        return color;
    }
    return getBackColor(ray);
}
void main() 
{
    vec3 ray = camera.front + camera.right * float(texcoord.x*camera.width - camera.width / 2) * camera.scale + camera.up * float(texcoord.y*camera.height - camera.height / 2) * camera.scale;

    
    FragColor = vec4(traceScene(ray), 1.0f);
   
}
