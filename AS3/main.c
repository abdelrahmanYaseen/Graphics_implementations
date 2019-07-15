#include <GLFW/glfw3.h>
// gcc -o app main.c -I/usr/indluce/libdrm -lglfw -lGL
int main(){
      GLFWwindow* win;
      glfwInit();
      win = glfwCreateWindow(800,600,"win",NULL,NULL);
      glfwMakeContextCurrent(win);

      while(!glfwWindowShouldClose(win)){
            glClear(GL_COLOR_BUFFER_BIT);
            glfwSwapBuffers(win);
            glfwPollEvents();      
      }
      glfwTerminate();
}
