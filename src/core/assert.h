#pragma once

#include <cstdio>
#include <cstdlib>

#ifdef NDEBUG
  // Release build (NDEBUG defined): the assert compiles to nothing.
  // The condition is NOT evaluated here, so PS2_ASSERT must never wrap an
  // expression whose side effects the program actually depends on.
  #define PS2_ASSERT(cond, msg) ((void)0)
#else
  // Debug build: check the condition and die loudly at the call site.
  #define PS2_ASSERT(cond, msg) \
     do { \
       if (!(cond)) { \
         printf("ASSERT FAILED: %s\n at %s:%d\n", \
             msg, __FILE__, __LINE__); \
         abort(); \
       } \
     } while (0)
#endif
