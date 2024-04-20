/*================================
          Lameball
     Phragware 2021-2024
================================*/

#include <PL/PL.h>

#define VER_MAJ 0
#define VER_MIN 4

typedef struct
{
    r32 w,h;
    r32 speed;
    v2 pos;
    v2 dir;
} Entity;

typedef struct
{
    int verMaj, verMin;
    Entity paddle;
    Entity ball;
} State;

void PL_ErrorCallback(void)
{
    PL_PrintErr("%s", PL_GetErrorString());
}

void PL_Startup(void)
{
    PL_SetUserMemorySize(sizeof(State));
    State *state = (State*)PL_GetUserMemory();
    state->verMaj = VER_MAJ;
    state->verMin = VER_MIN;
    
    PL_SetWindowTitle("LameBall v%d.%d", state->verMaj, state->verMin);
    PL_SetWindowPos(-1, -1, 1280, 720);
    
    state->paddle.pos.x = 0.15f;
    state->paddle.pos.y = 0.15f;
    state->paddle.w = 0.02f;
    state->paddle.h = 0.15f;
    state->paddle.speed = 0.1f;
    
    state->ball.pos.x = 0.5f;
    state->ball.pos.y = 0.5f;
    state->ball.w = 0.02f;
    state->ball.h = 0.02f;
    state->ball.speed = 0.1f;
    
    bool BounceRectDown = 0;
    bool BounceRectRight = 0;
    int BounceRectSpeed = 1;
    int HighScore = 1;
    bool BounceBounced = 0;
    bool BounceBouncedPlayer = 0;
    bool BounceBouncedGoal = 0;
    
    SDL_Rect BoundaryTop = {0,0, Application.Dimension.w, 6};
    SDL_Rect BoundaryBot = {0, Application.Dimension.h-6, Application.Dimension.w, 6};
    SDL_Rect BoundaryRight = {Application.Dimension.w-6, 0, 6, Application.Dimension.h};
    SDL_Rect GoalLine = {0, 0, 10, Application.Dimension.h};
    
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
}

void PL_Frame(void)
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
        
}
