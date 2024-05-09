#include "star.h"
#include <vector>
#include <glm/glm.hpp>
#include <mgdl-wii.h>

using namespace std;
using namespace glm;

static glm::vec3 rotatex(vec3 p, float angle) {
	float xt = p.x;
	float yt = p.y*cos(angle) - p.z*sin(angle);
	float zt = p.y*sin(angle) + p.z*cos(angle);
	return vec3(xt, yt, zt);
}

// -- rotate around y
static glm::vec3 rotatey(vec3 p, float angle){
	float xt = p.x*cos(angle) - p.z*sin(angle);
	float yt = p.y;
	float zt = p.x*sin(angle) + p.z*cos(angle);
	return vec3(xt, yt, zt);
}

// -- rotate around z
static glm::vec3 rotatez(vec3 p, float angle) {
	float xt = p.x*cos(angle) - p.y*sin(angle);
	float yt = p.x*sin(angle) + p.y*cos(angle);
	float zt = p.z;
	return vec3(xt, yt, zt);
}

glm::vec3 CalculateTriangleNormal(const vec3& common, const vec3& corner1, const vec3& corner2)
{
    vec3 toCorner1 = corner1-common;
    vec3 toCorner2 = corner2-common;
    return normalize(cross(toCorner1, toCorner2));
}

Mesh Star::CreateMesh(float thickness, float baseRadius, float pointRadius, int sides)
{
    Mesh S(2 * 5 * 6);

    vec3 point = vec3(1.0f, 0.0f, 0.0f);
    float fifth = PI*2/5.0f;
    float tenth = fifth/2.0f;

    for (short side = 0; side < sides; side++)
    {
        vec3 topCenter;
        topCenter = vec3(0, thickness * side * -1.0f, 0);

        for (int p = 0; p < 5; p++)
        {
            // star is facing Z axis
            vec3 baseRot1 = rotatez(point, fifth * p);
            vec3 baseRot2 = rotatez(point, fifth * (p+1));
            vec3 pointRot = rotatez(point, fifth * p + tenth);

            vec3 rimPoint = pointRot * pointRadius;
            vec3 basePoint1 = baseRot1 * baseRadius;
            vec3 basePoint2 = baseRot2 * baseRadius;

            vec3 normal1 = CalculateTriangleNormal(basePoint1, topCenter, rimPoint);
            if (side == 0)
            {
                S.positions.push_back(topCenter);
                S.positions.push_back(rimPoint);
                S.positions.push_back(basePoint1);
            }
            else
            {
                S.positions.push_back(topCenter);
                S.positions.push_back(basePoint1);
                S.positions.push_back(rimPoint);
                normal1 = CalculateTriangleNormal(basePoint1, rimPoint, topCenter);
            }
            for (int n = 0; n < 4; n++)
            {
                S.normals.push_back(normal1);
            }

            vec3 normal2 = CalculateTriangleNormal(basePoint2, rimPoint, topCenter);
            if (side == 0)
            {
                S.positions.push_back(topCenter);
                S.positions.push_back(basePoint2);
                S.positions.push_back(rimPoint);
            }
            else
            {
                S.positions.push_back(topCenter);
                S.positions.push_back(rimPoint);
                S.positions.push_back(basePoint2);
                normal2 = CalculateTriangleNormal(basePoint2, topCenter, rimPoint);
            }

            for (int n = 0; n < 4; n++)
            {
                S.normals.push_back(normal2);
            }
        }
    }
    printf("Created star mesh: %d triangles from %u vertices", S.GetTriangleCount(), S.positions.size());
    return S;
}