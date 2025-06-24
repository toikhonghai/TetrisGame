#include <gui/screen2_screen/Screen2View.hpp>
#include <cmsis_os.h>
#include "main.h"

extern osMessageQueueId_t myQueue01Handle;
extern TIM_HandleTypeDef htim3;

const Note screen2LoopNotes[] = { { 659, 150 }, // E5
		{ 587, 150 }, // D5
		{ 523, 150 }, // C5
		{ 494, 150 }, // B4
		{ 440, 150 }, // A4
		{ 392, 150 }, // G4
		{ 349, 200 }, // F4
		{ 392, 300 }, // G4
		{ 0, 100 }, // nghỉ
		{ 262, 500 }  // C4 - kết thúc
};

const int screen2NoteCount = sizeof(screen2LoopNotes)
		/ sizeof(screen2LoopNotes[0]);

int current2NoteIndex = 0;
bool isNotePlaying = false;
uint32_t noteStartTick = 0;
void Screen2View::playNote(uint16_t freq, uint16_t duration) {
	uint32_t period = 1000000 / freq;
	__HAL_TIM_SET_AUTORELOAD(&htim3, period - 1);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, period / 2);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

	noteStartTick = HAL_GetTick();
	isNotePlaying = true;
}
Screen2View::Screen2View() {
}

void Screen2View::setupScreen() {
	Screen2ViewBase::setupScreen();
	current2NoteIndex = 0;
}

void Screen2View::tearDownScreen() {
	Screen2ViewBase::tearDownScreen();
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
	isNotePlaying = false;
}
void Screen2View::handleTickEvent() {
	char buttonEvent;
	if (osMessageQueueGet(myQueue01Handle, &buttonEvent, NULL, 0) == osOK) {
		switch (buttonEvent) {
		case 'C':
			gotoGameScreen();
			break;
		}
	}
	if (!isNotePlaying) {
		const Note &note = screen2LoopNotes[current2NoteIndex];
		playNote(note.freq, note.duration);
	} else if (HAL_GetTick() - noteStartTick
			>= screen2LoopNotes[current2NoteIndex].duration) {
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
		isNotePlaying = false;
		current2NoteIndex = (current2NoteIndex + 1) % screen2NoteCount;
	}
}
void Screen2View::gotoGameScreen() {
	application().gotoScreen1ScreenBlockTransition();
}
