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
void Particles::Init() {
    // Snow flakes
    for (int i = 0; i < ParticleAmount; i++)
    {
        Flake a;
        // Create under the screen
        a.pos = Vector2(0,gdl::ScreenYres + 10);
        a.velocity = Vector2(0,0);
        a.color = 8 + (rand()% 32);
        flakes[i] = a;
    }
    time = 0.0f;
    spawnIndex = 0;
    spawnSpeed = 160.0f;
    gravity = 8.0f;
    aliveTime = 1.0f;
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
                flakes[i].pos = spawnPoint;
                float dx = (rand()%200-100)/100.0f;
                float dy = -0.1f + (rand()%100)/-100.0f;
                flakes[i].velocity = Vector2(dx * 4.0f, dy * 5.0f);
            }
        }
    }

    for (int i = 0; i < ParticleAmount; i++)
    {
        flakes[i].velocity.y += gravity * deltaTime; 

        flakes[i].pos.x += flakes[i].velocity.x;
        flakes[i].pos.y += flakes[i].velocity.y;
    }
}

void Particles::Draw()
{
    if (time < aliveTime)
    {
        DrawPoints(flakes);
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
}

void Plasma::Draw(short x, short y, bool flip)
{
    short l = x;
    short r = x + width * scale;
    short t = y;
    short b = y + height * scale;
    gdl::DrawBoxF(l-2, t-2, r+2, b+2, palette[PastelDarkPurple]);
    if (flip)
    {
        plasma.PutS(l,t, r, b);
    }
    else 
    {
        plasma.PutS(r, t, l, b);
    }
}
