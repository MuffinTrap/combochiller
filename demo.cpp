#include "demo.h"

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

static float timer = 0.0f;
static float textSpeed = 5.0f;
static bool showGreets = true;

static int greetsIndex = 0;
static int greetsGoal = 0;
static int greetsDone = 0;
static int lineIndex = 0;
static float letterIndex = 0;

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


// TODO time the sad faces to the
// silent parts in the music.
// it is almost there by default!

// Face: amount of lines
static int lines[] = {
    iCheers,7,
    iSad,3,
    iActually, 4,
    iFrustrated, 3,
    iCheers, 4,
    iNoFace, 6,
    iCheers, 2
};

static std::string greets[] = {
    // Cheers
    "Hiya, Muffintrap here!",
    "Cheers and greetings",
    "to everyone at Outline!",
    "I have been working",
    "on a Wii library,",
    "and wanted to do a",
    "little demo with it.",

    // Sad
    "The Wii can be a bit ",
    "quirky and I personally",
    "don't enjoy writing C++.",

    // Actually
    "But I really like having",
    "a semi-modern, hardware",
    "platform to work with.",
    "Also, no shaders!",

    // Frustrated
    "Sorry I did not have",
    "time to make the art",
    "and music myself.",

    // Cheers
    "But I like this",
    "a e s t h e t i c",
    "I managed to put",
    "together.",

    // No face
    "Greetings to other",
    "Nintendo sceners:",
    "Vurpo - Halcy",
    "RaccoonViolet",
    "Mrs Beanbag",
    "Aldroid & ToBach!",

    // Cheers
    "I hope you all",
    "have a great party!"
};

Template::Template()
{

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

    // TODO combine these to one call
    fontImage.LoadImageBuffer(m6x11_png, m6x11_png_size, gdl::Nearest, gdl::RGBA8);
    font.BindSheet(fontImage, 28, 32, ' ');
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
    vaporwave.LoadFromBuffer(askMeToStay_ogg, askMeToStay_ogg_size);
    menu = gdl::MenuCreator(&font, 1.5f, 1.2f);

    // settime((u64)0); // Setting time to 0 crashes Dolphin!
    deltaTimeStart = gettime();
    programStart = deltaTimeStart;
    deltaTime = 0.0f;

    // 
    cloudX = 0.0f;
    cloudSpeed = 13.0f;
    cloudWidth = sky3.Xsize() * skyScale;

    bigCloudSpeed = 1.0f;
    bigCloudX = sky4.Xsize()/3;

    vaporwave.PlayMusic(false);
}

void Template::Update()
{
    cloudX -= cloudSpeed*deltaTime;
    if (cloudX <= -cloudWidth)
    {
        cloudX += cloudWidth;
    }
    bigCloudX -= bigCloudSpeed * deltaTime;
}

void DrawTextDouble(const char* text, short x, short y, float scale, gdl::FFont* font)
{
    font->DrawText(text, x-font->GetWidth(text)*scale/2+4, y+4, scale, gdl::Color::Black);
    font->DrawText(text, x-font->GetWidth(text)*scale/2, y, scale, gdl::Color::White);
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
    DrawTextDouble("Chill out", gdl::ScreenCenterX, font.GetHeight()*1, fontScale, &font);



    // Input
    short top = 32;
    short left = 32;
    // DrawMenu(left, top + 120, 120);
    DrawTimingInfo(left, gdl::ScreenYres-font.GetHeight()*4*0.5f, 0.5f);

    // Cute animals?
    // DrawSprites();
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
    int faceIndex = lines[lineIndex];
    greetsGoal = lines[lineIndex+1];
    gdl::Image* face = nullptr;
    switch(faceIndex)
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
    DrawTextDouble(greets[greetsDone + greetsIndex].c_str(), 
    gdl::ScreenCenterX, gdl::ScreenCenterY+face->Ysize()*faceScale/2,
    1.0f, &font);

    // Timing
    int letters = greets[greetsDone + greetsIndex].length();
    letterIndex += textSpeed * deltaTime;
    if (letterIndex > letters)
    {
        letterIndex = 0.0f;
        greetsIndex += 1;
        if (greetsIndex == greetsGoal)
        {
            greetsDone += greetsGoal;
            greetsIndex = 0;
            lineIndex += 2;
        }
    }
    if (lineIndex >= sizeof(lines))
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
    // float temp=ticks_to_millisecs(now - programStart);
    deltaTime = (double)(now - deltaTimeStart) / (double)(TB_TIMER_CLOCK * 1000); // division is to convert from ticks to seconds
    deltaTimeStart = now;
    float ystep = font.GetHeight()*scale;
    font.Printf(x+4, y + ystep * 0+4, scale, gdl::Color::Black, "Deltatime %f", deltaTime);
    font.Printf(x, y + ystep * 0, scale, gdl::Color::LightRed, "Deltatime %f", deltaTime);

    font.Printf(x+4, y + ystep * 1+4, scale, gdl::Color::Black, "LetterIndex %2.1f", letterIndex);
    font.Printf(x, y + ystep * 1, scale, gdl::Color::LightRed, "LetterIndex %2.1f", letterIndex);
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
