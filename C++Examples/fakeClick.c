/* fakeMouse.c */
#include <X11/extensions/XTest.h>
#include <unistd.h>
int main (){
  Display *dpy = NULL;
  XEvent event;
  dpy = XOpenDisplay (NULL);
  /* Get the current pointer position */
  XQueryPointer (dpy, RootWindow (dpy, 0), &event.xbutton.root,
   &event.xbutton.window, &event.xbutton.x_root,
   &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y,
   &event.xbutton.state);
 
  /* Fake the pointer movement to new relative position */
  XTestFakeMotionEvent (dpy, 0, event.xbutton.x ,
  event.xbutton.y, CurrentTime);
  XSync(dpy, 0);
 // sleep(3);
 
  /* Fake the pointer movement to new absolate position */
  //XTestFakeMotionEvent (dpy, 0, 250, 250, CurrentTime);
 // sleep(3);
 //sleep(3);
  /* Fake the mouse button Press and Release events */
  //XTestFakeButtonEvent (dpy, 1, True,  CurrentTime);
sleep(3);
  XTestFakeButtonEvent (dpy, 1, False, CurrentTime);
  XCloseDisplay (dpy);
  return 0;
}
// gcc fakeMouse.c -o fakeMouse -lX11 -lXtst -lXext
