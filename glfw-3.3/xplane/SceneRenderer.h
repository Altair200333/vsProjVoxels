#pragma once

#include <vector>

#include "scene.h"
#include "window.h"
#include "shader.h"

#define HIT		 1		// Ray hit primitive
#define MISS	 0		// Ray missed primitive
#define INPRIM	-1		// Ray started inside primitive
class SceneRenderer final
{
protected:
    const Window* window;
    Scene* scene;
    Shader shader;
public:
    std::vector<glm::vec4> voxels;
    int demon = 10;
    double _bin_size = 1;
    struct Ray
    {
        glm::vec3 orig;
        glm::vec3 dir;
        Ray(glm::vec3 _orig, glm::vec3 _ray) :dir(_ray), orig(_orig) {};
    };
	
    bool intersect(Ray r, glm::vec3 min, glm::vec3 max)
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
	//map index of int coordinates to int
	int getId(int x, int y, int z, int dim)
	{
        return  z * dim * dim + y * dim + x;
	}
    int getId(glm::vec3 v, int dim)
    {
        return  static_cast<int>(v.z) * dim * dim + static_cast<int>(v.y) * dim + static_cast<int>(v.x);
    }
	
	glm::vec3 getNext(Ray r, float step)
    {
        glm::vec3 ray = glm::normalize(r.dir) * step;
        return r.orig + ray;
    }
	
	//this code is copy-pasted from https://github.com/francisengelmann/fast_voxel_traversal/blob/master/main.cpp which is based on http://www.cse.chalmers.se/edu/year/2010/course/TDA361/grid.pdf
	//awesome article btw
    std::vector<glm::vec3> voxel_traversal(glm::vec3 ray_start, glm::vec3 ray_end) {
        std::vector<glm::vec3> visited_voxels;

        // This id of the first/current voxel hit by the ray.
        // Using floor (round down) is actually very important,
        // the implicit int-casting will round up for negative numbers.
        glm::vec3 current_voxel(std::floor(ray_start[0] / _bin_size),
            std::floor(ray_start[1] / _bin_size),
            std::floor(ray_start[2] / _bin_size));

        // The id of the last voxel hit by the ray.
        // TODO: what happens if the end point is on a border?
        glm::vec3 last_voxel(std::floor(ray_end[0] / _bin_size),
            std::floor(ray_end[1] / _bin_size),
            std::floor(ray_end[2] / _bin_size));

        // Compute normalized ray direction.
        glm::vec3 ray = ray_end - ray_start;
        //ray.normalize();

        // In which direction the voxel ids are incremented.
        double stepX = (ray[0] >= 0) ? 1 : -1; // correct
        double stepY = (ray[1] >= 0) ? 1 : -1; // correct
        double stepZ = (ray[2] >= 0) ? 1 : -1; // correct

        // Distance along the ray to the next voxel border from the current position (tMaxX, tMaxY, tMaxZ).
        double next_voxel_boundary_x = (current_voxel[0] + stepX) * _bin_size; // correct
        double next_voxel_boundary_y = (current_voxel[1] + stepY) * _bin_size; // correct
        double next_voxel_boundary_z = (current_voxel[2] + stepZ) * _bin_size; // correct

        // tMaxX, tMaxY, tMaxZ -- distance until next intersection with voxel-border
        // the value of t at which the ray crosses the first vertical voxel boundary
        double tMaxX = (ray[0] != 0) ? (next_voxel_boundary_x - ray_start[0]) / ray[0] : DBL_MAX; //
        double tMaxY = (ray[1] != 0) ? (next_voxel_boundary_y - ray_start[1]) / ray[1] : DBL_MAX; //
        double tMaxZ = (ray[2] != 0) ? (next_voxel_boundary_z - ray_start[2]) / ray[2] : DBL_MAX; //

        // tDeltaX, tDeltaY, tDeltaZ --
        // how far along the ray we must move for the horizontal component to equal the width of a voxel
        // the direction in which we traverse the grid
        // can only be FLT_MAX if we never go in that direction
        double tDeltaX = (ray[0] != 0) ? _bin_size / ray[0] * stepX : DBL_MAX;
        double tDeltaY = (ray[1] != 0) ? _bin_size / ray[1] * stepY : DBL_MAX;
        double tDeltaZ = (ray[2] != 0) ? _bin_size / ray[2] * stepZ : DBL_MAX;

        glm::vec3 diff(0, 0, 0);
        bool neg_ray = false;
        if (current_voxel[0] != last_voxel[0] && ray[0] < 0) { diff[0]--; neg_ray = true; }
        if (current_voxel[1] != last_voxel[1] && ray[1] < 0) { diff[1]--; neg_ray = true; }
        if (current_voxel[2] != last_voxel[2] && ray[2] < 0) { diff[2]--; neg_ray = true; }
        visited_voxels.push_back(current_voxel);
        if (neg_ray) {
            current_voxel += diff;
            visited_voxels.push_back(current_voxel);
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
            visited_voxels.push_back(current_voxel);
        }
        return visited_voxels;
    }
    glm::vec3 voxel_traversal_closest(glm::vec3 ray_start, glm::vec3 ray_end)
	{
        int id = getId(ray_start, demon);
        // This id of the first/current voxel hit by the ray.
        // Using floor (round down) is actually very important,
        // the implicit int-casting will round up for negative numbers.
        glm::vec3 current_voxel(std::floor(ray_start[0] / _bin_size),
            std::floor(ray_start[1] / _bin_size),
            std::floor(ray_start[2] / _bin_size));

        // The id of the last voxel hit by the ray.
        // TODO: what happens if the end point is on a border?
        glm::vec3 last_voxel(std::floor(ray_end[0] / _bin_size),
            std::floor(ray_end[1] / _bin_size),
            std::floor(ray_end[2] / _bin_size));

        // Compute normalized ray direction.
        glm::vec3 ray = ray_end - ray_start;
        //ray.normalize();

        // In which direction the voxel ids are incremented.
        double stepX = (ray[0] >= 0) ? 1 : -1; // correct
        double stepY = (ray[1] >= 0) ? 1 : -1; // correct
        double stepZ = (ray[2] >= 0) ? 1 : -1; // correct

        // Distance along the ray to the next voxel border from the current position (tMaxX, tMaxY, tMaxZ).
        double next_voxel_boundary_x = (current_voxel[0] + stepX) * _bin_size; // correct
        double next_voxel_boundary_y = (current_voxel[1] + stepY) * _bin_size; // correct
        double next_voxel_boundary_z = (current_voxel[2] + stepZ) * _bin_size; // correct

        // tMaxX, tMaxY, tMaxZ -- distance until next intersection with voxel-border
        // the value of t at which the ray crosses the first vertical voxel boundary
        double tMaxX = (ray[0] != 0) ? (next_voxel_boundary_x - ray_start[0]) / ray[0] : DBL_MAX; //
        double tMaxY = (ray[1] != 0) ? (next_voxel_boundary_y - ray_start[1]) / ray[1] : DBL_MAX; //
        double tMaxZ = (ray[2] != 0) ? (next_voxel_boundary_z - ray_start[2]) / ray[2] : DBL_MAX; //

        // tDeltaX, tDeltaY, tDeltaZ --
        // how far along the ray we must move for the horizontal component to equal the width of a voxel
        // the direction in which we traverse the grid
        // can only be FLT_MAX if we never go in that direction
        double tDeltaX = (ray[0] != 0) ? _bin_size / ray[0] * stepX : DBL_MAX;
        double tDeltaY = (ray[1] != 0) ? _bin_size / ray[1] * stepY : DBL_MAX;
        double tDeltaZ = (ray[2] != 0) ? _bin_size / ray[2] * stepZ : DBL_MAX;

        glm::vec3 diff(0, 0, 0);
        bool neg_ray = false;
        if (current_voxel[0] != last_voxel[0] && ray[0] < 0) { diff[0]--; neg_ray = true; }
        if (current_voxel[1] != last_voxel[1] && ray[1] < 0) { diff[1]--; neg_ray = true; }
        if (current_voxel[2] != last_voxel[2] && ray[2] < 0) { diff[2]--; neg_ray = true; }
    	if(id != getId(current_voxel, demon) && voxels[getId(current_voxel, demon)].w != 0)
			return current_voxel;
        if (neg_ray) {
            current_voxel += diff;
            if (id != getId(current_voxel, demon) && voxels[getId(current_voxel, demon)].w != 0)
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
            if (id != getId(current_voxel, demon) && voxels[getId(current_voxel, demon)].w != 0)
                return current_voxel;
        }
        return { -1,-1,-1 };
    }
	SceneRenderer(const Window* _window, Scene* _scene) :window(_window), scene(_scene),
	   shader("Shaders/vertexshader.vs", "Shaders/fragmentshader.fs", "Shaders/geometry.gs")
	{
        voxels.resize(200, { 0,0,0,0 });
        voxels[getId(1, 1, 0, demon)] = { 1,1,1,1 };
        voxels[getId(0, 2, 0, demon)] = { 0,2,0,1 };
    	
        Ray r = { {0,0,0}, {1,1,1} };
        auto v = voxel_traversal({ 1,2, 0 },{ 0,0,0 });

        auto v1 = voxel_traversal_closest({ 1,2, 0 } ,{ 0,0,0 });
    	
		
	}
	
    void clearBuffer();

    void drawScene() const;
    void swapBuffers() const;
    ~SceneRenderer() = default;
};
