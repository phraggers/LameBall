# LameBall

I accidentally made this game.

It's kind of a cross between Pong and Breakout.

Your paddle is on the left on the screen.
Don't let the ball (red cube) hit the Goal (maroon bar)

## Controls:
CONTROL | KEYBOARD | CONTROLLER | MOUSE
---------------------------------------------
Up      |  W/UP    | Dpad Up    | Pointer
Down    |  S/DOWN  | Dpad Down  | Pointer
Boost   |  Shift   | (Todo)     | LeftClick
Finesse |  Ctrl    | (Todo)     | RightClick

Move mouse out of the window if you aren't using it
otherwise the paddle follows the mouse cursor
(Might make an option to toggle mouse input later)

## Score
- You gain a point when you defend the goal
- You lose a point when the ball hits the goal
- Every 10 points is a checkpoint
(You don't lose points when at a checkpoint)
The big grey number is current score
The little green number is best score

## NOTE
Still a work in progress, it's a little buggy
DON'T resize the window, it does weird things

There's a possible memory leak:
if the sound plays continually (More than single beep) just close the window!!!
