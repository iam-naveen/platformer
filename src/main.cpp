#include <SFML/Graphics.hpp>
#include <iostream>


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
    bool checkCollision(Collider& other, float push){
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
                }
                else{
                    Move(-intersectx*(1.0f - push), 0.0f);
                    other.Move(intersectx * push, 0.0f);
                }
                
            }
            else{
                if(deltay>0.0f){
                    Move(0.0f, intersecty*(1.0f - push));
                    other.Move(0.0f, -intersecty * push);
                }
                else{
                    Move( 0.0f, -intersecty*(1.0f - push));
                    other.Move( 0.0f, intersecty * push);
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
    
                if(currentImage.x >= imageCount.x){
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
    public:
    sf::RectangleShape body;
    Player(sf::Texture *texture, sf::Vector2u imagecount, float switchtime, float Speed):
    animation(texture, imagecount, switchtime)
    {
        this->speed = Speed;
        row = 0;
        faceRight = true;
        body.setSize(sf::Vector2f(30,45));
        body.setOrigin(body.getSize()/ 2.0f);
        body.setPosition({20 , 130});
        body.setTexture(texture);
    }
    void update(float deltaTime){
        sf::Vector2f movement(0.0f, 0.0f);
    
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)){
            movement.x += speed * deltaTime;}
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)){
            movement.x -= speed * deltaTime;}
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)){
            movement.y -= speed * deltaTime;}
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)){
            movement.y += speed * deltaTime;}
    
        if(movement.x ==0.0f){
            row = 0;}
        else{
            //row = 1;
            if(movement.x > 0){
                faceRight = true;
            }
            else{
                faceRight = false;
            }
        }
        
        animation.update(row, deltaTime, faceRight);
        body.setTextureRect(animation.uvRect);
        body.move(movement);
    }
    
    void changeimagecount(sf::Vector2u imagecount){
        animation.changeImageCout(imagecount);
    }
    
    void draw(sf::RenderWindow& window){
        window.draw(body);
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
    sf::RenderWindow window(sf::VideoMode({800,500}), "My Window", sf::Style::Close   | sf::Style::Resize);
    sf::View view({200.0f, 125.0f},{400.0f, 250.0f});
    sf::Texture playerTexture;
    playerTexture.loadFromFile("assets/sprite.png");
    //window.setFramerateLimit(24);
    Player player(&playerTexture,{20, 1}, 0.12f, 100.0f);
    //playerTexture.setRepeated(true);


    Platform platform1(nullptr, {100.0f,25.0f},{50.0f, 150.0f});
    Platform platform2(nullptr, {50.0f, 25.0f},{170.0f,220.0f});
    Platform platform3(nullptr, {35.0f,35.0f},{50.0f, 255.0f});
    Platform platform4(nullptr, {100.0f, 25.0f},{50.0f,285.0f});
    Platform platform5(nullptr, {26.0f,153.0f},{13.0f, 374.0f});
    Platform platform6(nullptr, {800.0f, 50.0f},{400.0f,475.0f});
    Platform platform7(nullptr, {145.0f, 25.0f},{170.0f,380.0f});
    Platform platform8(nullptr, {25.0f, 147.0f},{340.0f,377.0f});
    Platform platform9(nullptr, {125.0f, 25.0f},{290.0f,290.0f});
    Platform platform10(nullptr, {130.0f, 25.0f},{520.0f,236.0f});
    Platform platform11(nullptr, {25.0f, 148.0f},{573.0f,149.0f});
    Platform platform12(nullptr, {50.0f, 25.0f},{420.0f,410.0f});
    Platform platform13(nullptr, {50.0f, 25.0f},{510.0f,370.0f});
    Platform platform14(nullptr, {30.0f, 130.0f},{570.0f,385.0f});
    Platform platform15(nullptr, {70.0f, 30.0f},{652.0f,335.0f});
    Platform platform16(nullptr, {100.0f, 30.0f},{750.0f,135.0f});
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
        //std::cout<<player.getposition().x <<"  "<<player.getposition().y<<std::endl;
        player.update(deltaTime);
        
        
        platform1.getCollider().checkCollision(col, 1.0f);
        platform2.getCollider().checkCollision(col, 1.0f);
        platform3.getCollider().checkCollision(col, 0.0f);
        platform3.getCollider().checkCollision(col, 0.0f);
        platform4.getCollider().checkCollision(col, 1.0f);
        platform5.getCollider().checkCollision(col, 1.0f);
        platform6.getCollider().checkCollision(col, 1.0f);
        platform7.getCollider().checkCollision(col, 1.0f);
        platform8.getCollider().checkCollision(col, 1.0f);
        platform9.getCollider().checkCollision(col, 1.0f);
        platform10.getCollider().checkCollision(col, 1.0f);
        platform11.getCollider().checkCollision(col, 1.0f);
        platform12.getCollider().checkCollision(col, 1.0f);
        platform13.getCollider().checkCollision(col, 1.0f);
        platform14.getCollider().checkCollision(col, 1.0f);
        platform15.getCollider().checkCollision(col, 1.0f);
        platform16.getCollider().checkCollision(col, 1.0f);
        
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

        platform1.draw(window);
        platform2.draw(window);
        platform3.draw(window);
        platform4.draw(window);
        platform5.draw(window);
        platform6.draw(window);
        platform7.draw(window);
        platform8.draw(window);
        platform9.draw(window);
        platform10.draw(window);
        platform11.draw(window);
        platform12.draw(window);
        platform13.draw(window);
        platform14.draw(window);
        platform15.draw(window);
        platform16.draw(window);

        player.draw(window);
        window.display();
    
    }
}

