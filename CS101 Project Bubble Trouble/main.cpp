#include <simplecpp>
#include <sstream>
#include "shooter.h"
#include "bubble.h"

/* Simulation Vars */
const double STEP_TIME = 0.02;

/* Game Vars */
const int PLAY_Y_HEIGHT = 450;
const int LEFT_MARGIN = 70;
const int TOP_MARGIN = 20;
const int BOTTOM_MARGIN = (PLAY_Y_HEIGHT+TOP_MARGIN);
int health = 3;


void move_bullets(vector<Bullet> &bullets){
    // move all bullets
    for(unsigned int i=0; i<bullets.size(); i++){
        if(!bullets[i].nextStep(STEP_TIME)){
            bullets.erase(bullets.begin()+i);
        }
    }
}

void move_bubbles(vector<Bubble> &bubbles){
    // move all bubbles
    for (unsigned int i=0; i < bubbles.size(); i++)
    {
        bubbles[i].nextStep(STEP_TIME);
    }
}

vector<Bubble> create_bubbles()
{
    // create initial bubbles in the game
    vector<Bubble> bubbles;
    bubbles.push_back(Bubble(WINDOW_X/2.0, BUBBLE_START_Y, 4.0*BUBBLE_DEFAULT_RADIUS, -BUBBLE_DEFAULT_VX, 0, COLOR(255,100,150)));
    bubbles.push_back(Bubble(WINDOW_X/4.0, BUBBLE_START_Y, 4.0*BUBBLE_DEFAULT_RADIUS, BUBBLE_DEFAULT_VX, 0, COLOR(255,100,150)));
    return bubbles;
}

bool bullet_hits_bubble(vector<Bullet> &bullets, vector<Bubble> &bubbles)
{
    // When bullet hits bubble
    for(unsigned int i=0; i < bullets.size(); i++)
    {
      for(unsigned int j=0; j < bubbles.size(); j++)
        {
          if(pow(bullets[i].get_center_x()-bubbles[j].get_center_x(),2) + pow(bullets[i].get_center_y()-bubbles[j].get_center_y(),2) <
             pow((bullets[i].get_height()/2.0) + bubbles[j].get_radius(),2))
             {
               bullets.erase(bullets.begin()+i);
               if(bubbles[j].get_radius() == BUBBLE_RADIUS_THRESHOLD)  bubbles.erase(bubbles.begin()+j);    // to make bubble and bullet disappear
               else if (bubbles[j].get_radius() == 2*BUBBLE_DEFAULT_RADIUS)                                 // to break the bubble
               {
                 bubbles.push_back(Bubble(bubbles[j].get_center_x(),bubbles[j].get_center_y() , BUBBLE_DEFAULT_RADIUS, -BUBBLE_DEFAULT_VX*2, bubbles[j].get_vy(), COLOR(155,150,255)));
                 bubbles.push_back(Bubble(bubbles[j].get_center_x(),bubbles[j].get_center_y() , BUBBLE_DEFAULT_RADIUS, +BUBBLE_DEFAULT_VX*2, bubbles[j].get_vy(), COLOR(155,150,255)));
                 bubbles.erase(bubbles.begin()+j);
               }
               else if (bubbles[j].get_radius() == 4.0*BUBBLE_DEFAULT_RADIUS)
               {
                 bubbles.push_back(Bubble(bubbles[j].get_center_x(),bubbles[j].get_center_y() , 2.0*BUBBLE_DEFAULT_RADIUS, -BUBBLE_DEFAULT_VX*2, bubbles[j].get_vy(), COLOR(255,150,100)));
                 bubbles.push_back(Bubble(bubbles[j].get_center_x(),bubbles[j].get_center_y() , 2.0*BUBBLE_DEFAULT_RADIUS, +BUBBLE_DEFAULT_VX*2, bubbles[j].get_vy(), COLOR(255,150,100)));
                 bubbles.erase(bubbles.begin()+j);
               }
               return true;
             }
        }
    }
    return false;
}

void bubble_hits_shooter(vector<Bubble> &bubbles, Shooter &shooter, int &health)
{
    // when bubble hits shooter
    for(unsigned int j=0; j < bubbles.size(); j++)
    {
      if(abs(bubbles[j].get_center_x() - shooter.get_body_center_x()) < 12+bubbles[j].get_radius() &&
         abs(bubbles[j].get_center_x() - shooter.get_body_center_x()) > 7 +bubbles[j].get_radius() &&
         bubbles[j].get_center_y() > 425 - bubbles[j].get_radius())
      {
        health-=1;
      }
    }

}


int main()
{
    initCanvas("Bubble Trouble", WINDOW_X, WINDOW_Y);

    Line b1(0, PLAY_Y_HEIGHT, WINDOW_X, PLAY_Y_HEIGHT);
    b1.setColor(COLOR(0, 0, 255));

    Text Time(LEFT_MARGIN, TOP_MARGIN, "Time:");
    Text Health(450 - LEFT_MARGIN, TOP_MARGIN, "Health:");
    Text Score (455 - LEFT_MARGIN, BOTTOM_MARGIN, "Score:");

    string msg_cmd("Cmd: _");
    Text charPressed(LEFT_MARGIN, BOTTOM_MARGIN, msg_cmd);

    string msg_score("");
    Text score_card(500 - LEFT_MARGIN, BOTTOM_MARGIN, msg_score);

    string msg_time_left(" ");
    Text game_timer(LEFT_MARGIN+40, TOP_MARGIN, msg_time_left);

    string msg_lives_left("");
    Text health_counter(500 - LEFT_MARGIN, TOP_MARGIN, msg_lives_left);

    // Initialize time,health,score
    int time = 50;
    int unit_convertor = 2500;
    int score = 0;



    // Intialize the shooter
    Shooter shooter(SHOOTER_START_X, SHOOTER_START_Y, SHOOTER_VX);

    // Initialize the bubbles
    vector<Bubble> bubbles = create_bubbles();

    // Initialize the bullets (empty)
    vector<Bullet> bullets;

    XEvent event;

    // Main game loop
    while (true)
    {
        // Declaring stringstreams
        stringstream ss_score(msg_score);
        stringstream ss_timer(msg_time_left);
        stringstream ss_health(msg_lives_left);

        bool pendingEvent = checkEvent(event);
        if (pendingEvent)
        {
            // Get the key pressed
            char c = charFromEvent(event);
            msg_cmd[msg_cmd.length() - 1] = c;
            charPressed.setMessage(msg_cmd);

            // Update the shooter
            if(c == 'a')
                shooter.move(STEP_TIME, true);
            else if(c == 'd')
                shooter.move(STEP_TIME, false);
            else if(c == 'w')
                bullets.push_back(shooter.shoot());
            else if(c == 'q')
                return 0;
        }

        // Update the bubbles
        move_bubbles(bubbles);

        // Update the bullets
        move_bullets(bullets);

        // Update score_card
        if(bullet_hits_bubble(bullets,bubbles))
        {
          score+=1;
          ss_score << score;                // Adding the score to the stringstream ss_score
          ss_score >>  msg_score;           // and reading it into msg_score string
          score_card.setMessage(msg_score); // Update score_card
          if(score == 14)
          {
            Text msg_you_won(250,250,"You Won!");
            msg_you_won.setColor(COLOR(0,255,0));
            msg_you_won.setFill(true);
            msg_you_won.scale(3.0);
            wait(10);
            return false;
          }
        }

        // Update timer
        if(time >= 10)
        {
          time = STEP_TIME*unit_convertor;
          unit_convertor-=1;
          ss_timer << time;
          ss_timer << "/50";
          ss_timer >> msg_time_left;
          game_timer.setMessage(msg_time_left);
        }
        else
        {
          Text msg_remainder(250,50,"Last 10 Seconds");
          time = STEP_TIME*unit_convertor;
          unit_convertor-=1;
          ss_timer <<"0";
          ss_timer << time;
          ss_timer << "   ";
          ss_timer >> msg_time_left;
          game_timer.setMessage(msg_time_left);
        }
        if(time == 0)
        {
          Text time_over_msg(250,250,"Time Over!");
          time_over_msg.setColor(COLOR(255,0,0));
          time_over_msg.setFill(true);
          time_over_msg.scale(3.0);
          wait(10);
          return false;
        }

        //Update health
          bubble_hits_shooter(bubbles, shooter, health);
          ss_health << health ;
          ss_health << "/3";
          ss_health >> msg_lives_left;
          health_counter.setMessage(msg_lives_left);
          if(health == 0)
          {
            Text msg_game_lost(250,250,"You lose");
            msg_game_lost.setColor(COLOR(155,155,155));
            msg_game_lost.setFill(true);
            msg_game_lost.scale(3.0);
            wait(10);
            return false;
          }
        wait(STEP_TIME);
    }
}
