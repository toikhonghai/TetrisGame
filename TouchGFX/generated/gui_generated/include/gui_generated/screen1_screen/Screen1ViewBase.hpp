/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#ifndef SCREEN1VIEWBASE_HPP
#define SCREEN1VIEWBASE_HPP

#include <gui/common/FrontendApplication.hpp>
#include <mvp/View.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>
#include <touchgfx/widgets/Box.hpp>
#include <touchgfx/widgets/ScalableImage.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/widgets/BoxWithBorder.hpp>
#include <touchgfx/widgets/TextAreaWithWildcard.hpp>
#include <touchgfx/widgets/TextArea.hpp>

class Screen1ViewBase : public touchgfx::View<Screen1Presenter>
{
public:
    Screen1ViewBase();
    virtual ~Screen1ViewBase();
    virtual void setupScreen();

protected:
    FrontendApplication& application() {
        return *static_cast<FrontendApplication*>(touchgfx::Application::getInstance());
    }

    /*
     * Member Declarations
     */
    touchgfx::Box __background;
    touchgfx::ScalableImage scalableImage1;
    touchgfx::Container containerPlayArea;
    touchgfx::BoxWithBorder boxPlayArea;
    touchgfx::Container containerFallArea;
    touchgfx::TextAreaWithOneWildcard textScore;
    touchgfx::Container nextBlock;
    touchgfx::BoxWithBorder boxWithBorder1;
    touchgfx::TextArea HighScore;
    touchgfx::TextAreaWithOneWildcard highScore;

private:

};

#endif // SCREEN1VIEWBASE_HPP
