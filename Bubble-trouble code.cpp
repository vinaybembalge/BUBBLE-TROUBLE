#include <simplecpp>

class Bubble {
public:
    double x, y, radius, vx, vy;

    Bubble(double x, double y, double r, double vx, double vy) : x(x), y(y), radius(r), vx(vx), vy(vy) {}

    void move(double dt) {
        x += vx * dt;
        y += vy * dt;

        // Reflect bubble on hitting boundaries
        if (x - radius < 0 || x + radius > WINDOW_X) vx = -vx;
        if (y - radius < 0 || y + radius > WINDOW_Y) vy = -vy;
    }

    void draw() {
        Circle bubble(x, y, radius);
        bubble.setColor(COLOR(0, 255, 255));
        bubble.setFill(true);
        bubble.imprint();
    }

    bool isColliding(double bx, double by, double br) {
        double dist = sqrt(pow(x - bx, 2) + pow(y - by, 2));
        return dist <= (radius + br);
    }
};

class Shooter {
public:
    double x, y, vx;
    Rectangle body, barrel;

    Shooter(double x, double y) : x(x), y(y), vx(100) {
        body = Rectangle(x, y + 10, 40, 20);
        body.setColor(COLOR(255, 0, 0));
        body.setFill(true);
        barrel = Rectangle(x, y, 8, 20);
        barrel.setColor(COLOR(255, 0, 0));
        barrel.setFill(true);
    }

    void move(double dt, bool left) {
        if (left) x -= vx * dt;
        else x += vx * dt;

        if (x - 20 < 0) x = 20;
        if (x + 20 > WINDOW_X) x = WINDOW_X - 20;

        body.moveTo(x, y + 10);
        barrel.moveTo(x, y);
    }

    void draw() {
        body.imprint();
        barrel.imprint();
    }
};

class Bullet {
public:
    double x, y, vy;
    Circle bullet;

    Bullet(double x, double y) : x(x), y(y), vy(-200) {
        bullet = Circle(x, y, 5);
        bullet.setColor(COLOR(255, 255, 0));
        bullet.setFill(true);
    }

    void move(double dt) {
        y += vy * dt;
    }

    void draw() {
        bullet.moveTo(x, y);
        bullet.imprint();
    }

    bool isOffScreen() {
        return y < 0;
    }
};

int main() {
    initCanvas("Bubble Trouble", 800, 600);

    Shooter shooter(WINDOW_X / 2, WINDOW_Y - 50);
    vector<Bubble> bubbles;
    vector<Bullet> bullets;

    bubbles.push_back(Bubble(200, 200, 30, 100, 150));
    bubbles.push_back(Bubble(400, 100, 40, -150, 100));

    int score = 0;
    int health = 3;
    double dt = 0.05;

    while (true) {
        // Move and draw shooter
        if (getKeyPressed() == LEFT_ARROW) shooter.move(dt, true);
        if (getKeyPressed() == RIGHT_ARROW) shooter.move(dt, false);

        shooter.draw();

        // Shoot bullet
        if (getKeyPressed() == ' ') bullets.push_back(Bullet(shooter.x, shooter.y));

        // Move and draw bubbles
        for (Bubble &bubble : bubbles) {
            bubble.move(dt);
            bubble.draw();
        }

        // Move and draw bullets
        for (size_t i = 0; i < bullets.size(); i++) {
            bullets[i].move(dt);
            bullets[i].draw();

            if (bullets[i].isOffScreen()) bullets.erase(bullets.begin() + i);
        }

        // Check for collisions between bullets and bubbles
        for (size_t i = 0; i < bubbles.size(); i++) {
            for (size_t j = 0; j < bullets.size(); j++) {
                if (bubbles[i].isColliding(bullets[j].x, bullets[j].y, 5)) {
                    score += 10;
                    bubbles.erase(bubbles.begin() + i);
                    bullets.erase(bullets.begin() + j);
                    break;
                }
            }
        }

        // Check for game over
        for (Bubble &bubble : bubbles) {
            if (bubble.isColliding(shooter.x, shooter.y + 10, 20)) {
                health--;
                if (health <= 0) {
                    cout << "Game Over! Your score: " << score << endl;
                    closeCanvas();
                    return 0;
                }
            }
        }

        wait(dt);
    }
}
