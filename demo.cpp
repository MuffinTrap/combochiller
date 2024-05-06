#include "demo.h"
#include "timer.h"

#include <wiiuse/wpad.h>
#include "mgdl-input-wii.h"
#include <string>

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


// TODO time the sad faces to the
// silent parts in the music.
// it is almost there by default!

static std::string noLines[] = {""};

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
    "don't enjoy writing C++."
};

static std::string modern[] = {
    "But I really like having",
    "a semi-modern, hardware",
    "platform to work with.",
    "Also, no shaders!" }; 

static std::string art[] = {
    "Sorry I did not have",
    "time to make the art",
    "and music myself." };

static std::string together[] = {
    "But I like this",
    "a e s t h e t i c",
    "I managed to put",
    "together."};

static std::string names[] = {
    "Greetings to other",
    "Nintendo sceners:",
    "Vurpo - Halcy",
    "RaccoonViolet",
    "Mrs Beanbag",
    "Aldroid & ToBach!"};

static std::string party[] = {
    "I hope you all",
    "have a great party!" };

static Timer parts[] =
{
    Timer(noLines, 1, 5.0f, iNoFace),
    Timer(hiya,7, 23.49f, iCheers),

    Timer(quirky,3, 6.0f, iSad),

    Timer(modern,4, 23.75f, iActually),

    // Frutrated
    Timer(art, 3, 11.7f, iFrustrated),

// These ae all together in a 46.second part
    Timer(together, 4, 17.0f, iCheers),

    Timer(names, 6, 23.0f, iNoFace),

    Timer(party, 2,  6.0f, iCheers)
};
Template::Template()
{

}

float Template::GetProgress()
{
    return elapsed/demoDuration;
}

void Template::Init()
{
    sky1.LoadImageBuffer(sky1_png, sky1_png_size, gdl::Nearest, gdl::RGBA8);

    // Maybe linear for moving parts
    sky2.LoadImageBuffer(sky2_png, sky2_png_size, gdl::Nearest, gdl::RGBA8);
    sky3.LoadImageBuffer(sky3_png, sky3_png_size, gdl::Nearest, gdl::RGBA8);
    sky4.LoadImageBuffer(sky4_png, sky4_png_size, gdl::Nearest, gdl::RGBA8);

    cheers.LoadImageBuffer(cheers_png, cheers_png_size, gdl::Linear, gdl::RGBA8);
    sad.LoadImageBuffer(sad_png, sad_png_size, gdl::Linear, gdl::RGBA8);
    frustrated.LoadImageBuffer(frustrated_png, frustrated_png_size, gdl::Linear, gdl::RGBA8);
    actually.LoadImageBuffer(actually_png, actually_png_size, gdl::Linear, gdl::RGBA8);
    printf("faces loaded\n");

    // TODO combine these to one call
    fontImage.LoadImageBuffer(m6x11_png, m6x11_png_size, gdl::Nearest, gdl::RGBA8);
    font.BindSheet(fontImage, 22, 32, ' ');
    /*
    mel_image.LoadImageBuffer(mel_tiles_png, mel_tiles_png_size, gdl::Nearest, gdl::RGBA8);
    short spritesPerRow = 2;
    short rows = 2;
    // TODO: Calculate these automaticall from sprite size.
    gdl::SpriteSetConfig cfg = mel_sprites.CreateConfig(spritesPerRow*rows, rows, spritesPerRow, 62,62);
    mel_sprites.LoadSprites(cfg, &mel_image);

    pointerImage.LoadImageBuffer(pointer_png, pointer_png_size, gdl::Nearest, gdl::RGBA8);


    
    blip.LoadSound(blipSelect_wav, blipSelect_wav_size);
    */
    printf("Load music\n");
    vaporwave.LoadFromBuffer(askMeToStay_ogg, askMeToStay_ogg_size);
    printf("Music loaded\n");
    menu = gdl::MenuCreator(&font, 1.5f, 1.2f);
    printf("Menu created\n");

    // settime((u64)0); // Setting time to 0 crashes Dolphin!
    deltaTimeStart = gettime();
    programStart = deltaTimeStart;
    deltaTime = 0.0f;
    printf("Time set\n");

    // 
    cloudX = 0.0f;
    cloudSpeed = 13.0f;
    cloudWidth = sky3.Xsize() * skyScale;

    bigCloudSpeed = 1.0f;
    bigCloudX = sky4.Xsize()/3;

    vaporwave.PlayMusic(false);
    printf("Demo init done\n");
}

void Template::Update()
{
    cloudX -= cloudSpeed*deltaTime;
    if (cloudX <= -cloudWidth)
    {
        cloudX += cloudWidth;
    }
    bigCloudX -= bigCloudSpeed * deltaTime;

    parts[partIndex].Update(deltaTime);

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
            float fade = timeLeft/fadeOutDuration;
            gdl::SetMusicVolume(fade * 100);
            drawFadeRectangle = true;
            fadeAlpha = 255 * (fade-1.0f);
        }
    }
}

void DrawTextDouble(const char* text, short x, short y, float scale, gdl::FFont* font, 
int amount, int lastLetter, float lastLetterProgress)
{
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
        sprintf(xx, "%c", text[i]);
        font->DrawText(xx, dx + 4, dy + 4, scale, palette[0]);
        font->DrawText(xx, dx, dy, scale, palette[1]);
    }
}

void Template::Draw()
{
    // Draw Sky and parallax clouds

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

    // Big cloud
    sky4.Put(
            gdl::ScreenCenterX+bigCloudX,
            gdl::ScreenCenterY,
            gdl::Color::White, 
            gdl::Centered, gdl::Centered, 
           skyScale, 0.0f);

    // Clouds
    sky3.Put(
            gdl::ScreenCenterX+cloudX,
            gdl::ScreenCenterY,
            gdl::Color::White, 
            gdl::Centered, gdl::Centered, 
           skyScale, 0.0f);

    float nextCloud = gdl::ScreenCenterX+cloudX + cloudWidth;

    sky3.Put(nextCloud,
            gdl::ScreenCenterY,
            gdl::Color::White, 
            gdl::Centered, gdl::Centered, 
           skyScale, 0.0f);

    float fontScale = 2.0f;

    if (showGreets)
    {
        DrawGreets();
    }


    // ? Animate this?
    // DrawTextDouble("Chill out", gdl::ScreenCenterX, font.GetHeight()*1, fontScale, &font, 



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

void Template::DrawFace(gdl::Image* face)
{
    // TODO: Animate faces talking?
    // Frame for face
    float w = face->Xsize();
    float h = face->Ysize();
    float frameLeft = gdl::ScreenCenterX- w * faceScale/2;
    float frameTop = gdl::ScreenCenterY - h * faceScale/2;
    gdl::DrawBoxFG(frameLeft, frameTop, frameLeft + w*faceScale, frameTop+h*faceScale, palette[3], palette[3], palette[11], palette[11]); 
    gdl::DrawBox(frameLeft-1, frameTop-1, frameLeft + w*faceScale+1, frameTop+h*faceScale+1, palette[8]); 

    // Different places for faces?
    face->Put(gdl::ScreenCenterX, gdl::ScreenCenterY, 
        gdl::Color::White, 
        gdl::Centered, gdl::Centered, 
        faceScale, 0.0f);

}

void Template::DrawGreets()
{
    Timer& part = parts[partIndex];
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

    // TODO
    // animate face change
    if (face != nullptr)
    {
        DrawFace(face);
    }
    else
    {
        // Draw text differently when 
        // no face?
    }

    // TODO animate text
    // Draw letters closer together
    std::string line = part.GetLine();
    DrawTextDouble(line.c_str(), 
    gdl::ScreenCenterX, gdl::ScreenCenterY+face->Ysize()*faceScale/2,
    1.0f, &font, 
    line.length(), std::floor(part.letterIndex), part.GetLetterProgress());

    // Timing
    if (part.GetProgress()>= 1.0f)
    {
        partIndex += 1;
    }
    if (partIndex >= sizeof(parts))
    {
        showGreets = false;
    }
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
    float ease_duration = 60.0f;
    float width = 20.0f;
    float depth = 1.0f;
    float speed = 1.0f;
    float amplitude = 20.0f;
    u_int shades[] = {9, 8, 15, 3};
}

void Template::DrawTimingInfo(int x, int y, float scale)
{
    u64 now = gettime();
    elapsed = (double)(now - programStart) / (double)(TB_TIMER_CLOCK * 1000); // division is to convert from ticks to seconds
    deltaTime = (double)(now - deltaTimeStart) / (double)(TB_TIMER_CLOCK * 1000); // division is to convert from ticks to seconds
    deltaTimeStart = now;
    float ystep = font.GetHeight()*scale;
    font.Printf(x, y + ystep * 0, scale, palette[1], "Deltatime %f", deltaTime);
    font.Printf(x, y + ystep * 1, scale, palette[1], "Elapsed %f", elapsed);

    if (partIndex < sizeof(parts))
    {
        font.Printf(x, y + ystep * 2, scale, palette[4], "progress %2.1f", parts[partIndex].GetProgress());
    }
    /*

    ibmFont.Printf(x+4, y + ystep * 1+4, scale, gdl::Color::Black, "Normalized Deltatime: %f", gdl::Delta);
    ibmFont.Printf(x, y + ystep * 1, scale, gdl::Color::LightRed, "Normalized Deltatime: %f", gdl::Delta);

    ibmFont.Printf(x+4, y + ystep * 2+4, scale, gdl::Color::Black, "Elapsed milliseconds: %f", temp);
    ibmFont.Printf(x, y + ystep * 2, scale, gdl::Color::LightRed, "Elapsed milliseconds: %f", temp);

    ibmFont.Printf(x+4, y + ystep * 3+4, scale, gdl::Color::Black, "Music elapsed: %f", sampleMusic.GetElapsed());
    ibmFont.Printf(x, y + ystep * 3, scale, gdl::Color::LightRed, "Music elapsed: %f", sampleMusic.GetElapsed());
    */
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
