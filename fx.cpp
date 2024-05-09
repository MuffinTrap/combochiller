#include "fx.h"
#include "pastelpalette.h"

#include <cstdlib>

#include <mgdl-wii.h>
#include <gccore.h>

Vector2::Vector2()
{
    this->x = 0.0f;
    this->y = 0.0f;
}

Vector2::Vector2(float x, float y)
{
    this->x = x;
    this->y = y;
}

float RandFloat()
{
    return (rand()%1000)/1000.0f;
}
float RandDir()
{
    return ((rand()%2000)-1000)/1000.0f;
}

void Particles::Init() {
    // Snow flakes
    for (int i = 0; i < ParticleAmount; i++)
    {
        flakes[i] = Flake();
        // Create under the screen
        Reset(i, Vector2(0,gdl::ScreenYres + 10), Vector2(0,0), 8 + (rand()% 32), false);
    }
    time = 0.0f;
    spawnIndex = 0;
    spawnSpeed = 160.0f;
    gravity = 8.0f;
    aliveTime = 1.0f;
}

void Particles::Reset(int index, Vector2 position, Vector2 velocity, uint fruitIndex, bool alive){
    if (index < ParticleAmount)
    {
        Flake& p = flakes[index];
        p.pos = position;
        p.velocity = velocity;
        p.color = fruitIndex;
        p.active = alive;
    }
}

static void DrawPoints(Flake pointsArray[])
{
	GX_ClearVtxDesc();
    GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GX_Begin(GX_QUADS, GX_VTXFMT0, ParticleAmount * 4);

    short x;
    short y;
    for (int i = 0; i < ParticleAmount; i++)
    {
        Vector2 p = pointsArray[i].pos;
        x = (short)p.x;
        y = (short)p.y;
        uint col = palette[1+pointsArray[i].color%15];// No black particles
        short size = pointsArray[i].color;

    // Draw a diamond
        GX_Position2s16(x, y);
        GX_Color4u8(RED(col), GREEN(col), BLUE(col), ALPHA(col));
        GX_Position2s16(x + size/2, y + size/2);
        GX_Color4u8(RED(col), GREEN(col), BLUE(col), ALPHA(col));
        GX_Position2s16(x, y + size);
        GX_Color4u8(RED(col), GREEN(col), BLUE(col), ALPHA(col));
        GX_Position2s16(x-size/2 , y + size/2);
        GX_Color4u8(RED(col), GREEN(col), BLUE(col), ALPHA(col));
    }

    GX_End();
}


void Particles::Update(float deltaTime) {
    time += deltaTime;

    int amountNow = std::floor(spawnIndex);
    int createAmount = std::floor(time*spawnSpeed - spawnIndex);
    if (createAmount > 0)
    {
        spawnIndex = time * spawnSpeed;
        if (amountNow + createAmount < ParticleAmount)
        {
            for(int i = amountNow; i < amountNow + createAmount; i++)
            {
                float dx = RandDir() * 8.0f;
                float dy = -0.1f - RandFloat() * 2.0f;
                Reset(i, spawnPoint, Vector2(dx, dy), flakes[i].color, true);
            }
        }
    }

    for (int i = 0; i < ParticleAmount; i++)
    {
        Flake& p = flakes[i];
        if (p.active)
        {
            p.velocity.y += gravity * deltaTime; 

            p.pos.x += flakes[i].velocity.x;
            p.pos.y += flakes[i].velocity.y;

            if (p.pos.y > gdl::ScreenYres + 64)
            {
                p.active = false;
            }
        }
    }
}

void Particles::Draw()
{
    if (time < aliveTime)
    {
        DrawPoints(flakes);
    }
}

void Particles::StopAll()
{
    for(int i = 0; i < ParticleAmount; i++)
    {
        Flake& p = flakes[i];
        p.active = false;
    }
}

void Particles::DrawAsSprites(gdl::SpriteSet* sprites, float scale)
{
    for(int i = 0; i < ParticleAmount; i++)
    {
        Flake& p = flakes[i];
        if (p.active)
        {
            sprites->Put(p.pos.x, p.pos.y, p.color, gdl::Color::White, 0, 0, scale);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////


void Plasma::Init(short w, short h) {
    width = w;
    height = h;
    plasma.Create(w,h, gdl::Linear, gdl::RGBA8);
}

void Plasma::Update(float deltaTime) {

    for(short py=0; py<height; py++) {
        for(short px=0; px<width; px++) {
            u_char col = (sin(px*PI/270)*16)+(sin(py*PI/45)*2)+(sin(((py+px)+(4*angle))*PI/90)*8);
            plasma.Texture.PokePixel(px, py, palette[1+col%15]);
        }
    }
    plasma.Texture.Flush();
    angle		+= speed*deltaTime;

    float ydiff = targetY-y;
    if (ydiff > 1.0f)
    {
        y += (moveSpeed*ydiff) * deltaTime;
    }
}

void Plasma::Draw(short x, bool flip)
{
    short w = width;
    if (flip)
    {
         w *=-1;
    }

    short l = x;
    short r = x + w * scale;
    short t = y;
    short b = y + height * scale;

    gdl::DrawBoxF(l-2, t-2, r+2, b+2, palette[PastelDarkPurple]);

    plasma.PutS(l,t, r, b);
}
