#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

using namespace sf;
using namespace std;

class Animation;
class Player;
class Collider;


//--------------------------------------COLLIDER CLASS-----------------------------------------
class Collider {
  public:
    Collider(RectangleShape &Body) : body(Body) {}
    ~Collider() {}
    void Move(float dx, float dy) { body.move({dx, dy}); }
    bool checkCollision(Collider &other, Vector2f &direction, float push) {
        Vector2f otherPosition = other.Getposition();
        Vector2f otherHalfSize = other.GetHalfSize();
        Vector2f thisPosition = Getposition();
        Vector2f thisHalfSize = GetHalfSize();
        float deltax = otherPosition.x - thisPosition.x;
        float deltay = otherPosition.y - thisPosition.y;

        float intersectx = abs(deltax) - (otherHalfSize.x + thisHalfSize.x);
        float intersecty = abs(deltay) - (otherHalfSize.y + thisHalfSize.y);
        if (intersectx < 0.0f && intersecty < 0.0f) {
            push = std::min(std::max(push, 0.0f), 1.0f);
            if (intersectx > intersecty) {
                if (deltax > 0.0f) {
                    Move(intersectx * (1.0f - push), 0.0f);
                    other.Move(-intersectx * push, 0.0f);
                    direction.x = 1.0f;
                    direction.y = 0.0f;
                } else {
                    Move(-intersectx * (1.0f - push), 0.0f);
                    other.Move(intersectx * push, 0.0f);
                    direction.x = -1.0f;
                    direction.y = 0.0f;
                }

            } else {
                if (deltay > 0.0f) {
                    Move(0.0f, intersecty * (1.0f - push));
                    other.Move(0.0f, -intersecty * push);
                    direction.x = 0.0f;
                    direction.y = -1.0f;
                } else {
                    Move(0.0f, -intersecty * (1.0f - push));
                    other.Move(0.0f, intersecty * push);
                    direction.x = 0.0f;
                    direction.y = 1.0f;
                }
            }
            return true;
        }
        return false;
    }
    Vector2f Getposition() { return body.getPosition(); }
    Vector2f GetHalfSize() { return body.getSize() / 2.0f; }

  private:
    RectangleShape &body;
};
//------------------------------------------------------------------------------------------------



//--------------------------------------BOX CLASS------------------------------------------------
class Box {
  private:
    float speed;
    float gravity = 981;
    sf::RectangleShape body;
    sf::Vector2f velocity;

  public:
    Box(sf::Texture *texture, sf::Vector2f size, sf::Vector2f position) {
        body.setSize(size);
        body.setOrigin(body.getSize() / 2.0f);
        body.setPosition(position);
        body.setTexture(texture);
    }
    void update(float deltaTime) {
        velocity.x = 0.0f;

        velocity.y += (gravity * deltaTime);
        body.move(velocity * deltaTime);
    }

    void draw(sf::RenderWindow &window) { window.draw(body); }

    void onCollision(sf::Vector2f direction) {
        if (direction.x < 0.0f || direction.x > 0.0f) {
            // collision on left or right
            velocity.x = 0.0f;
        }
        if (direction.y > 0.0f || direction.y < 0.0f) {
            // collision on bottom or top
            velocity.y = 0.0f;
        }
    }
    sf::Vector2f getposition() { return body.getPosition(); }
    Collider getCollider() { return Collider(body); }
};

//-----------------------------------------------------------------------------------------------

//--------------------------------KEY CLASS-----------------------------------------------------
class Key {
  private:
    bool exist = true;
    sf::RectangleShape body;

  public:
    Key(sf::Texture *texture, sf::Vector2f position, sf::Vector2f size) {
        body.setPosition(position);
        body.setSize(size);
        body.setTexture(texture);
    }

    void draw(sf::RenderWindow &window) {
        if (exist == true) {
            window.draw(body);
        }
    }
    void False() { exist = false; }
    Collider getCollider() { return Collider(body); }
};
//---------------------------------------------------------------------------------------------

//=========================================OBJECT CLASS==========================================
class Object {
  private:
    sf::RectangleShape objBody;
    sf::Vector2f velocity;
    float speed = 90.0f;
    Vector2f range;

  public:
    Object(Texture *texture, Vector2f size, Vector2f position, Vector2f range) {
        this->range = range;
        objBody.setSize(size);
        objBody.setOrigin(size / 2.0f);
        objBody.setTexture(texture);
        objBody.setPosition(position);
    }

    void update(float deltaTime) {
        if (objBody.getPosition().y >= range.y) {
            speed = -std::abs(speed);
        }
        if (objBody.getPosition().y <= range.x) {
            speed = std::abs(speed);
        }
        velocity.x = 0.0f;
        velocity.y = speed;
        objBody.move(velocity * deltaTime);
    }
    void draw(RenderWindow &window) { window.draw(objBody); }
    Collider getCollider() { return Collider(objBody); }
    Vector2f getposition() { return objBody.getPosition(); }
};
//==============================================================================================

//======================================ANIMATION CLASS=======================================
class Animation {
  public:
    IntRect uvRect;

  private:
    Vector2u imageCount;
    Vector2u currentImage;

    float switchTime;
    float totalTime;

  public:
    Animation(Texture *texture, Vector2u imagecount, float switchtime) {
        this->imageCount = imagecount;
        this->switchTime = switchtime;
        totalTime = 0.0f;
        currentImage.x = 0;

        uvRect.width = texture->getSize().x / (float)imagecount.x;
        uvRect.height = texture->getSize().y / (float)imagecount.y;
    }
    ~Animation() {}

    void update(int row, float deltaTime, bool faceRight) {
        currentImage.y = row;
        totalTime += deltaTime;

        if (totalTime >= switchTime) {
            totalTime -= switchTime;
            currentImage.x++;

            if (currentImage.x >= 8) {
                currentImage.x = 0;
            }
        }

        uvRect.top = currentImage.y * uvRect.height;
        if (faceRight) {
            uvRect.left = currentImage.x * uvRect.width;
            uvRect.width = abs(uvRect.width);
        } else {
            uvRect.left = (currentImage.x + 1) * abs(uvRect.width);
            uvRect.width = -abs(uvRect.width);
        }
    }
};
//============================================================================================

//=================================PLATFORM CLASS============================================
class Platform {
  private:
    RectangleShape body;

  public:
    Platform(Texture *texture, Vector2f size, Vector2f position) {
        texture->setRepeated(true);
        body.setSize(size);
        body.setOrigin(size / 2.0f);
        body.setTexture(texture);
        body.setPosition(position);
    }
    void draw(RenderWindow &window) { window.draw(body); }
    Collider getCollider() { return Collider(body); }
};
//=============================================================================================

//============================= PLAYER CLASS====================================================
class Player {
  private:
    bool hasKey = false;
    Animation animation;
    unsigned int row;
    float speed;
    bool faceRight;
    float gravity = 981;

    sf::Clock clock;
    float elapse = 0.0f;

    Vector2f velocity;
    bool canjump;
    bool candoublejump;
    float jumpheight;

  public:
    RectangleShape body;
    Player(Texture *texture, Vector2u imagecount, float switchtime, float Speed,
           float jumpheight)
        : animation(texture, imagecount, switchtime) {
        this->speed = Speed;
        this->jumpheight = jumpheight;
        row = 0;
        faceRight = true;
        body.setSize(Vector2f(30, 45));
        body.setOrigin(body.getSize() / 2.0f);
        body.setPosition({20, 130});
        body.setTexture(texture);
    }
    void update(float deltaTime) {
        velocity.x = 0.0f;
        // RIGHT MOVEMENT
        if (Keyboard::isKeyPressed(Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
            if (body.getPosition().x <= 785) {
                velocity.x += speed;
            }
        }

        // LEFT MOVEMENT
        if (Keyboard::isKeyPressed(Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
            if (body.getPosition().x >= 15) {
                velocity.x -= speed;
            }
        }
        // JUMP MOVEMENT
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
            if (canjump) {
                elapse = clock.restart().asSeconds();
                canjump = false;
                velocity.y = -sqrtf(2.0f * gravity * jumpheight);
            }
            if (candoublejump) {
                elapse = clock.restart().asSeconds();
                if (elapse > 0.08f && elapse < 0.4f) {

                    candoublejump = false;
                    velocity.y = -sqrtf(1.25f * gravity * jumpheight);
                }
            }
        }

        // moving left
        if (velocity.x < 0) {
            faceRight = false;
            // case 1: on-air
            if (velocity.y < 0) {
                row = 2;
            }
            // case 2: on-land
            else if (velocity.y > 0) {
                row = 3;
            }
            // case 3: idle
            else if (velocity.y == 0) {
                row = 1;
            }
        }
        
        // moving right
        if (velocity.x > 0) {
            faceRight = true;
            // case 1: on-air
            if (velocity.y < 0) {
                row = 2;
            }
            // case 2: on-land
            else if (velocity.y > 0) {
                row = 3;
            }
            // case 3: idle
            else if (velocity.y == 0) {
                row = 1;
            }
        }
        
        // No movement
        if (velocity.x == 0.0f && velocity.y == 0) {
            row = 0;
        }
        

        velocity.y += (gravity * deltaTime);
        
        animation.update(row, deltaTime, faceRight);
        body.setTextureRect(animation.uvRect);
        body.move(velocity * deltaTime);
    }

    void draw(RenderWindow &window) { window.draw(body); }

    void onCollision(Vector2f direction) {
        if (direction.x < 0.0f) {
            // collision on left
            velocity.x = 0.0f;
        } else if (direction.x > 0.0f) {
            // collision on right
            velocity.x = 0.0f;
        }
        if (direction.y > 0.0f) {
            // collision on bottom
            velocity.y = 0.0f;
            canjump = true;
            candoublejump = true;
        } else if (direction.y < 0.0f) {
            // collision on top
            velocity.y = 0.0f;
        }
    }
    void KeyObtained() { hasKey = true; }
    
    bool HasKey(){return hasKey;}

    Vector2f getposition() { return body.getPosition(); }
    Collider getCollider() { return Collider(body); }
};
//===========================================================================================


//==================================MAIN FUNCTION============================================
int main() {
    // TEXT
    sf::Font font;
    if (!font.loadFromFile("assets/PixellettersFull.ttf")) {
        cout << "Cannot Load Font" << endl;
    }
    sf::Text text;
    text.setFont(font);
    text.setString("Congrats on Completing the Level!!");
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::Red);
    text.setOutlineThickness(2.0f);
    text.setOutlineColor(Color::Black);
    text.setStyle(sf::Text::Bold | sf::Text::Underlined);
    
    //WINDOW
    sf::RenderWindow window(sf::VideoMode({800, 500}), "My Platformer",
    sf::Style::Close);

    //VIEW
    sf::View view({200.0f, 125.0f}, {400.0f, 250.0f});
    sf::Vector2f viewCenter;

    //BACKGROUND
    sf::RectangleShape background;
    sf::Texture backgroundt;
    backgroundt.loadFromFile("assets/background.png");
    background.setSize(Vector2f(800, 500));
    background.setPosition(0, 0);
    background.setTexture(&backgroundt);

    //TEXTURES
    sf::Texture playerTexture;
    playerTexture.loadFromFile("assets/8x4sprite.png");
    sf::Texture boxt;
    boxt.loadFromFile("assets/barrel.png");
    sf::Texture mud;
    mud.loadFromFile("assets/mud.png");
    sf::Texture grass;
    grass.loadFromFile("assets/grassblock.png");
    sf::Texture keyt;
    keyt.loadFromFile("assets/key.png");
    sf::Texture ground;
    ground.loadFromFile("assets/ground.png");
    sf::Texture doort;
    doort.loadFromFile("assets/door.png");

    //START OF LEVEL 1
    start:
    bool is_game = true;
    Player player(&playerTexture, {8, 4}, 0.12f, 100.0f, 70.0f);
    Object object(&grass, {70.0f, 30.0f}, {652.0f, 335.0f}, {120.0f, 420.0f});
    Key key(&keyt, {45.0f, 425.0f}, {25.0f, 25.0f});
    Box box(&boxt, {35.0f, 35.0f}, {60.0f, 255.0f});
    Platform Door(&doort, {45.0f, 70.0f},{760.0f,86.0f});

    //------------------------------MAP-------------------------------------
    std::vector<Platform> platforms;
    platforms.push_back(Platform(&grass, {100.0f, 25.0f}, {50.0f, 150.0f}));
    platforms.push_back(Platform(&grass, {50.0f, 25.0f}, {170.0f, 220.0f}));
    platforms.push_back(Platform(&grass, {130.0f, 25.0f}, {50.0f, 285.0f}));
    platforms.push_back(Platform(&mud, {26.0f, 153.0f}, {13.0f, 374.0f}));
    platforms.push_back(Platform(&ground, {800.0f, 50.0f}, {400.0f, 475.0f}));
    platforms.push_back(Platform(&grass, {145.0f, 25.0f}, {170.0f, 405.0f}));
    platforms.push_back(Platform(&mud, {25.0f, 147.0f}, {340.0f, 375.0f}));
    platforms.push_back(Platform(&grass, {125.0f, 25.0f}, {290.0f, 290.0f}));
    platforms.push_back(Platform(&grass, {130.0f, 25.0f}, {520.0f, 236.0f}));
    platforms.push_back(Platform(&mud, {25.0f, 148.0f}, {573.0f, 149.0f}));
    platforms.push_back(Platform(&grass, {50.0f, 25.0f}, {420.0f, 410.0f}));
    platforms.push_back(Platform(&grass, {50.0f, 25.0f}, {510.0f, 370.0f}));
    platforms.push_back(Platform(&mud, {30.0f, 130.0f}, {570.0f, 385.0f}));
    platforms.push_back(Platform(&grass, {100.0f, 30.0f}, {750.0f, 135.0f}));
    //-----------------------------------------------------------------------
    

    Collider boxCollider = box.getCollider();
    Collider playerCollider = player.getCollider();

    float deltaTime = 0.0f;
    Clock clock;
//=========================================================GAME LOOP========================================================================
    while (window.isOpen()) {
        deltaTime = clock.restart().asSeconds();
        Event evnt;
        Vector2f direction;
        while (window.pollEvent(evnt)) {
            if (evnt.type == Event::Closed) {
                window.close();
            }
            if(evnt.key.code == sf::Keyboard::Key::R){
                window.clear();
                goto start;
            }
        }

        if (is_game){
            //-------------------------UPDATE---------------------------------
            player.update(deltaTime);
            box.update(deltaTime);
            object.update(deltaTime);
            //---------------------------------------------------------------
            
            //============================CHECKING FOR COLLISIONS==============================
            for (Platform &platform : platforms) {
                if (platform.getCollider().checkCollision(playerCollider, direction, 1.0f)) {
                    player.onCollision(direction);
                }
            }
            
            for (Platform &platform : platforms) {
                if (platform.getCollider().checkCollision(boxCollider, direction, 1.0f)) {
                    box.onCollision(direction);
                }
            }
            
            if (Door.getCollider().checkCollision(playerCollider, direction, 1.0f)) {
                if(player.HasKey()){
                    is_game = false;
                }
                
            }    
            if (box.getCollider().checkCollision(playerCollider, direction, 0.6f)) {
                player.onCollision(direction);
            }
            if (object.getCollider().checkCollision(playerCollider, direction, 1.0f)) {
                player.onCollision(direction);
            }
            if (key.getCollider().checkCollision(playerCollider, direction, 0.0f)) {
                key.False();
                player.KeyObtained();
            }
            //===========================COLLISION BLOCK END=======================================



        //------------------------------------------VIEW---------------------------------------------
            if (player.getposition().x <= 200) {
                viewCenter.x = 200;
            } else if (player.getposition().x >= 600) {
                viewCenter.x = 600;
            } else {
                viewCenter.x = player.getposition().x;
            }
            if (player.getposition().y <= 125) {
                viewCenter.y = 125;
            } else if (player.getposition().y >= 375) {
                viewCenter.y = 375;
            } else {
                viewCenter.y = player.getposition().y;
            }
            view.setCenter(viewCenter);
        }
        //---------------------------------------------------------------------------------------------


        //====================================== DRAWING BLOCK ============================================    
        window.clear();
        window.setView(view);
        
        window.draw(background);
        for (Platform &platform : platforms) {
            platform.draw(window);
        }
        Door.draw(window);
        box.draw(window);
        object.draw(window);
        key.draw(window);
        player.draw(window);
        if (!is_game) {
            sf::Vector2f center = view.getCenter();
            text.setPosition(center.x - 165.0f, center.y - 50.f);
            window.draw(text);
        }
        
        window.display();
        //=================================================================================================
    }
 //=====================================================GAME LOOP END======================================================================   
}
//========================================================MAIN FUNC END====================================================================