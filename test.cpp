#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <ctime>
#include <cstdlib>
#include <cmath>


using namespace std;
using namespace sf;

const float pi = 3.14159265358979323846;
class Object
{
    public:
    float speed;
    sf::Sprite sprite;
        Object(Texture& texture, sf::Vector2f size, sf::Vector2f pos=sf::Vector2f(0,0),float speed=1.0f)
        {
            this->size = size;
            this->pos = pos;
            this->speed=speed;
            sprite.setTexture(texture);
            // sprite.setOrigin((sf::Vector2f)texture.getSize() / 2.0f);
            sprite.setScale(sf::Vector2f(size.x / texture.getSize().x, size.y / texture.getSize().y));
            sprite.setPosition(pos);
        }
        void Draw(sf::RenderWindow& window)
        {
            window.draw(sprite);
        }
    protected:
        sf::Vector2f size;
        sf::Vector2f pos;
        
        

};
class Ninja: public Object
{
    public:
        Ninja(Texture& texture, sf::Vector2f size, sf::Vector2f pos, float speed)
            : Object(texture, size, pos, speed) 
        {
        }

        bool isCollide(Object& object)
        {
            return sprite.getGlobalBounds().intersects(object.sprite.getGlobalBounds());
        }


        void Move()
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            {
                sprite.move(-speed, 0);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            {
                sprite.move(speed, 0);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            {
                sprite.move(0, -speed);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            {
                sprite.move(0, speed);
            }
        }
};

class Shuriken: public Object
{
    public:
        float move_x, move_y;
        
        float ninja_x_for_homing,ninja_y_for_homing;
        Shuriken(Texture& texture, sf::Vector2f size, sf::Vector2f pos, float speed)
            : Object(texture, size, pos, speed) 
        {
            Shuriken::Spawn();
        }
        ~Shuriken() {
            delete this;
        }
    void Spawn()
    {
        srand(time(NULL));
        shuriken_x=rand()%2;
        shuriken_y=!shuriken_x;
        if (shuriken_x)
        {
            pos.y=rand()%900;
            shuriken_x=rand()%2;
            if (shuriken_x) 
            {pos.x=0; a1=-90; a2=90;} 
            else {pos.x=1200;a1=90; a2=270;}
        }
        if (shuriken_y)
        {
            pos.x=rand()%1200;
            shuriken_y=rand()%2;
            if (shuriken_y) 
            {pos.y=0; a1=0; a2=180;} 
            else {pos.y=900;a1=-180; a2=0;}
        }
        int a = rand()%(a2-a1) +a1;
        angle = a*pi/180;
        sprite.setPosition(pos);

    }

    void Find_Ninja(Ninja& ninja)
        {
            sf::Vector2f ninja_pos = ninja.sprite.getPosition();
            ninja_x_for_homing=ninja_pos.x;
            ninja_y_for_homing=ninja_pos.y;
        }
    
    virtual void Move()
    {
        move_x=speed*cos(angle);
        move_y=speed*sin(angle);
        sprite.move(move_x, move_y);
    }

    bool Destroy()
    {
        if (sprite.getPosition().x<0 || sprite.getPosition().x>1200 || sprite.getPosition().y<0 || sprite.getPosition().y>900) return true;
        return false;
    }

    void Set_Angle(int a)
    {
        angle = -a*pi/180;
    }

    void Left_Rotate(float a)
    {
        angle-=a*pi/180;
    }

    void Right_Rotate(float a)
    {
        angle+=a*pi/180;
    }

    private:
        bool shuriken_x;
        bool shuriken_y;
        int a1,a2;
        float angle;
};

class FastShuriken: public Shuriken
{
    public:
        FastShuriken(Texture& texture, sf::Vector2f size, sf::Vector2f pos, float speed)
            : Shuriken(texture, size, pos, speed*3) 
        {
        }
};

class ChaoticShuriken: public Shuriken
{
    public:
        ChaoticShuriken(Texture& texture, sf::Vector2f size, sf::Vector2f pos, float speed)
            : Shuriken(texture, size, pos, speed) 
        {
            clock_angle.restart();
        }

        virtual void Move() override
        {
            
            Shuriken::Move();
            if (clock_angle.getElapsedTime().asSeconds()>0.35)
            {
                srand(time(NULL));
                cout<<rand()%360<<endl;
                Set_Angle(rand()%360);
                clock_angle.restart();
                
            }
            
        }
    private:
        sf::Clock clock_angle;

};

class HomingShuriken: public Shuriken
{
    private:
        Clock clock_homing;
        
        Vector2f Find_Vector()
        {
            float shuriken_pos_x = sprite.getPosition().x;
            float shuriken_pos_y = sprite.getPosition().y;
            return Vector2f(ninja_x_for_homing-shuriken_pos_x,ninja_y_for_homing-shuriken_pos_y);
        }

        
        
        
        void Find_Angle(Vector2f vector)
        {
            float dot_product=vector.x*move_x+vector.y*move_y;
            float module_vector1=sqrt(vector.x*vector.x+vector.y*vector.y);
            float module_vector2=sqrt(move_x*move_x+move_y*move_y);
            float a=abs(acos(dot_product/(module_vector1*module_vector2)));
            float cross_product=vector.x*move_y-vector.y*move_x;
            if (a>=pi/180.0f)
            {
                
                if (cross_product<0) Right_Rotate(15);
                else Left_Rotate(15);
            }
        }
    
    public:
        HomingShuriken(Texture& texture, sf::Vector2f size, sf::Vector2f pos, float speed)
            : Shuriken(texture, size, pos, speed)
        {
            clock_homing.restart();
        }
    virtual void Move() override
    {
        Shuriken::Move();
        if (clock_homing.getElapsedTime().asSeconds()>0.25)
        {
            Find_Angle(Find_Vector());
            clock_homing.restart();
        }


    }
};



int main()
{
    Clock clockspawn;
    Clock clockFPS;
    Clock clock;
    float deltatime;

    const float SPEED_NINJA = 5.0f;
    const float SPEED_SHURIKEN = 5.0f;


    float move_ninja, move_shuriken;
    sf::RenderWindow window(sf::VideoMode(1200, 900), "Ninja");
    
    Texture textureninja;
    textureninja.loadFromFile("Images\\ninja.png");
    Ninja ninja(textureninja, sf::Vector2f(100, 100),sf::Vector2f(500,500),SPEED_NINJA);

    srand(time(NULL));
    int randshuriken;

    Texture textureshuriken1, textureshuriken2,textureshuriken3,textureshuriken4;
    textureshuriken1.loadFromFile("Images\\shuriken.png");
    textureshuriken2.loadFromFile("Images\\fast_shuriken.png");
    textureshuriken3.loadFromFile("Images\\chaotic_shuriken.png");
    textureshuriken4.loadFromFile("Images\\homing_shuriken.png");
    vector<Shuriken*> shurikens;

    Text text;
    Font font;
    font.loadFromFile("Fonts\\arial.ttf");
    text.setFont(font);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        deltatime = clockFPS.getElapsedTime().asSeconds()*100;
        clockFPS.restart();
        move_ninja=SPEED_NINJA*deltatime;
        move_shuriken=SPEED_SHURIKEN*deltatime;
        if (clockspawn.getElapsedTime().asSeconds()>1.5)
        {
            randshuriken=rand()%4;
            switch(randshuriken)
            {
                case 0:
                    shurikens.push_back(new Shuriken(textureshuriken1, sf::Vector2f(50, 50),sf::Vector2f(1,1),move_shuriken));
                    break;
                case 1:
                    shurikens.push_back(new FastShuriken(textureshuriken2, sf::Vector2f(50, 50),sf::Vector2f(1,1),move_shuriken));
                    break;
                case 2:
                    shurikens.push_back(new ChaoticShuriken(textureshuriken3, sf::Vector2f(50, 50),sf::Vector2f(1,1),move_shuriken));
                    break;
                case 3:
                    shurikens.push_back(new HomingShuriken(textureshuriken4, sf::Vector2f(50, 50),sf::Vector2f(1,1),move_shuriken));
                    break;
            }
            clockspawn.restart();
        }
        window.clear(sf::Color::Yellow);
        ninja.speed=move_ninja;
        ninja.Move();
        for (int i=0;i<shurikens.size();i++)

        {
            shurikens[i]->Find_Ninja(ninja);
            shurikens[i]->Move();
            shurikens[i]->Draw(window);
            
        }
        for (int i=shurikens.size()-1;i>=0;i--)
        {
            if (shurikens[i]->Destroy()) shurikens.erase(shurikens.begin()+i);
            if (ninja.isCollide(*(shurikens[i])))
            {
                clock.restart();
                text.setString("Game Over");
                text.setCharacterSize(50);
                text.setPosition(400, 400);
                
                shurikens.clear();
                while (clock.getElapsedTime().asSeconds() < 2)
                {
                   window.clear(sf::Color::Yellow);
                   
                    window.draw(text);
                    window.display(); /* code */
                }
                
            }
        }
        ninja.Draw(window);
        window.display();
    }

    return 0;
}