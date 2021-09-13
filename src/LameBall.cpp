/* ========================================================================
   $File:    LameBall [Source] $
   $Project: LameBall $
   $Date:    08-09-2021 $
   $Author:  Phil Bagshaw $
   $Notice:  (c)Phragware 2021 $
   ======================================================================== */

#include <stdio.h>
#include <SDL/SDL.h>
#include "LameBall.h"
#include "Events.cpp"

//TODO: https://raw.githubusercontent.com/gabomdq/SDL_GameControllerDB/master/gamecontrollerdb.txt
// SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt"); (alt: from char*, from RWops)
// Community sourced SDL Controller config, maybe use Curl to download it (it gets updated)
//TODO: Fix memory leak when audio bugs out
//TODO: finish Controller input controls
//TODO: Cleanup main loop (there's some variables that do the same thing)

int
main(int argc, char **argv)
{
    Application = {};
    Application.Dimension.x = SDL_WINDOWPOS_UNDEFINED;
    Application.Dimension.y = SDL_WINDOWPOS_UNDEFINED;
    Application.Dimension.w = 1280;
    Application.Dimension.h = 720;
    
    if(!SDL_Init(SDL_INIT_EVERYTHING))
    {
        SDL_version Version;
        SDL_GetVersion(&Version);
        SDL_Log("SDL Version: %d.%d.%d", Version.major, Version.minor, Version.patch);

        if(Version.major < 2 || Version.minor < 0 || Version.patch < 16)
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Warning!", "This Application is using an older version of sdl2.dll.\nExpected at least 2.0.16", 0);
        }

        Application.Window = SDL_CreateWindow("LameBall",
                                              Application.Dimension.x,
                                              Application.Dimension.y,
                                              Application.Dimension.w,
                                              Application.Dimension.h,
                                              SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

        if(Application.Window)
        {
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
            SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
            Application.Renderer = SDL_CreateRenderer(Application.Window, -1,
                                                      SDL_RENDERER_ACCELERATED);

            if(Application.Renderer)
            {
                Application.Running = true;

                //TEST: controller
                SDL_GameController *Controller = 0;

                //TEST: draw to screen
                SDL_Texture *Texture = SDL_CreateTexture(Application.Renderer,
                                                         SDL_PIXELFORMAT_RGBA8888,
                                                         SDL_TEXTUREACCESS_TARGET,
                                                         Application.Dimension.w,
                                                         Application.Dimension.h);
                SDL_Rect PlayerRect = {};
                PlayerRect.w = 25;
                PlayerRect.h = 120;
                int PlayerRectSpeed = 10;
                bool BouncePlayerHit = 0;
                    
                SDL_Rect BounceRect = {};
                BounceRect.w = 20;
                BounceRect.h = 20;
                BounceRect.x = (Application.Dimension.w/2) - (BounceRect.w/2);
                BounceRect.y = (Application.Dimension.h/2) - (BounceRect.h/2);
                bool BounceRectDown = 0;
                bool BounceRectRight = 0;
                int BounceRectSpeed = 1;
                int HighScore = 1;
                bool BounceBounced = 0;
                bool BounceBouncedPlayer = 0;
                bool BounceBouncedGoal = 0;

                SDL_Rect BoundaryTop = {0,0, Application.Dimension.w, 6};
                SDL_Rect BoundaryBot = {0, Application.Dimension.h-6,
                  Application.Dimension.w, 6};
                SDL_Rect BoundaryRight = {Application.Dimension.w-6, 0,
                  6, Application.Dimension.h};
                SDL_Rect GoalLine = {0, 0, 10, Application.Dimension.h};

                //TEST: Score 7-seg display (Units, Tens, Hundreds)
                SDL_Rect SevenSegU[7];
                {
                    int x = (Application.Dimension.w/2)+200;
                    int y = Application.Dimension.h/2;
                    SevenSegU[0] = {(x-10)-60,(y-50)-60,20,100};
                    SevenSegU[1] = {(x-10)-60,(y-50)+60,20,100};
                    SevenSegU[2] = {x-50,(y-10)-120,100,20};
                    SevenSegU[3] = {x-50,y-10,100,20};
                    SevenSegU[4] = {x-50,(y-10)+120,100,20};
                    SevenSegU[5] = {(x-10)+60,(y-50)-60,20,100};
                    SevenSegU[6] = {(x-10)+60,(y-50)+60,20,100};
                }
                    
                SDL_Rect SevenSegT[7];
                {
                    int x = Application.Dimension.w/2;
                    int y = Application.Dimension.h/2;
                    SevenSegT[0] = {(x-10)-60,(y-50)-60,20,100};
                    SevenSegT[1] = {(x-10)-60,(y-50)+60,20,100};
                    SevenSegT[2] = {x-50,(y-10)-120,100,20};
                    SevenSegT[3] = {x-50,y-10,100,20};
                    SevenSegT[4] = {x-50,(y-10)+120,100,20};
                    SevenSegT[5] = {(x-10)+60,(y-50)-60,20,100};
                    SevenSegT[6] = {(x-10)+60,(y-50)+60,20,100};
                }

                SDL_Rect SevenSegH[7];
                {
                    int x = (Application.Dimension.w/2)-200;
                    int y = Application.Dimension.h/2;
                    SevenSegH[0] = {(x-10)-60,(y-50)-60,20,100};
                    SevenSegH[1] = {(x-10)-60,(y-50)+60,20,100};
                    SevenSegH[2] = {x-50,(y-10)-120,100,20};
                    SevenSegH[3] = {x-50,y-10,100,20};
                    SevenSegH[4] = {x-50,(y-10)+120,100,20};
                    SevenSegH[5] = {(x-10)+60,(y-50)-60,20,100};
                    SevenSegH[6] = {(x-10)+60,(y-50)+60,20,100};
                }

                //TEST: High Score 7-seg display (Units, Tens, Hundreds)
                SDL_Rect SevenSegHSU[7];
                {
                    int x = (Application.Dimension.w/2)+100;
                    int y = Application.Dimension.h - (Application.Dimension.h/8);
                    SevenSegHSU[0] = {(x-5)-30,(y-25)-30,10,50};
                    SevenSegHSU[1] = {(x-5)-30,(y-25)+30,10,50};
                    SevenSegHSU[2] = {x-25,(y-5)-60,50,10};
                    SevenSegHSU[3] = {x-25,y-5,50,10};
                    SevenSegHSU[4] = {x-25,(y-5)+60,50,10};
                    SevenSegHSU[5] = {(x-5)+30,(y-25)-30,10,50};
                    SevenSegHSU[6] = {(x-5)+30,(y-25)+30,10,50};
                }
                    
                SDL_Rect SevenSegHST[7];
                {
                    int x = Application.Dimension.w/2;
                    int y = Application.Dimension.h - (Application.Dimension.h/8);
                    SevenSegHST[0] = {(x-5)-30,(y-25)-30,10,50};
                    SevenSegHST[1] = {(x-5)-30,(y-25)+30,10,50};
                    SevenSegHST[2] = {x-25,(y-5)-60,50,10};
                    SevenSegHST[3] = {x-25,y-5,50,10};
                    SevenSegHST[4] = {x-25,(y-5)+60,50,10};
                    SevenSegHST[5] = {(x-5)+30,(y-25)-30,10,50};
                    SevenSegHST[6] = {(x-5)+30,(y-25)+30,10,50};
                }

                SDL_Rect SevenSegHSH[7];
                {
                    int x = (Application.Dimension.w/2)-100;
                    int y = Application.Dimension.h - (Application.Dimension.h/8);
                    SevenSegHSH[0] = {(x-5)-30,(y-25)-30,10,50};
                    SevenSegHSH[1] = {(x-5)-30,(y-25)+30,10,50};
                    SevenSegHSH[2] = {x-25,(y-5)-60,50,10};
                    SevenSegHSH[3] = {x-25,y-5,50,10};
                    SevenSegHSH[4] = {x-25,(y-5)+60,50,10};
                    SevenSegHSH[5] = {(x-5)+30,(y-25)-30,10,50};
                    SevenSegHSH[6] = {(x-5)+30,(y-25)+30,10,50};
                }

                bool SevenSegLayout[10][7] = {
                  {1,1,1,0,1,1,1}, //0
                  {0,0,0,0,0,1,1}, //1
                  {0,1,1,1,1,1,0}, //2
                  {0,0,1,1,1,1,1}, //3
                  {1,0,0,1,0,1,1}, //4
                  {1,0,1,1,1,0,1}, //5
                  {1,1,1,1,1,0,1}, //6
                  {0,0,1,0,0,1,1}, //7
                  {1,1,1,1,1,1,1}, //8
                  {1,0,1,1,0,1,1}}; //9

                //TEST: audio
                SDL_AudioDeviceID AudioDevice = {};
                SDL_AudioSpec AudioSpec = {};
                AudioSpec.freq = 48000;
                AudioSpec.format = AUDIO_S16SYS;
                AudioSpec.channels = 2;
                AudioSpec.samples = 512;
                AudioDevice = SDL_OpenAudioDevice(0, 0, &AudioSpec, 0, 0);
                SDL_PauseAudioDevice(AudioDevice, 0);

                //NOTE: FrameTiming
                uint32 Timer_TotalFrameTicks = 0;
                uint32 Timer_TotalFrames = 0;
                uint32 Timer_StartTicks = SDL_GetTicks();
                uint32 Timer_10SecTotalFrameTicks = 0;
                uint32 Timer_10SecTotalFrames = 0;
                uint32 Timer_10SecStartTicks = SDL_GetTicks();
                    
                while(Application.Running)
                {
                    Timer_10SecTotalFrames++;
                    Timer_TotalFrames++;
                        
                    uint32 Cap_StartTicks = SDL_GetTicks();
                    uint64 Timer_StartPerf = SDL_GetPerformanceCounter();
                        
                    LAM_HandleEvents();

                    //NOTE: Reset Controller if disconnected & reconnected
                    if(!SDL_GameControllerGetAttached(Controller))
                    {
                        SDL_GameControllerClose(Controller);
                        Controller = 0;
                    }
                        
                    if(!Controller)
                    {
                        for(int iJoystick = 0; iJoystick < SDL_NumJoysticks(); ++iJoystick)
                        {
                            if(SDL_IsGameController(iJoystick))
                            {
                                Controller = SDL_GameControllerOpen(iJoystick);
                                if(!Controller)
                                {
                                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                                             "Failed to open Controller",
                                                             SDL_GetError(), 0);
                                }
                            }
                        }
                    }

                    //NOTE: Controller
                    if(SDL_GameControllerGetButton(Controller, SDL_CONTROLLER_BUTTON_DPAD_UP))
                        PlayerRect.y-=PlayerRectSpeed;
                    if(SDL_GameControllerGetButton(Controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN))
                        PlayerRect.y+=PlayerRectSpeed;

                    //NOTE: Keyboard
                    if(LAM_Keydown('w') || LAM_Keydown(SDLK_UP))
                    {
                        if(LAM_Keydown(SDLK_LSHIFT) || LAM_Keydown(SDLK_RSHIFT))
                            PlayerRect.y -= PlayerRectSpeed*2;

                        else if(LAM_Keydown(SDLK_LCTRL) || LAM_Keydown(SDLK_RCTRL))
                            PlayerRect.y -= PlayerRectSpeed/2;

                        else PlayerRect.y -= PlayerRectSpeed;
                    }

                    if(LAM_Keydown('s') || LAM_Keydown(SDLK_DOWN))
                    {
                        if(LAM_Keydown(SDLK_LSHIFT) || LAM_Keydown(SDLK_RSHIFT))
                            PlayerRect.y += PlayerRectSpeed*2;

                        else if(LAM_Keydown(SDLK_LCTRL) || LAM_Keydown(SDLK_RCTRL))
                            PlayerRect.y += PlayerRectSpeed/2;

                        else PlayerRect.y += PlayerRectSpeed;
                    }

                    //NOTE: Mouse
                    if(Application.MouseInWindow)
                    {
                        int MouseSpeed = PlayerRectSpeed/2;
                            
                        if(Application.MouseY > (PlayerRect.y + (PlayerRect.h/2)))
                        {
                            if(Application.MouseLeft)
                                PlayerRect.y += MouseSpeed*2;

                            else if(Application.MouseRight)
                                PlayerRect.y += MouseSpeed/2;
                                
                            else
                                PlayerRect.y += MouseSpeed;
                                
                        }

                        if(Application.MouseY < (PlayerRect.y + (PlayerRect.h/2)))
                        {
                            if(Application.MouseLeft)
                                PlayerRect.y -= MouseSpeed*2;

                            else if(Application.MouseRight)
                                PlayerRect.y -= MouseSpeed/2;
                                
                            else
                                PlayerRect.y -= MouseSpeed;
                        }
                    }

                    // TEST: player boundary
                    if(PlayerRect.y <= 0)
                        PlayerRect.y = 0;
                    if(PlayerRect.y + PlayerRect.h >= Application.Dimension.h)
                        PlayerRect.y = Application.Dimension.h - PlayerRect.h;
                    if(PlayerRect.x <= 0)
                        PlayerRect.x = 0;
                    if(PlayerRect.x + PlayerRect.w >= Application.Dimension.w)
                        PlayerRect.x = Application.Dimension.w - PlayerRect.w;

                    //TEST: Bounce Rect motion
                    if(BounceRectDown) BounceRect.y += (BounceRectSpeed - (BounceRectSpeed/50));
                    else BounceRect.y -= (BounceRectSpeed - (BounceRectSpeed/50));
                    if(BounceRectRight) BounceRect.x += BounceRectSpeed;
                    else BounceRect.x -= BounceRectSpeed;

                    //TEST: bounce rect boundary
                    bool BounceTopBoundary = 0;
                    bool BounceBotBoundary = 0;
                    bool BounceRightBoundary = 0;
                        
                    if(BounceRect.x + BounceRect.w >= Application.Dimension.w)
                    {
                        BounceRightBoundary = 1;
                        BounceBounced = 1;
                        BounceRectRight = 0;
                    }

                    if(BounceRect.x <= 0)
                    {
                        if(!(BounceRect.y >= PlayerRect.y &&
                             BounceRect.y <= PlayerRect.y + PlayerRect.h))
                        {
                            BounceBouncedGoal = 1;
                        }

                        BounceRectRight = 1;
                    }
                        
                    if(BounceRect.y + BounceRect.h >= Application.Dimension.h)
                    {
                        BounceBotBoundary = 1;
                        BounceBounced = 1;
                        BounceRectDown = 0;
                    }
                        
                    if(BounceRect.y <= 0)
                    {
                        BounceTopBoundary = 1;
                        BounceBounced = 1;
                        BounceRectDown = 1;
                    }

                    SDL_Rect Intersect = {};
                    //TEST: BounceRect intersect PlayerRect
                    if(SDL_IntersectRect(&PlayerRect, &BounceRect, &Intersect))
                    {
                        BounceRectRight = 1;
                        BouncePlayerHit = 1;
                            
                        if(Intersect.h > Intersect.w)
                        {
                            if(Intersect.x == BounceRect.x) // players right
                            {
                                PlayerRect.x -= Intersect.w;
                            }
                        }

                        else
                        {
                            if(Intersect.y == BounceRect.y) // players bot
                            {
                                BounceRectDown = 1;
                                PlayerRect.y -= Intersect.h;
                            }
                            if(Intersect.y == PlayerRect.y) // players top
                            {
                                BounceRectDown = 0;
                                PlayerRect.y += Intersect.h;
                            }
                        }
                    }

                    else
                    {
                        Intersect = {};
                        if(BouncePlayerHit) BounceRectSpeed++;
                        if(BouncePlayerHit) BounceBouncedPlayer = 1;
                        BouncePlayerHit = 0;
                    }

                    if(HighScore < BounceRectSpeed) HighScore = BounceRectSpeed;
                        
                        
                    //TEST: Audio sinewave
                    if(BounceBounced || BounceBouncedPlayer ||
                       BounceBouncedGoal || BouncePlayerHit)
                    {
                        persist int ToneFreq = 500;
                        persist real32 tSine;
                        if(BounceBounced) ToneFreq = 440;
                        if(BounceBouncedPlayer || BouncePlayerHit) ToneFreq = 880;
                        if(BounceBouncedGoal) ToneFreq = 220;
                        if(BounceBounced) BounceBounced = 0;
                        if(BounceBouncedPlayer || BouncePlayerHit) BounceBouncedPlayer = 0;
                        int Div = 25;
                        if(BounceBouncedGoal) Div = 6;
                        if(!tSine) tSine = 0;
                        int16 Volume = 1000;
                        int WavePeriod = AudioSpec.freq/ToneFreq;
                        for(int i = 0; i < (AudioSpec.freq/Div); i++)
                        {
                            real32 SineValue = sinf(tSine);
                            int16 SampleValue = (int16)(SineValue*Volume);
                            int16 Samples[2];
                            Samples[0] = SampleValue;
                            Samples[1] = SampleValue;
                            tSine += (PI32 * 2.0f) * (1.0f / (real32)WavePeriod);
                            SDL_QueueAudio(AudioDevice, &Samples[0], sizeof(int16)*2);
                        }
                    }

                    //TEST: draw to screen
                    SDL_SetRenderTarget(Application.Renderer, Texture);
                    SDL_SetRenderDrawColor(Application.Renderer, 0,0,0,0);
                    SDL_RenderClear(Application.Renderer);

                    // Goal Indicator
                    if(BounceBouncedGoal)
                    {
                        SDL_SetRenderDrawColor(Application.Renderer, 0xaa,0x11,0x11,0xff);
                        SDL_RenderClear(Application.Renderer);
                        BounceBouncedGoal = 0;
                        if(BounceRectSpeed%10 != 0)BounceRectSpeed--;
                        if(!BounceRectSpeed) BounceRectSpeed = 1;
                    }

                    //TEST: Draw 7-seg display for BounceRectSpeed
                    for(int iSeg = 0; iSeg < 7; iSeg++)
                    {
                        int Unit = BounceRectSpeed%10;
                        uint8 Color = ((SevenSegLayout[Unit][iSeg])?0x30:0x05);
                        SDL_RenderDrawRect(Application.Renderer, &SevenSegU[iSeg]);
                        SDL_SetRenderDrawColor(Application.Renderer,Color,Color,Color,0xff);
                        SDL_RenderFillRect(Application.Renderer, &SevenSegU[iSeg]);
                    }

                    for(int iSeg = 0; iSeg < 7; iSeg++)
                    {
                        int Unit = (BounceRectSpeed/10)%10;
                        uint8 Color = ((SevenSegLayout[Unit][iSeg])?0x30:0x05);
                        SDL_RenderDrawRect(Application.Renderer, &SevenSegT[iSeg]);
                        SDL_SetRenderDrawColor(Application.Renderer,Color,Color,Color,0xff);
                        SDL_RenderFillRect(Application.Renderer, &SevenSegT[iSeg]);
                    }

                    for(int iSeg = 0; iSeg < 7; iSeg++)
                    {
                        int Unit = (BounceRectSpeed/100)%10;
                        uint8 Color = ((SevenSegLayout[Unit][iSeg])?0x30:0x05);
                        SDL_RenderDrawRect(Application.Renderer, &SevenSegH[iSeg]);
                        SDL_SetRenderDrawColor(Application.Renderer,Color,Color,Color,0xff);
                        SDL_RenderFillRect(Application.Renderer, &SevenSegH[iSeg]);
                    }

                    //TEST: Draw 7-seg display for HighScore
                    for(int iSeg = 0; iSeg < 7; iSeg++)
                    {
                        int Unit = HighScore%10;
                        uint8 Color = ((SevenSegLayout[Unit][iSeg])?0x30:0x05);
                        uint8 High = ((SevenSegLayout[Unit][iSeg])?0x60:0x05);
                        SDL_RenderDrawRect(Application.Renderer, &SevenSegHSU[iSeg]);
                        SDL_SetRenderDrawColor(Application.Renderer,Color,High,Color,0xff);
                        SDL_RenderFillRect(Application.Renderer, &SevenSegHSU[iSeg]);
                    }

                    for(int iSeg = 0; iSeg < 7; iSeg++)
                    {
                        int Unit = (HighScore/10)%10;
                        uint8 Color = ((SevenSegLayout[Unit][iSeg])?0x30:0x05);
                        uint8 High = ((SevenSegLayout[Unit][iSeg])?0x60:0x05);
                        SDL_RenderDrawRect(Application.Renderer, &SevenSegHST[iSeg]);
                        SDL_SetRenderDrawColor(Application.Renderer,Color,High,Color,0xff);
                        SDL_RenderFillRect(Application.Renderer, &SevenSegHST[iSeg]);
                    }

                    for(int iSeg = 0; iSeg < 7; iSeg++)
                    {
                        int Unit = (HighScore/100)%10;
                        uint8 Color = ((SevenSegLayout[Unit][iSeg])?0x30:0x05);
                        uint8 High = ((SevenSegLayout[Unit][iSeg])?0x60:0x05);
                        SDL_RenderDrawRect(Application.Renderer, &SevenSegHSH[iSeg]);
                        SDL_SetRenderDrawColor(Application.Renderer,Color,High,Color,0xff);
                        SDL_RenderFillRect(Application.Renderer, &SevenSegHSH[iSeg]);
                    }

                    //BoundaryTop
                    uint8 BndClr = ((BounceTopBoundary)?0xcc:0x40);
                    SDL_RenderDrawRect(Application.Renderer, &BoundaryTop);
                    SDL_SetRenderDrawColor(Application.Renderer,BndClr,BndClr,BndClr,0xff);
                    SDL_RenderFillRect(Application.Renderer, &BoundaryTop);

                    //BoundaryBot
                    BndClr = ((BounceBotBoundary)?0xcc:0x40);
                    SDL_RenderDrawRect(Application.Renderer, &BoundaryBot);
                    SDL_SetRenderDrawColor(Application.Renderer,BndClr,BndClr,BndClr,0xff);
                    SDL_RenderFillRect(Application.Renderer, &BoundaryBot);

                    //BoundaryRight
                    BndClr = ((BounceRightBoundary)?0xcc:0x40);
                    SDL_RenderDrawRect(Application.Renderer, &BoundaryRight);
                    SDL_SetRenderDrawColor(Application.Renderer,BndClr,BndClr,BndClr,0xff);
                    SDL_RenderFillRect(Application.Renderer, &BoundaryRight);

                    //GoalLine
                    SDL_RenderDrawRect(Application.Renderer, &GoalLine);
                    SDL_SetRenderDrawColor(Application.Renderer, 0x50,0x10,0x10,0xff);
                    SDL_RenderFillRect(Application.Renderer, &GoalLine);
                       
                    // Player
                    SDL_RenderDrawRect(Application.Renderer, &PlayerRect);
                    SDL_SetRenderDrawColor(Application.Renderer, 0x15,0xf0,0xff,0xff);
                    SDL_RenderFillRect(Application.Renderer, &PlayerRect);

                    // Ball
                    SDL_RenderDrawRect(Application.Renderer, &BounceRect);
                    SDL_SetRenderDrawColor(Application.Renderer, 0xff,0,0,0xff);
                    SDL_RenderFillRect(Application.Renderer, &BounceRect);

                    // PlayerHit Indicator
                    SDL_RenderDrawRect(Application.Renderer, &Intersect);
                    SDL_SetRenderDrawColor(Application.Renderer, 0,0xff,0,0xff);
                    SDL_RenderFillRect(Application.Renderer, &Intersect);

                    SDL_SetRenderTarget(Application.Renderer, 0);
                    SDL_RenderCopy(Application.Renderer, Texture, 0,0);
                    SDL_RenderPresent(Application.Renderer);

                    // NOTE: FrameTiming
                    uint32 Timer_FrameTicks = SDL_GetTicks() - Timer_StartTicks;
                    Timer_TotalFrameTicks += Timer_FrameTicks;
                    real32 Timer_AvgFPS = Timer_TotalFrames / (Timer_FrameTicks / 1000.0f);

                    uint32 Timer_10SecFrameTicks = SDL_GetTicks() - Timer_10SecStartTicks;
                    Timer_10SecTotalFrameTicks += Timer_10SecFrameTicks;
                    real32 Timer_10SecAvgFPS = Timer_10SecTotalFrames / (Timer_10SecFrameTicks
                                                                         / 1000.0f);
                        
                    uint64 Timer_EndPerf = SDL_GetPerformanceCounter();
                    uint64 Timer_FramePerf = Timer_EndPerf - Timer_StartPerf;

                    char str[128];
                    sprintf_s(str, "LameBall [%.0ffps]", Timer_10SecAvgFPS);
                    SDL_SetWindowTitle(Application.Window, str);

                    //SDL_Log("AvgFPS(10sec):%f, AvgFPS(Total): %f, Perf: %d",
                    //      Timer_10SecAvgFPS, Timer_AvgFPS, Timer_FramePerf);

                    if(Timer_10SecTotalFrameTicks >= 10000)
                    {
                        Timer_10SecTotalFrameTicks = 0;
                        Timer_10SecTotalFrames = 0;
                        Timer_10SecStartTicks = SDL_GetTicks();
                    }


                    uint32 Cap_FrameTicks = SDL_GetTicks() - Cap_StartTicks;
                    real32 Cap_DesiredTicksPerFrame = 1000.0f/60.0f;
                    if(Cap_FrameTicks < Cap_DesiredTicksPerFrame)
                    {
                        SDL_Delay((uint32)(Cap_DesiredTicksPerFrame - Cap_FrameTicks));
                    }
                        

                } // while(running)
            }

            else //NOTE: SDL_CreateRenderer error
            {
                SDL_SetError("Failed to create Renderer! %s", SDL_GetError());
                SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, SDL_GetError());
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Error!",
                                         SDL_GetError(), 0);
            }
        }

        else //NOTE: SDL_CreateWindow error
        {
            SDL_SetError("Failed to create Window! %s", SDL_GetError());
            SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, SDL_GetError());
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Error!",
                                     SDL_GetError(), 0);
        }
    }

    else //NOTE: SDL_Init error
    {
        SDL_SetError("Failed to Init SDL! %s", SDL_GetError());
        SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, SDL_GetError());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Error!",
                                 SDL_GetError(), 0);
    }
    
    SDL_Quit();
    return 0;
}
