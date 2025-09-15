#include <stdio.h>

#ifdef TETHER_DEBUG
    #define DEBUG_PRINT(x) printf("DEBUG: %s\n", x)
#else
    #define DEBUG_PRINT(x)
#endif

int main(int argc, char** argv) {
    DEBUG_PRINT("Tether runtime starting");
    
    #ifdef TETHER_PLATFORM_WINDOWS
        printf("Running on Windows\n");
    #elif defined(TETHER_PLATFORM_MACOS)
        printf("Running on macOS\n"); 
    #elif defined(TETHER_PLATFORM_LINUX)
        printf("Running on Linux\n");
    #endif
    
    printf("Tether Language Runtime v0.1.0\n");
    return 0;
}