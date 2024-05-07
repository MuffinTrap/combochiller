#pragma once
#include "mgdl-wii.h"

typedef struct Vector2
{
	float x;
	float y;

    Vector2();
    Vector2(float x, float y);
} Vector2;

enum FX
{
    FXnone,
    FXparticles,
    FXfire,
    FXplasma,
    FXvoxelcube,
    FXtunnel,
    FXbirds,
    FXfruits
};
struct Flake {
	Vector2 pos;
	uint color;
};
class Particles
{
    public:
        struct Flake flakes[100];
        float time = 0.0f;
        void Init();
        void Update(float deltaTime);
        void Draw();
};

class Plasma
{
    public:
        gdl::Image image;
        float angle = 0;
        u_int colTable[256];
        void Init();
        void Update(float deltaTime);
        void Draw();
};

class Voxelcube
{
    void Init();
    void Update(float deltaTime);
    void Draw();
};

struct Fruit {
	Vector2 pos;
	uint spriteIndex;
};
class Tunnel
{
    public:
    gdl::SpriteSet sprites;
    gdl::Image fruitAtlas;
    Fruit fruits[128];
    float time = 0;

    void Init();
    void Update(float deltaTime);
    void Draw();
};

class Birds
{
    void Init();
    void Update(float deltaTime);
    void Draw();
};
