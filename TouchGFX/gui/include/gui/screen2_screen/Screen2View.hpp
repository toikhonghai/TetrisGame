#ifndef SCREEN2VIEW_HPP
#define SCREEN2VIEW_HPP

#include <gui_generated/screen2_screen/Screen2ViewBase.hpp>
#include <gui/screen2_screen/Screen2Presenter.hpp>


struct Note {
    uint16_t freq;
    uint16_t duration;
};

extern const Note screen2LoopNotes[];     // không static ở đây
extern const int screen2NoteCount;
extern int current2NoteIndex;
extern bool isNotePlaying;

class Screen2View : public Screen2ViewBase
{
public:
    Screen2View();
    virtual ~Screen2View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void playNote(uint16_t freq, uint16_t duration);

    void gotoGameScreen();
    void handleTickEvent() override;
protected:
};

#endif // SCREEN2VIEW_HPP
