#include "raylib.h"
#include "raymath.h"
#include <cstdlib>

inline float Random(float min, float max)
{
    return min + (rand() / ((float)RAND_MAX / (max - min)));
}

constexpr float SCREEN_WIDTH = 1200.0f;
constexpr float SCREEN_HEIGHT = 800.0f;
constexpr Vector2 CENTER{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };

// Ball can move half the screen width per-second
constexpr float BALL_SPEED = SCREEN_WIDTH * 0.5f;
constexpr float BALL_SIZE = 40.0f;

// Paddles can move half the screen height per-second
constexpr float PADDLE_SPEED = SCREEN_HEIGHT * 0.5f;
constexpr float PADDLE_WIDTH = 40.0f;
constexpr float PADDLE_HEIGHT = 80.0f;

struct Box
{
    float xMin;
    float xMax;
    float yMin;
    float yMax;
};

bool BoxOverlap(Box box1, Box box2)
{
    bool x = box1.xMax >= box2.xMin && box1.xMin <= box2.xMax;
    bool y = box1.yMax >= box2.yMin && box1.yMin <= box2.yMax;
    return x && y;
}

Rectangle BoxToRec(Box box)
{
    Rectangle rec;
    rec.x = box.xMin;
    rec.y = box.yMin;
    rec.width = box.xMax - box.xMin;
    rec.height = box.yMax - box.yMin;
    return rec;
}

Box BallBox(Vector2 position)
{
    Box box;
    box.xMin = position.x - BALL_SIZE * 0.5f;
    box.xMax = position.x + BALL_SIZE * 0.5f;
    box.yMin = position.y - BALL_SIZE * 0.5f;
    box.yMax = position.y + BALL_SIZE * 0.5f;
    return box;
}

Box PaddleBox(Vector2 position)
{
    Box box;
    box.xMin = position.x - PADDLE_WIDTH * 0.5f;
    box.xMax = position.x + PADDLE_WIDTH * 0.5f;
    box.yMin = position.y - PADDLE_HEIGHT * 0.5f;
    box.yMax = position.y + PADDLE_HEIGHT * 0.5f;
    return box;
}

void ResetBall(Vector2& position, Vector2& direction)
{
    position = CENTER;
    direction.x = rand() % 2 == 0 ? -1.0f : 1.0f;
    direction.y = 0.0f;
    direction = Vector2Rotate(direction, Random(0.0f, 360.0f) * DEG2RAD);
}

void DrawBall(Vector2 position, Color color)
{
    Box ballBox = BallBox(position);
    DrawRectangleRec(BoxToRec(ballBox), color);
}

void DrawPaddle(Vector2 position, Color color)
{
    Box paddleBox = PaddleBox(position);
    DrawRectangleRec(BoxToRec(paddleBox), color);
}

void RightWins()
{
    DrawText("Right Has Pinged The Pong", 450, 750, 20, GOLD);
}
void LeftWins()
{
    DrawText("Left Has Pinged The Pong", 450, 750, 20, GOLD);
   
}

int main()
{
    Vector2 ballPosition;
    Vector2 ballDirection;
    ResetBall(ballPosition, ballDirection);

    Vector2 paddle1Position, paddle2Position;
    paddle1Position.x = SCREEN_WIDTH * 0.05f;
    paddle2Position.x = SCREEN_WIDTH * 0.95f;
    paddle1Position.y = paddle2Position.y = CENTER.y;

    int testScore = 0;
    int GameColors = 0;
    int LeftScore = 0; //Changing the value to above 0 DOES NOT! Show on the screen
    int RightScore = 0;
    int WinScore = 5;
    int Difficulty = 0;

    int VictoryCondition1 = 0;
    int VictoryCondition2 = 0;

    LeftWins();
    RightWins();

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Jaxen Presents: Pong"); //A little personal flare
    InitAudioDevice();

    Sound PaddleHit = LoadSound("Audio/Paddle.wav");
    Sound WallHit = LoadSound("Audio/Wall.wav");
    Sound ScoreSound = LoadSound("Audio/Point.wav");

    SetTargetFPS(60);


    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        float ballDelta = BALL_SPEED * dt;
        float paddleDelta = PADDLE_SPEED * dt;

        // Move paddle with key input
        if (IsKeyDown(KEY_W))
            paddle1Position.y -= paddleDelta;
        if (IsKeyDown(KEY_S))
            paddle1Position.y += paddleDelta;

        if (IsKeyDown(KEY_I) || (IsKeyDown(KEY_UP)))
            paddle2Position.y -= paddleDelta;
        if (IsKeyDown(KEY_K) || (IsKeyDown(KEY_DOWN)))
            paddle2Position.y += paddleDelta;


        float phh = PADDLE_HEIGHT * 0.5f;
        paddle1Position.y = Clamp(paddle1Position.y, phh, SCREEN_HEIGHT - phh);
        paddle2Position.y = Clamp(paddle2Position.y, phh, SCREEN_HEIGHT - phh);

        // Change the ball's direction on-collision
        Vector2 ballPositionNext = ballPosition + ballDirection * ballDelta;
        Box ballBox = BallBox(ballPositionNext);
        Box paddle1Box = PaddleBox(paddle1Position);
        Box paddle2Box = PaddleBox(paddle2Position);
   
        if (ballBox.xMin < 0.0f) //Left Side
        {
            ballDirection.x *= -1.0f;
            RightScore += 1; 
            VictoryCondition1 = 0;
            VictoryCondition2 = 0;
            PlaySound(ScoreSound);
            ResetBall(ballPosition, ballDirection); 
        }
        if (ballBox.xMax > SCREEN_WIDTH) //Right Side
        {
            ballDirection.x *= -1.0f;
            LeftScore += 1;
            VictoryCondition1 = 0;
            VictoryCondition2 = 0;
            PlaySound(ScoreSound);
            ResetBall(ballPosition, ballDirection);
 
        }
        if (ballBox.yMin < 0.0f || ballBox.yMax > SCREEN_HEIGHT) //KEEP THIS! This is for the Y. No points should be scored on The Y axis.
        {
            ballDirection.y *= -1.0f;
           // VictoryCondition1 = 0; While i wouldnt loose marks for this, the message tends to go away too quickly like this. I figured give the winner some statifaction.
           // VictoryCondition2 = 0;
            PlaySound(WallHit);
         
        }
        if (BoxOverlap(ballBox, paddle1Box) || BoxOverlap(ballBox, paddle2Box)) //KEEP THIS! This is for the paddles vs Box
        {
            ballDirection.x *= -1.0f;
            VictoryCondition1 = 0;
            VictoryCondition2 = 0;
            PlaySound(PaddleHit);
           
        }

        // Update ball position after collision resolution, then render
        ballPosition = ballPosition + ballDirection * ballDelta;

        if (IsKeyPressed(KEY_ONE)) if (GameColors == 0)  //If And Equivalent
            GameColors = 1;
        else if (IsKeyPressed(KEY_ONE)) if (GameColors == 1)
            GameColors = 2;
        else if (IsKeyPressed(KEY_ONE)) if (GameColors == 2)
            GameColors = 0;

        if (IsKeyPressed(KEY_TWO)) if (Difficulty ==3) 
        {
            Difficulty = 0;
       
        }
        else if (IsKeyPressed(KEY_TWO)) if (Difficulty == 0)
        {
            Difficulty = 1;
    
            
        }
        else if (IsKeyPressed(KEY_TWO)) if (Difficulty == 1) 
        {
            Difficulty = 2;
  
        }
        else if (IsKeyPressed(KEY_TWO)) if (Difficulty == 2) 
        {
            Difficulty = 3;
        }

        switch (Difficulty)
        {
        default:
            case 0:
                WinScore = 5;
                break;
            case 1:
                WinScore = 10;
                break;
            case 2:
                WinScore = 15;
                break;
            case 3:
                WinScore = 20;
            break;
        }

        if (IsKeyDown(KEY_SPACE)) //Ball Reset Button
        {
            ResetBall(ballPosition, ballDirection);
        }
        if (IsKeyDown(KEY_ENTER)) //Super Reset 
        {
            LeftScore = 0;
            RightScore = 0;
            ResetBall(ballPosition, ballDirection);
        }
        if (IsKeyDown(KEY_ZERO)) if (IsKeyDown(KEY_NINE)) //True Reset
        {
            LeftScore = 0;
            RightScore = 0;
            GameColors = 0;
            Difficulty = 0;
            VictoryCondition1 = 0;
            VictoryCondition2 = 0;
            ResetBall(ballPosition, ballDirection);
        }
            

       /* if (RightScore == 5)
        {
            GameColors = 1;
        }
        else if (RightScore == 10)
        {
            GameColors = 2;
        else
        {
            GameColors = 0;
        } 
        This Worked in changing the colors PROVING the switch statement not only worked but it was actually that easy to change. This is how i knew it was my function changing the values that wasn't working
        */

        if (LeftScore || RightScore >= WinScore)
        {
            if (LeftScore == WinScore)
            {      
                //while (hit >= 0)  //This crashes the game 
                //{
                //    DrawText("Left Has Pinged The Pong", 210, 250, 20, GOLD);
                //}
                VictoryCondition1 += 1;
                LeftScore = 0;
                RightScore = 0;
            }
            if (RightScore == WinScore)
            {   
                VictoryCondition2 += 1;
                LeftScore = 0;
                RightScore = 0;
               
            }     
        }

        if (VictoryCondition1 >= 1)
        {
            LeftWins();
  
        }
        if (VictoryCondition2 >= 1)
        {
            RightWins();
     
        }
        
      
       // DrawText("Left Has Pinged The Pong", 450, 750, 20, GOLD);

        BeginDrawing();
        // Text format requires you to put a '%i' wherever you want an integer, then add said integer after the comma
        const char* LeftScoreText = TextFormat("Left Score: %i ", LeftScore);
        const char* RightScoreText = TextFormat("Right Score: %i ", RightScore);
        const char* WinScoreText = TextFormat("First To: %i ", WinScore);
        
        switch (GameColors)
        {
        default:
           case 0: //Default Mode
               ClearBackground(BLACK);
               DrawBall(ballPosition, WHITE);
               DrawPaddle(paddle1Position, BLUE);
               DrawPaddle(paddle2Position, ORANGE);
               DrawText(LeftScoreText, SCREEN_WIDTH * 0.5f - MeasureText(LeftScoreText, 20) * 3.0f, 50, 20, BLUE);
               DrawText(RightScoreText, SCREEN_WIDTH * 0.5f - MeasureText(RightScoreText, 20) * -2.0f, 50, 20, ORANGE);
               DrawText(WinScoreText, SCREEN_WIDTH * 0.5f - MeasureText(WinScoreText, 20) * 0.5f, 50, 20, WHITE);
              // DrawText("Theme 1/3, Default", 100.0, 750, 20, WHITE); didnt really like how this looked

            break;
           case 1: //Inverted Colors Mode
               ClearBackground(WHITE);
               DrawBall(ballPosition, BLACK);
               DrawPaddle(paddle1Position, ORANGE);
               DrawPaddle(paddle2Position, BLUE);
               DrawText(LeftScoreText, SCREEN_WIDTH * 0.5f - MeasureText(LeftScoreText, 20) * 3.0f, 50, 20, ORANGE);
               DrawText(RightScoreText, SCREEN_WIDTH * 0.5f - MeasureText(RightScoreText, 20) * -2.0f, 50, 20, BLUE);
               DrawText(WinScoreText, SCREEN_WIDTH * 0.5f - MeasureText(WinScoreText, 20) * 0.5f, 50, 20, BLACK);
               // DrawText("Theme 2/3, Inverted", 100.0, 750, 20, Black); didnt really like how this looked
           break;
           case 2: //OG MODE
               ClearBackground(BLACK);
               DrawBall(ballPosition, WHITE);
               DrawPaddle(paddle1Position, WHITE);
               DrawPaddle(paddle2Position, WHITE);
               DrawText(LeftScoreText, SCREEN_WIDTH * 0.5f - MeasureText(LeftScoreText, 20) * 3.0f, 50, 20, WHITE);
               DrawText(RightScoreText, SCREEN_WIDTH * 0.5f - MeasureText(RightScoreText, 20) * -2.0f, 50, 20, WHITE);
               DrawText(WinScoreText, SCREEN_WIDTH * 0.5f - MeasureText(WinScoreText, 20) * 0.5f, 50, 20, WHITE);
               //DrawText("Theme 3/3, ", 100.0, 750, 20, WHITE); didnt really like how this looked
               break;
        }
     
            EndDrawing();
       
    }

    CloseAudioDevice();
    CloseWindow();
    return 0;
}

//Unique Features!
//These are stated where they are where they are but i thought i would explain here for 100% clarity 
// Also note im not sure if all of these things count, but i figured id mention everything i did NOT on the rubric anyway. 
//Feature 1:
// The first change is i did was change the name in the window from "Pong" to "Jaxen Presents Pong.". 
// This does not improve the gameplay per say, it just adds a real nice layer of personalisation.

//Feature 2: Separate Paddle 2 Controls
//This you can play with another person or atleast have more fun by yourself. It was quite easy to implement given you already set up the first paddle.
// The reason it has two possible controls is because i didn't know what people would prefer.
// I prefer arrow keys but i actually talked to some people in our program who use IJKL controls instead of or in Combination with Wasd. 
// Im pretty sure this does not count and is required since we need to state who won each match anyway but it is not explicitly stated so i included it here anyway.

//Feature 3: Themed Colors.
//This is the one i was most excited to implement.
//All of last semester i  wanted to a Half Life like weapon wheel but we never had the chance. However, i experimented with how to do one. 
//The reason i like stuff like Half Life's Weapon select system is its incredible simple and so much more organized than just giving all 3 colors their own buttons
//It also seemed like a fun but easily doable challenge, which it proved to be with slightly unfamiliar syntax. 
//This is the first time ive implemented one in a project and its with something extremely cool.
//I had 3 themes i wanted to try! 1. Is the default contrasting paddles. 2. Is a light mode with a white background and blackball. And 3 is all white theme reminiscent of the original 70s Pong... or what was intially here before i modified it.
//All of them looked pretty decent in my opinion.
//You might have noticed i also made the paddles Orange and Blue by default. This is because they are contrasting colors and i thought they would be fitting for a 1v1 game like this.
//Since they are contrasting, it also worked perfectly for the "Light Mode" to just invert the twos colors. 
//I kept the ball unchanged because in the first two modes its the only object with its color, meaning its more likely to catch the players attention/focus
//This improves the game because someone might find any of those options more visually pleasing than the default.
//For those who don't like the colors, you can still pick the original and play that no problem. 

//Feature 4: Difficulty Settings
// I think the 5 we have to stop it at is too short, or atleast can be at times.
//Since i had to implement a similar system for the colors, why not add custom difficulty settings to increase it?
//i was going to add difficulty settings up to 30 for each paddle, but this seemed excessive so its capped at 20, or about 4x difficulty.

//Feature 5: Reset Buttons
//This probably doesn't count either but the ball kept getting stuck when i respawned it, so to stop that from happening i simply made it so space could send it back to the center
//I also made it KeyDown rather than KeyPressed so you could hold the Ball stopped and not get blindsighted when it spawns back.
// I am also aware that i could've added a sound to the resetball function and save time typing it in the points if statement. 
// This is the reason i didn't. I didn't want to play a sound while resetting the ball via other means.
//I decided to also make a button that resets both players points, as someone could maybe waste time changing color or difficulty or something
//And because i wanted to try to make a cheat code all last semester but never got a chance to i figured why not make a restore to defaults button that needed two keys pressed.
//Its very basic i know, but i effectively made a cheat code!
