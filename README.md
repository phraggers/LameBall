# LameBall

I accidentally made this game while testing some code.

It's kind of a cross between Pong and Breakout, mixed with Squash/Racquetball.

[>> DOWNLOAD <<](https://github.com/phraggers/LameBall/releases/download/v0.2/LameBall_v0.2.rar)

Your paddle is on the left on the screen.
Don't let the ball (red cube) hit the Goal (maroon bar)
- You gain a point when you defend the goal
- You lose a point when the ball hits the goal
- Every 10 points is a checkpoint
(You don't lose points when at a checkpoint)
- The big grey number is current score
- The little green number is best score
- The higher your score the faster the ball goes

![alt text](https://raw.githubusercontent.com/phraggers/LameBall/main/LameBallDemo.gif)

## Controls:
   CONTROL   | KEYBOARD | CONTROLLER | MOUSE
-------------|----------|------------|------------
Up           |  W/UP    | Dpad Up    | Pointer
Down         |  S/DOWN  | Dpad Down  | Pointer
Boost        |  Shift   | (Todo)     | LeftClick
Finesse      |  Ctrl    | (Todo)     | RightClick
Toggle Mouse |  F1      |  (N/A)     | (N/A)
Fullscreen   |  F11     |  (N/A)     | (N/A)

Press F1 (Toggle Mouse) if you aren't using Mouse
otherwise the paddle follows the mouse cursor

## Update Notes
Still a work in progress, it's a little buggy
DON'T resize the window, it does weird things

### v0.3
- Added Toggle Fullscreen (Press F11)
- Added Toggle Disable Mouse (Press F1)
- Updated Keydown detection to allow system-default Keydown repeat delay
- Increased audio buffer size (Accidentally cut it too short before)

### v0.2
- Changed all C++ code to pure C
- Added limit to audio buffer to prevent infinitely increasing buffer queue

### v0.1
- Initial commit
