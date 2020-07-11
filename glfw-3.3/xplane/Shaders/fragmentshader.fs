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
int demon = 200;

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

	for (;;)
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
    vec3 res = traverse2(src, src+ray*300.0f);
   
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
vec3 traceScene(vec3 ray)
{
    VoxelHit hit = traceHit2(ray, camera.position);
    if(hit.hit)
    {
        float slope = clamp(-(dot(hit.normal, normalize(lightDir))), 0.1, 1);
        vec3 color = voxels(getId(hit.pos, demon)).xyz;
        VoxelHit shadow = traceHit2(-lightDir, hit.pos+hit.relPos+hit.normal*0.0001f);
        if(voxels(getId(hit.pos, demon)).w == 2)
        {
            vec3 reflection = reflect(normalize(ray), hit.normal);
            VoxelHit reflectHit = traceHit2(reflection, hit.pos+hit.relPos+hit.normal*0.0001f);
            vec3 reflectedColor = vec3(0.1);
            if(reflectHit.hit)
            {
                reflectedColor = voxels(getId(reflectHit.pos, demon)).xyz;
                VoxelHit shadowOfReflection = traceHit2(-lightDir, reflectHit.pos+reflectHit.relPos+reflectHit.normal*0.0001f);
                if(shadowOfReflection.hit)
                    reflectedColor*=0.1f;
                color = color*0.5 + reflectedColor*0.5;
            }
        }
        if(shadow.hit)
            return color*0.1;
        
        return color*slope;
    }
    return vec3(0.1);
}
void main() 
{
    vec3 ray = camera.front + camera.right * float(texcoord.x*camera.width - camera.width / 2) * camera.scale + camera.up * float(texcoord.y*camera.height - camera.height / 2) * camera.scale;

    
    FragColor = vec4(traceScene(ray), 1.0f);
   
}
