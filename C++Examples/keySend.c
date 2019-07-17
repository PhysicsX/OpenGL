#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>

#define XF86AudioPlay 0x1008ff14
#define XF86AudioNext 0x1008ff17
#define XF86AudioPrev 0x1008ff16
#define XF86AudioStop 0x1008ff15
// gcc -Wall -Wextra -o mm keySend.c -lX11 -lXtst
int main(int argc, char **argv) {
    unsigned int key;
    unsigned int keycode;

 /*   // Ensure we have an argument
    if(argc != 2) return 1;

    // Determine what key to simulate
    if(strcmp(argv[1], "play") == 0 || strcmp(argv[1], "pause") == 0) {
        key = XF86AudioPlay;
    } else if(strcmp(argv[1], "stop") == 0) {
        key = XF86AudioStop;
    } else if(strcmp(argv[1], "next") == 0) {
        key = XF86AudioNext;
    } else if(strcmp(argv[1], "prev") == 0) {
        key = XF86AudioPrev;
    } */
sleep(3); // 0xff57 -> end
	key = 0x22; 
    // Connect to X
    Display *display;
    display = XOpenDisplay(NULL);

    // Get the keycode
    keycode = XKeysymToKeycode(display, key);
 //   printf("Simulating keycode %d press\n", keycode);
//
    // Simulate the keypress
    XTestFakeKeyEvent(display, keycode, 1, 0);
    // Release the key
    XTestFakeKeyEvent(display, keycode, 0, 0);
/*	key = 0xff0d;
    keycode = XKeysymToKeycode(display, key);
    printf("Simulating keycode %d press\n", keycode);

    // Simulate the keypress
    XTestFakeKeyEvent(display, keycode, 1, 0);
    // Release the key
    XTestFakeKeyEvent(display, keycode, 0, 0);
*/


    // Clear the X buffer which actually sends the key press
    XFlush(display);

    // Disconnect from X
    XCloseDisplay(display);

    return 0;
}
