#include "demo.h"
#include "timer.h"
#include "fx.h"
#include "drawing.h"

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
#include "font8x16_png.h"

#include "sad_png.h"
#include "cheers_png.h"
#include "frustrated_png.h"
#include "actually_png.h"
#include "heart_png.h"

#include "fruits_png.h"

#include "askMeToStay_ogg.h"

#include "pastelpalette.h"

// Timing
static u64 deltaTimeStart = 0;
static u64 programStart = 0;
static float demoDuration = 108.0f;
static float elapsed = 0.0f;

// Sky
static float skyScale = 2.0f;
static float cloudWidth;

// Parts
static bool showGreets = true;
FX partFx; // Currently active effect

// Fade
static bool drawFadeRectangle = true;
static int fadeAlpha = 255;
static float fadeOutDuration = 8.0f;
static float fadeInDuration = 3.0f;


// Faces
static Vector2 faceposition;
static float faceScale = 0.5f;

static Vector2 defaultTextPosition;
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
static const int iHeart = 5;

// TODO: Add lovely face?! Hearts!

#include "parts.h"

static Particles particleEffect = Particles();
static Plasma fruitPlasma = Plasma();


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
    ibmImage.LoadImageBuffer(font8x16_png, font8x16_png_size, gdl::Nearest, gdl::RGBA8);
    ibm.BindSheet(ibmImage, 8, 16, ' ');

    sky1.LoadImageBuffer(sky1_png, sky1_png_size, gdl::Nearest, gdl::RGBA8);

    // Maybe linear for moving parts
    sky2.LoadImageBuffer(sky2_png, sky2_png_size, gdl::Nearest, gdl::RGBA8);
    sky3.LoadImageBuffer(sky3_png, sky3_png_size, gdl::Nearest, gdl::RGBA8);
    sky4.LoadImageBuffer(sky4_png, sky4_png_size, gdl::Nearest, gdl::RGBA8);

    cheers.LoadImageBuffer(cheers_png, cheers_png_size, gdl::Linear, gdl::RGBA8);
    sad.LoadImageBuffer(sad_png, sad_png_size, gdl::Linear, gdl::RGBA8);
    frustrated.LoadImageBuffer(frustrated_png, frustrated_png_size, gdl::Linear, gdl::RGBA8);
    actually.LoadImageBuffer(actually_png, actually_png_size, gdl::Linear, gdl::RGBA8);
    heart.LoadImageBuffer(heart_png, heart_png_size, gdl::Linear, gdl::RGBA8);

    fruits.LoadImageBuffer(fruits_png, fruits_png_size, gdl::Nearest, gdl::RGBA8);
    gdl::SpriteSetConfig cfg = fruitSprites.CreateConfig(5, 16,16);
    fruitSprites.LoadSprites(cfg, &fruits);

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

    faceScale = 0.75f;
    faceposition = Vector2(gdl::ScreenCenterX, cheers.Ysize()*faceScale/2 + font.GetHeight());
    PartsSetup(Vector2(cheers.Xsize(), cheers.Ysize()), faceScale, faceposition);

    // Text and effects


    // Starting face position under the screen so cheers comes up
    faceposition = Vector2(gdl::ScreenCenterX, gdl::ScreenYres*2);
    defaultTextPosition = Vector2(gdl::ScreenCenterX, font.GetHeight() + cheers.Ysize()*faceScale);


    // Init effects
    particleEffect.Init();
    particleEffect.aliveTime = GetPart(partHiya).duration/2.0f;

    fruitPlasma.Init((std::floor(gdl::ScreenXres/4)-40)/2, (gdl::ScreenYres-40)/2);
    fruitPlasma.speed = 1.4f;

    vaporwave.PlayMusic(false);
}

u_int ColorToFruit(u_int color)
{
    switch(color)
    {
        case 9: return 8; // Peach
        case 10:  return 7; // Yellow apple
        case 13: return 0;
        case 11: return 4; // Banana
        case 12: return 1; // Pear
        case 14: return 0; // Blueberry
        case 15: return 13; // Grapefruit

        default: return 6; // Strawberry
    }
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
        if ((u_int)(partIndex + 1) >= parts.size())
        {
            partIndex = 0; // Reset to avoid out of range 
            showGreets = false;
        }
        else
        {
            SetPart(partIndex + 1);
        }
    }
    switch(part.effect)
    {
        case FXparticles:
            particleEffect.spawnPoint = Vector2(faceposition.x, faceposition.y - cheers.Ysize());
            particleEffect.Update(deltaTime);
        break;

        case FXplasma:
        case FXfruits:
            fruitPlasma.Update(deltaTime);
            break;

        default:
        break;
    }

    // SKIP PARTS

    if (gdl::WiiInput::ButtonPress(WPAD_BUTTON_PLUS))
    {
        SetPart(partIndex + 1);
        elapsed += (part.duration - part.elapsed);
        vaporwave.JumpToSeconds(elapsed);
    }
    if (gdl::WiiInput::ButtonPress(WPAD_BUTTON_MINUS))
    {
        // Reset current
        Timer &part = GetPart(partIndex);
        elapsed -= (part.elapsed);
        part.ResetTimers();

        // Reset previous
        SetPart(partIndex-1);
        part = GetPart(partIndex);
        elapsed -= (part.duration);
        part.ResetTimers();
        vaporwave.JumpToSeconds(elapsed);
    }

    // FADE

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
            if (fade < 0.0f)
            {
                fade = 0.0f;
            }
            gdl::SetMusicVolume(fade * 100);
            drawFadeRectangle = true;

            // Alpha must go up
            fadeAlpha = 0 + 255 * (1.0f-fade);
        }
    }
}

void Template::Draw()
{
    FX partFx = GetPart(partIndex).effect;
    // Draw Sky and parallax clouds
    DrawClouds(partFx);

    // Draw part's effect
    switch(partFx)
    {
        case FXparticles:
        particleEffect.Draw();
        break;

        case FXfire:
        break;

        case FXvoxelcube:
        break;

        case FXtunnel:
        break;

        case FXfruits:
        case FXplasma: 
            fruitPlasma.Draw(20,20, false);
            fruitPlasma.Draw(gdl::ScreenCenterX+gdl::ScreenXres/4+20, 20, true);
        break;

        case FXbirds:
        break;

        case FXglitch:
        break;
    };

    if (showGreets)
    {
       DrawGreets();
    }
    if (drawFadeRectangle)
    {
        u_int black = palette[0] - 255 + fadeAlpha;
        gdl::DrawBoxF(0, 0, gdl::ScreenXres, gdl::ScreenYres, black);
    }


    // DEBUG
    // DrawSprites();
    // DrawMenu(left, top + 120, 120);
    DrawTimingInfo(32, gdl::ScreenYres-font.GetHeight()*4*0.5f, 0.5f);
}

// Rotation of 0 means that the full face is showing
// Over 90 means backside is showing
void Template::DrawFace(gdl::Image* face, float rotation, Vector2 target)
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


    // Move face towards target
    float dx = target.x - faceposition.x;
    float dy = target.y - faceposition.y;
    faceposition.x += dx * 0.1f;
    faceposition.y += dy * 0.1f;

    float cx = faceposition.x;
    float cy = faceposition.y;

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

void Template::DrawLinesNoFX(Timer& part)
{
    int textX = defaultTextPosition.x;
    int textY = defaultTextPosition.y;
    if (part.textCenter.x != 0 && part.textCenter.y != 0)
    {
        textX = part.textCenter.x;
        textY = part.textCenter.y;
    }
    // Inactive line settings
    float prog = 1.0f;
    u_int color = 7;
    float scale = 1.0f;
    for (int l = 1; l <= part.showAmount; l++)
    {
        // Remember that this is a negative number :D
        int relative = (-part.showAmount) + l;
        std::string line = part.GetLineAt(relative);
        if (line.empty())
        {
            continue;
        }

        int last = line.length()-1;
        if (relative == 0)
        {
            // Drawing the latest line
            last = std::floor(part.letterIndex);
            prog = part.GetLetterProgress();
            color = 1;
            scale = 1.0f;
            if (part.GetLineFXAt(relative) == LineFX_NULL)
            {
                color = part.GetLineEffectAt(relative).color;
            }
        }
        DrawTextDouble(
            line,
            textX,
            textY + (part.showAmount -1 + relative) * font.GetHeight(),
            scale, &font, 
            last, prog, palette[color]);
    }
}

void Template::DrawLinesFruit(Timer& part)
{
    float fruitScale = 2.0f; // Fruit 16px, font 32px
    float fruitLetterScale = 1.0f;
    int fruitSize = 16*fruitScale;
    float rowHeight = fruitSize * 1.5f;
    int fruitLinesStartY = gdl::ScreenCenterY + rowHeight * 3;

    for (int fl = 0; fl < part.amount; fl++)
    {
        std::string line = part.GetLineEx(fl);
        const LineEffect& effect = part.GetLineEffectEx(fl);
        if (fl > part.greetsIndex)
        {
            // Ahead of letters, draw line of fruits
            DrawFruitsDouble(
                line,
                gdl::ScreenCenterX,
                fruitLinesStartY + (-6 + fl) * rowHeight,
                fruitScale, &fruitSprites, 
                0, line.length()-1, ColorToFruit(effect.color));
        }
        else if (fl == part.greetsIndex)
        {
            // Draw fruits that do not have letters yet
            int first = std::floor(part.letterIndex);
            DrawFruitsDouble(
                line,
                gdl::ScreenCenterX,
                fruitLinesStartY + (-6 + fl)* rowHeight,
                fruitScale, &fruitSprites, 
                first, line.length()-1, ColorToFruit(effect.color));
        }
        else {
            // Do not draw any fruits
        }
    }

    // Draw letters
    // Inactive line settings
    float prog = 1.0f;
    u_int color = 7;
    for (int l = 0; l < part.showAmount; l++)
    {
        if (l > part.greetsIndex)
        {
            // not there yet
            break;
        }
        std::string line = part.GetLineEx(l);
        const LineEffect& effect = part.GetLineEffectEx(l);
        int last = line.length()-1;
        color = palette[effect.color];
        if (l == part.greetsIndex)
        {
            // Drawing the latest line
            last = std::floor(part.letterIndex);
            prog = part.GetLetterProgress();
            color = palette[1]; // White
        }
        DrawTextDouble(
            line,
            gdl::ScreenCenterX, 
            fruitLinesStartY + (-6 + l)* rowHeight,
            fruitLetterScale, &font, 
            last, prog, color,
            fruitSize); 
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

        case iHeart:
            face = &heart;
        break;
    }

    // Todo
    // when rotating, Draw previous face with +180 rotation to
    // make the flip
    float rotation = part.GetFaceRotation(); 

    float faceBottom = faceposition.y;
    if (face != nullptr)
    {
        DrawFace(face, rotation, part.facePositionTarget);
        faceBottom += face->Ysize()*faceScale/2;
    }
    else
    {
        // Draw text differently when 
        // no face?
    }
    switch(part.effect)
    {
        case FXfruits:
            DrawLinesFruit(part);
        break;

        case FXnone:
        case FXglitch:
        default:
            DrawLinesNoFX(part);
        break;
    };

}
void Template::DrawSprites()
{
    const gdl::Sprite* first = fruitSprites.SpriteInfo(0);
    float scale = 2.0f;
    short spriteW = first->w * scale;
    short spriteH = first->h * scale;
    short placeX = gdl::ScreenXres-spriteW;
    short placeY = 0;
    for (short i = 0; i < 15; i++)
    {
        ibm.Printf(placeX - spriteW*2, placeY, scale, gdl::Color::White, "%d:", i);
        fruitSprites.Put(placeX, placeY, i, gdl::Color::White, 0, 0, scale);
        placeY += spriteH;
    }
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

static std::string bluescreen[] = {
    ":(",
    "It seems that I have crashed.",
    "But no worry!",
    "It's just a little segfault.",
    "----------------------------------------------",
    "Just a little segfault?!",
    "You have no legs left!"
};

void Template::DrawClouds(FX effect)
{
    if (effect == FXglitch)
    {
        gdl::DrawBoxF(0, 0, gdl::ScreenXres, gdl::ScreenYres, gdl::Color::Blue);
        int y = 16;
        float s = 1.0;
        int r = ibm.GetHeight() * s;
        for (int i = 0; i < 7; i++)
        {
            // TODO IBM font
            ibm.DrawText(bluescreen[i].c_str(), 16, y + i*r, s, palette[PastelWhite]);
        }
    }
    else 
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
    }

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
