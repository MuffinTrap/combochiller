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
    FXfruits,
    FXglitch
};
struct Flake {
	Vector2 pos;
    Vector2 velocity;
	uint color; // Doubles as size
};
constexpr int ParticleAmount = 200;
class Particles
{
    public:
        struct Flake flakes[ParticleAmount];
        float time = 0.0f;
        Vector2 spawnPoint;
        float spawnIndex;
        float spawnSpeed;
        float gravity;
        float aliveTime;
        void Init();
        void Update(float deltaTime);
        void Draw();
};

class Plasma
{
    public:
        gdl::Image image;
        float angle = 0;
        float speed = 0.1f;
        short width;
        short height;
        float scale = 2.0f;
        gdl::Image plasma;
        void Init(short w, short h);
        void Update(float deltaTime);
        void Draw(short x, short y, bool flip);
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
