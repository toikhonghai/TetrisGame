#include <gui/screen3_screen/Screen3View.hpp>
#include <cmsis_os.h>

extern osMessageQueueId_t myQueue01Handle;
Screen3View::Screen3View()
{

}

void Screen3View::setupScreen()
{
    Screen3ViewBase::setupScreen();
    buttonRetryClick = 0;
    buttonExitClick = 0;
}

void Screen3View::tearDownScreen()
{
    Screen3ViewBase::tearDownScreen();
}
void Screen3View::handleTickEvent(){
	char buttonEvent;
	if (osMessageQueueGet(myQueue01Handle, &buttonEvent, NULL, 0) == osOK)
	{
		switch (buttonEvent)
		{
			case 'A': // Retry
				gotoPlayScreen();
				break;
			case 'B': // Exit
				gotoHomeScreen();
				break;
		}
	}
}
void Screen3View::gotoPlayScreen(){
	application().gotoScreen1ScreenBlockTransition();
}
void Screen3View::gotoHomeScreen(){
	application().gotoScreen2ScreenBlockTransition();
}
