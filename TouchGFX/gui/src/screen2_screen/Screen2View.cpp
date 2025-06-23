#include <gui/screen2_screen/Screen2View.hpp>
#include <cmsis_os.h>

extern osMessageQueueId_t myQueue01Handle;
Screen2View::Screen2View()
{
}

void Screen2View::setupScreen()
{
    Screen2ViewBase::setupScreen();
}

void Screen2View::tearDownScreen()
{
    Screen2ViewBase::tearDownScreen();
}
void Screen2View::handleTickEvent()
{
    char buttonEvent;
    if (osMessageQueueGet(myQueue01Handle, &buttonEvent, NULL, 0) == osOK)
    {
        switch (buttonEvent)
        {
        case 'C':
            gotoGameScreen();
            break;
        }
    }
}
void Screen2View::gotoGameScreen()
{
    application().gotoScreen1ScreenBlockTransition();
}
