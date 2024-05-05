#pragma once

#include <mgdl-wii.h>

class Template
{
    gdl::Image sad;
    gdl::Image cheers;
    gdl::Image frustrated;
    gdl::Image actually;
    gdl::SpriteSet muffin_sprites;
    gdl::Image fontImage;
    gdl::Image sky1;
    gdl::Image sky2;
    gdl::Image sky3;
    gdl::Image sky4;
    gdl::FFont font;
    gdl::Music vaporwave;
    gdl::Sound vhsPlaySound;
    gdl::MenuCreator menu;

    public:
        Template();
        void Init();
        void Update();
        void Draw();

    private:
        void DrawMenu(int x, int y, int width);
        void DrawInputInfo(int x, int y);
        void DrawTimingInfo(int x, int y, float scale);
        void DrawSprites();
        void DrawGreets();
        void DrawFace(gdl::Image* face);

        void DrawRibbons();

        float cloudSpeed;
        float bigCloudSpeed;
        float bigCloudX;
        float cloudX;
        double deltaTime;
};
