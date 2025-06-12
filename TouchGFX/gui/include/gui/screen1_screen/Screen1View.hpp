#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>
#include <touchgfx/Bitmap.hpp>
#include <touchgfx/widgets/Image.hpp>

struct Piece {
    uint8_t shape[4][4];
    int color;
};

const int colors[7] = {0x00BFFF, 0xFFFF00, 0xFF00FF, 0x00FF00, 0x800080, 0xFF0000, 0x00FFFF};
const char piecePool[7] = {'Z', 'S', 'L', 'J', 'O', 'T', 'I'};

#define MAX_BLOCKS 300
extern volatile uint8_t buttonLeftPressed;
extern volatile uint8_t buttonRightPressed;
extern volatile uint8_t buttonRotatePressed;
extern volatile uint8_t buttonHardDropPressed;

class Screen1View : public Screen1ViewBase
{
public:
    Screen1View();
    virtual ~Screen1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void updatePiecePosition();
    virtual void spawnNewPiece();
    virtual void movePiece();
    virtual void movePieceDown();
    virtual void movePieceLeft();
    virtual void movePieceRight();
    virtual void rotatePiece();
    virtual void hardDropPiece();
    virtual void handleTickEvent();
    uint32_t customRandom();
	void initRandom();
	int getRandomPiece();

protected:
    int grid[24][10];
    Piece currentPiece;
    int pieceX, pieceY;
    int score;
    int fallSpeed;
    int tempFallSpeed;
    bool waitingForSpawn;
    int spawnDelayCounter;
    bool gameOver;
    bool movedDown, movedLeft, movedRight, rotated, hardDrop;
    char piece;
    int tickCounter;
    int gameOverDelayCounter;
    int counter;
    uint32_t randomSeed;

    // Thêm array blocks và blockCount
    touchgfx::Image blocks[MAX_BLOCKS];
    int blockCount;

    void generatePiece(char pieceType);
    void rotateClockwise(uint8_t matrix[4][4]);
    void handleGameLogic();
    bool checkCollision();
    bool checkCollisionDown();
    bool checkCollisionLeft();
    bool checkCollisionRight();
    void savePieceToGrid();
    void clearLines();
    void updateScreen();
    void drawBlock(int x, int y, int color);
    void adjustFallSpeed();
};
#endif // SCREEN1VIEW_HPP
