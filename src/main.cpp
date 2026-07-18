#include <glad/gl.h>
#include <SDL.h>
#include <cstdio>
#include <cstdlib>

#define PS2_ASSERT(cond, msg) \
   do { \
     if (!(cond)) { \
       printf("ASSERT FAILED: %s\n at %s:%d\n", \
           msg, __FILE__, __LINE__); \
       abort(); \
     } \
   } while (0)

#define LOG(msg) printf("%s\n", msg)

int main(int argc, char** argv) {
   if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      LOG(SDL_GetError());
      return 1;
   }

   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

   SDL_Window* window = SDL_CreateWindow(
      "ps2-engine",
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      1280, 720,
      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
   if (window == nullptr) {
      LOG(SDL_GetError());
      SDL_Quit();
      return 1;
   }

   SDL_GLContext glContext = SDL_GL_CreateContext(window);
   if (glContext == nullptr) {
     LOG(SDL_GetError());
     SDL_DestroyWindow(window);
     SDL_Quit();
     return 1;
   }

   if (gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress) == 0) { 
      LOG("Failed to load GL functions via glad");
      SDL_GL_DeleteContext(glContext); //teardown in reverse
      SDL_DestroyWindow(window);
      SDL_Quit();
      return 1;
   }

   LOG((const char*)glGetString(GL_VERSION));

   SDL_GL_SetSwapInterval(1);

   bool running = true;
   while (running) {
     SDL_Event event;
     while (SDL_PollEvent(&event)) {
       if (event.type == SDL_QUIT) {
         running = false;
       }
     }

     glClearColor(0.1f, 0.2f, 0.4f, 1.0f);
     glClear(GL_COLOR_BUFFER_BIT);

     SDL_GL_SwapWindow(window);
   }

   SDL_GL_DeleteContext(glContext);
   SDL_DestroyWindow(window);
   SDL_Quit();
   return 0;
}
