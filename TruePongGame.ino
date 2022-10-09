#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define UP_BUTTON 2
#define DOWN_BUTTON 3
#define UP_BUTTON2 4
#define DOWN_BUTTON2 5
#define buzzer 11

const unsigned long PADDLE_RATE = 33;
const unsigned long BALL_RATE = 30;
const uint8_t PADDLE_HEIGHT = 12;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 OLED(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void drawCourt();

uint8_t ball_x = 64, ball_y = 16;
uint8_t ball_dir_x = 1, ball_dir_y = 1;

unsigned long ball_update;
unsigned long paddle_update;

const uint8_t PLAYER2_X = 12;
uint8_t player2_y = 10;

const uint8_t PLAYER_X = 115;
uint8_t player_y = 10;

int MAX_SCORE = 8;
int PLAYER_SCORE = 0;
int PLAYER2_SCORE = 0;

boolean gameIsRunning = true;
boolean resetBall = false;

void showScore();
void gameover();

void setup() {
    OLED.begin(SSD1306_SWITCHCAPVCC, 0x3C);

    
    OLED.display();
    unsigned long start = millis();

    pinMode(UP_BUTTON, INPUT);
    pinMode(DOWN_BUTTON, INPUT);
    pinMode(UP_BUTTON2, INPUT);
    pinMode(DOWN_BUTTON2, INPUT);
    pinMode(buzzer,OUTPUT);
    digitalWrite(UP_BUTTON,1);
    digitalWrite(DOWN_BUTTON,1);
    digitalWrite(UP_BUTTON2,1);
    digitalWrite(DOWN_BUTTON2,1);
    OLED.clearDisplay();
    drawCourt();

    while(millis() - start < 2000);

    OLED.display();

    ball_update = millis();
    paddle_update = ball_update;
    ball_x = random(25,65); 
    ball_y = random(3,30);
}

void loop() {
    OLED.display();
    unsigned long time = millis();

    static bool up_state = false;
    static bool down_state = false;
    static bool up_state2 = false;
    static bool down_state2 = false;
    
    up_state |= (digitalRead(UP_BUTTON) == LOW);
    down_state |= (digitalRead(DOWN_BUTTON) == LOW);
    up_state2 |= (digitalRead(UP_BUTTON2) == LOW);
    down_state2 |= (digitalRead(DOWN_BUTTON2) == LOW);

    if(resetBall)
    {
      ball_x = random(25,70); 
      ball_y = random(3,30);
      do
      {
      ball_dir_x = random(-1,2);
      }while(ball_dir_x==0);

       do
      {
      ball_dir_y = random(-1,2);
      }while(ball_dir_y==0);
      
      
      resetBall=false;
    }
    
    if(time > ball_update && gameIsRunning) {
        uint8_t new_x = ball_x + ball_dir_x;
        uint8_t new_y = ball_y + ball_dir_y;
   
        // Check if we hit the vertical walls
        if(new_x == 0) 
        {
           PLAYER_SCORE++;
            if(PLAYER_SCORE==MAX_SCORE)
            {
              gameOver();
            }
            else
            {
              showScore();
            }
        }
        if(new_x == 127) {
           PLAYER2_SCORE++;
            if(PLAYER2_SCORE==MAX_SCORE)
            {
              gameOver();
            }else
            {
              showScore();
            }
        }

        // Check if we hit the horizontal walls.
        if(new_y == 0 || new_y == 31) {
          tone(buzzer,100);
          noTone(buzzer);
            ball_dir_y = -ball_dir_y;
            new_y += ball_dir_y + ball_dir_y;
        }

        // Check if we hit the CPU paddle
        if(new_x == PLAYER2_X && new_y >= player2_y && new_y <= player2_y + PADDLE_HEIGHT) {
          tone(buzzer,100);
          noTone(buzzer);
            ball_dir_x = -ball_dir_x;
            new_x += ball_dir_x + ball_dir_x;
        }

        // Check if we hit the player paddle
        if(new_x == PLAYER_X
           && new_y >= player_y
           && new_y <= player_y + PADDLE_HEIGHT)
        {
          tone(buzzer,100);
          noTone(buzzer);
            ball_dir_x = -ball_dir_x;
            new_x += ball_dir_x + ball_dir_x;
        }

        OLED.drawPixel(ball_x, ball_y, BLACK);
        OLED.drawPixel(new_x, new_y, WHITE);
        ball_x = new_x;
        ball_y = new_y;

        ball_update += BALL_RATE;

        
    }

    if(time > paddle_update && gameIsRunning) {
        paddle_update += PADDLE_RATE;

        // CPU paddle
        OLED.drawFastVLine(PLAYER2_X, player2_y, PADDLE_HEIGHT, BLACK);
        if(up_state2) {
            player2_y -= 1;
        }
        if(down_state2) {
            player2_y += 1;
        }
        up_state2 = down_state2 = false;
        if(player2_y < 1) player2_y = 1;
        if(player2_y + PADDLE_HEIGHT > 31) player2_y = 31 - PADDLE_HEIGHT;
        OLED.drawFastVLine(PLAYER2_X, player2_y, PADDLE_HEIGHT, WHITE);

        // Player paddle
        OLED.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, BLACK);
        if(up_state) {
            player_y -= 1;
        }
        if(down_state) {
            player_y += 1;
        }
        up_state = down_state = false;
        if(player_y < 1) player_y = 1;
        if(player_y + PADDLE_HEIGHT > 31) player_y = 31 - PADDLE_HEIGHT;
        OLED.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, WHITE);

        
    }
}


void drawCourt() {
    OLED.drawRect(0, 0, 128, 32, WHITE);
}

void showScore()
{
  gameIsRunning = false;
  OLED.clearDisplay();
  OLED.display();
  drawCourt();
  
  OLED.setCursor(20,12);
  OLED.setTextColor(WHITE);
  OLED.setTextSize(1);
  OLED.print("Score");

  OLED.setCursor(60,12);
  OLED.setTextColor(WHITE);
  OLED.setTextSize(1);
  OLED.print(String(PLAYER2_SCORE));

  OLED.setCursor(80,12);
  OLED.setTextColor(WHITE);
  OLED.setTextSize(1);
  OLED.print(":");

  OLED.setCursor(100,12);
  OLED.setTextColor(WHITE);
  OLED.setTextSize(1);
  OLED.print(String(PLAYER_SCORE));
OLED.display();
 delay(2000);
 unsigned long start = millis();

 OLED.clearDisplay();
 drawCourt();
while(millis() - start < 2000);
ball_update = millis();    
paddle_update = ball_update;
gameIsRunning = true;
resetBall=true;
}

void gameOver()
{
  gameIsRunning = false;
  OLED.clearDisplay();
  OLED.display();
  drawCourt();
  if(PLAYER_SCORE>PLAYER2_SCORE)
  {
    OLED.setCursor(30,4);
    OLED.setTextColor(WHITE);
    OLED.setTextSize(1);
    OLED.print("Player1 Win");
  }else
  {
    OLED.setCursor(30,4);
    OLED.setTextColor(WHITE);
    OLED.setTextSize(1);
    OLED.print("Player2 WiN");
  }

  OLED.setCursor(40,20);
  OLED.setTextColor(WHITE);
  OLED.setTextSize(1);
  OLED.print(String(PLAYER2_SCORE));
  
  OLED.setCursor(60,20);
  OLED.setTextColor(WHITE);
  OLED.setTextSize(1);
  OLED.print(":");

  OLED.setCursor(80,20);
  OLED.setTextColor(WHITE);
  OLED.setTextSize(1);
  OLED.print(String(PLAYER_SCORE));

  OLED.display();
  
  delay(2000);

  while(digitalRead(UP_BUTTON) == HIGH && digitalRead(DOWN_BUTTON) == HIGH)  
  {
    delay(100);
  }
  gameIsRunning = true;
  
  PLAYER2_SCORE = PLAYER_SCORE = 0;
  
  unsigned long start = millis();
  OLED.clearDisplay();
  OLED.display();
  drawCourt();
 
 while(millis() - start < 2000);
 ball_update = millis();    
 paddle_update = ball_update;
 gameIsRunning = true;
 resetBall=true;
}
