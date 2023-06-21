//Rodrigo Nogueira
#include "DirectX.h"

const int Width = 1000;
const int Height = 667;
const float framerate = 30;
bool gameover;
int game_state = 0;
int score = 0;
const  int  ENEMY_NUMBERS = 4;
bool enemy_valid[ENEMY_NUMBERS];
float travis_speed = 5;
float enemy_speed = travis_speed;
int enemy_cooling[ENEMY_NUMBERS];
Model2D explosion[ENEMY_NUMBERS];
bool explosion_valid[ENEMY_NUMBERS];
int last_explosion = -1;
int die = -1;


int world_width = Width, world_height = Height;

std::unique_ptr<SoundEffect> explode;

Keyboard::State kb;
Mouse::ButtonStateTracker mousetracker;
Model2D space, travis, enemy[ENEMY_NUMBERS];

void UpdateExplosions()
{
    for (int i = 0; i < ENEMY_NUMBERS; i++)
    {
        if (explosion_valid[i])
            explosion[i].frame++;

        if (explosion[i].frame >= explosion[i].frame_total)
            explosion_valid[i] = false;
    }
    if (die >= 0 && explosion[die].frame >= explosion[die].frame_total)
    {
        MessageBox(NULL, L"Defeated", L"Game Over", MB_OK);
        gameover = true;
    }

    if (score == ENEMY_NUMBERS)
    {
        MessageBox(NULL, L"Victory", L"Game Over", MB_OK);
        gameover = true;
    }

}

void Updatetravis()
{
    kb = keyboard->GetState();
    travis.move_x = 0;  travis.move_y = 0;

    if (kb.Up)
    {
        travis.move_y = -travis_speed;
        travis.frame = 0;
    }
    if (kb.Down)
    {
        travis.move_y = travis_speed;
        travis.frame = 2;
    }
    if (kb.Left)
    {
        travis.move_x = -travis_speed;
        travis.frame = 3;
    }
    if (kb.Right)
    {
        travis.move_x = travis_speed;
        travis.frame = 1;
    }

    travis.x += travis.move_x;
    travis.y += travis.move_y;
    if (travis.x < 0)
        travis.x = 0;
    if (travis.x > Width - travis.frame_width)
        travis.x = Width - travis.frame_width;
    if (travis.y < 0)
        travis.y = 0;
    if (travis.y > Height - travis.frame_height)
        travis.y = Height - travis.frame_height;

    //gameover
    for (int i = 0; i < ENEMY_NUMBERS; i++)
    {
        if (enemy_valid[i] && CheckModel2DCollided(travis, enemy[i]) && die < 0)
        {
            last_explosion++;
            if (last_explosion >= ENEMY_NUMBERS)
                last_explosion = 0;
            explosion[last_explosion].x = travis.x;
            explosion[last_explosion].y = travis.y;
            explosion[last_explosion].frame = 0;
            explosion_valid[last_explosion] = true;
            die = last_explosion;
        }
    }
}


void UpdateEnemies()
{
    for (int i = 0; i < ENEMY_NUMBERS; i++)
    {
        if (enemy_valid[i])
        {
            enemy[i].x -= rand() % (0,5);
            enemy[i].y += rand() % (5,10);
            if (enemy[i].x < 0)
            {
                enemy[i].x = rand() % (world_width);
                enemy[i].y = 0;
            }
            if (enemy[i].x > Width - enemy[i].frame_width)
            {
                enemy[i].x = rand() % (world_width);
                enemy[i].y = 0;
            }
            if (enemy[i].y < 0)
            {
                enemy[i].x = rand() % (world_width);
                enemy[i].y = 0;
            }
            if (enemy[i].y > Height - enemy[i].frame_height)
            {
                enemy[i].x = rand() % (world_width);
                enemy[i].y = 0;
            }

        }
    }
}




bool Game_Init(HWND hwnd)
{
    gameover = false;

    if (InitD3D(hwnd) == false)
        return false;
    if (InitInput(hwnd) == false)
        return false;
    
    space = CreateModel2D(L"space.jpg");
    if (space.texture == NULL)
        return false;
    travis = CreateModel2D(L"travis.png", 4, 1);
    if (travis.texture == NULL)
    {
        MessageBox(NULL, L"Loading travis.png error",
            L"Error", MB_OK | MB_ICONERROR);
    }
    travis.x = 0;
    travis.y = 300;

    //Load Asteroids
    for (int i = 0; i < ENEMY_NUMBERS; i++)
    {
        enemy[i] = CreateModel2D(L"asteroid.png");
        enemy_valid[i] = true;

        enemy[i].x = rand() % (world_width);
        enemy[i].y = 0;

        enemy[i].frame = 0;
        enemy[i].move_x = 0;
        enemy[i].move_y = -enemy_speed;
        }

    //LOAD EXPLOSIONS
    for (int i = 0; i < ENEMY_NUMBERS; i++)
    {
        explosion[i] = CreateModel2D(L"explosion.png", 8, 8);
        explosion_valid[i] = false;
       
    }

    


    return true;
}

void Game_Run()
{
    long static start = 0;
    float frame_interval = 1000.0 / framerate;

    if (keyboard->GetState().Escape)
        gameover = true;

    if (GetTickCount64() - start >= frame_interval)
    {
        //reset timing
        start = GetTickCount64();
        ClearScreen();

        spriteBatch->Begin();

        DrawModel2D(space);
        Updatetravis();
        DrawModel2D(travis);
        for (int i = 0; i < ENEMY_NUMBERS; i++)
            if (enemy_valid[i])
                DrawModel2D(enemy[i]);
        UpdateEnemies();
        UpdateExplosions();
        for (int i = 0; i < ENEMY_NUMBERS; i++)
            if (explosion_valid[i])
                DrawModel2D(explosion[i]);

        spriteBatch->End();
        swapchain->Present(0, 0);
    }
}

void Game_End()
{
    explode.release();
    travis.texture->Release();
    space.texture->Release();
    CleanD3D();
}


