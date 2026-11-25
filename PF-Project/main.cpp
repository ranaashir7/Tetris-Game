#include <SFML/Graphics.hpp>
#include <cmath>
#include <optional>

int main()
{
    const float width = 800.f;
    const float height = 600.f;

    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "SFML Test 2 - Bouncing Ball");
    window.setFramerateLimit(60);

    // Paddle controlled with the arrow keys (or A / D)
    sf::RectangleShape paddle({ 120.f, 20.f });
    paddle.setFillColor(sf::Color::Cyan);
    paddle.setPosition({ 340.f, 550.f });

    // Bouncing ball
    sf::CircleShape ball(15.f);
    ball.setFillColor(sf::Color::Red);
    ball.setPosition({ 100.f, 100.f });
    sf::Vector2f velocity(250.f, 200.f);

    sf::Clock clock;

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();

        // Events
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                    window.close();
            }
        }

        // Paddle movement
        const float paddleSpeed = 450.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
            paddle.move({ -paddleSpeed * dt, 0.f });

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
            paddle.move({ paddleSpeed * dt, 0.f });

        // Keep paddle inside the window
        sf::Vector2f pPos = paddle.getPosition();
        if (pPos.x < 0.f)
            paddle.setPosition({ 0.f, pPos.y });
        if (pPos.x + paddle.getSize().x > width)
            paddle.setPosition({ width - paddle.getSize().x, pPos.y });

        // Ball movement
        ball.move(velocity * dt);
        sf::Vector2f bPos = ball.getPosition();
        float diameter = ball.getRadius() * 2.f;

        // Bounce off walls and ceiling
        if (bPos.x < 0.f)
            velocity.x = std::abs(velocity.x);
        else if (bPos.x + diameter > width)
            velocity.x = -std::abs(velocity.x);

        if (bPos.y < 0.f)
            velocity.y = std::abs(velocity.y);

        // Bounce off paddle
        if (ball.getGlobalBounds().findIntersection(paddle.getGlobalBounds()) && velocity.y > 0.f)
            velocity.y = -std::abs(velocity.y);

        // Ball fell off the bottom: reset it
        if (bPos.y > height)
        {
            ball.setPosition({ width / 2.f, 50.f });
            velocity = { 250.f, 200.f };
        }

        // Draw
        window.clear(sf::Color(20, 20, 30));
        window.draw(paddle);
        window.draw(ball);
        window.display();
    }
}