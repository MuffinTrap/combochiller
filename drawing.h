#pragma once

#include <mgdl-wii.h>
#include "fx.h"
#include "timer.h"
#include "pastelpalette.h"


void DrawTextDouble(std::string text, short x, short y, float scale, gdl::FFont* font, 
int lastLetter, float lastLetterProgress, u_int color, int letterWidth = -1)
{
    gdl_assert(((u_int)lastLetter < text.length()), "DrawTextDouble index out of text %u>=%u", lastLetter, text.length());
    int lw = letterWidth;
    if (lw < 0)
    {
        lw = font->GetWidth();
    }
    int wordWidth = lw * text.length() * scale;
    int w = wordWidth/text.length();
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
            float hw = lw * scale * 0.5f;
            float hh = font->GetHeight()* scale * 0.5f;
            dx += (hw) - (hw*lastLetterProgress);
            dy += (hh) - (hh*lastLetterProgress);
            scale *= lastLetterProgress;
        }
        gdl_assert((text[i]!=0), "DrawTextDouble tried to draw NULL character");
        sprintf(xx, "%c", text[i]);

        // Move the shadow with line progress

        font->DrawText(xx, dx + 4*scale, dy + 4*scale, scale, palette[0]);
        font->DrawText(xx, dx, dy, scale, color);
    }
}

void DrawFruitsDouble(std::string text, short x, short y, float scale, gdl::SpriteSet* sprites, 
int firstLetter, int lastLetter, u_int spriteIndex)
{
    gdl_assert(((u_int)lastLetter < text.length()), "DrawFruitsDouble index out of text %u>=%u", lastLetter, text.length());

    const gdl::Sprite* first = sprites->SpriteInfo(0);
    short spriteW = first->w * scale;
    int wordWidth = text.length() * spriteW;
    int w = wordWidth/text.length();
    int startX = x-wordWidth/2;

    int dx;
    int dy;
    for (int i = firstLetter; i <= lastLetter; i++)
    {
        if (text[i] == ' ')
        {
            continue;
        }
        dx = startX + i * w;
        dy = y;
        sprites->Put(dx + 4*scale, dy + 4*scale, spriteIndex, palette[0], 0,0, scale);
        sprites->Put(dx, dy, spriteIndex, gdl::Color::White, 0,0, scale);
    }

}