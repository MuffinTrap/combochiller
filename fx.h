#pragma once
#include "mgdl-wii.h"
#include "star.h"
#include <vector>

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
    FXplasma,
    FXvoxelcube,
    FXtunnel,
    FXbirds,
    FXfruits,
    FXglitch
};

float RandFloat();
float RandDir();

struct Flake {
	Vector2 pos;
    Vector2 velocity;
	uint color; // Doubles as size and fruit index :3
    bool active;
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
        void Reset(int index, Vector2 position, Vector2 velocity, uint fruitIndex, bool alive);
        void StopAll();
        void Draw();
        void DrawAsSprites(gdl::SpriteSet* sprites, float scale);
};

class Plasma
{
    public:
        float angle = 0;
        float speed = 0.1f;
        float scale = 2.0f;
        short width;
        short height;
        gdl::Image plasma;
        float y;
        float targetY;
        float moveSpeed;

        void Init(short w, short h);
        void Update(float deltaTime);
        void Draw(short x, bool flip);
};

class Voxelcube
{
    void Init();
    void Update(float deltaTime);
    void Draw();
};

constexpr int StarAmount = 16;

struct StarRing
{
    float z;
    float angle;
};

struct StarParticle {
	glm::vec3 pos;
	uint color;
};
class Tunnel
{
    public:
    Mesh starMesh;
    std::vector<StarParticle> stars;
    std::vector<StarRing> rings;
    float time = 0;
    float rotationSpeed = 16.0f;
    float speed = 10.0f;
    float lastRingDepth = 80.0f;
    float stepZ = 10.0f;
    float angleStep = PI*2.0f/12;
    float scale;
    Tunnel();

    void Init(float radius, int ringAmount, float starSize, float scale);
    void Update(float deltaTime);
    void Draw();
};

class Birds
{
    void Init();
    void Update(float deltaTime);
    void Draw();
};
