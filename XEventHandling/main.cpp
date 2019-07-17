#include <iostream>

#include <stdio.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/extensions/XTest.h>
#include <X11/extensions/XInput.h>


struct Point{
              int x;
              int y;
             }p1,p2;
int main()
{
 Display *dpy = XOpenDisplay(0);

 char *window_name = (char*)"Drawing window";

 int whiteColor = WhitePixel(dpy, DefaultScreen(dpy));

 Window parent = XCreateSimpleWindow(dpy, RootWindow(dpy, DefaultScreen(dpy)), 0, 0,
                     230, 150, 0, whiteColor, whiteColor);

 XStoreName(dpy, parent, window_name);

 XMapWindow(dpy, parent);

 XSelectInput(dpy, parent, ButtonPressMask| PointerMotionMask| LeaveWindowMask| EnterWindowMask| ButtonReleaseMask );

 Drawable d = parent;

 XGCValues values;

 values.line_width = 4;

 values.line_style = LineSolid;

 GC gc = XCreateGC(dpy, d, GCLineWidth, &values);

 int flag = 0;

 for(;;) {
   XEvent e;
   XNextEvent(dpy, &e);
   if(e.xany.window == parent)
     {
       switch(e.type)
        {
         case EnterNotify: printf("\n POINTER Entered in Window");
                           break;
         case LeaveNotify: printf("\n POINTER Left Window");
                           break;
         case ButtonPress:if(flag == 0 && e.xbutton.button==1)
                          {
                          printf("\n Button %d Pressed at (%d, %d)", e.xbutton.button, e.xbutton.x, e.xbutton.y);
                          p1.x = e.xbutton.x;
                          p1.y = e.xbutton.y;
                          flag =1;
                          }
                          break;
         case ButtonRelease:if(flag == 1 && e.xbutton.button == 3)
                          {
                           printf("\n Button %d Pressed at (%d, %d)", e.xbutton.button, e.xbutton.x, e.xbutton.y);
                           flag =0;
                           XUnmapWindow(dpy, parent);
                           XMapRaised(dpy, parent);
                           XMoveWindow(dpy, parent, 0, 100);
                          }
                          break;
        case MotionNotify: if(flag ==1)
                          {
                           printf("\n Motion Notified at (%d, %d)", e.xbutton.button, e.xbutton.x, e.xbutton.y);
                           p2.x = e.xbutton.x;
                           p2.y = e.xbutton.y;
                           XDrawLine(dpy, parent, gc, p1.x, p1.y, p2.x, p2.y);
                           p1.x = p2.x;
                           p1.y = p2.y;
                           }
                           break;
               }
          }
  }
    return 0;

}
