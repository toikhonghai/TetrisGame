#include <gui/screen3_screen/Screen3View.hpp>

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
	if (buttonRetryClick == 1)
	    {
	        buttonRetryClick = 0;
	        gotoPlayScreen();
	    }
	else if(buttonExitClick == 1){
		buttonExitClick = 0;
		gotoHomeScreen();
	}
}
void Screen3View::gotoPlayScreen(){
	application().gotoScreen1ScreenBlockTransition();
}
void Screen3View::gotoHomeScreen(){
	application().gotoScreen2ScreenBlockTransition();
}
