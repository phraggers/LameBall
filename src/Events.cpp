/* ========================================================================
   $File:    Events [Source] $
   $Project: LameBall $
   $Date:    09-09-2021 $
   $Author:  Phil Bagshaw $
   $Notice:  (c)Phragware 2021 $
   ======================================================================== */

internal bool
LAM_Keydown(SDL_Keycode Key)
{
    for(int iKeydown = 0; iKeydown < KEYDOWN_COUNT; ++iKeydown)
    {
        if(Application.Keydowns[iKeydown] == Key)
        {
            return true;
        }
    }

    return false;
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
              Application.Running = false;
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
                    Application.Running = false;
                } break;
                
                case SDL_WINDOWEVENT_EXPOSED:
                {
                    Application.Exposed = true;
                    Application.Minimized = false;
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
                    Application.Exposed = false;
                    Application.Minimized = true;
                } break;

                case SDL_WINDOWEVENT_MAXIMIZED:
                {
                    Application.Maximized = true;
                } break;

                case SDL_WINDOWEVENT_RESTORED:
                {
                    Application.Minimized = false;
                    Application.Maximized = false;
                } break;

                case SDL_WINDOWEVENT_ENTER:
                {
                    Application.MouseInWindow = true;
                } break;

                case SDL_WINDOWEVENT_LEAVE:
                {
                    Application.MouseInWindow = false;
                } break;

                case SDL_WINDOWEVENT_FOCUS_GAINED:
                {
                    Application.KeyboardFocus = true;
                } break;

                case SDL_WINDOWEVENT_FOCUS_LOST:
                {
                    Application.KeyboardFocus = false;
                } break;

                case SDL_WINDOWEVENT_CLOSE:
                {
                    Application.Running = false;
                } break;
              }
          } break;

          case SDL_KEYDOWN:
          {
              bool KeydownExists = false;

              for(int iKeydown = 0; iKeydown < KEYDOWN_COUNT; ++iKeydown)
              {
                  if(Application.Keydowns[iKeydown] == Event.key.keysym.sym)
                  {
                      KeydownExists = true;
                      break;
                  }
              }

              if(!KeydownExists)
              {
                  for(int iKeydown = 0; iKeydown < KEYDOWN_COUNT; ++iKeydown)
                  {
                      if(Application.Keydowns[iKeydown] == 0)
                      {
                          Application.Keydowns[iKeydown] = Event.key.keysym.sym;
                          break;
                      }
                  }
              }
          } break;

          case SDL_KEYUP:
          {
              for(int iKeydown = 0; iKeydown < KEYDOWN_COUNT; ++iKeydown)
              {
                  if(Application.Keydowns[iKeydown] == Event.key.keysym.sym)
                  {
                      Application.Keydowns[iKeydown] = 0;
                      break;
                  }
              }
              
          } break;

          case SDL_MOUSEMOTION:
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
