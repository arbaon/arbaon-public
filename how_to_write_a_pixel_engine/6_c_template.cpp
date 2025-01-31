#include "./6_b_wrapper.h"

class engine : public pixels
{
public:
    engine(int W, int H, int bW, int bH, const char *title, bool cap) : pixels(W, H, bW, bH, title, cap) {}
    int x, y, vx, vy, tick;
    //custom one time initialise
    void userinit()
    {
        x = 10, y = 10, vx = 1, vy = 0, tick = 0;
    }
    //custom main update loop
    void userupdate()
    {
        //screen.clear(0xFF00FFFF);
        screen.clear(0xFF00FFFF);
        //Block(x, y, x + 20, y + 20, 0, &screen);
        Block(x,y,x+20,y+20,0,&screen);
        if (tick > 9) // slow down the animation
        {
            if (x > 369)
            {
                if (y < 11)
                    vy = 1, vx = 0;
                if (y > 269)
                    vy = 0, vx = -1;
            }
            if (x < 11)
            {
                if (y > 269)
                    vy = -1, vx = 0;
                if (y < 11)
                    vy = 0, vx = 1;
            }
            x += vx, y += vy;
            tick = 0;
        }
        tick++;
    }
};
// The main program
int main(int argc, char **argv)
{
    engine eng(800, 600, 400, 300, "Sample", false); // set the screen and display sizes
    if (NULL != eng.window)
    {
        eng.Display();
    }
    return 0;
}