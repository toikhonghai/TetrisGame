#include <gui/screen2_screen/Screen2View.hpp>

Screen2View::Screen2View()
{

}

void Screen2View::setupScreen()
{
    Screen2ViewBase::setupScreen();
    buttonPlayClick = 0;

}

void Screen2View::tearDownScreen()
{
    Screen2ViewBase::tearDownScreen();
}
void Screen2View::handleTickEvent()
{
    if (buttonPlayClick == 1)
    {
        buttonPlayClick = 0; // reset cờ tránh bị lặp
        gotoGameScreen();    // gọi hàm chuyển màn hình
    }
}
void Screen2View::gotoGameScreen()
{
	application().gotoScreen1ScreenBlockTransition();

}
