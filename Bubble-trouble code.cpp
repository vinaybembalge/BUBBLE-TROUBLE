#include <simplecpp>
#include <vector>
#include <cmath>
#include <iostream>
using namespace std;

const int WINDOW_X = 800;
const int WINDOW_Y = 600;

class Bubble {
public:
    double x, y, radius, vx, vy;
    Circle bubble;

    Bubble(double x, double y, double r, double vx, double vy)
        : x(x), y(y), radius(r), vx(vx), vy(vy), bubble(x, y, r) {
        bubble.setColor(COLOR(0, 255, 255));
        bubble.setFill(true);
    }

    void move(double dt) {
        x += vx * dt;
        y += vy * dt;

        // Reflect bubble on hitting boundaries
        if (x - radius < 0 || x + radius > WINDOW_X) {
            vx = -vx;
            x += vx * dt; // move back inside
        }
        if (y - radius < 0 || y + radius > WINDOW_Y) {
            vy = -vy;
            y += vy * dt; // move back inside
        }
        bubble.moveTo(x, y);
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

    Shooter(double x, double y)
        : x(x), y(y), vx(200),
          body(x, y + 10, 40, 20),
          barrel(x, y, 8, 20) {
        body.setColor(COLOR(255, 0, 0));
        body.setFill(true);
        barrel.setColor(COLOR(255, 0, 0));
        barrel.setFill(true);
    }

    void move(double dt, int direction) {
        // direction: -1 for left, +1 for right, 0 for none
        x += direction * vx * dt;
        if (x - 20 < 0) x = 20;
        if (x + 20 > WINDOW_X) x = WINDOW_X - 20;

        body.moveTo(x, y + 10);
        barrel.moveTo(x, y);
    }
};

class Bullet {
public:
    double x, y, vy;
    Circle bullet;

    Bullet(double x, double y)
        : x(x), y(y), vy(-400), bullet(x, y, 5) {
        bullet.setColor(COLOR(255, 255, 0));
        bullet.setFill(true);
    }

    void move(double dt) {
        y += vy * dt;
        bullet.moveTo(x, y);
    }

    bool isOffScreen() {
        return y < 0;
    }
};

int main() {
    initCanvas("Bubble Trouble", WINDOW_X, WINDOW_Y);

    Shooter shooter(WINDOW_X / 2, WINDOW_Y - 50);
    vector<Bubble> bubbles;
    vector<Bullet> bullets;

    // Add some bubbles
    bubbles.push_back(Bubble(200, 200, 30, 100, 150));
    bubbles.push_back(Bubble(400, 100, 40, -150, 100));

    int score = 0;
    int health = 3;
    double dt = 0.02;

    Text score_text(70, 20, "Score: 0");
    Text health_text(700, 20, "Health: 3");

    while (true) {
        // --- Input Handling ---
        int direction = 0;
        if (kbhit()) {
            char key = getch();
            if (key == 75) direction = -1; // Left arrow (ASCII code 75)
            if (key == 77) direction = 1;  // Right arrow (ASCII code 77)
        }

        shooter.move(dt, direction);

        // Mouse click to shoot
        if (mouseButtonPressed()) {
            bullets.push_back(Bullet(shooter.x, shooter.y));
            // Wait for button release to avoid rapid fire
            while (mouseButtonPressed()) wait(0.01);
        }

        // --- Update Bubbles ---
        for (size_t i = 0; i < bubbles.size(); ++i) {
            bubbles[i].move(dt);
        }

        // --- Update Bullets ---
        for (size_t i = 0; i < bullets.size();) {
            bullets[i].move(dt);
            if (bullets[i].isOffScreen()) {
                bullets.erase(bullets.begin() + i);
            } else {
                ++i;
            }
        }

        // --- Bullet-Bubble Collision ---
        for (size_t i = 0; i < bubbles.size(); ) {
            bool hit = false;
            for (size_t j = 0; j < bullets.size(); ++j) {
                if (bubbles[i].isColliding(bullets[j].x, bullets[j].y, 5)) {
                    score += 10;
                    score_text.setMessage("Score: " + to_string(score));
                    bubbles.erase(bubbles.begin() + i);
                    bullets.erase(bullets.begin() + j);
                    hit = true;
                    break;
                }
            }
            if (!hit) ++i;
        }

        // --- Bubble-Shooter Collision ---
        for (size_t i = 0; i < bubbles.size(); ++i) {
            if (bubbles[i].isColliding(shooter.x, shooter.y + 10, 20)) {
                health--;
                health_text.setMessage("Health: " + to_string(health));
                if (health <= 0) {
                    Text game_over(WINDOW_X / 2, WINDOW_Y / 2, "Game Over! Score: " + to_string(score));
                    wait(3);
                    closeCanvas();
                    return 0;
                }
                // Move bubble away after hit
                bubbles[i].x = 100;
                bubbles[i].y = 100;
            }
        }

        wait(dt);
    }
}

