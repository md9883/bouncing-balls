#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfx.h>
#include <string>
#include <cstdlib>
#include <vector>
#include <chrono>
#include <thread>
#include <queue>
#include<SDL2/SDL_mixer.h>
using namespace std;

Mix_Chunk* lose;
Mix_Chunk* win;
Mix_Chunk* collide;

const int MAXN=50;

struct ball {
    int x11, y11, color_r1, color_g1, color_b1;
    int x12, y12, color_r2, color_g2, color_b2;
    bool visited = false;
    bool ghofl = false;
};


vector <ball> balls;

bool isAdjacent(ball ball1, ball ball2) {
    return sqrt((ball1.x11 - ball2.x11)*(ball1.x11 - ball2.x11) + (ball1.y11 - ball2.y11)*(ball1.y11 - ball2.y11))<= 50;
}
bool loser=false, winner=false, game=false;
bool fire=false, bomb=false;

// Define the function to check if two balls have the same color
bool isSameColor(ball ball1, ball ball2) {
    return (ball1.color_r1 == ball2.color_r1 && ball1.color_g1 == ball2.color_g1 && ball1.color_b1 == ball2.color_b1);
}

// Define the function to perform BFS on the balls
void bfs(vector<ball> balls, vector<vector<ball>>& groups) {
    queue<ball> q;
    vector<bool> visited(balls.size(), false);

    for (int i = 0; i < balls.size(); i++) {
        if (!visited[i]) {
            q.push(balls[i]);
            visited[i] = true;

            vector<ball> group;
            group.push_back(balls[i]);

            while (!q.empty()) {
                ball current = q.front();
                q.pop();

                for (int j = 0; j < balls.size(); j++) {
                    if (!visited[j] && isAdjacent(current, balls[j]) && isSameColor(current, balls[j])) {
                        q.push(balls[j]);
                        visited[j] = true;
                        group.push_back(balls[j]);
                    }
                }
                if (group.size() >= 3) {
                    group.erase(group.begin(),group.begin()+2);
//                    balls.erase(next(balls.begin(),i));

                }
            }

            groups.push_back(group);
        }
    }
}

struct ball crash_ball;
bool is_crash_ball_moved = false;
bool is_crash_ball_crashed = false;
int x_mouse;
int y_mouse;
int toop=0, score=0;
int pause_counter=3;

int dx = 10;
int dy = 10;
int rad_ball = 25;
double ddy=0;
int y=0;
int volume=128;
int bomb_counter=3;
int fire_counter=3;
vector<std::vector<ball>> groups;
void intial_ball(SDL_Renderer *Renderer,int L,int W,int r,int , bool pic);
void draw_ball(SDL_Renderer *Renderer,int L,int W,int r,int n_line, bool normalmode, bool pic, SDL_Texture *cannonTexture);
void intial_crash_ball(bool pic ,bool bonusmode);
void crashed_ball(SDL_Renderer *Renderer, bool pic, bool normalmode, bool bonusmode);
void drawGradientCircle(SDL_Renderer *Renderer, int x, int y, int radius, bool pic);
void textRGBA2(SDL_Renderer*m_renderer, int x, int y, const char* text,int f_size,int r, int g, int b, int a)
{
    TTF_Init();
    TTF_Font *font = TTF_OpenFont("Snowtimes.ttf", f_size);
    int textWidth, textHeight;
    TTF_SizeText(font, text, &textWidth, &textHeight);

    SDL_Rect rectText{x, y, 0, 0};
    Uint8 rr = r;    Uint8 gg = g;    Uint8 bb = b;    Uint8 aa = a;
    SDL_Color color{rr,gg,bb,aa};

    SDL_Surface *textSur = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *textTex = SDL_CreateTextureFromSurface(m_renderer, textSur);
    SDL_FreeSurface(textSur);
    SDL_QueryTexture(textTex, nullptr, nullptr, &rectText.w, &rectText.h);
    SDL_RenderCopy(m_renderer, textTex, nullptr, &rectText);
    SDL_DestroyTexture(textTex);
    TTF_CloseFont(font);
}
#define IoFast ios_base::sync_with_stdio(0);cin.tie(0);
int main(int argc, char* argv[]) {
    IoFast;
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Log("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        SDL_Quit();
        return 1;
    }

    // Create an application window with the following settings:
    SDL_Window* window = SDL_CreateWindow(
            "bouncing balls",         // Window title
            SDL_WINDOWPOS_UNDEFINED,       // Initial x position
            SDL_WINDOWPOS_UNDEFINED,       // Initial y position
            1500,                           // Width, in pixels
            750,                           // Height, in pixels
            SDL_WINDOW_SHOWN               // Flags
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Surface* settingSurface = IMG_Load("set.jpg"); // Load the image
    SDL_Texture* settingTexture = SDL_CreateTextureFromSurface(renderer, settingSurface);
    SDL_FreeSurface(settingSurface);
    SDL_Surface* scoreboardiconSurface = IMG_Load("scoreboardicon.jpg"); // Load the image
    SDL_Texture* scoreboardiconTexture = SDL_CreateTextureFromSurface(renderer, scoreboardiconSurface);
    SDL_FreeSurface(scoreboardiconSurface);
    SDL_Surface* scoreboardSurface = IMG_Load("scoreboard.jpg"); // Load the image
    SDL_Texture* scoreboardTexture = SDL_CreateTextureFromSurface(renderer, scoreboardSurface);
    SDL_FreeSurface(scoreboardSurface);
    SDL_Surface* loserSurface = IMG_Load("loser.jpg"); // Load the image
    SDL_Texture* loserTexture = SDL_CreateTextureFromSurface(renderer, loserSurface);
    SDL_FreeSurface(loserSurface);
    SDL_Surface* winnerSurface = IMG_Load("win.jpg"); // Load the image
    SDL_Texture* winnerTexture = SDL_CreateTextureFromSurface(renderer, winnerSurface);
    SDL_FreeSurface(winnerSurface);
    SDL_Surface* gameSurface = IMG_Load("back.png"); // Load the image
    SDL_Texture* gameTexture = SDL_CreateTextureFromSurface(renderer, gameSurface);
    SDL_FreeSurface(gameSurface);
    SDL_Surface* pic1Surface = IMG_Load("back1.jpg"); // Load the image
    SDL_Texture* pic1Texture = SDL_CreateTextureFromSurface(renderer, pic1Surface);
    SDL_FreeSurface(pic1Surface);
    SDL_Surface* pic2Surface = IMG_Load("back2.jpg"); // Load the image
    SDL_Texture* pic2Texture = SDL_CreateTextureFromSurface(renderer, pic2Surface);
    SDL_FreeSurface(pic2Surface);
    SDL_Surface* cannonSurface = IMG_Load("cannon.webp"); // Load the image
    SDL_Texture* cannonTexture = SDL_CreateTextureFromSurface(renderer, cannonSurface);
    SDL_FreeSurface(cannonSurface);

    //music
    Mix_Music *bgMusic = Mix_LoadMUS("main.mp3");
    auto start_time = std::chrono::system_clock::now();
    if (bgMusic != NULL) {
        Mix_PlayMusic(bgMusic, -1);
    }
    int numebr_of_lines = 4;
    string b, f, p;
    bool picPaused1 = true;
    bool picPaused2 = false;
    bool q=true, user=false, mode=false, infinitemode=false, normalmode=false;
    bool scoreboard=false, setting=false, settinggame=false, bonusmode=false;

    intial_ball(renderer,1500,750,rad_ball,numebr_of_lines,picPaused1);
    intial_crash_ball(picPaused1, bonusmode);
    draw_ball(renderer,1500,750,rad_ball,numebr_of_lines, normalmode, picPaused1, cannonTexture);

    int RNX = 200, RNY = 200;;
    bfs(balls, groups);

    bool musicPaused1 = false; // Initially, music is not paused
    SDL_Rect buttonRect1 = { 700, 450, 100, 100 }; // Button position and size
    SDL_Rect buttonRectpic1 = { 430, 250, 170, 100 };
    SDL_Rect buttonRectpic2 = { 900, 250, 170, 100 };

    lose = Mix_LoadWAV("lose.wav");
    win = Mix_LoadWAV("win.wav");
    collide = Mix_LoadWAV("pop.wav");

    // Main loop flag
    bool running = true;
    SDL_Event e;

    int xMouse, yMouse;
    string inputText="";


    while (running) {
        // Handle events on the queue
        Mix_VolumeMusic(volume);
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
            if (e.type == SDL_KEYDOWN){
                if (e.key.keysym.sym == SDLK_1) {
                    y-=20;
                }
                if (e.key.keysym.sym == SDLK_2) {
                    y+=20;
                }
                if (e.key.keysym.sym == SDLK_m) {
                    q=true, user=false, mode=false, infinitemode=false, normalmode=false, scoreboard=false, setting= false, game=false, loser=false;
                }
                if (e.key.keysym.sym == SDLK_l) {
                    volume-=20;
                }
                if (e.key.keysym.sym == SDLK_h) {
                    volume+=20;
                }
                if (e.key.keysym.sym == SDLK_s) {
                    q=false, user=false, mode=false, infinitemode=false, normalmode=false, scoreboard=false, setting=false,loser=false, settinggame=true, game=false;
                }
            }
        }
        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (q) {
            Mix_ResumeMusic();
            Mix_VolumeChunk(lose,0);
            for (int i = 0; i < 15; i++) {
                int RR = rand() % 255, GR = rand() % 255, BR = rand() % 255;
                boxRGBA(renderer, 0 + 100 * i, 0 + 50 * i, 100 + 100 * i, 50 + 50 * i, RR, GR, BR, 255+y);
                SDL_Delay(4);

            }
            boxRGBA(renderer, 650, 500, 850, 600, 255, 0, 0, 255+y);
            textRGBA2(renderer, 700, 485, "start", 100, 255, 255, 2, 255+y);
            SDL_Rect settingRect = {1500 - 130, 20, 130, 130}; // Create a rectangle to display the image
            SDL_RenderCopy(renderer, settingTexture, NULL, &settingRect);
            SDL_Rect scoreboardiconRect = {1000, 0, 180, 180}; // Create a rectangle to display the image
            SDL_RenderCopy(renderer, scoreboardiconTexture, NULL, &scoreboardiconRect);


            while (SDL_PollEvent(&e) != 0) {
                SDL_GetGlobalMouseState(&xMouse, &yMouse);
                if (e.type == SDL_MOUSEBUTTONDOWN) {
                    if (xMouse > 650 && xMouse < 850 && yMouse > 500 && yMouse < 600) {
                        SDL_RenderClear(renderer);
                        q = false;
                        user = true;
                    }
                    if (xMouse > 1500 - 130 && xMouse < (1500) && yMouse > (0) && yMouse < 150) {
                        SDL_RenderClear(renderer);
                        q = false;
                        user = false;
                        setting = true;
                    }
                    if (xMouse > 1000 && xMouse < (1200) && yMouse > (0) && yMouse < 200) {
                        SDL_RenderClear(renderer);
                        scoreboard = true;
                        q = false;
                        user = false;
                    }
                }
            }
        }

        if (scoreboard){
            int x=0;
            SDL_Rect scoreboardRect = { 0 , 0  , 1500, 750 }; // Create a rectangle to display the image
            SDL_RenderCopy(renderer, scoreboardTexture, NULL, &scoreboardRect);
            std::ifstream myfile;
            myfile.open("user.txt");
            string myline;
            if ( myfile.is_open() ) {
                while ( myfile ) { // equivalent to myfile.good()
                    getline (myfile, myline);
                    textRGBA2(renderer,500,100+x,myline.c_str(),90,0,0,0,255+y);
                    x+=65;
                }
            }
            myfile.close();
        }
        if (setting){
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_GetGlobalMouseState(&xMouse, &yMouse);
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (xMouse >= buttonRect1.x && xMouse < buttonRect1.x + buttonRect1.w &&
                    yMouse >= buttonRect1.y && yMouse < buttonRect1.y + buttonRect1.h) {
                    // Toggle music playback state
                    if (musicPaused1) {
                        Mix_ResumeMusic();
                    } else {
                        Mix_PauseMusic();
                    }
                    musicPaused1 = !musicPaused1; // Toggle the flag
                }
                if (xMouse >= buttonRectpic2.x && xMouse < buttonRectpic2.x + buttonRectpic2.w &&
                    yMouse >= buttonRectpic2.y && yMouse < buttonRectpic2.y + buttonRectpic2.h) {
                    picPaused2 = !picPaused2; // Toggle the flag
                    picPaused1 = !picPaused1; // Toggle the flag
                }
                if (xMouse >= buttonRectpic1.x && xMouse < buttonRectpic1.x + buttonRectpic1.w &&
                    yMouse >= buttonRectpic1.y && yMouse < buttonRectpic1.y + buttonRectpic1.h) {
                    picPaused2 = !picPaused2; // Toggle the flag
                    picPaused1 = !picPaused1; // Toggle the flag
                }
            }
            //music botton
            if (musicPaused1) {
                // Button is paused
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255+y); // Red color
            } else {
                // Button is playing
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255+y); // Green color
            }
            SDL_RenderFillRect(renderer, &buttonRect1);
            textRGBA2(renderer,715,445,"music",75,0,0,0,255+y);
            if (picPaused2) {
                // Button is paused
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255+y); // Red color
            } else {
                // Button is playing
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255+y); // Green color
            }
            SDL_RenderFillRect(renderer, &buttonRectpic2);
            textRGBA2(renderer,915,265,"background 2",50,255,255,255,255+y);
            if (picPaused1) {
                // Button is paused
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color
            } else {
                // Button is playing
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green color
            }
            SDL_RenderFillRect(renderer, &buttonRectpic1);
            textRGBA2(renderer,445,265,"background 1",50,255,255,255,255+y);
        }
        if (settinggame){
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_Rect buttonRectgame = { 0 , 650  , 100, 100 }; // Create a rectangle to display the image
            SDL_RenderCopy(renderer, gameTexture, NULL, &buttonRectgame);
            SDL_GetGlobalMouseState(&xMouse, &yMouse);
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (xMouse >= buttonRect1.x && xMouse < buttonRect1.x + buttonRect1.w &&
                    yMouse >= buttonRect1.y && yMouse < buttonRect1.y + buttonRect1.h) {
                    // Toggle music playback state
                    if (musicPaused1) {
                        Mix_ResumeMusic();
                    }
                    else {
                        Mix_PauseMusic();
                    }
                    musicPaused1 = !musicPaused1; // Toggle the flag
                }
                if (xMouse >= buttonRectpic2.x && xMouse < buttonRectpic2.x + buttonRectpic2.w &&
                    yMouse >= buttonRectpic2.y && yMouse < buttonRectpic2.y + buttonRectpic2.h) {
                    picPaused2 = !picPaused2; // Toggle the flag
                    picPaused1 = !picPaused1; // Toggle the flag
                }
                if (xMouse >= buttonRectpic1.x && xMouse < buttonRectpic1.x + buttonRectpic1.w &&
                    yMouse >= buttonRectpic1.y && yMouse < buttonRectpic1.y + buttonRectpic1.h) {
                    picPaused2 = !picPaused2; // Toggle the flag
                    picPaused1 = !picPaused1; // Toggle the flag
                }
                if (xMouse >= buttonRectgame.x && xMouse < buttonRectgame.x + buttonRectgame.w &&
                    yMouse >= buttonRectgame.y && yMouse < buttonRectgame.y + buttonRectgame.h) {
                    settinggame=false;
                    game=true;
                }
            }
            //music botton
            if (musicPaused1) {
                // Button is paused
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255+y); // Red color
            } else {
                // Button is playing
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255+y); // Green color
            }
            SDL_RenderFillRect(renderer, &buttonRect1);
            textRGBA2(renderer,715,445,"music",75,0,0,0,255+y);
            if (picPaused2) {
                // Button is paused
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color
            } else {
                // Button is playing
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green color
            }
            SDL_RenderFillRect(renderer, &buttonRectpic2);
            textRGBA2(renderer,915,265,"background 2",50,255,255,255,255);
            if (picPaused1) {
                // Button is paused
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color
            } else {
                // Button is playing
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green color
            }
            SDL_RenderFillRect(renderer, &buttonRectpic1);
            textRGBA2(renderer,445,265,"background 1",50,255,255,255,255);
        }

         if (user){
             SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
             SDL_RenderClear(renderer);
             SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255+y);
             boxRGBA(renderer, 500, 300, 1000, 550, 100, 20, 30, 255+y);
             textRGBA2(renderer,520,300,"enter your username:",100,255,200,100,255+y);

             while (SDL_PollEvent(&e) != 0)
             {
                 if (e.type == SDL_TEXTINPUT) {
                     inputText += e.text.text;
                 }
                 if (e.type == SDL_KEYDOWN){
                     if (e.key.keysym.sym == SDLK_SPACE) {
                         SDL_RenderClear(renderer);
                         ofstream fout("user.txt", ios::app);
                         fout<<inputText<<"                      ";
                         fout.close();
                         q = false;
                         user = false;
                         mode=true;
                     }
                 }
             }
             textRGBA2(renderer,520,400,inputText.c_str(),100,2,255,255,255+y);
         }
         if (mode){
             SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
             SDL_RenderClear(renderer);
             boxRGBA(renderer, 300, 250, 500, 400, 60, 70, 30, 255+y);
             textRGBA2(renderer,340,270,"infinite",85,85,255,2,255+y);
             boxRGBA(renderer, 1000, 250, 1200, 400, 0, 0, 100, 255+y);
             textRGBA2(renderer,1040,270,"normal",85,120,120,255,255+y);
             boxRGBA(renderer, 650, 500, 850, 650, 200, 0, 200, 255+y);
             textRGBA2(renderer,700,520,"bonus",85,255,255,255,255+y);
             while (SDL_PollEvent(&e) != 0) {
                 SDL_GetGlobalMouseState(&xMouse, &yMouse);
                 if (e.type == SDL_MOUSEBUTTONDOWN) {
                     if (xMouse > 300 && xMouse < 500 && yMouse > 250 && yMouse < 400) {
                         SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                         SDL_RenderClear(renderer);
                         mode=false;
                         infinitemode = true;
                         game=true;
                     }
                     if (xMouse > 1000 && xMouse < 1200 && yMouse > 250 && yMouse < 400) {
                         SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                         SDL_RenderClear(renderer);
                         mode=false;
                         normalmode=true;
                         game=true;
                     }
                     if (xMouse > 650 && xMouse < 850 && yMouse > 500 && yMouse < 650) {
                         SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                         SDL_RenderClear(renderer);
                         mode=false;
                         bonusmode=true;
                         game=true;
                     }
                 }
             }
         }

         if (game){
             SDL_RenderClear( renderer );
             if (picPaused1){
                 SDL_Rect pic1Rect = {0, 0, 1500, 750}; // Create a rectangle to display the image
                 SDL_RenderCopy(renderer, pic1Texture, NULL, &pic1Rect);
             }
             else{
                 SDL_Rect pic2Rect = {0, 0, 1500, 750}; // Create a rectangle to display the image
                 SDL_RenderCopy(renderer, pic2Texture, NULL, &pic2Rect);
             }
             SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
             auto now = std::chrono::system_clock::now();
             auto elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count();
             textRGBA2(renderer, 20, 700, to_string(elapsed_seconds).c_str(), 40, 255, 255, 2, 255 + y);
             this_thread::sleep_for(std::chrono::seconds(1/10));
             if (bonusmode) {
                 b = "bomb:" + to_string(bomb_counter);
                 f = "fire:" + to_string(fire_counter);
                 textRGBA2(renderer, 1000, 650, b.c_str(), 60, 255, 255, 255, 255 + y);
                 textRGBA2(renderer, 1300, 650, f.c_str(), 60, 233, 108, 7, 255 + y);
             }
             if (normalmode){
                 p= "pause:" + to_string(pause_counter);
                 textRGBA2(renderer, 1200, 650, p.c_str(), 60, 150, 150, 150, 255 + y);
             }

             if (elapsed_seconds > 300) {
                 ofstream fout("user.txt", ios::app);
                 fout<<score<<endl;
                 fout.close();
                 game=false;
                 loser=true;
             }
             score= (1-(elapsed_seconds/300))*10 + toop*5;
             draw_ball(renderer,1500,750,rad_ball,numebr_of_lines,normalmode, picPaused1, cannonTexture);
             crashed_ball(renderer, picPaused1, normalmode, bonusmode);

         }
        if(loser){
            Mix_FreeChunk(collide);
            SDL_Rect losericonRect = {200, 0, 1100, 750}; // Create a rectangle to display the image
            SDL_RenderCopy(renderer, loserTexture, NULL, &losericonRect);
            textRGBA2(renderer, 20, 550, "score:", 100, 0, 255, 2, 255 + y);
            textRGBA2(renderer, 30, 650, to_string(score).c_str(), 70, 255, 0, 2, 255 + y);
            Mix_PauseMusic();
            Mix_VolumeChunk(lose,volume);
            Mix_PlayChannel(-1, lose, -1);
        }
        if (winner){
            Mix_FreeChunk(collide);
            ofstream fout("user.txt", ios::app);
            fout<<score<<endl;
            fout.close();
            SDL_Rect winnericonRect = {200, 0, 1100, 750}; // Create a rectangle to display the image
            SDL_RenderCopy(renderer, winnerTexture, NULL, &winnericonRect);
            textRGBA2(renderer, 20, 550, "score:", 100, 0, 255, 2, 255 + y);
            textRGBA2(renderer, 30, 650, to_string(score).c_str(), 70, 2, 0, 255, 255 + y);
            Mix_PauseMusic();
            Mix_VolumeChunk(win,volume);
            Mix_PlayChannel(-1, win, -1);
        }
        SDL_RenderPresent(renderer);
        SDL_RenderClear( renderer );
    }

    // Free resources and close SDL
    Mix_FreeMusic(bgMusic);
    Mix_CloseAudio();
    SDL_DestroyTexture(gameTexture);
    SDL_DestroyTexture(settingTexture);
    SDL_DestroyTexture(loserTexture);
    SDL_DestroyTexture(winnerTexture);
    SDL_DestroyTexture(scoreboardTexture);
    SDL_DestroyTexture(scoreboardiconTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void random_color(int &r, int &g, int &b, bool pic)
{
    int rc = rand() % 5;
    if (pic) {
        if (rc == 0) {
            //pink
            r = 255;
            g = 59;
            b = 139;
        } else if (rc == 1) {
            //cyan
            r = 30;
            g = 240;
            b = 206;
        } else if (rc == 2) {
            //blue
            r = 0;
            g = 4;
            b = 122;
        } else if (rc == 3) {
            //purple
            r = 172;
            g = 72;
            b = 252;
        } else if (rc == 4) {
            //white
            r = 255;
            g = 255;
            b = 255;

        }
    }
    else{
        if (rc == 0) {
            //blue
            r = 0;
            g = 0;
            b = 255;
        } else if (rc == 1) {
            //red
            r = 255;
            g = 0;
            b = 0;
        } else if (rc == 2) {
            //green
            r = 0;
            g = 255;
            b = 0;
        } else if (rc == 3) {
            //yellow
            r = 255;
            g = 255;
            b = 0;
        } else if (rc == 4) {
            //white
            r = 255;
            g = 255;
            b = 255;

        }
    }
}

void intial_ball(SDL_Renderer *Renderer,int L,int W,int r,int n_line, bool pic) {
    int x_center = r;
    int y_center = r;
    int n_colum = L / (2 * r);
    int color = 0;
    struct ball new_ball;
    int rr,g,b;
    for (int i = 0; i < n_line; i++) {
        for (int j = 0; j < n_colum; j++) {

            new_ball.x11 = x_center;
            new_ball.y11 = y_center;
            random_color(rr, g, b, pic);
            new_ball.color_r1 = rr;
            new_ball.color_g1 = g;
            new_ball.color_b1 = b;
            balls.push_back(new_ball);
            x_center += 2 * r;
        }
        x_center = r;
        y_center += 2 * r;
    }
}

void intial_crash_ball(bool pic, bool bonusmode)
{
    crash_ball.x11 = 750;
    crash_ball.y11 = 610;
    crash_ball.x12 = 750;
    crash_ball.y12 = 680;
    int r, g, b, rk, gk, bk ;

    random_color(r, g, b, pic);
    random_color(rk, gk, bk, pic);
    crash_ball.color_r1 = r;
    crash_ball.color_g1 = g;
    crash_ball.color_b1 = b;

    if(is_crash_ball_crashed){
        crash_ball.color_r1 = crash_ball.color_r2;
        crash_ball.color_g1 = crash_ball.color_g2;
        crash_ball.color_b1 = crash_ball.color_b2;
        random_color(rk, gk, bk, pic);
        crash_ball.color_r2 = rk;
        crash_ball.color_g2 = gk;
        crash_ball.color_b2 = bk;
    }
    else {
        random_color(rk, gk, bk, pic);
        crash_ball.color_r2 = rk;
        crash_ball.color_g2 = gk;
        crash_ball.color_b2 = bk;
    }

    is_crash_ball_crashed = false;
    is_crash_ball_moved = false;
}
void draw_ball(SDL_Renderer *Renderer,int L,int W,int r,int n_line, bool normalmode, bool pic, SDL_Texture *cannonTexture)
{
    int rr, g, b, rk, gk, bk ;
    int n_colum = L/(2*r);
    for(int i=0;i<n_line;i++)
    {
        for(int j=0;j<n_colum;j++)
        {
            if(balls[i*n_colum+j].y11+ddy+r>=570){
                ofstream fout("user.txt", ios::app);
                fout<<score<<endl;
                fout.close();
                game=false;
                loser=true;
                return;
            }
            filledCircleRGBA(Renderer,balls[i*n_colum+j].x11,balls[i*n_colum+j].y11+ddy,r,balls[i*n_colum+j].color_r1,balls[i*n_colum+j].color_g1,balls[i*n_colum+j].color_b1,255+y);
        }
    }
    if (normalmode) {
        ddy += 0.05;
    }

    SDL_Rect cannonRect = {700, 600, 100, 150}; // Create a rectangle to display the image
    SDL_RenderCopy(Renderer, cannonTexture, NULL, &cannonRect);
    aalineRGBA(Renderer,0,570,1500,570,255,255,255,255+y);
}

void crashed_ball(SDL_Renderer *Renderer, bool pic, bool normalmode, bool bonusmode)
{
    Mix_VolumeChunk(collide, 0);
    Mix_PlayChannel(-1, collide, -1);
    SDL_Event *event = new SDL_Event();
    if( SDL_PollEvent(event) ) {
        SDL_GetGlobalMouseState(&x_mouse, &y_mouse);
        if (event->type == SDL_KEYDOWN) {
            switch (event->key.keysym.sym) {
                case SDLK_SPACE: {
                    if (!is_crash_ball_crashed) {
                        is_crash_ball_moved = true;
                        dx = (x_mouse - crash_ball.x11) / 20;
                        dy = (y_mouse - crash_ball.y11) / 20;
                        break;
                    }
                }
                case SDLK_c: {
                    if (!is_crash_ball_crashed) {
                        int rr, gg, bb;
                        rr = crash_ball.color_r1;
                        gg = crash_ball.color_g1;
                        bb = crash_ball.color_b1;
                        crash_ball.color_r1 = crash_ball.color_r2;
                        crash_ball.color_g1 = crash_ball.color_g2;
                        crash_ball.color_b1 = crash_ball.color_b2;
                        crash_ball.color_r2 = rr;
                        crash_ball.color_g2 = gg;
                        crash_ball.color_b2 = bb;
                    }
                }
                case SDLK_p: {
                    if (normalmode) {
                        if (pause_counter > 0) {
                            ddy -= 0.05;
                            pause_counter--;
                            SDL_Delay(3000);
                        }
                    }
                    break;
                }
                case SDLK_f: {
                    if (bonusmode) {
                        if (fire_counter>0) {
                            crash_ball.color_r1 = 233;
                            crash_ball.color_g1 = 108;
                            crash_ball.color_b1 = 7;
                            fire = true;
                            fire_counter--;
                        }
                    }
                    break;
                }
                case SDLK_b:{
                    if (bonusmode) {
                        if (bomb_counter>0) {
                            crash_ball.color_r1 = 0;
                            crash_ball.color_g1 = 0;
                            crash_ball.color_b1 = 0;
                            bomb = true;
                            bomb_counter--;
                        }
                    }
                }
            }
        }
    }
    filledCircleRGBA(Renderer,crash_ball.x11,crash_ball.y11,25,crash_ball.color_r1,crash_ball.color_g1,crash_ball.color_b1,255+y);
    filledCircleRGBA(Renderer,crash_ball.x12,crash_ball.y12,25,crash_ball.color_r2,crash_ball.color_g2,crash_ball.color_b2,255+y);
    if (!is_crash_ball_crashed)
    {
        if(!is_crash_ball_moved){
            aalineRGBA(Renderer,crash_ball.x11,crash_ball.y11,x_mouse,y_mouse,255,0,0,255+y);
        }
        else{
            if(crash_ball.x11>1500||crash_ball.x11<0)
                dx*=-1;
            if(crash_ball.y11>750||crash_ball.y11<0)
                dy*=-1;

            crash_ball.x11+=dx;
            crash_ball.y11+=dy;

            for(int i=0;i<balls.size();i++)
            {
                if(sqrt((crash_ball.x11-balls[i].x11)*(crash_ball.x11-balls[i].x11) + (crash_ball.y11-balls[i].y11)*(crash_ball.y11-balls[i].y11))<50)
                {
                    is_crash_ball_crashed = true;
                    if (is_crash_ball_crashed){
                        //Mix_PauseMusic();
                        Mix_VolumeChunk(collide, volume);
                        Mix_PlayChannel(-1, collide, -1);
                    }
                    crash_ball.x11 -= crash_ball.x11%rad_ball;
                    crash_ball.y11 -= crash_ball.y11%rad_ball;
                    crash_ball.y11 += rad_ball;

                    if(crash_ball.color_r1==balls[i].color_r1&&crash_ball.color_g1==balls[i].color_g1&&crash_ball.color_b1==balls[i].color_b1)
                    {
                        toop++;
                        bfs (balls,groups);
                        balls.erase(next(balls.begin(),i));
                        intial_crash_ball(pic, bonusmode);
                    }
                    else if(fire) {
                        balls.erase(next(balls.begin(), i));
                        balls.erase(next(balls.begin(), i));
                        intial_crash_ball(pic, bonusmode);
                        toop+=2;
                        fire = false;
                    }
                    else if (bomb){
                        balls.erase(next(balls.begin(), i));
                        balls.erase(next(balls.begin(), i));
                        balls.erase(next(balls.begin(), i));
                        balls.erase(next(balls.begin(), i));
                        intial_crash_ball(pic, bonusmode);
                        toop+=4;
                        bomb = false;
                    }
                    else{
                        balls.push_back(crash_ball);
                        intial_crash_ball(pic, bonusmode);
                    }
                    break;
                }
            }
        }
    }
    if (balls.size()==0){
        game=false;
        winner=true;
        ofstream fout("user.txt", ios::app);
        fout<<score<<endl;
        fout.close();
    }
}
void drawGradientCircle(SDL_Renderer *Renderer, int x, int y, int radius, bool pic) {
    for (int i = 0; i <= radius; i++) {
        float percent = static_cast<float>(i) / rad_ball;
        int rk,gk,bk,rr,gg,bb;
        SDL_Color color1;
        SDL_Color color2;
        switch (rand() % 2) {

            case 0: {
                color1 = {255, 0, 255, 255}; // purple
                color2 = {5, 0, 255, 255}; // Blue
                break;
            }

            case 1: {
                color1 = {255, 255, 255, 255}; // purple
                color2 = {0, 255, 255, 255}; // Blue
                break;
            }
        }
        Uint8 r = static_cast<Uint8>((1 - percent) *color1.r  + percent *color2.r);
        Uint8 g = static_cast<Uint8>((1 - percent) * color1.g + percent * color2.g);
        Uint8 b = static_cast<Uint8>((1 - percent) *color1.b  + percent * color2.b);

        SDL_SetRenderDrawColor(Renderer, r, g, b, 255);

        for (int j = 0; j < 360; j++) {
            float angle = j * M_PI / 180;
            int cx = x + i * cos(angle);
            int cy = y + i * sin(angle);
            SDL_RenderDrawPoint(Renderer, cx, cy);
        }
    }
}