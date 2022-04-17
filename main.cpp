#include <SFML/Graphics.hpp>
#include <time.h>
#include <list>
#include <iostream>
#include <cmath>
#include <SFML/Audio.hpp>

//Modified by: James Ostrowski
//Date: 11/29/21

//Description: Asteroids Game, use the spaceship to shoot and destroy the surrounding asteroids.

//UFO image: UFO by Freepik, https://www.flaticon.com, License: Free for personal use.
//Sound: ufomono by NoiseCollecor, https://www.freesound.org, License: Creative Commons attribution license.

using namespace sf;
using namespace std;

const int W = 1200; //width of the background
const int H = 800; //height of the background


float DEGTORAD = 0.017453f; //default float for ship movement.
//Animation Class
//Description: creates an animation for the objects within the game and their movemnet.
class Animation
{
public:
	float Frame, speed; //member variables for frame rate and speed
	Sprite sprite; //new sprite
    std::vector<IntRect> frames; //vector of frames

	Animation(){}

    Animation (Texture &t, int x, int y, int w, int h, int count, float Speed) // animation constructor
	{
	    Frame = 0; //set frame to 0
        speed = Speed; //set speed based on parameter input

		for (int i=0;i<count;i++) { //adds frames to the vector based on input count parameter.
            frames.push_back(IntRect(x + i * w, y, w, h));
        }
		sprite.setTexture(t); //set texture based on image input
		sprite.setOrigin(w/2,h/2); //set origin based on width and height.
        sprite.setTextureRect(frames[0]); //set texture based on frames.
	}

	//Description: updates the current speed and frames of the textures(images).
	//Inputs: none
	//Outputs: none
	void update()
	{
    	Frame += speed; //changes frame based on speed.
		int n = frames.size(); //gets the amount
		if (Frame >= n){ //subtracts if too many.
		    Frame -= n;
		}
		if (n>0){
		    sprite.setTextureRect( frames[int(Frame)] );
		}
	}

	bool isEnd(){ //check to see if it the end based on frames.
	  return Frame+speed>=frames.size();
	}

};

//Class Entity
//An object within the game
class Entity
{
public:
float x,y,dx,dy,R,angle; //parameters of the entity
bool life; //if entity is alive and not destroyed.
std::string name;
Animation anim; //creates and animation

Entity(){ //default life is 1.
  life=1;
}
//Description: Changes the setting and angle of entity
//Inputs: animation, ints for coordinates, angle of object and radius.
//Outputs: none.
void settings(Animation &a,int X,int Y,float Angle=0,int radius=1){
  anim = a; //sets animation equal to a.
  x=X; y=Y; //Changes x an y coordinates.
  angle = Angle; //changes angle
  R = radius; //changes radius
}

virtual void update(){}; //virtual update function.

//Description:
void draw(RenderWindow &app){
  anim.sprite.setPosition(x,y); //set image position
  anim.sprite.setRotation(angle+90); //set rotation for image.
  app.draw(anim.sprite); //draw image.

  CircleShape circle(R);
  circle.setFillColor(Color(255,0,0,170)); //change color.
  circle.setPosition(x,y);//set position of circle.
  circle.setOrigin(R,R); //set origin of circle.
  //app.draw(circle);
}

virtual ~Entity(){}; //Virtual destructor
};

//Class asteroid
//Description: Creates asteroid for game.
class asteroid: public Entity
{
public:
  asteroid(){ //constructor
    dx=rand()%8-4; //changes position x and y randomly.
    dy=rand()%8-4;
    name="asteroid"; //named
    ++objectcount; //increase object count through constructor.
  }
  ~asteroid(){
      --objectcount; //decrease object count through destructor.
  }
  static unsigned int getobjectcount(){return objectcount;}

void  update(){ //updates coordinates based on entity class
   x+=dx; //moves asteroid randomly
   y+=dy;

   if (x>W) { //if asteroid goes off the screen reset, next 12 lines
       x=0;
   }
   if (x<0) {
       x = W;
   }
   if (y>H) {
       y=0;
   }
   if (y<0){
       y=H;
   }
  }
private:
    static unsigned int objectcount; //static member variable for asteroid count.

};
unsigned int asteroid::objectcount{0}; //declared on global scope.
//Class Bullet
//Description: creates bullet animation
class bullet: public Entity
{
public:
  bullet(){ //constructor
    name="bullet";
  }

void  update(){
   dx=cos(angle*DEGTORAD)*6; //matches movement based on same factor as spaceship
   dy=sin(angle*DEGTORAD)*6;
  // angle+=rand()%6-3;
   x+=dx; //changes coordinate.
   y+=dy;

   if (x>W || x<0 || y>H || y<0){ //deletes bullet
       life=0;
   }
  }

};
//class UFO
//description: display a UFO across the screen at a select time.
class UFO: public Entity  {
public:
    UFO(){ //UFO constructor
        name = "UFO"; //entity name
        y = 100;//starting position.
    }

    void update(){ //remove image when off the screen
        if(x > W || x < 0){
            life = 0;
        }
    }
};

//Class Player
//Description creates a player for spaceship
class player: public Entity
{
public:
   bool thrust; //for when player is moving forward.

   player(){ //constructor
     name="player"; //name of entity
   }

   void update(){ //changes speed based on thrust
     if (thrust) { dx+=cos(angle*DEGTORAD)*0.2;
        dy+=sin(angle*DEGTORAD)*0.2;
      }
     else {
         dx*=0.99; //default
         dy*=0.99;
     }

    int maxSpeed=15;
    float speed = sqrt(dx*dx+dy*dy); //gets speed based on x and y rate of change.
    if (speed>maxSpeed) {
        dx *= maxSpeed/speed; //changes speed based to not exceed max.
        dy *= maxSpeed/speed;
     }

    x+=dx; //changes coordinates based on speed.
    y+=dy;

    if (x>W) { //resets when off screen
        x = 0;
    }
    if (x<0){ //resets when off screen
        x=W;
    }
    if (y>H){ //resets when off screen
        y=0;
    }
    if (y<0){ //resets when off screen
        y=H;
    }
   }

};


bool isCollide(Entity *a,Entity *b) { //checks to see if two entities are colliding.
  return (b->x - a->x)*(b->x - a->x)+
         (b->y - a->y)*(b->y - a->y)<
         (a->R + b->R)*(a->R + b->R);
}


int main() {
    srand(time(0));
    const int t = 10; //const int for UFO timer.
    RenderWindow app(VideoMode(W, H), "Asteroids!"); //creates window.
    app.setFramerateLimit(60); //sets framerate.

    Texture t1,t2,t3,t4,t5,t6,t7,t8;
    if(!t1.loadFromFile("images/spaceship.png")){return EXIT_FAILURE;} //ship image
    if(!t2.loadFromFile("images/background.jpg")){return EXIT_FAILURE;} // outer space
    if(!t3.loadFromFile("images/explosions/type_C.png")){return EXIT_FAILURE;} //explosion
    if(!t4.loadFromFile("images/rock.png")){return EXIT_FAILURE;} //asteroid
    if(!t5.loadFromFile("images/fire_blue.png")){return EXIT_FAILURE;}//bullet
    if(!t6.loadFromFile("images/rock_small.png")){return EXIT_FAILURE;} //small asteroid
    if(!t7.loadFromFile("images/explosions/type_B.png")){return EXIT_FAILURE;} //different explosion type
    if(!t8.loadFromFile("images/ufo.png")){return EXIT_FAILURE;} //UFO image

    t1.setSmooth(true); //smooths images.
    t2.setSmooth(true);

    SoundBuffer buffer; //Creates buffer for background music.
    if(!buffer.loadFromFile("Sounds/UFO.wav")){return EXIT_FAILURE;} //checks to make sure file opens

    Sound sound; //creates sound.
    sound.setBuffer(buffer); //sets sound to sound from file.

    Sprite background(t2);


    Animation sExplosion(t3, 0,0,256,256, 48, 0.5); //sets animation for all images on next 8 lines
    Animation sRock(t4, 0,0,64,64, 16, 0.2);
    Animation sRock_small(t6, 0,0,64,64, 16, 0.2);
    Animation sBullet(t5, 0,0,32,64, 16, 0.8);
    Animation sPlayer(t1, 40,0,40,40, 1, 0);
    Animation sPlayer_go(t1, 40,40,40,40, 1, 0);
    Animation sExplosion_ship(t7, 0,0,192,192, 64, 0.5);
    Animation sUFO(t8,0,0,600,600,1,0);
    sUFO.sprite.setScale(0.2,0.2); //scales down image i chose for UFO
    Clock clock; //clock for timer.
    float timer = 0; //timer for UFO
    std::list<Entity*> entities; //creates a vector of entities.

    for(int i=0;i<15;i++) { //creates 15 asteroids.
      asteroid *a = new asteroid();
      a->settings(sRock, rand()%W, rand()%H, rand()%360, 25); //changes the settings and position of each one.
      entities.push_back(a); //adds to vector of entities
    }

    player *p = new player(); //creates a player.
    p->settings(sPlayer,200,200,0,20); //sets player coordinates
    entities.push_back(p); //adds to vector of entities

    UFO *u = new UFO();
    u->settings(sUFO, 0, 100, 0, 20);
    entities.push_back(u);


    /////main loop/////
    while (app.isOpen()){ //while game open
        Event event;
        while (app.pollEvent(event)) {
            if (event.type == Event::Closed) { //if user closes game
                app.close();
            }

            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Space) { //if user presses space bar.
                    bullet *b = new bullet(); //creates bullet when space bar pressed to shoot.
                    b->settings(sBullet, p->x, p->y, p->angle, 10); //sets path of the bullet.
                    entities.push_back(b); //adds bullet to entities.
                }
            }
        }
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

    if (Keyboard::isKeyPressed(Keyboard::Right)) p->angle+=3; // movement of space ship next 3 lines based on keys pressed.
    if (Keyboard::isKeyPressed(Keyboard::Left))  p->angle-=3;
    if (Keyboard::isKeyPressed(Keyboard::Up)) p->thrust=true;
    else p->thrust=false; //if no key is pressed



    for(auto a:entities) //nested loop to check between two entities
     for(auto b:entities) {
         if (a->name == "asteroid" && b->name == "bullet") { //if bullet and asteroid
             if (isCollide(a, b)) { //if bullets collides with an asteroid
                 a->life = false; //delete both entities.
                 b->life = false;

                 Entity *e = new Entity(); //create new entity to represent explosion
                 e->settings(sExplosion, a->x, a->y); //sets coordinates for explosion
                 e->name = "explosion";
                 entities.push_back(e); //add to entities


                 for (int i = 0; i < 2; i++) { //spawns new asteroid in its place.
                     if (a->R == 15) continue;
                     Entity *e = new asteroid(); //creates new asteroid but smaller
                     e->settings(sRock_small, a->x, a->y, rand() % 360, 15);
                     entities.push_back(e); //adds to entities.
                 }

             }
         }

      if (a->name=="player" && b->name=="asteroid") { // if player collides with an asteroid
          if (isCollide(a, b)) {
              b->life = false; //asteroid deleted.

              Entity *e = new Entity(); //creates ship explosion
              e->settings(sExplosion_ship, a->x, a->y); //sets coordinates of explosion
              e->name = "explosion";
              entities.push_back(e); //adds to entities.

              p->settings(sPlayer, W / 2, H / 2, 0, 20); // player gets reset to center of game.
              p->dx = 0;
              p->dy = 0;
          }
      }
         if (a->name=="player" && b->name=="UFO") { // if player collides with an asteroid
             if (isCollide(a, b)) {
                 b->life = false; //asteroid deleted.

                 Entity *e = new Entity(); //creates ship explosion
                 e->settings(sExplosion_ship, a->x, a->y); //sets coordinates of explosion
                 e->name = "explosion";
                 entities.push_back(e); //adds to entities.

                 p->settings(sPlayer, W / 2, H / 2, 0, 20); // player gets reset to center of game.
                 p->dx = 0;
                 p->dy = 0;
             }
         }
         if (a->name == "UFO" && b->name == "bullet") { //if bullet and asteroid
             if (isCollide(a, b)) { //if bullets collides with an asteroid
                 a->life = false; //delete both entities.
                 b->life = false;

                 Entity *e = new Entity(); //create new entity to represent explosion
                 e->settings(sExplosion, a->x, a->y); //sets coordinates for explosion
                 e->name = "explosion";
                 entities.push_back(e); //add to entities
             }
         }
    }


    if (p->thrust) { //if ship is moving forward
        p->anim = sPlayer_go;
    }
    else {
        p->anim = sPlayer;
    }

    for(auto e:entities)
     if (e->name=="explosion") //checks to invoke explosion
      if (e->anim.isEnd()) { //set explosion life to 0
          e->life=0;
      }

    if(asteroid::getobjectcount() == 0){
        for(int i=0;i<15;i++) { //creates 15 asteroids.
            asteroid *a = new asteroid();
            a->settings(sRock, rand()%W, rand()%H, rand()%360, 25); //changes the settings and position of each one.
            entities.push_back(a); //adds to vector of entities
        }
    }
    //if (rand()%150==0){
    //   asteroid *a = new asteroid(); //creates random asteroid
    //   a->settings(sRock, 0,rand()%H, rand()%360, 25);
    //   entities.push_back(a); //add object to entities.
    // }

    for(auto i=entities.begin();i!=entities.end();) { //loop through entities
      Entity *e = *i;

      e->update(); //update object positions on the screen
      e->anim.update(); //updates animations of each object

      if (e->life==false){  //if life is 0 or false delete object.
          i=entities.erase(i);
          delete e; //delete object.
      }
      else{
          i++;
      }
    }



   //////draw//////
   app.draw(background); //draw background
   if (timer > t && timer < 10.05){//play sounds when ufo on screen
       sound.play();
   }
   for(auto i:entities) { //display objects ot the screen
       if(i->name == "UFO"){ //if UFO is and entity
          if(timer > t) { //if timer is greater than constant (t = 10 seconds between each UFO sighting)
              i->x +=2; //move UFO across the screen
              i->draw(app); //draw UFO
          }
   } else {
           i->draw(app); //if not UFO draw entity
       }

   }
   if(timer > t+11){ //11 seconds for UFO to cross the screen, waits until UFO gone to reset timer.
       timer = 0;
       sound.stop(); //Stop the UFO sound.
       UFO *u = new UFO(); //Creates a new UFO after the old one is deleted and makes sure only 1 exists at a time.
       u->settings(sUFO, 0, 100, 0, 20);
       entities.push_back(u); //add to entities.
   }

   app.display();
    }

    return 0;
}
