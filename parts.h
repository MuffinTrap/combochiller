#pragma once
#include "timer.h"
#include "fx.h"
#include <mgdl-wii.h>

enum PartName
{
    partIntro = 0,
    partHiya = 1,
    partQuirky = 2,
    partModern = 3,
    partTogether = 4,
    partArt = 5,
    partSceners = 6,
    partNames = 7,
    partParty = 8,
    partEnding = 9
};

static std::string intro[] = {
    "       ",
    "FCCCF - Wii & Chill"
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
    "The Wii can be",
    "a bit quirky",
    "and C++ means",
    "lots of typing &",
    "null pointers."
};

static std::string modern[] = {
    "But I like having",
    "a semi-modern hardware",
    "platform to play with.",
    "It is also fun to make",
    "games for.",
    "Also, no shaders!", 
    "Shaders are confusing..."
    };

static std::string together[] = {
    "I hope you enjoy this",
    "a e s t h e t i c",
    "I managed to put",
    "together."
    }; 

static std::string art[] = {
    "Sorry I did not",
    "have time to",
    "make the art",
    "and music.",
    "All you see",
    "and hear",
    "are free-to-use",
    "assets."
    };


static std::string sceners[] = {
    "Greetings to all",
    "Nintendo sceners!"
};
static std::string names[] = {
    "Vurpo",
    "Halcy",
    "RaccoonViolet",
    "Mrs Beanbag",
    "Aldroid",
    "ToBach",
    " "}; // Otherwise last fruit shows under h

static std::string party[] = {
    "I hope you all",
    "  have a great party!" };

static std::string ending[] = {
    "   "
};

float fullRotation = PI*2.0f;
static std::vector<Timer> parts =
{
    Timer(intro, 2, 1, 5.0f, iNoFace, 0, 0),
    Timer(hiya,7, 2, 23.49f, iCheers, 0.5f, fullRotation*2),

    // how to get rotation to start from PI
    // How to do the null pointer effect?
    Timer(quirky,5, 5, 6.0f, iSad, 1.0f, PI),

    Timer(modern,7, 2, 15.75f, iActually, 0.5f, fullRotation),

    Timer(together, 4, 2, 7.0f, iCheers, 1.0f, PI),

    // Frustrated
    Timer(art, 8, 4, 11.7f, iFrustrated, 1.5f, PI),

    // These are all together in a 46.second part

    Timer(sceners, 2, 2, 8.0f, iNoFace, 0, 0, 4.0f),
    Timer(names, 7, 7, 15.0f, iNoFace, 0, 0, 6.0f),
    Timer(party, 2, 2,  10.0f, iHeart, 0.7f, fullRotation, 6.0f),

    // Over the end
    Timer(ending,1,1,28.0f, iHeart,1.4,fullRotation)
};


static int partIndex = 0;
Timer& GetPart(int index)
{
    if (index < 0)
    {
        return parts.front();
    }
    else if ((u_int)index >= parts.size())
    {
        return parts.back();
    }
    else 
    {
        return parts[index];
    }
}

void SetPart(int index)
{
    if (index < 0)
    {
        partIndex = 0;
    }
    else if ((u_int)index >= parts.size())
    {
        partIndex = parts.size()-1;
    }
    else 
    {
        partIndex = index;
    }
}

void PartsSetup(Vector2 faceDimensions, float faceScale, Vector2 facePosition)
{
    // Fruits to all lines in names
    Timer& nms = parts[partNames];
    nms.AddLineEffect(LineEffect(0, LineFX_FRUITS, 14)); // Vurpo - blue
    nms.AddLineEffect(LineEffect(1, LineFX_FRUITS, 9)); // Halcy - rose
    nms.AddLineEffect(LineEffect(2, LineFX_FRUITS, 15)); // Raccoon - violet
    nms.AddLineEffect(LineEffect(3, LineFX_FRUITS, 10)); // BeanBag - yellow and blue
    nms.AddLineEffect(LineEffect(3, LineFX_FRUITS, 13)); // BeanBag - yellow and blue
    nms.AddLineEffect(LineEffect(4, LineFX_FRUITS, 12)); // Aldroid - green
    nms.AddLineEffect(LineEffect(5, LineFX_FRUITS, 11)); // ToBach - orange
    nms.AddLineEffect(LineEffect(6, LineFX_FRUITS, 11)); // ToBach - orange

    // Face position targets
    GetPart(partIntro).facePositionTarget = faceposition;
    GetPart(partIntro).textCenter = Vector2(gdl::ScreenCenterX, gdl::ScreenCenterY);

    GetPart(partHiya).facePositionTarget = faceposition;  // Hiya
    GetPart(partHiya).effect = FXparticles;

    // Quirky
    int quart = gdl::ScreenXres/4.0f;
    GetPart(partQuirky).facePositionTarget =  Vector2(gdl::ScreenCenterX + quart, faceposition.y); 
    GetPart(partQuirky).textCenter =  Vector2(gdl::ScreenCenterX - quart, faceposition.y);
    GetPart(partQuirky).AddLineEffect(LineEffect(4, LineFX_NULL, 0));
    GetPart(partQuirky).effect = FXglitch;

    // Modern
    GetPart(partModern).facePositionTarget = faceposition;
    // Together
    GetPart(partTogether).facePositionTarget = faceposition;
    // Art
    GetPart(partArt).facePositionTarget = Vector2(gdl::ScreenCenterX-quart, faceposition.y);
    GetPart(partArt).textCenter = Vector2(gdl::ScreenCenterX+quart, faceposition.y);

    // Sceners, names, party
    GetPart(partSceners).facePositionTarget = faceposition;
    GetPart(partSceners).textCenter = Vector2(gdl::ScreenCenterX, gdl::ScreenCenterY);
    GetPart(partSceners).effect = FXplasma;

    GetPart(partNames).facePositionTarget = faceposition;
    GetPart(partNames).textCenter = Vector2(gdl::ScreenCenterX, gdl::ScreenCenterY);
    GetPart(partNames).effect = FXfruits;

    GetPart(partParty).facePositionTarget = Vector2(gdl::ScreenCenterX, gdl::ScreenCenterY);
    GetPart(partParty).effect = FXtunnel;

    GetPart(partEnding).facePositionTarget = Vector2(gdl::ScreenCenterX, gdl::ScreenYres*-2.0f);
    GetPart(partEnding).effect = FXtunnel;
}