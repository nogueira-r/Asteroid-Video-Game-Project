//Rodrigo Nogueira
#include "DirectX.h"

const int Width = 1000;
const int Height = 667;
const float framerate = 30;
bool gameover;
int game_state = 0;
int score = 0;

float travis_speed = 10;

const  int  ASTEROID_NUMBERS = 8;
bool asteroid_valid[ASTEROID_NUMBERS];
float asteroid_speed = 5;
int asteroid_cooling[ASTEROID_NUMBERS];
bool explosion_valid[ASTEROID_NUMBERS];
int last_explosion = -1;
int die = -1;
bool powerup_valid = false;
bool scorecap = false;

bool powerbar_valid;


const int CACTI_NUMBER = 1;
const int cacti_width = 5;
const int cacti_height = 5;
const int cacti_rows = Height / cacti_height;
const int cacti_columns = Width / cacti_width;
bool cacti_valid[CACTI_NUMBER];

const int ASTRO_NUMBER = 1;
const int astro_width = 5;
const int astro_height = 5;
const int astro_rows = Height / astro_height;
const int astro_columns = Width / astro_width;
bool astro_valid[ASTRO_NUMBER];

const int CACTUS_NUMBER = 1;
const int cactus_width = 5;
const int cactus_height = 5;
const int cactus_rows = Height / cactus_height;
const int cactus_columns = Width / cactus_width;
bool cactus_valid[CACTUS_NUMBER];

const int REESES_NUMBER = 1;
const int reeses_width = 5;
const int reeses_height = 5;
const int reeses_rows = Height / reeses_height;
const int reeses_columns = Width / reeses_width;
bool reeses_valid[REESES_NUMBER];

const int DAVID_NUMBER = 1;
const int david_width = 5;
const int david_height = 5;
const int david_rows = Height / david_height;
const int david_columns = Width / david_width;
bool david_valid[DAVID_NUMBER];

int world_width = Width, world_height = Height;

std::unique_ptr<SpriteFont> spriteFont;
std::unique_ptr<SoundEffect> explode, itslit, straightup, laflame, thatwasfun, letsgo;

Keyboard::State kb;
Mouse::ButtonStateTracker mousetracker;
Model2D space, travis, asteroid[ASTEROID_NUMBERS], cacti[CACTI_NUMBER], astro[ASTRO_NUMBER], cactus[CACTUS_NUMBER], reeses[REESES_NUMBER], david[DAVID_NUMBER], explosion[ASTEROID_NUMBERS], powerbar;

void UpdateExplosions()
{
    for (int i = 0; i < ASTEROID_NUMBERS; i++)
    {
        if (explosion_valid[i])
            explosion[i].frame++;

        if (explosion[i].frame >= explosion[i].frame_total)
            explosion_valid[i] = false;
    }

    if (die >= 0 && explosion[die].frame >= explosion[die].frame_total)
        gameover = true;
}

void UpdatePowerbar()
{
    if (score < 6)
        powerbar.frame = 0;
    else if (score < 12)
        powerbar.frame = 1;
    else if (score < 18)
        powerbar.frame = 2;
    else if (score < 24)
        powerbar.frame = 3;
    else if (score < 29)
        powerbar.frame = 4;
    else if (score >= 30)
        powerbar.frame = 5;
}

void UpdateTravis()
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
    for (int i = 0; i < ASTEROID_NUMBERS; i++)
    {
        if (asteroid_valid[i] && CheckModel2DCollided(travis, asteroid[i]) && die < 0 && powerup_valid == false)
        {
            last_explosion++;
            if (last_explosion >= ASTEROID_NUMBERS)
                last_explosion = 0;
            explosion[last_explosion].x = travis.x;
            explosion[last_explosion].y = travis.y;
            explosion[last_explosion].frame = 0;
            explosion_valid[last_explosion] = true;
            if (powerup_valid == false)
                PlaySound(explode.get());
            ZeroMemory(&travis, sizeof(Model2D));
            die = last_explosion;


        }
    }

    //powerup
    if (score >= 30 && scorecap == false)
    {
        scorecap = true;
        powerup_valid = true;
    }

    if (powerup_valid == true)
    {
        for (int i = 0; i < ASTEROID_NUMBERS; i++)
        {
            if (CheckModel2DCollided(travis, asteroid[i]))
            {
                last_explosion++;
                if (last_explosion >= ASTEROID_NUMBERS)
                    last_explosion = 0;
                explosion[last_explosion].x = travis.x;
                explosion[last_explosion].y = travis.y;
                explosion[last_explosion].frame = 0;
                explosion_valid[last_explosion] = true;
                PlaySound(letsgo.get());
                ZeroMemory(&asteroid[i], sizeof(Model2D));
                ZeroMemory(&powerbar, sizeof(Model2D));
                powerup_valid = false;
            }

        }
    }

}


void UpdateAsteroids()
{
    for (int i = 0; i < ASTEROID_NUMBERS; i++)
    {
        if (asteroid_valid[i])
        {
            asteroid[i].x -= rand() % (-5,5);
            asteroid[i].y += asteroid_speed;
            if (asteroid[i].x < -150)
            {
                asteroid[i].x = rand() % (world_width);
                asteroid[i].y = -150;
            }
            if (asteroid[i].y > Height - (asteroid[i].frame_height-150))
            {
                asteroid[i].x = rand() % (world_width);
                asteroid[i].y = -150;
                asteroid_speed += 0.05;
            }
        }
    }
}

void UpdateCacti()
{
    for (int j = 0; j < CACTI_NUMBER; j++)
    {
        if (cacti_valid[j] && CheckModel2DCollided(travis, cacti[j]))
        {
            PlaySound(itslit.get());
            wchar_t s[80];
            swprintf(s, 80, L"+1");
            spriteFont->DrawString(spriteBatch.get(), s, XMFLOAT2(travis.x - 10, travis.y - 10), Colors::White);
            score+=1;
            for (int i = 0; i < CACTI_NUMBER; i++)
            {   // randomly build cacti
                int row = rand() % cacti_rows;
                int column = rand() % cacti_columns;
                cacti[i] = CreateModel2D(L"cacti.png");
                cacti[i].x = column * cacti_width;
                cacti[i].y = row * cacti_height;
                cacti_valid[i] = true;

                if (CheckModel2DCollided(travis, cacti[i]))
                {
                    ZeroMemory(&cacti[i], sizeof(Model2D));
                    cacti_valid[i] = false;
                }
            }
        }
    }
}

void UpdateAstro()
{
    for (int j = 0; j < ASTRO_NUMBER; j++)
    {
        if (astro_valid[j] && CheckModel2DCollided(travis, astro[j]))
        {
            PlaySound(straightup.get());
            wchar_t s[80];
            swprintf(s, 80, L"+2");
            spriteFont->DrawString(spriteBatch.get(), s, XMFLOAT2(travis.x - 10, travis.y - 10), Colors::White);
            score+=2;
            for (int i = 0; i < ASTRO_NUMBER; i++)
            {   // randomly build astroheads
                int row = rand() % astro_rows;
                int column = rand() % astro_columns;
                astro[i] = CreateModel2D(L"astroworld.png");
                astro[i].x = column * astro_width;
                astro[i].y = row * astro_height;
                astro_valid[i] = true;

                if (CheckModel2DCollided(travis, astro[i]))
                {
                    ZeroMemory(&astro[i], sizeof(Model2D));
                    astro_valid[i] = false;
                }
            }
        }
    }
}


void UpdateCactus()
{
    for (int j = 0; j < CACTUS_NUMBER; j++)
    {
        if (cactus_valid[j] && CheckModel2DCollided(travis, cactus[j]))
        {
            PlaySound(laflame.get());
            wchar_t s[80];
            swprintf(s, 80, L"+3");
            spriteFont->DrawString(spriteBatch.get(), s, XMFLOAT2(travis.x-10, travis.y-10), Colors::White);
            score+=3;
            for (int i = 0; i < CACTUS_NUMBER; i++)
            {   // randomly build cactuses
                int row = rand() % cactus_rows;
                int column = rand() % cactus_columns;
                cactus[i] = CreateModel2D(L"cactus.png");
                cactus[i].x = column * cactus_width;
                cactus[i].y = row * cactus_height;
                cactus_valid[i] = true;

                if (CheckModel2DCollided(travis, cactus[i]))
                {
                    ZeroMemory(&cactus[i], sizeof(Model2D));
                    cactus_valid[i] = false;
                }
            }
        }
    }
}

void UpdateReeses()
{
    for (int j = 0; j < REESES_NUMBER; j++)
    {
        if (reeses_valid[j] && CheckModel2DCollided(travis, reeses[j]))
        {
            PlaySound(laflame.get());
            wchar_t s[80];
            swprintf(s, 80, L"+4");
            spriteFont->DrawString(spriteBatch.get(), s, XMFLOAT2(travis.x - 10, travis.y - 10), Colors::White);
            score += 4;
            for (int i = 0; i < REESES_NUMBER; i++)
            {   // randomly build reeseses
                int row = rand() % reeses_rows;
                int column = rand() % reeses_columns;
                reeses[i] = CreateModel2D(L"reeses.png");
                reeses[i].x = column * reeses_width;
                reeses[i].y = row * reeses_height;
                reeses_valid[i] = true;

                if (CheckModel2DCollided(travis, reeses[i]))
                {
                    ZeroMemory(&reeses[i], sizeof(Model2D));
                    reeses_valid[i] = false;
                }
            }
        }
    }
}

void UpdateDavid()
{
    for (int j = 0; j < DAVID_NUMBER; j++)
    {
        if (david_valid[j] && CheckModel2DCollided(travis, david[j]))
        {
            PlaySound(thatwasfun.get());
            wchar_t s[80];
            swprintf(s, 80, L"YOU WON!");
            spriteFont->DrawString(spriteBatch.get(), s, XMFLOAT2(travis.x-10, travis.y-10), Colors::White);
            gameover = true;
            if (CheckModel2DCollided(travis, david[j]))
            {
                ZeroMemory(&david[j], sizeof(Model2D));
                david_valid[j] = false;
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
    if (InitSound() == false)
        return false;

    try {
        spriteFont = std::make_unique<SpriteFont>(dev, L"myfont.spritefont");
    }
    catch (std::runtime_error e)
    {
        MessageBox(NULL, L"Loading spritefont error", L"Error", MB_OK | MB_ICONERROR);
            return false;
    }



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


    //LOAD POWERUP BAR
    powerbar = CreateModel2D(L"powerbar.png", 6, 1);
    if (powerbar.texture == NULL)
        return false;
    powerbar.x = 10;
    powerbar.y = 20;

    // LOAD CACTI
    for (int i = 0; i < CACTI_NUMBER; i++)
    {   
        int row = rand() % cacti_rows;
        int column = rand() % cacti_columns;
        cacti[i] = CreateModel2D(L"cacti.png");
        cacti[i].x = column * cacti_width;
        cacti[i].y = row * cacti_height;
        cacti_valid[i] = true;

        if (CheckModel2DCollided(travis, cacti[i]))
        {
            ZeroMemory(&cacti[i], sizeof(Model2D));
            cacti_valid[i] = false;
        }
    }
    
    // LOAD ASTRO
    for (int i = 0; i < ASTRO_NUMBER; i++)
    {   
        int row = rand() % astro_rows;
        int column = rand() % astro_columns;
        astro[i] = CreateModel2D(L"astroworld.png");
        astro[i].x = column * astro_width;
        astro[i].y = row * astro_height;
        astro_valid[i] = true;

        if (CheckModel2DCollided(travis, astro[i]))
        {
            ZeroMemory(&astro[i], sizeof(Model2D));
            astro_valid[i] = false;
        }
    }

    // LOAD CACTUS
    for (int i = 0; i < CACTUS_NUMBER; i++)
    {
        int row = rand() % cactus_rows;
        int column = rand() % cactus_columns;
        cactus[i] = CreateModel2D(L"cactus.png");
        cactus[i].x = column * cactus_width;
        cactus[i].y = row * cactus_height;
        cactus_valid[i] = true;

        if (CheckModel2DCollided(travis, cactus[i]))
        {
            ZeroMemory(&cactus[i], sizeof(Model2D));
            cactus_valid[i] = false;
        }
    }

    // LOAD REESES
    for (int i = 0; i < REESES_NUMBER; i++)
    {
        int row = rand() % reeses_rows;
        int column = rand() % reeses_columns;
        reeses[i] = CreateModel2D(L"reeses.png");
        reeses[i].x = column * reeses_width;
        reeses[i].y = row * reeses_height;
        reeses_valid[i] = true;

        if (CheckModel2DCollided(travis, reeses[i]))
        {
            ZeroMemory(&reeses[i], sizeof(Model2D));
            reeses_valid[i] = false;
        }
    }

    // LOAD DAVID
        for (int i = 0; i < DAVID_NUMBER; i++)
        {
            int row = rand() % david_rows;
            int column = rand() % david_columns;
            david[i] = CreateModel2D(L"david.png");
            david[i].x = column * david_width;
            david[i].y = row * david_height;
            david_valid[i] = true;

            if (CheckModel2DCollided(travis, david[i]))
            {
                ZeroMemory(&david[i], sizeof(Model2D));
                david_valid[i] = false;
            }
        }

    //Load Asteroids
    for (int i = 0; i < ASTEROID_NUMBERS; i++)
    {
        asteroid[i] = CreateModel2D(L"asteroid.png");
        asteroid_valid[i] = true;

        asteroid[i].x = rand() % (world_width);
        asteroid[i].y = rand() % (-1000);

        asteroid[i].frame = 0;
        asteroid[i].move_x = 0;
        asteroid[i].move_y = -asteroid_speed;
        }

    //LOAD EXPLOSIONS
    for (int i = 0; i < ASTEROID_NUMBERS; i++)
    {
        explosion[i] = CreateModel2D(L"explosion.png", 8, 8);
        explosion_valid[i] = false;
       
    }

    letsgo = LoadSound(L"letsgo.wav");
    if (letsgo == NULL)
        return false;

    explode = LoadSound(L"alright.wav");
    if (explode == NULL)
        return false;

    itslit = LoadSound(L"itslit.wav");
    if (itslit == NULL)
        return false;

    straightup = LoadSound(L"straightup.wav");
    if (straightup == NULL)
        return false;

    laflame = LoadSound(L"laflame.wav");
    if (laflame == NULL)
        return false;

    thatwasfun = LoadSound(L"thatwasfun.wav");
    if (thatwasfun == NULL)
        return false;

    return true;
}

void Game_Run()
{
    long static start = 0;
    float frame_interval = 1000.0 / framerate;

    if (keyboard->GetState().Escape)
    {
        gameover = true;
    }

    if (GetTickCount64() - start >= frame_interval)
    {
        //reset timing
        start = GetTickCount64();
        ClearScreen();

        spriteBatch->Begin();

        DrawModel2D(space);

        DrawModel2D(powerbar);

        UpdateCactus();
        for (int i = 0; i < CACTUS_NUMBER; i++)
        {
            DrawModel2D(cactus[i]);
        }

        UpdateReeses();
        for (int i = 0; i < REESES_NUMBER; i++)
        {
            DrawModel2D(reeses[i]);
        }
        
        if (score > 39)
        {
            wchar_t s[900];
            swprintf(s, 600, L"DAVID HAS SPAWNED IN! MEET WITH HIM TO OPEN THE GATES TO UTOPIA");
            spriteFont->DrawString(spriteBatch.get(), s, XMFLOAT2(200, 100), Colors::White);
            UpdateDavid();
            for (int i = 0; i < DAVID_NUMBER; i++)
            {
                DrawModel2D(david[i]);
            }
        }


        UpdateCacti();
        for (int i = 0; i < CACTI_NUMBER; i++)
        {
            DrawModel2D(cacti[i]);
        }

        UpdateAstro();
        for (int i = 0; i < ASTRO_NUMBER; i++)
        {
            DrawModel2D(astro[i]);
        }

        for (int i = 0; i < ASTEROID_NUMBERS; i++)
        {
            if (explosion_valid[i])
            {
                DrawModel2D(explosion[i]);
            }
        }
        UpdateTravis();
        DrawModel2D(travis);

        for (int i = 0; i < ASTEROID_NUMBERS; i++)
        {
            if (asteroid_valid[i])
            {
                DrawModel2D(asteroid[i]);
            }
        }

        wchar_t s[900];
        swprintf(s, 600, L"Score = %d", score);
        spriteFont->DrawString(spriteBatch.get(), s, Colors::White);

        UpdateAsteroids();
        UpdateExplosions();
        UpdatePowerbar();

        spriteBatch->End();
        swapchain->Present(0, 0);
    }
}

void Game_End()
{

    wchar_t message[80];
    if (die >= 0 && explosion[die].frame >= explosion[die].frame_total)
        swprintf(message, 80, L"R.I.P. SCREW\nYOUR SCORE WAS %d", score);
    else
        swprintf(message, 80, L"YOU WENT SICKO MODE\nYOUR SCORE WAS %d", score);
    MessageBox(NULL, message, L"BEEN TRILL", MB_OK);

    letsgo.release();
    laflame.release();
    straightup.release();
    itslit.release();
    explode.release();
    space.texture->Release();
    CleanD3D();
}


