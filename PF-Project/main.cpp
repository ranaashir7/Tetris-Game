#include <SFML/Graphics.hpp>
#include <optional>
using namespace std;

// =====================================================================
//                              CONSTANTS
// =====================================================================
const int ROWS = 20;
const int COLS = 10;
const int CELL = 40;      // size of one square in pixels
const int BOARD_X = 50;   // left edge of the board on screen
const int BOARD_Y = 100;  // top edge of the board on screen

// =====================================================================
//                              GAME DATA
// =====================================================================
int grid[ROWS][COLS] = { 0 };   // 0 = empty, 1 to 7 = a coloured block

// Every piece is a 4x4 picture written as 4 rows of 4 characters.
// 'X' = filled, '.' = empty. 7 pieces, 4 rotations each.
// Piece order: 0=I  1=O  2=T  3=S  4=Z  5=J  6=L
const char shapes[7][4][17] = {
    // 0 = I
    {
        "...." "XXXX" "...." "....",
        "..X." "..X." "..X." "..X.",
        "...." "...." "XXXX" "....",
        ".X.." ".X.." ".X.." ".X.."
    },
    // 1 = O
    {
        ".XX." ".XX." "...." "....",
        ".XX." ".XX." "...." "....",
        ".XX." ".XX." "...." "....",
        ".XX." ".XX." "...." "...."
    },
    // 2 = T
    {
        ".X.." "XXX." "...." "....",
        ".X.." ".XX." ".X.." "....",
        "...." "XXX." ".X.." "....",
        ".X.." "XX.." ".X.." "...."
    },
    // 3 = S
    {
        ".XX." "XX.." "...." "....",
        ".X.." ".XX." "..X." "....",
        "...." ".XX." "XX.." "....",
        "X..." "XX.." ".X.." "...."
    },
    // 4 = Z
    {
        "XX.." ".XX." "...." "....",
        "..X." ".XX." ".X.." "....",
        "...." "XX.." ".XX." "....",
        ".X.." "XX.." "X..." "...."
    },
    // 5 = J
    {
        "X..." "XXX." "...." "....",
        ".XX." ".X.." ".X.." "....",
        "...." "XXX." "..X." "....",
        ".X.." ".X.." "XX.." "...."
    },
    // 6 = L
    {
        "..X." "XXX." "...." "....",
        ".X.." ".X.." ".XX." "....",
        "...." "XXX." "X..." "....",
        "XX.." ".X.." ".X.." "...."
    }
};

// The piece that is currently falling
int curPiece = 0;   // which piece, 0 to 6
int curRot = 0;     // which rotation, 0 to 3
int curRow = 0;     // row of the top-left corner of the piece's 4x4 box
int curCol = 3;     // column of the top-left corner of the piece's 4x4 box

// Gravity
float fallDelay = 0.5f;   // seconds between each step down (smaller = faster)
float fallTimer = 0.f;    // time collected since the last step down

// =====================================================================
//                         FUNCTION PROTOTYPES
// =====================================================================
// Base (done)
sf::Color getColor(int value);
void drawGrid(sf::RenderWindow& window);
bool isFilled(int piece, int rot, int r, int c);
void drawPiece(sf::RenderWindow& window);

// Board and rules (teammate B)
bool canPlace(int piece, int rot, int row, int col);
void lockPiece();
int clearRows();
void spawnPiece();

// Pieces and controls (teammate C)
bool moveLeft();
bool moveRight();
bool moveDown();
void rotatePiece();

// =====================================================================
//                                MAIN
// =====================================================================
int main()
{
    sf::RenderWindow window(sf::VideoMode({ 800, 1000 }), "Tetris");
    window.setFramerateLimit(60);

    sf::Clock clock;

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();

        // 1. INPUT
        while (const optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                    window.close();

                // TEMPORARY: Space cycles pieces for testing
                if (key->code == sf::Keyboard::Key::Space)
                    curPiece = (curPiece + 1) % 7;

                // Controls (teammate C fills in the functions)
                if (key->code == sf::Keyboard::Key::Left)
                    moveLeft();
                if (key->code == sf::Keyboard::Key::Right)
                    moveRight();
                if (key->code == sf::Keyboard::Key::Down)
                    moveDown();
                if (key->code == sf::Keyboard::Key::Up)
                    rotatePiece();
            }
        }

        // 2. UPDATE
        fallTimer += dt;
        if (fallTimer >= fallDelay)
        {
            fallTimer = 0.f;

            bool moved = moveDown();

            // TEMPORARY: fake floor so we can test the landing code.
            // Delete these two lines once teammates B and C finish canPlace and moveDown.
            if (curRow >= ROWS - 4)
                moved = false;

            if (!moved)
            {
                lockPiece();
                clearRows();
                spawnPiece();
            }
        }

        // 3. DRAW
        window.clear(sf::Color(20, 20, 30));
        drawGrid(window);
        drawPiece(window);
        window.display();
    }

    return 0;
}

// =====================================================================
//                      BASE FUNCTIONS (DONE)
// =====================================================================

// Turns a grid number into a colour
sf::Color getColor(int value)
{
    if (value == 1) return sf::Color::Cyan;
    if (value == 2) return sf::Color::Yellow;
    if (value == 3) return sf::Color::Magenta;
    if (value == 4) return sf::Color::Green;
    if (value == 5) return sf::Color::Red;
    if (value == 6) return sf::Color::Blue;
    if (value == 7) return sf::Color(255, 165, 0);   // orange
    return sf::Color(45, 45, 60);                    // 0 = empty cell
}

// Draws every cell of the grid
void drawGrid(sf::RenderWindow& window)
{
    float size = CELL - 2;
    sf::RectangleShape cell({ size, size });

    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            float x = BOARD_X + c * CELL;
            float y = BOARD_Y + r * CELL;

            cell.setPosition({ x, y });
            cell.setFillColor(getColor(grid[r][c]));
            window.draw(cell);
        }
    }
}

// Is square (r, c) of this piece's 4x4 box filled?
bool isFilled(int piece, int rot, int r, int c)
{
    return shapes[piece][rot][r * 4 + c] == 'X';
}

// Draws the falling piece on top of the grid
void drawPiece(sf::RenderWindow& window)
{
    float size = CELL - 2;
    sf::RectangleShape cell({ size, size });
    cell.setFillColor(getColor(curPiece + 1));   // piece 0 uses colour 1, and so on

    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 4; c++)
        {
            if (isFilled(curPiece, curRot, r, c))
            {
                float x = BOARD_X + (curCol + c) * CELL;
                float y = BOARD_Y + (curRow + r) * CELL;
                cell.setPosition({ x, y });
                window.draw(cell);
            }
        }
    }
}

// =====================================================================
//                  TEAMMATE B: BOARD AND RULES
// =====================================================================

// Returns true if the piece fits with its 4x4 box at (row, col):
// every filled square must be inside the board and on an empty grid cell.
// TODO (B): write the real check using isFilled() and grid[][]
bool canPlace(int piece, int rot, int row, int col)
{
    return true;   // placeholder: says everything fits
}

// Copies the current piece into grid[][] so it becomes part of the board.
// Use curPiece + 1 as the value so the colours match.
void lockPiece()
{
    // TODO (B)
}

// Finds full rows, removes them and moves everything above them down.
// Returns how many rows were cleared (used for the score).
int clearRows()
{
    // TODO (B)
    return 0;
}

// Puts a new piece at the top.
void spawnPiece()
{
    // TODO (B): choose a random piece
    curRot = 0;
    curRow = 0;
    curCol = 3;
}

// =====================================================================
//                TEAMMATE C: PIECES AND CONTROLS
// =====================================================================

// Each move function returns true if the piece moved, false if it was blocked.
// moveDown() returning false is how the game knows the piece has landed.
// TODO (C): only move if canPlace(...) says the new spot is free

bool moveLeft()
{
    curCol--;
    return true;
}

bool moveRight()
{
    curCol++;
    return true;
}

bool moveDown()
{
    curRow++;
    return true;
}

// TODO (C): only rotate if canPlace(...) says it fits
void rotatePiece()
{
    curRot = (curRot + 1) % 4;
}