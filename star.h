#pragma once

#include "mesh.h"
#include <glm/glm.hpp>

class Star
{
    public:
        glm::vec3 position;
        glm::vec3 rotations;

        static Mesh CreateMesh(float thickness, float baseRadius, float pointRadius, int sides);
};