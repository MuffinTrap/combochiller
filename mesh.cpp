#include "mesh.h"

Mesh::Mesh(size_t size)
{
    positions.reserve(size);
    normals.reserve(size);
    indices.reserve(size);
}

int Mesh::GetTriangleCount()
{
    return positions.size()/3;
}