#pragma once

#include <mgdl-wii.h>
#include "fx.h"
#include "mesh.h"
#include <glm/glm.hpp>
#include <string>

void InitMeshDrawing();

void DrawMesh(glm::vec3 position, u_int color, Mesh& mesh);

void DrawTextDouble(std::string text, short x, short y, float scale, gdl::FFont* font, 
int lastLetter, float lastLetterProgress, u_int color, int letterWidth = -1);

Vector2 GetFruitPosition(std::string text, short x, short y, float scale, gdl::SpriteSet* sprites, 
int letterIndex);

void DrawFruitsDouble(std::string text, short x, short y, float scale, gdl::SpriteSet* sprites, 
int firstLetter, int lastLetter, u_int spriteIndex);
