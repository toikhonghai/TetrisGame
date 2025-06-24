#include <gui/screen1_screen/Screen1View.hpp>
#include <touchgfx/Utils.hpp>
#include <touchgfx/Bitmap.hpp>
#include <BitmapDatabase.hpp>
#include <cstdlib>
#include <stdio.h>
#include <main.h>
#include "cstring"
#include <cmsis_os.h>
#include <stdint.h>


extern osMessageQueueId_t myQueue01Handle;
extern TIM_HandleTypeDef htim3;

SoundNote gameOverNotes[] = {
	    {659, 150}, // E5
	    {587, 150}, // D5
	    {523, 150}, // C5
	    {494, 150}, // B4
	    {440, 150}, // A4
	    {392, 150}, // G4
	    {349, 200}, // F4
	    {392, 300}, // G4
	    {0,   100}, // nghỉ
	    {262, 500}  // C4 - kết thúc
	};
SoundNote lineClearSound[] = {
    { 523, 80 },
    { 659, 80 },
    { 784, 100 },
};

size_t screen1NoteIndex = 0;
bool playingGameOverSequence = false;

#ifndef HIGHSCORE_FLASH_ADDR
#define HIGHSCORE_FLASH_ADDR 0x080E0000 // Sector 11, STM32F429ZIT6
#endif

void Screen1View::startSound(uint32_t freq, uint32_t durationMs) {
    uint32_t period = 1000000 / freq;
    __HAL_TIM_SET_AUTORELOAD(&htim3, period - 1);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, period / 2); // 50% duty

    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

    soundStartTime = HAL_GetTick();
    soundDuration = durationMs;
    isPlayingSound = true;
}
void Screen1View::playLeftMoveSound() {
    startSound(600, 30);
}

void Screen1View::playRightMoveSound() {
    startSound(700, 30);
}

void Screen1View::playRotateSound() {
    startSound(900, 50);
}

void Screen1View::playHardDropSound() {
    startSound(400, 80);
}
void Screen1View::playGameOverSound() {
    playingGameOverSequence = true;
    screen1NoteIndex = 0;
}

Screen1View::Screen1View()
{
    initRandom();
    for (int i = 0; i < 24; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            grid[i][j] = 0;
        }
    }

    pieceX = 4;
    pieceY = 0;
    score = 0;

    // highestScore = 0;
    fallSpeed = 40;

    tempFallSpeed = fallSpeed;
    waitingForSpawn = false;
    spawnDelayCounter = 0;
    gameOver = false;
    movedDown = false;
    movedLeft = false;
    movedRight = false;
    rotated = false;
    hardDrop = false;
    tickCounter = 0;
    gameOverDelayCounter = 0;
    blockCount = 0;
    nextBlockCount = 0;
    check = true;

    piece = piecePool[getRandomPiece()];
    generatePiece(piece);
    nextPiece = currentPiece;
    piece = piecePool[getRandomPiece()];

    generatePiece(piece);
}

void Screen1View::initRandom()
{
    randomSeed = HAL_GetTick();
    if (randomSeed == 0)
        randomSeed = 12345;

    randomSeed ^= (randomSeed << 13);
    randomSeed ^= (randomSeed >> 17);
    randomSeed ^= (randomSeed << 5);
}

uint32_t Screen1View::customRandom()
{
    randomSeed = randomSeed * 1103515245 + 12345;
    return (randomSeed / 65536) % 32768;
}

int Screen1View::getRandomPiece()
{
    return customRandom() % 7;
}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
    updateScreen();


}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View::generatePiece(char pieceType)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            currentPiece.shape[i][j] = 0;
        }
    }
    switch (pieceType)
    {
    case 'Z':
        currentPiece.color = colors[0];
        currentPiece.shape[0][1] = 1;
        currentPiece.shape[1][1] = 1;
        currentPiece.shape[1][2] = 1;
        currentPiece.shape[2][2] = 1;
        break;
    case 'S':
        currentPiece.color = colors[1];
        currentPiece.shape[1][1] = 1;
        currentPiece.shape[2][1] = 1;
        currentPiece.shape[0][2] = 1;
        currentPiece.shape[1][2] = 1;
        break;
    case 'L':
        currentPiece.color = colors[2];
        currentPiece.shape[2][0] = 1;
        currentPiece.shape[2][1] = 1;
        currentPiece.shape[1][2] = 1;
        currentPiece.shape[2][2] = 1;
        break;
    case 'J':
        currentPiece.color = colors[3];
        currentPiece.shape[1][0] = 1;
        currentPiece.shape[1][1] = 1;
        currentPiece.shape[1][2] = 1;
        currentPiece.shape[2][2] = 1;
        break;
    case 'O':
        currentPiece.color = colors[4];
        currentPiece.shape[1][1] = 1;
        currentPiece.shape[2][1] = 1;
        currentPiece.shape[1][2] = 1;
        currentPiece.shape[2][2] = 1;
        break;
    case 'T':
        currentPiece.color = colors[5];
        currentPiece.shape[0][1] = 1;
        currentPiece.shape[1][1] = 1;
        currentPiece.shape[2][1] = 1;
        currentPiece.shape[1][2] = 1;
        break;
    case 'I':
        currentPiece.color = colors[6];
        currentPiece.shape[1][0] = 1;
        currentPiece.shape[1][1] = 1;
        currentPiece.shape[1][2] = 1;
        currentPiece.shape[1][3] = 1;
        break;
    }
}

bool Screen1View::checkCollision()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (currentPiece.shape[i][j])
            {
                int x = pieceX + j;
                int y = pieceY + i;
                if (x < 0 || x >= 10 || y >= 23 || (y >= 0 && grid[y][x]))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Screen1View::checkCollisionDown()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (currentPiece.shape[i][j])
            {
                int x = pieceX + j;
                int y = pieceY + i + 1;
                if (y >= 23 || (y >= 0 && grid[y][x]))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Screen1View::checkCollisionLeft()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (currentPiece.shape[i][j])
            {
                int x = pieceX + j - 1;
                int y = pieceY + i;
                if (x < 0 || (y >= 0 && y < 24 && grid[y][x]))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Screen1View::checkCollisionRight()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (currentPiece.shape[i][j])
            {
                int x = pieceX + j + 1;
                int y = pieceY + i;
                if (x >= 10 || (y >= 0 && y < 24 && grid[y][x]))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

void Screen1View::savePieceToGrid()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (currentPiece.shape[i][j])
            {
                int x = pieceX + j;
                int y = pieceY + i;
                if (y >= 0 && y < 24 && x >= 0 && x < 10)
                {
                    grid[y][x] = currentPiece.color;
                }
            }
        }
    }
}

void Screen1View::spawnNewPiece()
{
    if (check)
    {
        tmpPiece = currentPiece;

        piece = piecePool[getRandomPiece()];

        generatePiece(piece);
        nextPiece = currentPiece;
        currentPiece = tmpPiece;
        drawNextBlock();
        updateScreen();
        check = false;
    }
    if (waitingForSpawn)
    {
        spawnDelayCounter++;
        if (spawnDelayCounter >= 30)
        {
            currentPiece = nextPiece;
            pieceX = 4;
            pieceY = 0;
            waitingForSpawn = false;
            spawnDelayCounter = 0;

            //            piece = piecePool[getRandomPiece()];
            //            generatePiece(piece);
            //            nextPiece = currentPiece;

            currentPiece = nextPiece;
            if (checkCollision())
            {
                gameOver = true;
            }
            check = true;
        }
    }
}

void Screen1View::clearLines()
{
    for (int y = 23; y >= 0; y--)
    {
        bool fullLine = true;
        for (int x = 0; x < 10; x++)
        {
            if (grid[y][x] == 0)
            {
                fullLine = false;
                break;
            }
        }
        if (fullLine)
        {
            for (int yy = y; yy > 0; yy--)
            {
                for (int x = 0; x < 10; x++)
                {
                    grid[yy][x] = grid[yy - 1][x];

                }
            }
            for (int x = 0; x < 10; x++)
            {
                grid[0][x] = 0;
            }
            //playLineClearSound();
            score += 1;
            y++;
            adjustFallSpeed();
        }
    }
}

void Screen1View::adjustFallSpeed()
{

    int newFallSpeed = 40 - (score) * 4;

    fallSpeed = newFallSpeed < 10 ? 10 : newFallSpeed;
}

void Screen1View::rotateClockwise(uint8_t matrix[4][4])
{
    uint8_t temp[4][4];
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            temp[j][i] = matrix[i][j];
        }
    }
    // Đảo ngược từng hàng
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            matrix[i][j] = temp[i][3 - j];
        }
    }
}

void Screen1View::handleGameLogic()
{
    clearLines();

    if (movedDown)
    {
        if (!checkCollisionDown())
        {
            pieceY++;
        }
        else
        {
            savePieceToGrid();
            waitingForSpawn = true;
            spawnDelayCounter = 0;
            //            score += 1;
        }
        movedDown = false;
    }

    if (hardDrop)
    {
        while (!checkCollisionDown())
        {
            pieceY++;
            updateScreen(); // Cập nhật màn hình để hiển thị chuyển động
        }
        savePieceToGrid();
        waitingForSpawn = true;
        spawnDelayCounter = 0;
        //        score += 2;

        hardDrop = false;
    }

    if (movedLeft)
    {
        if (!checkCollisionLeft())
        {
            pieceX--;
        }
        movedLeft = false;
    }

    if (movedRight)
    {
        if (!checkCollisionRight())
        {
            pieceX++;
        }
        movedRight = false;
    }

    if (rotated)
    {
        uint8_t temp[4][4];
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                temp[i][j] = currentPiece.shape[i][j];
            }
        }
        rotateClockwise(currentPiece.shape);
        int originalX = pieceX, originalY = pieceY;
        bool valid = false;

        // Kiểm tra vị trí hiện tại
        if (!checkCollision())
        {
            valid = true;
        }
        // Thử dịch trái 1
        else
        {
            pieceX--;
            if (!checkCollision())
            {
                valid = true;
            }
            else
            {
                pieceX = originalX;
            }
        }
        // Thử dịch phải 1
        if (!valid)
        {
            pieceX++;
            if (!checkCollision())
            {
                valid = true;
            }
            else
            {
                pieceX = originalX;
            }
        }
        // Thử dịch lên 1
        if (!valid)
        {
            pieceY--;
            if (!checkCollision())
            {
                valid = true;
            }
            else
            {
                pieceY = originalY;
            }
        }

        // Nếu vẫn va chạm, khôi phục trạng thái ban đầu
        if (!valid)
        {
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    currentPiece.shape[i][j] = temp[i][j];
                }
            }
        }
        rotated = false;
    }
}

void Screen1View::drawBlock(int x, int y, int color)
{
    if (blockCount >= MAX_BLOCKS)
    {
        return;
    }

    touchgfx::Image &block = blocks[blockCount];

    if (color == 0x00BFFF)
    {
        block.setBitmap(touchgfx::Bitmap(BITMAP_BLUE_BLOCK_ID));
    }
    else if (color == 0xFFFF00)
    {
        block.setBitmap(touchgfx::Bitmap(BITMAP_YELLOW_BLOCK_ID));
    }
    else if (color == 0xFF00FF)
    {
        block.setBitmap(touchgfx::Bitmap(BITMAP_PINK_BLOCK_ID));
    }
    else if (color == 0x00FF00)
    {
        block.setBitmap(touchgfx::Bitmap(BITMAP_GREEN_BLOCK_ID));
    }
    else if (color == 0x800080)
    {
        block.setBitmap(touchgfx::Bitmap(BITMAP_PURPLE_BLOCK_ID));
    }
    else if (color == 0xFF0000)
    {
        block.setBitmap(touchgfx::Bitmap(BITMAP_RED_BLOCK_ID));
    }
    else if (color == 0x00FFFF)
    {
        block.setBitmap(touchgfx::Bitmap(BITMAP_CYAN_BLOCK_ID));
    }
    else
    {
        block.setBitmap(touchgfx::Bitmap(BITMAP_BLUE_BLOCK_ID));
    }
    block.setPosition(x * 14, y * 14, 14, 14);
    containerFallArea.add(block);
    blockCount++;
}

void Screen1View::drawNextBlock()
{
    nextBlock.removeAll();
    nextBlockCount = 0;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (nextPiece.shape[i][j] && nextBlockCount < 16)
            {
                touchgfx::Image &block = nextBlockImages[nextBlockCount];
                if (nextPiece.color == 0x00BFFF)
                {
                    block.setBitmap(touchgfx::Bitmap(BITMAP_NBLUE_BLOCK_ID));
                }
                else if (nextPiece.color == 0xFFFF00)
                {
                    block.setBitmap(touchgfx::Bitmap(BITMAP_NYELLOW_BLOCK_ID));
                }
                else if (nextPiece.color == 0xFF00FF)
                {
                    block.setBitmap(touchgfx::Bitmap(BITMAP_NPINK_BLOCK_ID));
                }
                else if (nextPiece.color == 0x00FF00)
                {
                    block.setBitmap(touchgfx::Bitmap(BITMAP_NGREEN_BLOCK_ID));
                }
                else if (nextPiece.color == 0x800080)
                {
                    block.setBitmap(touchgfx::Bitmap(BITMAP_NPURPLE_BLOCK_ID));
                }
                else if (nextPiece.color == 0xFF0000)
                {
                    block.setBitmap(touchgfx::Bitmap(BITMAP_NRED_BLOCK_ID));
                }
                else if (nextPiece.color == 0x00FFFF)
                {
                    block.setBitmap(touchgfx::Bitmap(BITMAP_NCYAN_BLOCK_ID));
                }
                else
                {
                    block.setBitmap(touchgfx::Bitmap(BITMAP_NBLUE_BLOCK_ID));
                }
                block.setPosition(j * 9, i * 9, 9, 9);
                nextBlock.add(block);
                nextBlockCount++;
            }
        }
    }
    nextBlock.invalidate();
}

void Screen1View::updateScreen()
{
    containerFallArea.removeAll();
    blockCount = 0;
    for (int i = 0; i < 24; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (grid[i][j] && blockCount < MAX_BLOCKS)
            {
                drawBlock(j, i, grid[i][j]);
            }
        }
    }
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (currentPiece.shape[i][j] && blockCount < MAX_BLOCKS)
            {
                int x = pieceX + j;
                int y = pieceY + i;
                if (y >= 0 && y < 24 && x >= 0 && x < 10)
                {
                    drawBlock(x, y, currentPiece.color);
                }
            }
        }
    }

    containerFallArea.invalidate();
    Unicode::snprintf(textScoreBuffer, TEXTSCORE_SIZE, "%d", score);
    textScore.setWildcard1(textScoreBuffer);
    textScore.invalidate();

    uint32_t flashHighScore = readHighScoreFromFlash();
    Unicode::snprintf(highScoreBuffer, HIGHSCORE_SIZE, "%d", flashHighScore);
    highScore.setWildcard1(highScoreBuffer);
    highScore.invalidate();

    if (gameOver)
    {
        gameOverDelayCounter = 0;
    }
}

void Screen1View::movePiece()
{
    // (Không thay đổi)
}

void Screen1View::updatePiecePosition()
{
    if (!waitingForSpawn && !gameOver)
    {
        movedDown = true;
        handleGameLogic();
        updateScreen();
    }
}

void Screen1View::movePieceDown()
{
    movedDown = true;
    handleGameLogic();
    updateScreen();
}

void Screen1View::movePieceLeft()
{
    movedLeft = true;
    handleGameLogic();
    updateScreen();
}

void Screen1View::movePieceRight()
{
    movedRight = true;
    handleGameLogic();
    updateScreen();
}

void Screen1View::rotatePiece()
{
    rotated = true;
    handleGameLogic();
    updateScreen();
}

void Screen1View::hardDropPiece()
{
    hardDrop = true;
    handleGameLogic();
    updateScreen();
}

void Screen1View::handleTickEvent()
{
    if (!gameOver)
    {
        char buttonEvent;
        while (osMessageQueueGet(myQueue01Handle, &buttonEvent, NULL, 0) == osOK)
        {
            switch (buttonEvent)
            {
            case 'A':
                movePieceLeft();
                playLeftMoveSound();
                break;
            case 'B':
                movePieceRight();
                playRightMoveSound();
                break;
            case 'C':
                rotatePiece();
                playRotateSound();
                break;
            case 'E':
                fallSpeed = 5;
                movePieceDown();
                playHardDropSound();
                break;
            }
            if (buttonEvent == 'E')
            {
                fallSpeed = tempFallSpeed;
            }
        }

        tickCounter++;
        if (tickCounter >= fallSpeed)
        {
            updatePiecePosition();
            tickCounter = 0;
        }
        spawnNewPiece();
    }
    else
    {
        if (!playingGameOverSequence)
        {
            gameOverDelayCounter++;
            if (gameOverDelayCounter >= 300)
            {
                // Bắt đầu phát nhạc game over
                playingGameOverSequence = true;
                screen1NoteIndex = 0;
                startSound(gameOverNotes[screen1NoteIndex].freq, gameOverNotes[screen1NoteIndex].duration);
                screen1NoteIndex;

                // Lưu điểm cao nếu cần
                uint32_t flashHighScore = readHighScoreFromFlash();
                if (score > flashHighScore)
                {
                    saveHighScoreToFlash(score);
                }
            }
        }
    }

    // Dừng âm khi hết thời gian
    if (isPlayingSound && (HAL_GetTick() - soundStartTime >= soundDuration))
    {
        HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
        isPlayingSound = false;
    }

    // Phát tiếp note trong chuỗi âm thanh game over
    if (playingGameOverSequence && !isPlayingSound)
    {
        if (screen1NoteIndex < (sizeof(gameOverNotes) / sizeof(SoundNote)))
        {
            startSound(gameOverNotes[screen1NoteIndex].freq, gameOverNotes[screen1NoteIndex].duration);
            screen1NoteIndex++;
        }
        else
        {
            playingGameOverSequence = false;
            gotoGameOverScreen();  // 👉 CHỈ chuyển màn hình sau khi phát xong
        }
    }
}

void Screen1View::gotoGameOverScreen()
{
    application().gotoScreen3ScreenBlockTransition();
}

uint32_t Screen1View::readHighScoreFromFlash()
{
    uint32_t value = *(uint32_t *)HIGHSCORE_FLASH_ADDR;
    if (value == 0xFFFFFFFF || value > 1000000)
        return 0;
    return value;
}

void Screen1View::saveHighScoreToFlash(uint32_t newHighScore)
{
    // Implement the logic to save the new high score to flash
    // This is a placeholder and should be replaced with the actual implementation
}
