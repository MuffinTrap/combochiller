#include "fx.h"
#include "pastelpalette.h"
#include <mgdl-wii.h>

void Particles::Init() {
        // Snow flakes
        for (int i = 0; i < 100; i++)
        {
            float x = rand() % gdl::ScreenXres;
            float y = rand() % gdl::ScreenYres;
			Flake a;
			a.pos.x = x;
			a.pos.y = y;
			a.color = palette[i%16];
            flakes[i] = a;
        }
    }

static void DrawPoints(Flake pointsArray[], int amount, short size)
{
    if (size <= 1)
    {
        size = 1;
    }

    short x;
    short y;

    GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GX_Begin(GX_QUADS, GX_VTXFMT0, amount * 4);

    for (int i = 0; i < amount; i++)
    {
        struct Vector2 p = pointsArray[i].pos;
        x = (short)p.x;
        y = (short)p.y;
        uint col = pointsArray[i].color;

        GX_Position2s16(x, y);
        GX_Color4u8(RED(col), GREEN(col), BLUE(col), ALPHA(col));
        GX_Position2s16(x + size, y);
        GX_Color4u8(RED(col), GREEN(col), BLUE(col), ALPHA(col));
        GX_Position2s16(x + size, y + size);
        GX_Color4u8(RED(col), GREEN(col), BLUE(col), ALPHA(col));
        GX_Position2s16(x, y + size);
        GX_Color4u8(RED(col), GREEN(col), BLUE(col), ALPHA(col));
    }

    GX_End();
}


void Particles::Update(float deltaTime) {
    time += deltaTime * 0.01f;

    for (int i = 0; i < 100; i++)
    {
        flakes[i].pos.x += 1.1f + sin(time) * 2.0f;
        flakes[i].pos.y += 1 * gdl::Delta;
        if (flakes[i].pos.x > gdl::ScreenXres) {
            flakes[i].pos.x = 0.0f;
        }
        if (flakes[i].pos.y > gdl::ScreenYres) {
            flakes[i].pos.y = 0.0f;
        }
    }

    DrawPoints(flakes, 100, 4);
}

////////////////////////////////////////////////////////////////////////////////
#define J 255.f/360*3
#define K 255.f/360*2
#define L 255.f/360*1

void Plasma::Init() {
    image.Create(128, 128, gdl::Linear, gdl::RGBA8);
    for (short c=0; c<256; c++) {

        int r = 255*fabs(sin((J*c)*ROTPI));
        int g = 255*fabs(sin((K*c)*ROTPI));
        int b = 255*fabs(sin((L*c)*ROTPI));

        colTable[c] = RGBA(r, g, b, 255);

    }
}

void Plasma::Update(float delta) {

    for(short py=0; py<128; py++) {
        for(short px=0; px<128; px++) {
            short col = (sin(px*PI/270)*255)+(sin(py*PI/45)*31)+(sin(((py+px)+(4*angle))*PI/90)*63);
            image.Texture.PokePixel(px, py, colTable[abs(col)%256]);
        }
    }
    // Flush texture so that updated pixels will be drawn properly
    image.Texture.Flush();
    image.PutS(0, 0, gdl::ScreenXres, gdl::ScreenYres, gdl::Color::White);

    angle		+= 0.1f*delta;
}