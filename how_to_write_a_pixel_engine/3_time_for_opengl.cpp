#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION true
#endif
#include <GLFW/glfw3.h>
#include <iostream>

char winname[20];
void drawquad()
{
   glLoadIdentity();   // Reset the view
   glTranslatef(0.0f, 0.0f, 0.0f);   
   glBegin(GL_QUADS);
      glVertex3f(-0.8f, -0.8f,  0.0f);
      glVertex3f(0.8f, -0.8f,  0.0f);
      glVertex3f( 0.8f,  0.8f,  0.0f);
      glVertex3f(-0.8f,  0.8f,  0.0f);
   glEnd();
}
void controls(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, GL_TRUE);
}
GLFWwindow *initWindow(const int SW, const int SH,const char * TITLE)
{
    sprintf(winname, "%s", TITLE);
    if (!glfwInit()) return NULL;
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    GLFWwindow *window = glfwCreateWindow(SW, SH, winname, NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // lockout the 60fps framecount
    glfwSetKeyCallback(window, controls);
    return window;
}
void display(GLFWwindow *window)
{
    double t1 = glfwGetTime(), t2, fps; //The 1st time, and the 2nd time plus the final result
    int frames; //frame counter
    char title_string[20];
    //viewport GL - RED Screen
    GLint windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);
    glClearColor(255.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    while (!glfwWindowShouldClose(window))
    {
        t2 = glfwGetTime();
        if ((t2 - t1) > 1.0 || frames == 0) //how many frames are counted in 1 second between t1 & t2
        {
            fps = (double)frames / (t2 - t1);
            sprintf(title_string, "%s FPS: %.1f %1.f",winname,fps,t2);
            glfwSetWindowTitle(window, title_string);
            t1 = t2, frames = 0;
        }
        frames++;
        drawquad();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
int main(int argc, char **argv)
{
    GLFWwindow *window = initWindow(800, 600,"Testing");
    if (NULL != window)
        display(window);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

//bitwise operators
