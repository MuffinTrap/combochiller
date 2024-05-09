#pragma once 

#include <vector>
#include <glm/glm.hpp>

struct Mesh
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<short> indices;

    Mesh(size_t size = 0);
    int GetTriangleCount();
};