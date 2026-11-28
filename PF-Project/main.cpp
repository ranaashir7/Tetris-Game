#include <SFML/Graphics.hpp>
#include <fstream>
#include <string>
using namespace std;
using namespace sf;

const int ROWS = 20;
const int COLS = 10;
const float CELL = 30.f;
const float BOARD_X = 20.f;
const float BOARD_Y = 20.f;

int board[ROWS][COLS];
char pieces[7][4][4];

bool piecesLoaded;
bool fontLoaded;

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
}

void handleInput(RenderWindow& window)
{
    while (const optional event = window.pollEvent())
    {
        if (event->is<Event::Closed>())
        {
            window.close();
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