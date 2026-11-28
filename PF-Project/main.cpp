#include <SFML/Graphics.hpp>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;
using namespace sf;

const int ROWS = 20;
const int COLS = 10;
const float CELL = 30.f;
const float BOARD_X = 20.f;
const float BOARD_Y = 20.f;

const int START_ROW = 0;    // a new piece starts here
const int START_COL = 3;
const int LINES_PER_LEVEL = 10;
const int LINE_POINTS[5] = { 0, 100, 300, 500, 800 };

const float PANEL_X = 350.f;
const int START_DELAY = 30;
const int DELAY_STEP = 5;
const int MIN_DELAY = 5;
const int GAME_OVER = 2;

int board[ROWS][COLS];
char pieces[7][4][4];

char shape[4][4];       
int currentPiece;       
int pieceRow, pieceCol;

int score, lines, level;
int nextPiece;
int fallDelay;
int frameCounter;
int gameState;

bool piecesLoaded;
bool fontLoaded;
bool gameover; //for temporary running

Font font;
Color colors[8] = {
    Color(25, 25, 45),      // 0: empty
    Color(0, 255, 255),     // 1: I (cyan)
    Color(255, 255, 0),     // 2: O (yellow)
    Color(170, 90, 255),    // 3: T (purple)
    Color(0, 255, 0),       // 4: S (green)
    Color(255, 0, 0),       // 5: Z (red)
    Color(0, 0, 255),       // 6: J (blue)
    Color(255, 150, 0)      // 7: L (orange)
};

void setupGame();
void handleInput(RenderWindow& window);
void drawGame(RenderWindow& window);
bool loadFont();
void drawCell(RenderWindow& window, float x, float y, Color color);
void drawText(RenderWindow& window, const string& words, float x, float y, int size, Color color = Color(255, 255, 255));

// TASK 1
bool loadPieces();
void drawBoard(RenderWindow& window);
void drawError(RenderWindow& window);

// TASK 2
void loadShape(int pieceNumber);
void drawPiece(RenderWindow& window);


// TASK 4
int clearLines();
void addScore(int cleared);

// TASK 5
void updateSpeed();
void endGame();
void drawMiniPiece(RenderWindow& window, int pieceNumber, float x, float y);
void drawSidePanel(RenderWindow& window);
void clearSaveFile();


bool canPlace(char s[4][4], int row, int col);
void moveLeft();
void moveRight();
void moveDown();

void rotatePiece();



// TASK 4 ITSELF
int clearLines()
{
    int cleared = 0;        // keeps track of how many rows are cleared
    int r = ROWS - 1;       // R start from the very bottom row

    while (r >= 0) {
        // check if row r is full runs till R 
        bool full = true;
        for (int c = 0; c < COLS; c++) {
            if (board[r][c] == 0)
            {
                full = false;
            }
        }

        if (full) {
            // move every row above it down by one
            for (int row = r; row > 0; row--) {
                for (int c = 0; c < COLS; c++)
                {
                    board[row][c] = board[row - 1][c];
                }
            }

            // top row becomes empty
            for (int c = 0; c < COLS; c++) {
                board[0][c] = 0;
            }

            cleared++;
            // we don't do r-- here, because the row that just
            // moved down into r could be full too so to accomodate for that clear will inc 
        }
        else {
            r--;  // r decrements here at the end
        }
    }

    return cleared;
}
void addScore(int cleared)
{
    if (cleared == 0) { //checks if no lines were cleared exists function immediately then
        return;
    }

    score += LINE_POINTS[cleared];  // adds points based on how many lines were cleared to score
    lines += cleared;
    level = lines / LINES_PER_LEVEL + 1;
    updateSpeed();
}
// TASK 5
// level 1 is 30 frames per drop, then 25, 20 and so on, but never below 5
void updateSpeed()
{
    fallDelay = START_DELAY - (level - 1) * DELAY_STEP;

    if (fallDelay < MIN_DELAY)
    {
        fallDelay = MIN_DELAY;
    }
}

void endGame()
{
    gameState = GAME_OVER;
    clearSaveFile();        // so a finished game can't be continued
}

// small version of a piece, used for the NEXT box
void drawMiniPiece(RenderWindow& window, int pieceNumber, float x, float y)
{
    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 4; c++)
        {
            if (pieces[pieceNumber][r][c] == '#')
            {
                RectangleShape square({ 18.f, 18.f });
                square.setFillColor(colors[pieceNumber + 1]);
                square.setPosition({ x + c * 20.f, y + r * 20.f });
                window.draw(square);
            }
        }
    }
}

void drawSidePanel(RenderWindow& window)
{
    drawText(window, "SCORE", PANEL_X, 30, 22);
    drawText(window, to_string(score), PANEL_X, 58, 28, Color(255, 255, 0));

    drawText(window, "LINES", PANEL_X, 110, 22);
    drawText(window, to_string(lines), PANEL_X, 138, 28, Color(255, 255, 0));

    drawText(window, "LEVEL", PANEL_X, 190, 22);
    drawText(window, to_string(level), PANEL_X, 218, 28, Color(255, 255, 0));

    drawText(window, "NEXT", PANEL_X, 275, 22);
    drawMiniPiece(window, nextPiece, PANEL_X, 310);
}
void clearSaveFile()
{
    // TODO 7: teammate C fills this in
}

int main()
{
    RenderWindow window(VideoMode({ 520, 640 }), "Tetris");
    window.setFramerateLimit(60);

    setupGame();

    while (window.isOpen())
    {
        handleInput(window);
        drawGame(window);
    }
    return 0;
}

void setupGame()
{
    piecesLoaded = loadPieces();
    fontLoaded = loadFont();

    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            board[r][c] = 0;
        }
    }
    //added for temporary test
    currentPiece = 2;
    loadShape(currentPiece);
    pieceRow = 0;
    pieceCol = 3;
}

void handleInput(RenderWindow& window)
{
    while (const optional event = window.pollEvent())
    {
        if (event->is<Event::Closed>())
        {
            window.close();
        }
        else if (const auto* key = event->getIf<Event::KeyPressed>())
        {
            // To check my moving functions
            if (key->code == Keyboard::Key::Left)
            {
                moveLeft();
            }
            else if (key->code == Keyboard::Key::Right)     
            {
                moveRight();
            }
            else if (key->code == Keyboard::Key::Down)
            {
                moveDown();
            }
            else if (key->code == Keyboard::Key::Up)
            {
                rotatePiece();
            }
        }
    }
}

void drawGame(RenderWindow& window)
{
    window.clear(Color(15, 15, 30));

    if (!piecesLoaded)
    {
        drawError(window);
    }
    else
    {
        drawBoard(window);
        drawPiece(window);
    }

    window.display();
}

bool loadFont()
{
    if (font.openFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        return true;
    }
    if (font.openFromFile("arial.ttf"))
    {
        return true;
    }
    return false;
}

void drawCell(RenderWindow& window, float x, float y, Color color)
{
    RectangleShape block({ CELL - 2, CELL - 2 });
    block.setFillColor(color);
    block.setPosition({ x + 1, y + 1 });
    window.draw(block);
}

void drawText(RenderWindow& window, const string& words, float x, float y, int size, Color color)
{
    if (!fontLoaded)
    {
        return;
    }
    Text text(font, words, size);
    text.setFillColor(color);
    text.setPosition({ x, y });
    window.draw(text);
}


// Task 1- 

bool loadPieces()
{
    ifstream fin("pieces.txt");
    if (!fin)
    {
        return false;
    }

    int count;
    fin >> count;
    if (count != 7)
    {
        return false;
    }

    for (int p = 0; p < 7; p++)
    {
        for (int r = 0; r < 4; r++)
        {
            for (int c = 0; c < 4; c++)
            {
                fin >> pieces[p][r][c];
            }
        }
    }

    fin.close();
    return true;
}

void drawBoard(RenderWindow& window)
{
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            drawCell(window, BOARD_X + c * CELL, BOARD_Y + r * CELL, colors[board[r][c]]);
        }
    }

    RectangleShape border({ COLS * CELL, ROWS * CELL });
    border.setPosition({ BOARD_X, BOARD_Y });
    border.setFillColor(Color(0, 0, 0, 0));
    border.setOutlineColor(Color(200, 200, 200));
    border.setOutlineThickness(3.f);
    window.draw(border);
}

void drawError(RenderWindow& window)
{
    window.clear(Color(120, 0, 0));
    drawText(window, "ERROR: pieces.txt not found!", 40, 260, 28);
    drawText(window, "Put it in the project folder and restart.", 40, 310, 20);
}
void loadShape(int pieceNumber)
{
    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 4; c++)
        {
            shape[r][c] = pieces[pieceNumber][r][c];
        }
    }
}

void drawPiece(RenderWindow& window)
{
    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 4; c++)
        {
            if (shape[r][c] == '#')
            {
                drawCell(window, BOARD_X + (pieceCol + c) * CELL, BOARD_Y + (pieceRow + r) * CELL, colors[currentPiece + 1]);
            }
        }
    }
}

bool canPlace(char s[4][4], int row, int col)
{
    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 4; c++)
        {
            if (s[r][c] == '#')
            {
                int boardRow = row + r;
                int boardCol = col + c;
                
                if (boardCol <0 || boardCol >= COLS || boardRow >= ROWS)//this prevents from going out of boundry
                {
                    return false;
                }
                if (board[boardRow][boardCol] != 0)// this stops the motion when the collision is happened
                {
                    return false;
                }
            }
        }
    }
    return true;
}

void moveLeft()
{
    if (canPlace(shape, pieceRow, pieceCol - 1))
    {
        pieceCol--;
    }
}
void moveRight()
{
    if (canPlace(shape, pieceRow, pieceCol + 1))
    {
        pieceCol++;
    }
}

void moveDown()
{
    if (canPlace(shape, pieceRow + 1, pieceCol))
    {
        pieceRow++;
    }
}
void rotatePiece()
{
    char rotated[4][4];

    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 4; c++)
        {
            rotated[c][3 - r] = shape[r][c];
        }
    }

    if (canPlace(rotated, pieceRow, pieceCol))
    {
        for (int r = 0; r < 4; r++)
        {
            for (int c = 0; c < 4; c++)
            {
                shape[r][c] = rotated[r][c];
            }
        }
    }
}