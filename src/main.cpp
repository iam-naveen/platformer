#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>


class Animation;
class Player;
class Collider;

class Collider{
    public:
    Collider(sf::RectangleShape& Body):
        body(Body)
    {}
    ~Collider(){}
    void Move(float dx, float dy){
        body.move({dx,dy});
    }
    bool checkCollision(Collider& other, sf:: Vector2f& direction, float push){
        sf::Vector2f otherPosition = other.Getposition();
        sf::Vector2f otherHalfSize = other.GetHalfSize();
        sf::Vector2f thisPosition = Getposition();
        sf::Vector2f thisHalfSize = GetHalfSize();
        float deltax = otherPosition.x - thisPosition.x;
        float deltay = otherPosition.y-thisPosition.y;
        
        float intersectx = abs(deltax) - (otherHalfSize.x + thisHalfSize.x);
        float intersecty = abs(deltay) - (otherHalfSize.y + thisHalfSize.y);
        if(intersectx<0.0f && intersecty<0.0f){
            push = std::min(std::max(push,0.0f),1.0f);
            if(intersectx > intersecty){
                if(deltax>0.0f){
                    Move(intersectx*(1.0f - push), 0.0f);
                    other.Move(-intersectx * push, 0.0f);
                    direction.x = 1.0f;
                    direction.y = 0.0f;
                }
                else{
                    Move(-intersectx*(1.0f - push), 0.0f);
                    other.Move(intersectx * push, 0.0f);
                    direction.x = - 1.0f ;
                    direction.y = 0.0f ;
                }
                
            }
            else{
                if(deltay>0.0f){
                    Move(0.0f, intersecty*(1.0f - push));
                    other.Move(0.0f, -intersecty * push);
                    direction.x = 0.0f;
                    direction.y = -1.0f;
                }
                else{
                    Move( 0.0f, -intersecty*(1.0f - push));
                    other.Move( 0.0f, intersecty * push);
                    direction.x = 0.0f;
                    direction.y = 1.0f;
                }
                
            }
            return true;
        }
        return false;
    }
    sf::Vector2f Getposition(){
        return body.getPosition();
    }
    sf::Vector2f GetHalfSize(){
        return body.getSize() / 2.0f;
    }
    
    private:
        sf::RectangleShape& body;
    
};
class Object{
    private:
    sf::RectangleShape objBody;
    sf::Vector2f velocity;
    float speed = 90.0f;
    sf::Vector2f range;

public:
    Object(sf::Texture* texture, sf::Vector2f size, sf::Vector2f position, sf::Vector2f range){
        this->range = range;
        objBody.setSize(size);
        objBody.setOrigin(size / 2.0f);
        objBody.setTexture(texture);
        objBody.setPosition(position);
    }
    
    void update(float deltaTime){
        if(objBody.getPosition().y >= range.y){
            speed= - std::abs(speed);
        }
        if(objBody.getPosition().y <= range.x){
            speed= std::abs(speed);
        }
        velocity.x = 0.0f;
        velocity.y = speed;
        objBody.move(velocity * deltaTime); 
    }
    void draw(sf::RenderWindow& window){
        window.draw(objBody);
    }
    Collider getCollider(){
        return Collider(objBody);
    }
    sf::Vector2f getposition(){
        return objBody.getPosition();
    }

};




class Animation{
    public:
        sf::IntRect uvRect;

    private:
        sf::Vector2u imageCount;
        sf::Vector2u currentImage;
        
        float switchTime;
        float totalTime;
    
    public:
        Animation(sf::Texture *texture, sf::Vector2u imagecount, float switchtime){
            this->imageCount = imagecount;
            this->switchTime = switchtime;
            totalTime = 0.0f;
            currentImage.x =  0;
    
            uvRect.width = texture->getSize().x / (float)imagecount.x;
            uvRect.height = texture->getSize().y / (float)imagecount.y;
        }
        ~Animation(){}
    
        void update(int row, float deltaTime, bool faceRight){
            currentImage.y = row;
            totalTime += deltaTime;
    
            if(totalTime >= switchTime){
                totalTime -=switchTime;
                currentImage.x ++;
    
                if(currentImage.x >= 8){
                    currentImage.x = 0;
                }
            }
    
            uvRect.top = currentImage.y * uvRect.height;
            if(faceRight){
                uvRect.left = currentImage.x * uvRect.width;
                uvRect.width = abs(uvRect.width);
            }
            else{
                uvRect.left = (currentImage.x+1) * abs(uvRect.width);
                uvRect.width = -abs(uvRect.width);
            }
    
        }
        void changeImageCout(sf::Vector2u imagecount){
            this->imageCount = imagecount;
        }
        
    };


class Platform{
private:
    sf::RectangleShape body;

public:
    Platform(sf::Texture* texture, sf::Vector2f size, sf::Vector2f position){
        body.setSize(size);
        body.setOrigin(size / 2.0f);
        body.setTexture(texture);
        body.setPosition(position);

    }
    void draw(sf::RenderWindow& window){
        window.draw(body);
    }
    Collider getCollider(){
        return Collider(body);
    }

};
    
class Player{
    private:
    Animation animation;
    unsigned int row;
    float speed;
    bool faceRight;
    float gravity = 981;

    sf::Vector2f velocity;
    bool canjump;
    float jumpheight;



    public:
    sf::RectangleShape body;
    Player(sf::Texture *texture, sf::Vector2u imagecount, float switchtime, float Speed, float jumpheight):
    animation(texture, imagecount, switchtime)
    {
        this->speed = Speed;
        this->jumpheight = jumpheight;
        row = 0;
        faceRight = true;
        body.setSize(sf::Vector2f(30,45));
        body.setOrigin(body.getSize()/ 2.0f);
        body.setPosition({20 , 130});
        body.setTexture(texture);
    }
    void update(float deltaTime){
        velocity.x = 0.0f; 
        //RIGHT MOVEMENT
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)){
            if(body.getPosition().x <= 785){
                velocity.x += speed;}
        }

        //LEFT MOVEMENT
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)){
            if(body.getPosition().x >= 15){
                velocity.x -= speed;
            }
        }

        //JUMP MOVEMENT
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && canjump){
            canjump = false;
            velocity.y = -sqrtf(2.0f * gravity * jumpheight);
        }
        velocity.y += (gravity * deltaTime);
    
    
        if(velocity.x ==0.0f){
            row = 0;
        }
        else{
            row = 1;
            if(velocity.x > 0){
                faceRight = true;
            }
            else{
                faceRight = false;
            }
        }
        
        animation.update(row, deltaTime, faceRight);
        body.setTextureRect(animation.uvRect);
        body.move(velocity * deltaTime); 
    }
    
    void draw(sf::RenderWindow& window){
        window.draw(body);
    }

    void onCollision(sf:: Vector2f direction){
        if(direction.x < 0.0f){
            //collision on left
            velocity.x = 0.0f;
        }
        else if(direction.x > 0.0f){
            //collision on right    
            velocity.x = 0.0f;
        }
        if(direction.y > 0.0f){
            //collision on bottom
            velocity.y = 0.0f;
            canjump = true;
        }
        else if(direction.y < 0.0f){
            //collision on top   
            velocity.y = 0.0f;
        }

    }

    sf::Vector2f getposition(){
        return body.getPosition();
    }
    Collider getCollider(){
        return Collider(body);       
    }
    
};







static const float VIEW_HEIGHT = 512.0f;

void ResizeView(const sf::RenderWindow& window, sf::View& view ){
    float aspectratio =float(window.getSize().x ) / float(window.getSize().y);
    view.setSize({VIEW_HEIGHT* aspectratio, VIEW_HEIGHT});
}
int main()
{   
    sf::Vector2f viewCenter;
    sf::RectangleShape background;
    sf::Texture backgroundt;
    backgroundt.loadFromFile("assets/background.png");
    background.setSize(sf:: Vector2f(800, 500));
    background.setPosition(0,0);
    background.setTexture(&backgroundt);

    sf::RenderWindow window(sf::VideoMode({800,500}), "My Window", sf::Style::Close   | sf::Style::Resize);
    sf::View view({200.0f, 125.0f},{400.0f, 250.0f});
    sf::Texture playerTexture;
    playerTexture.loadFromFile("assets/sprite.png");
    
    sf::Texture box;
    box.loadFromFile("assets/box.png");
    
    sf::Texture mud;
    mud.loadFromFile("assets/mud.png");
    


    //window.setFramerateLimit(24);
    Player player(&playerTexture,{14, 8}, 0.12f, 100.0f, 60.0f);
    Object object(&mud, {70.0f, 30.0f}, {652.0f,335.0f}, {120.0f,420.0f});
    //playerTexture.setRepeated(true);
    
    
    
    
    
    
    
    std::vector<Platform> platforms;
    
    Platform Box(&box , {35.0f,35.0f},{50.0f, 255.0f});
    platforms.push_back(Platform(&mud, {100.0f,25.0f},{50.0f, 150.0f}));
    platforms.push_back(Platform(&mud, {50.0f, 25.0f},{170.0f,220.0f}));
    platforms.push_back(Platform(&mud, {100.0f, 25.0f},{50.0f,285.0f}));
    platforms.push_back(Platform(&mud, {26.0f,153.0f},{13.0f, 374.0f}));
    platforms.push_back(Platform(&mud, {800.0f, 50.0f},{400.0f,475.0f}));
    platforms.push_back(Platform(&mud, {145.0f, 25.0f},{170.0f,420.0f}));
    platforms.push_back(Platform(&mud, {25.0f, 147.0f},{340.0f,377.0f}));
    platforms.push_back(Platform(&mud, {125.0f, 25.0f},{290.0f,290.0f}));
    platforms.push_back(Platform(&mud, {130.0f, 25.0f},{520.0f,236.0f}));
    platforms.push_back(Platform(&mud, {25.0f, 148.0f},{573.0f,149.0f}));
    platforms.push_back(Platform(&mud, {50.0f, 25.0f},{420.0f,410.0f}));
    platforms.push_back(Platform(&mud, {50.0f, 25.0f},{510.0f,370.0f}));
    platforms.push_back(Platform(&mud, {30.0f, 130.0f},{570.0f,385.0f}));
    //platforms.push_back(Platform(&mud, {70.0f, 30.0f},{652.0f,335.0f}));
    platforms.push_back(Platform(&mud, {100.0f, 30.0f},{750.0f,135.0f}));
    
    
    Collider col1 = Box.getCollider();
    Collider col = player.getCollider(); 

    float deltaTime = 0.0f;
    sf::Clock clock;
    
    while (window.isOpen())
    {
        deltaTime = clock.restart().asSeconds();
        sf::Event evnt;
        while (window.pollEvent(evnt))
        {
            if (evnt.type == sf::Event::Closed){
                window.close();
            }
            if (evnt.type == sf::Event::Resized){
                ResizeView(window, view);
            }  
        }

        player.update(deltaTime);
        object.update(deltaTime);

        sf::Vector2f direction;

        for(Platform& platform: platforms){
            if(platform.getCollider().checkCollision(col, direction, 1.0f)){
                player.onCollision(direction);
            }
        }
        
        for(Platform& platform: platforms){
            if(platform.getCollider().checkCollision(col1, direction, 1.0f)){
                player.onCollision(direction);
            }
        }
        if(Box.getCollider().checkCollision(col, direction, 0.5f)){
            player.onCollision(direction);
        }
        if(object.getCollider().checkCollision(col, direction, 1.0f)){
            player.onCollision(direction);
        }


        if(player.getposition().x <=200){
            viewCenter.x = 200;
        }
        else if(player.getposition().x >=600){
            viewCenter.x = 600;
        }
        else{
            viewCenter.x = player.getposition().x;
        }
        if(player.getposition().y<=125){
            viewCenter.y = 125;
        }
        else if(player.getposition().y>=375){
            viewCenter.y = 375;
        }
        else{
            viewCenter.y = player.getposition().y;
        }
        view.setCenter(viewCenter);
        
        window.clear(sf::Color::Blue);
        window.setView(view);
        window.draw(background);
        for(Platform& platform: platforms){
            platform.draw(window);
        }
        Box.draw(window);
        object.draw(window);
        player.draw(window);
        window.display();
    
    }
}

