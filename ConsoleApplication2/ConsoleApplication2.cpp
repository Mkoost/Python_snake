#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <iostream>
#include <ctime>

#define WIDTH 50
#define HEIGHT 50

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

#define MENU 2
#define GAME 1
#define EXIT 0


class Snake {
public:
    sf::Vector2<int> xy;
    int n;
    Snake* next;
    Snake* prev;

    Snake() {
        this->n = 0;
        this->xy = sf::Vector2<int>(WIDTH / 2, HEIGHT / 2);
        this->next = this;
        this->prev = this;
    }


    void add_block() {
        Snake* a = new Snake(sf::Vector2<int>(-1, -1), this, this->prev);
        if (this->next->n == 0) {
            this->next = a;
            this->prev = a;
        }
        else {
            this->prev->next = a;
            this->prev = a;
        }

    }

    void add_n_blocks(int n) {
        for (int i = 0; i < n; i++){
            this->add_block();
        }

    }

    void all_delete() {
        Snake* a = this->prev;
        Snake* b;

         while (a != this){
             b = a->prev;
             delete a;
             a = b;
        }

    }

    void move(sf::Vector2<int> xy) {
        this->prev->xy = this->xy;
        this->prev->xy += xy;
        if (this->prev->xy.x < 0) this->prev->xy.x = 49;
        else if (this->prev->xy.x > 49) this->prev->xy.x = 0;
        if (this->prev->xy.y < 0) this->prev->xy.y = 49;
        else if (this->prev->xy.y > 49) this->prev->xy.y = 0;
    }
protected:
    Snake(sf::Vector2<int> xy, Snake* next, Snake* prev) {
        this->xy = xy;
        this->prev = prev;
        this->next = next;
        this->n = prev->n + 1;
    }



};

class Snake_controller {
public:
    Snake* snake;
    Snake* movable_snk;
    int state;
    int steps[2] = {-1, -1};

    Snake_controller() {
        this->snake = new Snake();
        this->movable_snk = snake;
        this->state = 0;
    }

    Snake_controller(int n) {
        this->snake = new Snake();
        this->movable_snk = snake;
        this->snake->add_n_blocks(n-1);
        this->state = 0;
    }
    void all_delete() {
        this->snake->all_delete();
        delete snake;
    }

    int draw_snake(sf::RenderWindow& window) {
        int n = -1;
        int next_n = snake->n;
        Snake* a = this->snake;

        sf::RectangleShape shape(sf::Vector2f(window.getSize().x / WIDTH, window.getSize().y / HEIGHT));
        shape.setFillColor(sf::Color::Green);

        while (n < next_n) {
            shape.setPosition(sf::Vector2f(window.getSize().x / WIDTH * a->xy.x, window.getSize().y / HEIGHT * a->xy.y));
            window.draw(shape);


            n = next_n;
            next_n = a->next->n;
            a = a->next;

            if (a->n != this->movable_snk->n && this->movable_snk->xy == a->xy) return 1;
        }
        return 0;

    }

    void move_snake(sf::Vector2<int> xy) {
        this->movable_snk->move(xy);
        this->movable_snk = this->movable_snk->prev;

    }

    void move(sf::Vector2<int>& xy) {
        if (this->steps[0] != -1)
            this->state = steps[0];
        this->steps[0] = this->steps[1];
        steps[1] = -1;

        if (state == UP) { this->move_snake(sf::Vector2<int>(0, -1)); }
        else if (state == DOWN) { this->move_snake(sf::Vector2<int>(0, 1)); }
        else if (state == LEFT) { this->move_snake(sf::Vector2<int>(-1, 0)); }
        else if (state == RIGHT) { this->move_snake(sf::Vector2<int>(1, 0)); }

        if (this->movable_snk->xy == xy) {
            this->snake->add_block();
            xy.x = std::rand() % WIDTH;
            xy.y = std::rand() % HEIGHT;

        }
    }

    void update() {

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            if (this->state != DOWN)
                this->add_steps(UP);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            if (this->state != UP)
                this->add_steps(DOWN);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            if (this->state != RIGHT)
                this->add_steps(LEFT);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            if (state != LEFT)
                this->add_steps(RIGHT);
        }
    }

    int size() {
        return this->snake->n + 1;
    }

    void add_steps(int some) {
        if (this->steps[0] == -1) this->steps[0] = some;
        else if (this->steps[1] == -1) this->steps[1] = some;
        else {
            std::swap(this->steps[0], this->steps[1]);
            this->steps[1] = some;
        }

    }
};



void snake_cout_n(Snake& a, int n = -1) {
    std::cout << "[" << a.xy.x << " " << a.xy.y << "] ";
    if (n < a.n) { 
        snake_cout_n(*a.next, a.n); 
    }
}

int menu(sf::RenderWindow& window) {
    return GAME;
}

int game(sf::RenderWindow& window)
{
    Snake_controller snake(2500);
    int n = 0;
    sf::Vector2i eat_point;

    std::srand(std::time(nullptr));

    eat_point.x = std::rand() % WIDTH;
    eat_point.y = std::rand() % HEIGHT;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {

                snake.all_delete();
                return GAME;
            }
        
            
        }

        snake.update();
        n++;

        if (n == 10){
            snake.move(eat_point);
            n = 0;
        }

        window.clear();
        
        if (snake.draw_snake(window)) break;

        sf::RectangleShape shape(sf::Vector2f(window.getSize().x / WIDTH, window.getSize().y / HEIGHT));
        shape.setFillColor(sf::Color::Red);
        shape.setPosition(sf::Vector2f(window.getSize().x / WIDTH * eat_point.x, window.getSize().y / HEIGHT * eat_point.y));
        window.draw(shape);


        window.display();
    }
    snake.all_delete();
    return EXIT;
}

int main() {
    int n = GAME;
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "Snake");
    window.setFramerateLimit(60);
    
    while (n != EXIT) {
        switch (n)
        {
        case GAME:
            n = game(window);
            break;

        case MENU:
            n = menu(window);
            break;

        default:
            break;
        }
    }
    game(window);
}