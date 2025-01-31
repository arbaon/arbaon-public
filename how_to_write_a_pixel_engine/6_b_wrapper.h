#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION true
#endif
#include <GLFW/glfw3.h>
#include <iostream>
// a dynamic array
template <typename A>
A *DynArray(int w, int h)
{
    A *a = new A[w * h];
    return a;
}
template <typename B>
// sprite or texture surface
struct surface
{
    int w, h, m, tw, th;
    bool state;
    B *b;
    void create(int w1, int h1, int tx = 1, int ty = 1)
    {
        b = DynArray<B>(w1, h1);
        w = w1, h = h1, m = w1 * h1;
        tw = w1 / tx, th = h1 / ty;
        state = true;
    }

    void clear(B c = 0)
    {
        for (int i = 0; i < m; i++)
            b[i] = c;
    }
};
// pixels class, all of this class could be put in a header file
class pixels
{
public:
//everything is public - change as you see fit
    GLFWwindow *window;
    surface<uint32_t> screen;
    GLuint textures[2];
    char title_string[30], wintitle[15];
    bool framecap, showfps = true;
    double fps;
    pixels(int W, int H, int bW, int bH, const char *title, bool cap);
    ~pixels();
    static void kbcontrols(GLFWwindow *window, int key, int scancode, int action, int mods);
    GLFWwindow *initWindow(const int SW, const int SH, const char *TITLE);
    void Display();
    void GenTexture(uint32_t *s, GLuint v, int W, int H);
    void UpdateTexture(uint32_t *s, int screenW, int screenH);
    void DrawQuad(float x, float y, float z);
    void Pixel(int x, int y,uint32_t p, surface<uint32_t> *s);
    void Block(int x1, int y1, int x2, int y2, uint32_t p, surface<uint32_t> *s);
    virtual void userupdate(){};
    virtual void userinit(){};
};
//constructor
pixels::pixels(int W, int H, int bW, int bH, const char *title, bool cap)
{
    framecap = cap;
    sprintf(wintitle, "%s", title);
    window = initWindow(W, H, title);
    screen.create(bW, bH);
    screen.clear();
}
//deconstructor
pixels::~pixels()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}
//update the quad texture
void pixels::UpdateTexture(uint32_t *s, int screenW, int screenH)
{
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenW, screenH, 0, GL_RGBA, GL_UNSIGNED_BYTE, s);
}
//generate the texture
void pixels::GenTexture(uint32_t *s, GLuint v, int W, int H)
{
    glGenTextures(1, &textures[v]);                                    // generate a texture from the given id
    glBindTexture(GL_TEXTURE_2D, textures[v]);                         // bind the texture to that id
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // once
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // once
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);       // once
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);       // once
    UpdateTexture(s, W, H);                                            // once for now.
}
//draw the quad
void pixels::DrawQuad(float x, float y, float z)
{
    glLoadIdentity(); // Reset the view
    glTranslatef(x, y, z);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);
    glEnd();
}
//escape to quit!
void pixels::kbcontrols(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, GL_TRUE);
}
//setup the window
GLFWwindow *pixels::initWindow(const int SW, const int SH, const char *TITLE)
{
    if (!glfwInit())
        return NULL;
    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow *window = glfwCreateWindow(SW, SH, TITLE, NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);
    if (!framecap)
        glfwSwapInterval(0); // lockout the 60fps framecount
    glfwSetKeyCallback(window, kbcontrols);

    glEnable(GL_TEXTURE_2D);                     // Enable 2D texture
    GenTexture(screen.b, 0, screen.w, screen.h); // generate a texture using the screen array with the green values
    return window;
}
//run the display
void pixels::Display()
{
    double t1 = glfwGetTime(), t2;
    int frames; // frame counter
    char title_string[20];
    GLint windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    userinit();
    while (!glfwWindowShouldClose(window))
    {
        t2 = glfwGetTime();
        if ((t2 - t1) > 1.0 || frames == 0)
        {
            fps = (double)frames / (t2 - t1);
            if (showfps)
            {
                sprintf(title_string, " %s FPS: %.1f", wintitle, fps);
                glfwSetWindowTitle(window, title_string);
            }
            t1 = t2, frames = 0;
        }
        frames++;
        userupdate();
        UpdateTexture(screen.b, screen.w, screen.h); // update the texture
        DrawQuad(0, 0, 0);                           // draw the quad
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
void pixels::Pixel(int x, int y, uint32_t p, surface<uint32_t> *s)
{
    s->b[(y * s->w) + x] = p;
}
void pixels::Block(int x1, int y1, int x2, int y2, uint32_t p, surface<uint32_t> *s)
{
    for (int x = x1; x < x2; x++)
        for (int y = y1; y < y2; y++)
            Pixel(x, y, p, s);
}
    