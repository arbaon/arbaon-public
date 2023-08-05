#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION true
#endif
#include <GLFW/glfw3.h>
#include <iostream>
void controls(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, GL_TRUE);
}
GLFWwindow *initWindow(const int SW, const int SH)
{
    if (!glfwInit())
        return NULL;
    GLFWwindow *window = glfwCreateWindow(SW, SH, "Change Me", NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, controls);
    return window;
}
void display(GLFWwindow *window)
{
    while (!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
int main(int argc, char **argv)
{
    GLFWwindow *window = initWindow(800, 600);
    if (NULL != window)
        display(window);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}