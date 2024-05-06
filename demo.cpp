#include "demo.h"
#include "timer.h"
#include "fx.h"

#include <wiiuse/wpad.h>
#include "mgdl-input-wii.h"
#include <string>
#include <cstring>
#include <vector>

#include <ogc/lwp_watchdog.h>

#include "sky1_png.h"
#include "sky2_png.h"
#include "sky3_png.h"
#include "sky4_png.h"
#include "m6x11_png.h"

#include "sad_png.h"
#include "cheers_png.h"
#include "frustrated_png.h"
#include "actually_png.h"

#include "askMeToStay_ogg.h"
/*
#include "mel_tiles_png.h"
#include "pointer_png.h"
#include "font8x16_png.h"
#include "sample3_ogg.h"
#include "blipSelect_wav.h"
*/

#include "pastelpalette.h"

static u64 deltaTimeStart = 0;
static u64 programStart = 0;
static float skyScale = 2.0f;
static float faceScale = 0.5f;
static float cloudWidth;


static int partIndex = 0;
static bool showGreets = true;

static bool drawFadeRectangle = true;
static int fadeAlpha = 255;

static float demoDuration = 120.0f;
static float elapsed = 0.0f;
static float fadeOutDuration = 3.0f;
static float fadeInDuration = 3.0f;

// Actually : 0
// Cheers : 1
// Frustrated : 2
// Sad : 3
// No Face : 4
static const int iActually = 0;
static const int iCheers = 1;
static const int iFrustrated = 2;
static const int iSad = 3;
static const int iNoFace = 4;

// Add lovely face?! Hearts!


FX partFx;


// TODO time the sad faces to the
// silent parts in the music.
// it is almost there by default!

static std::string noLines[] = {
    "       ",
    "FCCCF - Wii & Chill    ",
    "  "
};

static std::string hiya[] = {
    "Hiya, Muffintrap here!",
    "Cheers and greetings",
    "to everyone at Outline!",

    "I have been working",
    "on a Wii library,",
    "and wanted to do a",
    "little demo with it."};

static std::string quirky[] = {
    "The Wii can be a bit ",
    "quirky and I personally",
    "don't enjoy writing C++.",
    "So much work and null pointers."
};

static std::string modern[] = {
    "But I really like having",
    "a semi-modern, hardware",
    "platform to work with.",

    "It is also fun to make",
    "games for.",
    "Also, no shaders!", 
    "Shaders are confusing..."
    }; 

static std::string art[] = {
    "Sorry I did not have",
    "time to make the art",
    "and music myself.",
    "All you see and hear",
    "are free-to-use assets"
    };

static std::string together[] = {
    "I hope you enjoy this",
    "a e s t h e t i c",
    "I managed to put",
    "together."};

static std::string sceners[] = {
    "Greetings to other",
    "Nintendo sceners:"
};
static std::string names[] = {
    "Vurpo",
    "Halcy",
    "RaccoonViolet",
    "Mrs Beanbag",
    "Aldroid",
    "ToBach!"};

static std::string party[] = {
    "I hope you all",
    "have a great party!" };

float fullRotation = PI*2.0f;
static std::vector<Timer> parts =
{
    Timer(noLines, 3, 5.0f, iNoFace),
    Timer(hiya,7, 23.49f, iCheers, 0.5f, fullRotation*3),

    // how to get rotation to start from PI
    Timer(quirky,4, 6.0f, iSad, 1.0f, PI),

    Timer(modern,7, 23.75f, iActually, 0.5f, fullRotation),

    // Frutrated
    Timer(art, 5, 11.7f, iFrustrated, 1.5f, PI),

// These ae all together in a 46.second part
    Timer(together, 4, 17.0f, iCheers, 1.0f, PI),

    Timer(sceners, 2, 3.0f, iNoFace),
    Timer(names, 6, 20.0f, iNoFace),

    Timer(party, 2,  6.0f, iCheers, 0.7f, fullRotation)
};

Timer& GetPart(int partIndex)
{
    gdl_assert(
        (partIndex < parts.size()), 
        "Index outside parts array: %u/%u", partIndex, parts.size()
    );
    return parts[partIndex];
}

Template::Template()
{

}

float Template::GetProgress()
{
    return elapsed/demoDuration;
}

void Template::Init()
{

    // TODO combine these to one call
    fontImage.LoadImageBuffer(m6x11_png, m6x11_png_size, gdl::Nearest, gdl::RGBA8);
    font.BindSheet(fontImage, 22, 32, ' ');

    sky1.LoadImageBuffer(sky1_png, sky1_png_size, gdl::Nearest, gdl::RGBA8);

    // Maybe linear for moving parts
    sky2.LoadImageBuffer(sky2_png, sky2_png_size, gdl::Nearest, gdl::RGBA8);
    sky3.LoadImageBuffer(sky3_png, sky3_png_size, gdl::Nearest, gdl::RGBA8);
    sky4.LoadImageBuffer(sky4_png, sky4_png_size, gdl::Nearest, gdl::RGBA8);

    cheers.LoadImageBuffer(cheers_png, cheers_png_size, gdl::Linear, gdl::RGBA8);
    sad.LoadImageBuffer(sad_png, sad_png_size, gdl::Linear, gdl::RGBA8);
    frustrated.LoadImageBuffer(frustrated_png, frustrated_png_size, gdl::Linear, gdl::RGBA8);
    actually.LoadImageBuffer(actually_png, actually_png_size, gdl::Linear, gdl::RGBA8);

    vaporwave.LoadFromBuffer(askMeToStay_ogg, askMeToStay_ogg_size);
    /*
    mel_image.LoadImageBuffer(mel_tiles_png, mel_tiles_png_size, gdl::Nearest, gdl::RGBA8);
    short spritesPerRow = 2;
    short rows = 2;
    // TODO: Calculate these automaticall from sprite size.
    gdl::SpriteSetConfig cfg = mel_sprites.CreateConfig(spritesPerRow*rows, rows, spritesPerRow, 62,62);
    mel_sprites.LoadSprites(cfg, &mel_image);

    
    blip.LoadSound(blipSelect_wav, blipSelect_wav_size);
    */
    // menu = gdl::MenuCreator(&font, 1.5f, 1.2f);

    deltaTimeStart = gettime();
    programStart = deltaTimeStart;
    deltaTime = 0.0f;

    cloudX = 0.0f;
    cloudSpeed = 13.0f;
    cloudWidth = sky3.Xsize() * skyScale;

    farCloudX = -cloudWidth/2;
    farCloudSpeed = 0.5f;

    bigCloudSpeed = 1.0f;
    bigCloudX = sky4.Xsize()/3;

    printf("Demo init done\n");
    vaporwave.PlayMusic(false);
}

void Template::Update()
{
    UpdateTiming();


    cloudX -= cloudSpeed*deltaTime;
    if (cloudX <= -cloudWidth)
    {
        cloudX += cloudWidth;
    }

    farCloudX -= farCloudSpeed*deltaTime;
    if (farCloudX <= -cloudWidth)
    {
        farCloudX += cloudWidth;
    }

    bigCloudX -= bigCloudSpeed * deltaTime;

    Timer &part = GetPart(partIndex);
    part.Update(deltaTime);
    if (part.GetProgress() >= 1.0f)
    {
        partIndex += 1;
    }
    if (gdl::WiiInput::ButtonPress(WPAD_BUTTON_PLUS))
    {
        partIndex+=1;
        elapsed += (part.duration - part.elapsed);
    }
    if (partIndex >= parts.size())
    {
        partIndex = 0; // Reset to avoid out of range 
        showGreets = false;
    }

    drawFadeRectangle = false;
    // Do also fade inout from black?
    if (elapsed <= fadeInDuration)
    {
        float fade = elapsed/fadeInDuration;
        gdl::SetMusicVolume(fade * 100);
        drawFadeRectangle = true;
        fadeAlpha = 255 * (1.0f- fade);
    }
    else
    {
        float timeLeft = demoDuration - elapsed ;
        if ( timeLeft <= fadeOutDuration)
        {
            // Fade gets smaller!
            float fade = timeLeft/fadeOutDuration;
            gdl::SetMusicVolume(fade * 100);
            drawFadeRectangle = true;

            // Alpha must go up
            fadeAlpha = 0 + 255 * (1.0f-fade);
        }
    }
}

void DrawTextDouble(const char* text, short x, short y, float scale, gdl::FFont* font, 
int amount, int lastLetter, float lastLetterProgress)
{
    gdl_assert((lastLetter < strlen(text)), "DrawTextDouble index out of text %u>=%u", lastLetter, strlen(text));
    int wordWidth = font->GetWidth(text) * scale;
    int w = wordWidth/amount;
    char xx[2];
    int startX = x-wordWidth/2;
    int dx;
    int dy;
    for (int i = 0; i <= lastLetter; i++)
    {
        dx = startX + i * w;
        dy = y;
        if (i == lastLetter)
        {
            // Center the letter that is growing bigger
            // Add to halfway and go back
            dx += (font->GetWidth() * scale*0.5f) - (font->GetWidth()*scale*lastLetterProgress*0.5f);
            dy += (font->GetHeight()* scale *0.5f) - (font->GetWidth()*scale*lastLetterProgress*0.5f);
            scale *= lastLetterProgress;
        }
        gdl_assert((text[i]!=0), "DrawTextDouble tried to draw NULL character");
        sprintf(xx, "%c", text[i]);
        font->DrawText(xx, dx + 4, dy + 4, scale, palette[0]);
        font->DrawText(xx, dx, dy, scale, palette[1]);
    }
}

void Template::Draw()
{
    // Draw Sky and parallax clouds
    DrawClouds();

    // Draw part's effect
    /*
    partFx = parts[partIndex].effect;
    switch(partFx)
    {
        case FXparticles:
        break;

        case FXfire:
        break;

        case FXvoxelcube:
        break;

        case FXtunnel:
        break;

        case FXplasma:
        break;

        case FXbirds:
        break;
    };
    */

    if (showGreets)
    {
       DrawGreets();
    }

    // Input
    short top = 32;
    short left = 32;
    // DrawMenu(left, top + 120, 120);
    DrawTimingInfo(left, gdl::ScreenYres-font.GetHeight()*4*0.5f, 0.5f);

    // Cute animals?
    // DrawSprites();

    if (drawFadeRectangle)
    {
        u_int black = palette[0] - 255 + fadeAlpha;
        gdl::DrawBoxF(0, 0, gdl::ScreenXres, gdl::ScreenYres, black);
    }
}

// Rotation of 0 means that the full face is showing
// Over 90 means backside is showing
void Template::DrawFace(gdl::Image* face, float rotation)
{
    gdl_assert((face != nullptr), "Cannot draw null face!");
    while (rotation >= PI*2)
    {
        rotation -= PI*2;
    }
    // TODO: Animate faces talking?
    // Frame for face
    float w = face->Xsize();
    float h = face->Ysize();
    float cx = gdl::ScreenCenterX; 
    float cy = gdl::ScreenCenterY; 

    float rotationMulti = cos(rotation);
    float frameLeft = cx - w * faceScale/2 * rotationMulti;
    float frameRight = cx + w*faceScale/2 * rotationMulti;

    float frameTop = cy - h * faceScale/2;
    float frameBottom = cy + h*faceScale/2;

    gdl::DrawBoxFG(frameLeft, frameTop, frameRight , frameBottom, palette[3], palette[3], palette[11], palette[11]); 
    gdl::DrawBox(frameLeft-1, frameTop-1, frameRight+1, frameBottom+1, palette[8]); 

    // Different places for faces?
    if (rotation >= -PI/2 && rotation <= PI/2)
    {
        face->PutS(frameLeft, frameTop, frameRight, frameBottom,
            gdl::Color::White);
    }
}

void Template::DrawGreets()
{
    Timer& part = GetPart(partIndex);
    gdl::Image* face = nullptr;
    switch(part.faceIndex)
    {
        case iSad:
        face = &sad;
        break;
        case iCheers:
        face = &cheers;
        break;
        case iFrustrated:
        face = &frustrated;
        break;
        case iActually:
        face = &actually;
        break;
    }

    // Todo
    // when rotating, Draw previous face with +180 rotation to
    // make the flip
    float rotation = part.GetFaceRotation(); 

    float faceBottom = gdl::ScreenCenterY;
    if (face != nullptr)
    {
        DrawFace(face, rotation);
        faceBottom += face->Ysize()*faceScale/2;
    }
    else
    {
        // Draw text differently when 
        // no face?
    }

    std::string line = part.GetLine();

    DrawTextDouble(line.c_str(), 
        gdl::ScreenCenterX, faceBottom,
        1.0f, &font, 
        line.length(), std::floor(part.letterIndex), part.GetLetterProgress());
}
/*
void Template::DrawSprites()
{
    const gdl::Sprite* first = mel_sprites.SpriteInfo(0);
    float scale = 2.0f;
    short spriteW = first->w * scale;
    short spriteH = first->h * scale;
    short placeX = gdl::ScreenXres-spriteW;
    short placeY = 0;
    for (short i = 0; i < 4; i++)
    {
        mel_sprites.Put(placeX, placeY, i, gdl::Color::White, 0, 0, scale);
        placeY += spriteH;
    }
}
*/

// Create quads
// use the twister formula/code
void Template::DrawRibbons()
{
    /*
    float ease_duration = 60.0f;
    float width = 20.0f;
    float depth = 1.0f;
    float speed = 1.0f;
    float amplitude = 20.0f;
    u_int shades[] = {9, 8, 15, 3};
    */
}

void Template::UpdateTiming()
{
    u64 now = gettime();
    deltaTime = (double)(now - deltaTimeStart) / (double)(TB_TIMER_CLOCK * 1000); // division is to convert from ticks to seconds
    elapsed += deltaTime;//  (double)(now - programStart) / (double)(TB_TIMER_CLOCK * 1000); // division is to convert from ticks to seconds
    deltaTimeStart = now;
}
void Template::DrawTimingInfo(int x, int y, float scale)
{
    float ystep = font.GetHeight()*scale;
    font.Printf(x, y + ystep * 0, scale, palette[1], "Deltatime %f", deltaTime);
    font.Printf(x, y + ystep * 1, scale, palette[1], "Elapsed %f", elapsed);

    font.Printf(x, y + ystep * 2, scale, palette[1], "part %u/%u", partIndex, parts.size());
    Timer& part = GetPart(partIndex);
    font.Printf(x, y + ystep * 3, scale, palette[2], "progress %2.1f", part.GetProgress());
    /*

    ibmFont.Printf(x+4, y + ystep * 1+4, scale, gdl::Color::Black, "Normalized Deltatime: %f", gdl::Delta);
    ibmFont.Printf(x, y + ystep * 1, scale, gdl::Color::LightRed, "Normalized Deltatime: %f", gdl::Delta);

    ibmFont.Printf(x+4, y + ystep * 2+4, scale, gdl::Color::Black, "Elapsed milliseconds: %f", temp);
    ibmFont.Printf(x, y + ystep * 2, scale, gdl::Color::LightRed, "Elapsed milliseconds: %f", temp);

    ibmFont.Printf(x+4, y + ystep * 3+4, scale, gdl::Color::Black, "Music elapsed: %f", sampleMusic.GetElapsed());
    ibmFont.Printf(x, y + ystep * 3, scale, gdl::Color::LightRed, "Music elapsed: %f", sampleMusic.GetElapsed());
    */
}

void Template::DrawClouds()
{
    // Sky background
    sky1.Put(
            gdl::ScreenCenterX,
            gdl::ScreenCenterY,
            gdl::Color::White, 
            gdl::Centered, gdl::Centered, 
           skyScale, 0.0f);
    sky2.Put(
            gdl::ScreenCenterX,
            gdl::ScreenCenterY,
            gdl::Color::White, 
            gdl::Centered, gdl::Centered, 
           skyScale, 0.0f);

    // Far Clouds
    float farScale = skyScale *1.0;
    sky3.Put(
            gdl::ScreenCenterX+farCloudX,
            gdl::ScreenYres-sky3.Ysize()*farScale,
            palette[2],
            gdl::Centered, 0,
           farScale, 0.0f);

    float nextCloud = gdl::ScreenCenterX+farCloudX + cloudWidth;

    sky3.Put(nextCloud,
            gdl::ScreenYres-sky3.Ysize()*farScale,
            palette[2],
            gdl::Centered, 0,
           farScale, 0.0f);


    // Big cloud
    sky4.Put(
            gdl::ScreenCenterX+bigCloudX,
            gdl::ScreenCenterY,
            gdl::Color::White, 
            gdl::Centered, gdl::Centered, 
           skyScale, 0.0f);

    // Near Clouds
    sky3.Put(
            gdl::ScreenCenterX+cloudX,
            gdl::ScreenCenterY,
            gdl::Color::White, 
            gdl::Centered, gdl::Centered, 
           skyScale, 0.0f);

    nextCloud = gdl::ScreenCenterX+cloudX + cloudWidth;

    sky3.Put(nextCloud,
            gdl::ScreenCenterY,
            gdl::Color::White, 
            gdl::Centered, gdl::Centered, 
           skyScale, 0.0f);

}

// Use this for timing maybe?
/*
void Template::DrawMenu(int x, int y, int w)
{
    gdl::vec2 cp = gdl::WiiInput::GetCursorPosition();
    menu.StartMenu(x, y, w, cp.x, cp.y, gdl::WiiInput::ButtonPress(WPAD_BUTTON_A));

    menu.Text("Hi! I am menu.");
    menu.Panel(2, gdl::Color::Yellow);
    if (menu.Button("Play Ogg"))
    {
        sampleMusic.PlayMusic(true);
    }
    if (menu.Button("Play Sound"))
    {
        blip.Play(1.0f, 100.0f);
    }
    if (menu.Button("Fire Assert"))
    {
        gdl_assert(false, "Assert button pressed!");
    }
}
*/
