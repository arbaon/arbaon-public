#include <ncurses/ncurses.h>
#include <locale.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
/******************************************************   defs  **********************************************************/
#define FPS 0.02f //50 frames per sec
#define XMIN 3    //min player x movement
#define XMAX 54   // max player x movement
#define XSWARM 14 //starting pos
#define YSWARM 6
#define XPLAY 15 //starting pos
#define YPLAY 29
#define XBAR 6
#define YBAR 22
#define MASTERSPEED 40

/****************************************************** globals **********************************************************/

struct timespec t1 = {0, 0}, t2 = {0, 0}, ts = {1, 0}; //time comparisons
time_t rdseed;                                         //seed for rand()
float delta = FPS;                                     //frame countdown
short invaders[55];                                    //invader array
bool invcol[11];                                       //invader columns
bool barracks[240];                                    //protective house
bool barrow[5];                                        // lines of protective houses
int score = 0, lives = 3, highscore = 0;

/************************************************* structs for objects ****************************************************/

typedef struct // x & y
{
    int x;
    int y;
} vec;
typedef struct //a & b button
{
    bool a;
    bool b;
} button;
typedef struct
{
    vec pos;
    button fire;
} projectile;
typedef struct
{
    vec pos;
    bool active;
} object;

//vec landed={1,0};
/****************************************************** functions **********************************************************/

bool fpsLoop() //framerate 50fps
{
    clock_gettime(CLOCK_MONOTONIC, &t2);
    float fElapsedTime = ((double)t2.tv_sec + 1.0e-9 * t2.tv_nsec) - ((double)t1.tv_sec + 1.0e-9 * t1.tv_nsec);
    t1 = t2;
    delta = delta - fElapsedTime;
    if (delta <= 0.0f)
    {
        delta = 0.02f;
        return true; //return true if countdown complete.
    }
    else
        return false;
}
void moveSwarm(vec *swarm, int *fx)
{
    vec tswarm = *swarm;
    for (int x = 0; x < 11; x++)
    {
        invcol[x] = invaders[x] | invaders[x + 11] | invaders[x + 22] | invaders[x + 33] | invaders[x + 44]; //check the columns
    }
    int lspace = 0, rspace = 0;
    for (int x = 0; x < 11; x++)
    {
        if (invcol[x] < 1)
            lspace += 3;
        else
            break;
    }
    for (int x = 10; x > -1; x--)
    {
        if (invcol[x] < 1)
            rspace += 3;
        else
            break;
    }

    if (*fx > 0)
    {
        if (tswarm.x < 26 + rspace)
        {
            swarm->x += *fx;
        }
        else
        {
            if (tswarm.y < 30)
            {
                swarm->y += 1;
                *fx = -1;
            }
        }
    }
    else
    {
        if (tswarm.x > (3 - lspace))
        {
            swarm->x += *fx;
        }
        else
        {
            if (tswarm.y < 30)
            {
                swarm->y += 1;
                *fx = 1;
            }
        }
    }
}
bool checkBarracks(int x, int y)
{
    bool barhit = false;
    int offsety;
    if (y > 21 && y < 27)
    {
        if (x > 5 && x < 54)
        {
            offsety = y - 22;
            if (barracks[(x - 6) + (offsety * 48)])
            {
                barracks[(x - 6) + (offsety * 48)] = false;
                barhit = true;
            }
        }
    }
    return barhit;
}
void crushBarracks(int y)
{
    int width = y * 48;
    for (int x = 0; x < 48; x++)
    {
        barracks[x + width] = false;
    }
    barrow[y] = false;
}
bool checkCollide(int y1, int x1, int y2, int x2, int x2size) //2=missile
{
    bool retval = false;
    if (x1 >= x2 && x1 < x2 + x2size)
    {
        if (y1 == y2)
            retval = true;
    }
    return retval;
}
void moveMissile(WINDOW *gamewin, vec *pos, button *fire, vec swarm, bool *mother, bool *bomb, vec motherv, vec bombv)
{
    vec next = *pos;
    int offsety, offsetx;
    bool barhit, ahit = false;
    if (next.y > 1)
    {
        next.y = next.y - 1;
        barhit = checkBarracks(next.x, next.y);
        if (next.y >= swarm.y && next.y < swarm.y + 9)
        {
            if (next.x >= swarm.x && next.x < swarm.x + 31)
            {
                int tmpy = next.y - swarm.y;
                int tmpx = next.x - swarm.x;
                int n1_tmpy = tmpy / 2;
                int n1_tmpx = tmpx / 3;
                int n2_tmpy = tmpy % 2;
                int n2_tmpx = tmpx % 3;
                if (n2_tmpy == 0 && n2_tmpx == 0)
                {
                    offsety = n1_tmpy + n2_tmpy;
                    offsetx = n1_tmpx + n2_tmpx;
                    if (invaders[offsetx + (offsety * 11)])
                    {
                        invaders[offsetx + (offsety * 11)] = false;
                        score += (5 - offsety) * 10;
                        barhit = true;
                    }
                }
            }
        }
        //test barracks..
        if (bomb)
        {
            ahit = checkCollide(next.y, next.x, bombv.y, bombv.x, 1);
            if (ahit)
            {
                *bomb = false;
                barhit = true;
            }
        }
        //test bomb
        if (mother)
        {
            ahit = checkCollide(next.y, next.x, motherv.y, motherv.x, 3);
            if (ahit)
            {
                *mother = false;
                score+=100;
                barhit = true;
            }
        }
    }
    else
    {
        barhit = true;
    }
    if (barhit)
    {
        fire->a = false;
        fire->b = false;
    }
    else
    {
        pos->y -= 1;
    }
}
void moveBomb(vec *pos, vec player, bool *bomb, bool *playhit)
{
    bool barhit = false;
    vec next = *pos;
    next.y++;
    if (next.y > 1 && next.y < 30)
    {
        if (checkBarracks(next.x, next.y))
            barhit = true;
        if (checkCollide(next.y, next.x, player.y, player.x, 3))
        {
            barhit = true;
            *playhit = true;
        }
    }
    else
    {
        barhit = true;
    }
    if (barhit)
    {
        *bomb = false;
    }
    else
    {
        pos->y++;
    }
}
bool randMotherShip()
{
    srand((unsigned)time(&rdseed));
    int mum = rand() % 10; //10
    if (mum == 5)         //5
    {
        return true;
    }
    return false;
}
int randBomb(int defeated)
{
    //bomb test..
    int dropb = rand() % defeated;
    int icount = 0, ival = 0;
    for (int i = 0; i < 55; i++)
    {
        if (invaders[i] > 0)
        {
            if (icount == dropb)
                ival = i;
            icount++;
        }
    }
    return ival;
}
void dropBomb(int defeated, vec swarm, vec *bomber, bool *droper)
{
    vec vectest, bomb;
    bomb = *bomber;
    int btest = randBomb(defeated);
    vectest.y = btest / 11;
    vectest.x = btest % 11;
    if (vectest.x > 0)
        bomb.x = swarm.x + (vectest.x * 3);
    else
        bomb.x = swarm.x + vectest.x;
    if (vectest.y > 0)
        bomb.y = swarm.y + (vectest.y * 2);
    else
        bomb.y = swarm.y + vectest.y;
    bomber->x = bomb.x;
    bomber->y = bomb.y;
    *droper = true;
}
void drawSplash(WINDOW *gamewin, char *message1, char *message2)
{
    werase(gamewin);
    wattron(gamewin, (COLOR_PAIR(1) | A_BOLD));
    box(gamewin, 0, 0);
    mvwprintw(gamewin, 8, 25, "%s", message1);
    mvwprintw(gamewin, 10, 27, "%s", message2);
    wattroff(gamewin, A_BOLD);
    refresh();
    wrefresh(gamewin);
    nanosleep(&ts, NULL);
}
void drawSprite(WINDOW *gamewin, int y, int x, int T)
{
    char *sprt;
    switch (T)
    {
    case 1:
        wattron(gamewin, (COLOR_PAIR(2) | A_BOLD));
        sprt = "_|_";
        break;
    case 2:
        wattron(gamewin, (COLOR_PAIR(1) | A_BOLD));
        sprt = "|";
        break;
    case 3:
        wattron(gamewin, (COLOR_PAIR(1) | A_BOLD));
        sprt = "T";
        break;
    case 4:
        wattron(gamewin, (COLOR_PAIR(3) | A_BOLD));
        sprt = "<o>";
        break;
    case 10:
        wattron(gamewin, (COLOR_PAIR(3) | A_BOLD));
        sprt = "H";
        break;
    case 11:
        wattron(gamewin, (COLOR_PAIR(2) | A_BOLD));
        sprt = "A";
        break;
    case 12:
        wattron(gamewin, (COLOR_PAIR(4) | A_BOLD));
        sprt = "X";
        break;
    case 13:
        wattron(gamewin, (COLOR_PAIR(5) | A_BOLD));
        sprt = "O";
        break;
    case 14:
        wattron(gamewin, (COLOR_PAIR(1) | A_BOLD));
        sprt = "R";
        break;
    default:
        break;
    }
    mvwprintw(gamewin, y, x, "%s", sprt);
    wattroff(gamewin, A_BOLD);
}
void drawScore(WINDOW *gamewin)
{
    wattron(gamewin, (COLOR_PAIR(4) | A_BOLD)); //change the color..
    mvwprintw(gamewin, 1, 1, "SCORE : %d  LIVES : %d : HIGH SCORE : %d", score, lives, highscore);
    wattroff(gamewin, A_BOLD);
}
void drawInfo(WINDOW *gamewin)
{
    werase(gamewin);
    wattron(gamewin, (COLOR_PAIR(1) | A_BOLD));
    box(gamewin, 0, 0);
    drawScore(gamewin);
    drawSprite(gamewin,8,19,4);
    mvwprintw(gamewin, 8, 25, "=  x  100  BONUS");
    drawSprite(gamewin, 10,20,10);
    mvwprintw(gamewin, 10, 25, "=  x  40  POINTS");
    drawSprite(gamewin, 12,20,11);
    mvwprintw(gamewin, 12, 25, "=  x  40  POINTS");
    drawSprite(gamewin, 14,20,12);
    mvwprintw(gamewin, 14, 25, "=  x  30  POINTS");
    drawSprite(gamewin, 16,20,13);
    mvwprintw(gamewin, 16, 25, "=  x  20  POINTS");
    drawSprite(gamewin, 18,20,14);
    mvwprintw(gamewin, 18, 25, "=  x  10  POINTS");
    mvwprintw(gamewin, 24, 20, "PRESS 'S' TO START!");
    wattroff(gamewin, A_BOLD);
    refresh();
    wrefresh(gamewin);
    nanosleep(&ts, NULL);
    nanosleep(&ts, NULL);
}
void drawBarracks(WINDOW *gamewin)
{
    wattron(gamewin, COLOR_PAIR(2)); //change the color..
    int width = 0;
    for (int y = 0; y < 5; y++)
    {
        for (int x = 0; x < 48; x++)
        {
            if (barracks[x + width] == true)
            {
                mvwprintw(gamewin, y + YBAR, x + XBAR, "#");
            }
        }
        width += 48;
    }
}
vec drawSwarm(WINDOW *gamewin, vec swarm)
{
    vec icount = {55, 0};
    int width = 0, xoffset = 0, yoffset = 0;
    for (int y = 0; y < 5; y++)
    {
        for (int x = 0; x < 11; x++)
        {
            if (invaders[x + width])
            {
                if (swarm.y + y + yoffset < 31)
                    drawSprite(gamewin, swarm.y + y + yoffset, swarm.x + x + xoffset, 10 + y);
                if (swarm.y + y + yoffset > icount.y)
                    icount.y = swarm.y + y + yoffset;
            }
            else
            {
                icount.x--;
            }
            if (swarm.y + y + yoffset < 31)
                xoffset += 2;
        }
        xoffset = 0;
        yoffset += 1;
        width += 11;
    }
    return icount;
}
void initInvaders(WINDOW *gamewin, vec *rswarm, vec *rplayer, int wave)
{
    //reset invaders
    if (wave > 0)
        drawSplash(gamewin, "GET READY!", "");
    for (int i = 0; i < 55; i++)
        invaders[i] = true;
    for (int i = 0; i < 11; i++)
        invcol[i] = 1;

    for (int i = 0; i < 240; i++)
        barracks[i] = false;
    for (int i = 0; i < 5; i++)
        barrow[i] = 1;
    int count = 0;
    for (int i = 0; i < 5; i++)
    {
        for (int a = 0; a < 4; a++)
        {
            for (int b = 0; b < 6; b++)
            {
                switch (i)
                {
                case 0:
                    if (b > 0 && b < 5)
                        barracks[count] = true;
                    break;
                case 4:
                    if (b < 1 || b > 4)
                        barracks[count] = true;
                    break;
                default:
                    barracks[count] = true;
                    break;
                }
                count++;
            }
            if (a < 3)
                count += 8;
        }
    }
    rswarm->x = XSWARM;
    rswarm->y = YSWARM;
    rplayer->x = XPLAY;
    rplayer->y = YPLAY;
}
bool countLives()
{
    if (lives < 1)
    {
        if (score > highscore)
            highscore = score;
        score = 0;
        return true;
    }
    else
    {
        lives = lives - 1;
        return false;
    }
}
int main()
{
    //set the curses screen and keyboard options

    setlocale(LC_ALL, "");
    initscr();
    start_color();
    curs_set(0);
    noecho();
    raw();
    timeout(0);
    keypad(stdscr, true);

    bool gameEnd = false, gameTick = false;
    int wave = 0, fx = 1, choice;
    int invspeed = MASTERSPEED, invspeedcount = 1; //0
    int bombspeed = 6, bombspeedcount = 1;
    int misspeed = 1, misspeedcount = 1;
    int motherspeed = 10, motherspeedcount = 1;
    int waiting = 200,waitingcount=1;

    vec landed = {1, 0};

    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);

    WINDOW *gamewin = newwin(32, 60, 1, 2);
    button start = {0, 0};
    button dead = {0, 0}; //a by bomb, b by landing
    vec swarm;            //= {14,6}; //starting position of invaders
    vec player;           // = {15,29}; //player start
    projectile missile = {{0, 0}, {0, 0}};
    object bomb = {{0, 0}, false};
    object mother = {{0, 0}, false};

    initInvaders(gamewin, &swarm, &player, 0);

    while (!gameEnd)
    {
        gameTick = fpsLoop();
        if (start.b)
        {
            initInvaders(gamewin, &swarm, &player, wave);
            start.b = false;
        }
        if (gameTick)
        {
            werase(gamewin);
            wattron(gamewin, COLOR_PAIR(1));
            box(gamewin, 0, 0);
            drawScore(gamewin);
            if (!start.a)
                {
                if (waitingcount <= 0)
                    {
                        drawInfo(gamewin);
                        waitingcount = waiting;
                    }
                mvwprintw(gamewin, 18, 20, "PRESS 'S' TO START!");
                }
            if (missile.fire.a && missile.fire.b)
                drawSprite(gamewin, missile.pos.y, missile.pos.x, 2);
            if (bomb.active && start.a)
                drawSprite(gamewin, bomb.pos.y, bomb.pos.x, 3);
            if (mother.active)
                drawSprite(gamewin, mother.pos.y, mother.pos.x, 4);
            landed = drawSwarm(gamewin, swarm);
            drawSprite(gamewin, player.y, player.x, 1);
            drawBarracks(gamewin);
            refresh();
            wrefresh(gamewin);
            if (invspeedcount < 0 && start.a)
            {
                moveSwarm(&swarm, &fx);
                invspeedcount = invspeed;
            }
            if (landed.y >= YBAR && landed.y < YBAR + 5)
                crushBarracks(landed.y - YBAR); //aliens reach the defenses
            if (landed.y >= YBAR + 7)
                dead.b = true; //aliens landed

            if (missile.fire.a && missile.fire.b)
                if (misspeedcount < 0 && start.a)
                {
                    moveMissile(gamewin, &missile.pos, &missile.fire, swarm, &mother.active, &bomb.active, mother.pos, bomb.pos);
                    misspeedcount = misspeed;
                }
            if (!missile.fire.b && missile.fire.a)
            {
                missile.fire.b = true;
                missile.pos.x = player.x + 1;
                missile.pos.y = player.y;
            }
            if (!bomb.active)
                dropBomb(landed.x, swarm, &bomb.pos, &bomb.active); //start the bombing
            else
            {
                if (bombspeedcount < 0 && start.a)
                {
                    moveBomb(&bomb.pos, player, &bomb.active, &dead.a); //move the bomb find the casulties.
                    bombspeedcount = bombspeed;
                }
            }
            if (mother.active)
            {
                if (motherspeedcount < 0 )
                    if (mother.pos.x < 56)
                    {
                        mother.pos.x++;
                        motherspeedcount = motherspeed;
                    }
                    else
                        mother.active = false;
            }
            else
            {
                if (randMotherShip())
                {
                    mother.active = true;
                    mother.pos.x = 1;
                    mother.pos.y = 3;
                }
            }

            misspeedcount--;
            invspeedcount--;
            bombspeedcount--;
            motherspeedcount--;
            if (!start.a) waitingcount--;
        }

        choice = -1;
        choice = getch();
        switch (choice)
        {
        case KEY_UP:
            if (!missile.fire.a && start.a)
                missile.fire.a = true;
            break;
        case KEY_LEFT:
            if (player.x > XMIN && start.a)
                player.x--;
            break;
        case KEY_RIGHT:
            if (player.x < XMAX && start.a)
                player.x++;
            break;
        case 'q':
            gameEnd = true;
            break;
        case 's':
            if (!start.a)
            {
                start.a = true;
                wave = 1;
                start.b = true;
            }
            break;
        default:
            break;
        }
        if (dead.a || dead.b)
        {
            bool certDead = countLives();
            if (certDead)
            {
                drawSplash(gamewin, "GAME OVER", "");
                wave = 0;
                start.a = false;
                start.b = false;
                missile.fire.a = false;
                missile.fire.b = false;
                bomb.active = false;
                mother.active = false;
                lives = 3;
                initInvaders(gamewin, &swarm, &player, wave);
            }
            else
            {
                drawSplash(gamewin, "YOU DIED!", "_|_");
                drawSplash(gamewin, "GET READY", "");
                if (dead.b)
                {
                    swarm.y = swarm.y - 5;
                }
            }
            dead.a = false;
            dead.b = false;
        }
        if (landed.x <= 0)
        {
            drawSplash(gamewin, "CONGRATULATIONS!", "");
            wave++;
            initInvaders(gamewin, &swarm, &player, wave);
            drawSplash(gamewin, "GET READY!", "");
        }
        else
        {
                invspeed = 15+(landed.x/2)-wave;
        }
        
    }

    endwin();
    return 0;
}
