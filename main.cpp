#include <mgdl-wii.h>
#include <wiiuse/wpad.h>

#include "mgdl-input-wii.h"
#include "demo.h"

void init()
{
    fatInitDefault();
	gdl::InitSystem(gdl::ModeAuto, gdl::AspectAuto, gdl::HiRes);
    gdl::SetClearColor(gdl::Color::Black);
    gdl::WiiInput::Init();
    gdl::ConsoleMode();
}

int main()
{
    init();
    {
        Template temp = Template();
        temp.Init();

        gdl::ConsoleMode();
        //Uncomment to see console messages
        //before game starts
        /*
        while(1)
        {
            gdl::WiiInput::StartFrame();
            if (gdl::WiiInput::ButtonPress(WPAD_BUTTON_A)){
                break;
            }
            VIDEO_WaitVSync();
        }
        */
        while(1)
        {
            gdl::WiiInput::StartFrame();

            if (gdl::WiiInput::ButtonPress(WPAD_BUTTON_HOME)){
                gdl::wii::Exit();
            }

            temp.Update();

            gdl::PrepDisplay();
            if (temp.GetProgress() <= 1.0f)
            {
                temp.Draw();
            }
            else {
                gdl::wii::Exit();
            }
            gdl::Display();

        }
    }
}