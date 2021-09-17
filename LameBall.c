/* ========================================================================
   $File:    LameBall.c $
   $Project: LameBall $
   $Date:    08-09-2021 $
   $Author:  Phil Bagshaw $
   $Notice:  (c)Phragware 2021 $
   ======================================================================== */

#include <SDL/SDL.h>
#include "Events.c"
//TODO: https://raw.githubusercontent.com/gabomdq/SDL_GameControllerDB/master/gamecontrollerdb.txt
// SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt"); (alt: from char*, from RWops)
// Community sourced SDL Controller config, maybe use Curl to download it (it gets updated)
//TODO: finish Controller input controls
//TODO: Cleanup main loop (there's some variables that do the same thing)
//TODO: recalculate all the stuff that happens before game loop when window is resized

#define internal static
#define persist static
#define global static
#define INT8_MIN -0x80
#define INT8_MAX 0x7f
#define UINT8_MAX 0xffU
#define INT16_MIN -0x8000
#define INT16_MAX 0x7fff
#define UINT16_MAX 0xffffU
#define INT32_MIN -0x80000000LL
#define INT32_MAX 0x7FFFFFFFLL
#define UINT32_MAX 0xffffffffULL
#define INT64_MIN -0x8000000000000000LL
#define INT64_MAX 0x7FFFFFFFFFFFFFFFLL
#define UINT64_MAX 0xffffffffffffffffULL
#define REAL32_MIN 1.17549e-038f
#define REAL32_MAX 3.40282e+038f
#define REAL64_MIN 2.22507e-308
#define REAL64_MAX 1.79769e+308
typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;
typedef float real32;
typedef double real64;
typedef int8 bool;

#define KEYDOWN_COUNT 0xf
global struct
Lam_Application
{
    SDL_Window *Window;
    SDL_Renderer *Renderer;
    SDL_Rect Dimension;
    bool Running;
    bool Exposed;
    bool Minimized;
    bool Maximized;
    bool Fullscreen;
    bool MouseInWindow;
    bool KeyboardFocus;
    SDL_Keycode Keydowns[KEYDOWN_COUNT];
    bool DisableMouse;
    int MouseX;
    int MouseY;
    bool MouseLeft;
    bool MouseRight;
} Application;

internal void
LAM_SetKeyUp(SDL_Keycode Key)
{
    for(int iKeydown = 0; iKeydown < KEYDOWN_COUNT; ++iKeydown)
    {
        if(Application.Keydowns[iKeydown] == Key)
        {
            Application.Keydowns[iKeydown] = 0;
            break;
        }
    }
    
}

internal void
LAM_SetKeyDown(SDL_Keycode Key)
{
    bool KeydownExists = 0;

    for(int iKeydown = 0; iKeydown < KEYDOWN_COUNT; ++iKeydown)
    {
        if(Application.Keydowns[iKeydown] == Key)
        {
            KeydownExists = 1;
            break;
        }
    }

    if(!KeydownExists)
    {
        for(int iKeydown = 0; iKeydown < KEYDOWN_COUNT; ++iKeydown)
        {
            if(Application.Keydowns[iKeydown] == 0)
            {
                Application.Keydowns[iKeydown] = Key;
                break;
            }
        }
    }

}

internal bool
LAM_KeySingle(SDL_Keycode Key)
{
    for(int iKeydown = 0; iKeydown < KEYDOWN_COUNT; ++iKeydown)
    {
        if(Application.Keydowns[iKeydown] == Key)
        {
            LAM_SetKeyUp(Key);
            return 1;
        }
    }

    return 0;    
}

internal bool
LAM_Keydown(SDL_Keycode Key)
{
    for(int iKeydown = 0; iKeydown < KEYDOWN_COUNT; ++iKeydown)
    {
        if(Application.Keydowns[iKeydown] == Key)
        {
            return 1;
        }
    }

    return 0;
}

internal void
LAM_HandleEvents()
{
    SDL_Event Event;
    while(SDL_PollEvent(&Event))
    {
        switch(Event.type)
        {
          case SDL_QUIT:
          {
              Application.Running = 0;
          } break;

          case SDL_WINDOWEVENT:
          {
              switch(Event.window.event)
              {
                case SDL_WINDOWEVENT_SHOWN:
                {
                    //NOTE: Should only happen at startup.
                    // This is NOT the same as EXPOSED
                    SDL_Log("Event: Window Shown");
                } break;
                
                case SDL_WINDOWEVENT_HIDDEN:
                {
                    //NOTE: This should never happen, just quit if it does.
                    SDL_Log("Unexpected Event: Window Hidden");
                    Application.Running = 0;
                } break;
                
                case SDL_WINDOWEVENT_EXPOSED:
                {
                    Application.Exposed = 1;
                    Application.Minimized = 0;
                } break;

                case SDL_WINDOWEVENT_MOVED:
                {
                    Application.Dimension.x = Event.window.data1;
                    Application.Dimension.y = Event.window.data2;
                } break;

                case SDL_WINDOWEVENT_SIZE_CHANGED:
                {
                    Application.Dimension.w = Event.window.data1;
                    Application.Dimension.h = Event.window.data2;
                } break;

                case SDL_WINDOWEVENT_MINIMIZED:
                {
                    Application.Exposed = 0;
                    Application.Minimized = 1;
                } break;

                case SDL_WINDOWEVENT_MAXIMIZED:
                {
                    Application.Maximized = 1;
                } break;

                case SDL_WINDOWEVENT_RESTORED:
                {
                    Application.Minimized = 0;
                    Application.Maximized = 0;
                } break;

                case SDL_WINDOWEVENT_ENTER:
                {
                    Application.MouseInWindow = 1;
                } break;

                case SDL_WINDOWEVENT_LEAVE:
                {
                    Application.MouseInWindow = 0;
                } break;

                case SDL_WINDOWEVENT_FOCUS_GAINED:
                {
                    Application.KeyboardFocus = 1;
                } break;

                case SDL_WINDOWEVENT_FOCUS_LOST:
                {
                    Application.KeyboardFocus = 0;
                } break;

                case SDL_WINDOWEVENT_CLOSE:
                {
                    Application.Running = 0;
                } break;
              }
          } break;

          case SDL_KEYDOWN:
          {
              LAM_SetKeyDown(Event.key.keysym.sym);
          } break;

          case SDL_KEYUP:
          {
              LAM_SetKeyUp(Event.key.keysym.sym);
          } break;

          case SDL_MOUSEMOTION:
          case SDL_MOUSEWHEEL:
          {
              if(Application.MouseInWindow)
              {
                  SDL_GetMouseState(&Application.MouseX, &Application.MouseY);
              }
          } break;

          case SDL_MOUSEBUTTONDOWN:
          {
              if(Event.button.button == SDL_BUTTON_LEFT) Application.MouseLeft = 1;
              if(Event.button.button == SDL_BUTTON_RIGHT) Application.MouseRight = 1;
          } break;

          case SDL_MOUSEBUTTONUP:
          {
              if(Event.button.button == SDL_BUTTON_LEFT) Application.MouseLeft = 0;
              if(Event.button.button == SDL_BUTTON_RIGHT) Application.MouseRight = 0;
          } break;


          default:
          {
              break;
          }
        }
    } // while PollEvent
}

int
main(int argc, char **argv)
{
    SDL_memset(&Application, 0, sizeof(Application));
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
                Application.Running = 1;

                //TEST: controller
                SDL_GameController *Controller = 0;

                //TEST: draw to screen
                SDL_Texture *Texture = SDL_CreateTexture(Application.Renderer,
                                                         SDL_PIXELFORMAT_RGBA8888,
                                                         SDL_TEXTUREACCESS_TARGET,
                                                         Application.Dimension.w,
                                                         Application.Dimension.h);
                SDL_Rect PlayerRect;
                SDL_memset(&PlayerRect, 0, sizeof(SDL_Rect));
                PlayerRect.w = 25;
                PlayerRect.h = 120;
                int PlayerRectSpeed = 10;
                bool BouncePlayerHit = 0;
                    
                SDL_Rect BounceRect;
                SDL_memset(&BounceRect, 0, sizeof(SDL_Rect));
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
                    SevenSegU[0] = (SDL_Rect){(x-10)-60,(y-50)-60,20,100};
                    SevenSegU[1] = (SDL_Rect){(x-10)-60,(y-50)+60,20,100};
                    SevenSegU[2] = (SDL_Rect){x-50,(y-10)-120,100,20};
                    SevenSegU[3] = (SDL_Rect){x-50,y-10,100,20};
                    SevenSegU[4] = (SDL_Rect){x-50,(y-10)+120,100,20};
                    SevenSegU[5] = (SDL_Rect){(x-10)+60,(y-50)-60,20,100};
                    SevenSegU[6] = (SDL_Rect){(x-10)+60,(y-50)+60,20,100};
                }
                    
                SDL_Rect SevenSegT[7];
                {
                    int x = Application.Dimension.w/2;
                    int y = Application.Dimension.h/2;
                    SevenSegT[0] = (SDL_Rect){(x-10)-60,(y-50)-60,20,100};
                    SevenSegT[1] = (SDL_Rect){(x-10)-60,(y-50)+60,20,100};
                    SevenSegT[2] = (SDL_Rect){x-50,(y-10)-120,100,20};
                    SevenSegT[3] = (SDL_Rect){x-50,y-10,100,20};
                    SevenSegT[4] = (SDL_Rect){x-50,(y-10)+120,100,20};
                    SevenSegT[5] = (SDL_Rect){(x-10)+60,(y-50)-60,20,100};
                    SevenSegT[6] = (SDL_Rect){(x-10)+60,(y-50)+60,20,100};
                }

                SDL_Rect SevenSegH[7];
                {
                    int x = (Application.Dimension.w/2)-200;
                    int y = Application.Dimension.h/2;
                    SevenSegH[0] = (SDL_Rect){(x-10)-60,(y-50)-60,20,100};
                    SevenSegH[1] = (SDL_Rect){(x-10)-60,(y-50)+60,20,100};
                    SevenSegH[2] = (SDL_Rect){x-50,(y-10)-120,100,20};
                    SevenSegH[3] = (SDL_Rect){x-50,y-10,100,20};
                    SevenSegH[4] = (SDL_Rect){x-50,(y-10)+120,100,20};
                    SevenSegH[5] = (SDL_Rect){(x-10)+60,(y-50)-60,20,100};
                    SevenSegH[6] = (SDL_Rect){(x-10)+60,(y-50)+60,20,100};
                }

                //TEST: High Score 7-seg display (Units, Tens, Hundreds)
                SDL_Rect SevenSegHSU[7];
                {
                    int x = (Application.Dimension.w/2)+100;
                    int y = Application.Dimension.h - (Application.Dimension.h/8);
                    SevenSegHSU[0] = (SDL_Rect){(x-5)-30,(y-25)-30,10,50};
                    SevenSegHSU[1] = (SDL_Rect){(x-5)-30,(y-25)+30,10,50};
                    SevenSegHSU[2] = (SDL_Rect){x-25,(y-5)-60,50,10};
                    SevenSegHSU[3] = (SDL_Rect){x-25,y-5,50,10};
                    SevenSegHSU[4] = (SDL_Rect){x-25,(y-5)+60,50,10};
                    SevenSegHSU[5] = (SDL_Rect){(x-5)+30,(y-25)-30,10,50};
                    SevenSegHSU[6] = (SDL_Rect){(x-5)+30,(y-25)+30,10,50};
                }
                    
                SDL_Rect SevenSegHST[7];
                {
                    int x = Application.Dimension.w/2;
                    int y = Application.Dimension.h - (Application.Dimension.h/8);
                    SevenSegHST[0] = (SDL_Rect){(x-5)-30,(y-25)-30,10,50};
                    SevenSegHST[1] = (SDL_Rect){(x-5)-30,(y-25)+30,10,50};
                    SevenSegHST[2] = (SDL_Rect){x-25,(y-5)-60,50,10};
                    SevenSegHST[3] = (SDL_Rect){x-25,y-5,50,10};
                    SevenSegHST[4] = (SDL_Rect){x-25,(y-5)+60,50,10};
                    SevenSegHST[5] = (SDL_Rect){(x-5)+30,(y-25)-30,10,50};
                    SevenSegHST[6] = (SDL_Rect){(x-5)+30,(y-25)+30,10,50};
                }

                SDL_Rect SevenSegHSH[7];
                {
                    int x = (Application.Dimension.w/2)-100;
                    int y = Application.Dimension.h - (Application.Dimension.h/8);
                    SevenSegHSH[0] = (SDL_Rect){(x-5)-30,(y-25)-30,10,50};
                    SevenSegHSH[1] = (SDL_Rect){(x-5)-30,(y-25)+30,10,50};
                    SevenSegHSH[2] = (SDL_Rect){x-25,(y-5)-60,50,10};
                    SevenSegHSH[3] = (SDL_Rect){x-25,y-5,50,10};
                    SevenSegHSH[4] = (SDL_Rect){x-25,(y-5)+60,50,10};
                    SevenSegHSH[5] = (SDL_Rect){(x-5)+30,(y-25)-30,10,50};
                    SevenSegHSH[6] = (SDL_Rect){(x-5)+30,(y-25)+30,10,50};
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
                SDL_AudioDeviceID AudioDevice;
                SDL_memset(&AudioDevice, 0, sizeof(SDL_AudioDeviceID));
                SDL_AudioSpec AudioSpec;
                SDL_memset(&AudioSpec, 0, sizeof(SDL_AudioSpec));
                AudioSpec.freq = 48000;
                AudioSpec.format = AUDIO_S16SYS;
                AudioSpec.channels = 2;
                AudioSpec.samples = 512;
                AudioDevice = SDL_OpenAudioDevice(0, 0, &AudioSpec, 0, 0);
                SDL_PauseAudioDevice(AudioDevice, 0);
				real32 tSine = 0;
				int ToneFreq = 500;

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

                    if(LAM_Keydown(SDLK_F11))
                    {
                        if(Application.Fullscreen)
                        {
                            SDL_SetWindowFullscreen(Application.Window, 0);
                            Application.Fullscreen = 0;
                        }

                        else
                        {
                            SDL_SetWindowFullscreen(Application.Window, 1);
                            Application.Fullscreen = 1;
                        }
                    }

                    if(LAM_Keydown(SDLK_LCTRL) && LAM_Keydown(SDLK_LSHIFT) &&
                       LAM_KeySingle(SDLK_F12))
                    {
                        int X = BounceRectSpeed%10;
                        int Y = 10 - X;
                        BounceRectSpeed += Y;
                    }

                    if(LAM_KeySingle(SDLK_F1))
                    {
                        Application.DisableMouse = !Application.DisableMouse;
                    }

                    //NOTE: Mouse
                    if(!Application.DisableMouse && Application.MouseInWindow)
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

                    SDL_Rect Intersect;
                    SDL_memset(&Intersect, 0, sizeof(SDL_Rect));
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
                        SDL_memset(&Intersect, 0, sizeof(SDL_Rect));
                        if(BouncePlayerHit) BounceRectSpeed++;
                        if(BouncePlayerHit) BounceBouncedPlayer = 1;
                        BouncePlayerHit = 0;
                    }

                    if(HighScore < BounceRectSpeed) HighScore = BounceRectSpeed;
                        
                        
                    //TEST: Audio sinewave
                    if(BounceBounced || BounceBouncedPlayer ||
                       BounceBouncedGoal || BouncePlayerHit)
                    {
						int Div = 25;
					
                        if(BounceBouncedPlayer || BouncePlayerHit)
						{
							ToneFreq = 880;
							BounceBouncedPlayer = 0;
						}
						
						if(BounceBounced)
						{
							ToneFreq = 440;
							BounceBounced = 0;
						}
						
						if(BounceBouncedGoal)
						{
							ToneFreq = 220;
							Div = 6;
						}
                         
                        int16 Volume = 1000;
                        int WavePeriod = AudioSpec.freq/ToneFreq;
                        for(int i = 0; i < (AudioSpec.freq/Div); i++)
                        {
                            real32 SineValue = sinf(tSine);
                            int16 SampleValue = (int16)(SineValue*Volume);
                            int16 Samples[2];
                            Samples[0] = SampleValue;
                            Samples[1] = SampleValue;
                            tSine += ((real32)M_PI * 2.0f) * (1.0f / (real32)WavePeriod);
                            if(SDL_GetQueuedAudioSize(AudioDevice) < 1024*256)
                                SDL_QueueAudio(AudioDevice, &Samples[0], sizeof(int16)*2);
                        }
                    }

                    //TEST: draw to texture
                    SDL_SetRenderTarget(Application.Renderer, Texture);

                    // Goal Indicator
                    if(BounceBouncedGoal)
                    {
                        SDL_SetRenderDrawColor(Application.Renderer, 0xaa,0x11,0x11,0xff);
                        SDL_RenderClear(Application.Renderer);
                        BounceBouncedGoal = 0;
                        if(BounceRectSpeed%10 != 0)BounceRectSpeed--;
                        if(!BounceRectSpeed) BounceRectSpeed = 1;
                    }

                    else
                    {
                        SDL_SetRenderDrawColor(Application.Renderer, 0,0,0,0);
                        SDL_RenderClear(Application.Renderer);                        
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
