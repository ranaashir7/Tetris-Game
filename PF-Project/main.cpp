#include <SFML/Graphics.hpp>
#include <optional>
using namespace std;

// ---------- CONSTANTS ----------
const int ROWS = 20;
const int COLS = 10;
const int CELL = 40;      // size of one square in pixels
const int BOARD_X = 50;   // left edge of the board on screen
const int BOARD_Y = 100;  // top edge of the board on screen

// ---------- GAME DATA ----------
int grid[ROWS][COLS] = { 0 };   // 0 = empty, 1 to 7 = a coloured block

// ---------- FUNCTION PROTOTYPES ----------
sf::Color getColor(int value);
void drawGrid(sf::RenderWindow& window);

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 800, 1000 }), "Tetris");
    window.setFramerateLimit(60);

    // TEMPORARY TEST: delete this loop once you see the colours
    for (int c = 0; c < 7; c++)
        grid[19][c] = c + 1;

    while (window.isOpen())
    {
        // 1. INPUT
        while (const optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                    window.close();
            }
        }

        // 2. UPDATE (nothing yet)

        // 3. DRAW
        window.clear(sf::Color(20, 20, 30));
        drawGrid(window);
        window.display();
    }

    return 0;
}

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
    float size = CELL - 2;                // 2 pixel gap so the cells look like a grid
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