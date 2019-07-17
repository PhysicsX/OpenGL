/**
 * original file leeched from somewhere.
 * only got keycodes changes.
 * improved and added keycode to string translations.
 *
 * nir tzachar, 27/8/2003
 *
 * leeched from nir tzachar's account.
 * removed busy waiting via select(), better output
 * upgraded to C++
 *
 * michael orlov - Aug 27, 2003
 *
 * Compile:
 *  g++ -ansi -Wall -pedantic -O3 -o realkey realkey.C -L/usr/X11R6/lib -lX11
 *    or
 *  icc -ansi -Wall -w1 -O3 -o realkey realkey.C -L/usr/X11R6/lib -lX11
 *
 *  strip -s -R .comment -R .note -R .note.ABI-tag realkey
 */

#include <iostream>
#include <algorithm>
#include <string>
#include <map>
#include <set>
#include <cstdlib>
#include <csignal>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace {
  bool cont = true;             // flag for while loop

  Display *display = 0;         // X server connection

  std::string key_map[256];       // key names
  std::string key_map_upper[256]; // key names, upper case

  std::set<int> caps_set, shift_set, ctrl_set, alt_set, meta_set;
  
  void
  quitter(int sig)
  {
    cont = false;
  }

  void fill_mappings()
  {
    /*
     * shorten names of some keys
     */
    const char *const names_map_raw[] = {
      "Caps_Lock",      "",
      "Control_L",      "",
      "Control_R",      "",
      "Shift_L",        "",
      "Shift_R",        "",
      "Alt_L",          "",
      "Alt_R",          "",
      "Meta_L",         "",
      "Meta_R",         "",
      "ISO_Prev_Group", "",
      "ISO_Next_Group", "",
  
      "Home",         "Nav",
      "Up",           "Nav",
      "Prior",        "Nav",
      "Left",         "Nav",
      "Right",        "Nav",
      "End",          "Nav",
      "Down",         "Nav",
      "Next",         "Nav",
      "Insert",       "Nav",
      "Delete",       "Nav",
      "KP_Home",      "Nav",
      "KP_Up",        "Nav",
      "KP_Prior",     "Nav",
      "KP_Left",      "Nav",
      "KP_Begin",     "Nav",
      "KP_Right",     "Nav",
      "KP_End",       "Nav",
      "KP_Down",      "Nav",
      "KP_Next",      "Nav",
      "KP_Insert",    "Nav",
      "KP_Delete",    "Nav",
  
      "Return",       "\n",
      "KP_Enter",     "\n",
      "Escape",       "Esc",
      "BackSpace",    "Bsp",

      "exclam",       "!",
      "at",           "@",
      "numbersign",   "#",
      "dollar",       "$",
      "percent",      "%",
      "asciicircum",  "^",
      "ampersand",    "&",
      "asterisk",     "*",
      "parenleft",    "(",
      "parenright",   ")",
      "minus",        "-",
      "underscore",   "_",
      "equal",        "=",
      "plus",         "+",
      "bracketleft",  "[",
      "bracketright", "]",
      "braceleft",    "{",
      "braceright",   "}",
      "semicolon",    ";",
      "colon",        ":",
      "apostrophe",   "'",
      "quotedbl",     "\"",
      "grave",        "`",
      "asciitilde",   "~",
      "backslash",    "\\",
      "bar",          "|",
      "comma",        ",",
      "less",         "<",
      "greater",      ">",
      "period",       ".",
      "slash",        "/",
      "question",     "?",
      "space",        " ",
      "KP_0",         "0",
      "KP_1",         "1",
      "KP_2",         "2",
      "KP_3",         "3",
      "KP_4",         "4",
      "KP_5",         "5",
      "KP_6",         "6",
      "KP_7",         "7",
      "KP_8",         "8",
      "KP_9",         "9",
      "KP_Multiply",  "*",
      "KP_Subtract",  "-",
      "KP_Add",       "+",
      "KP_Decimal",   ".",
      "KP_Divide",    "/"
    };

    std::map<std::string, std::string> names_map;

    for (unsigned i = 0; i < sizeof(names_map_raw)/sizeof(const char *); i += 2)
      names_map[names_map_raw[i]] = names_map_raw[i+1];

    /*
     * fill up keyboard mapping
     */
    int min_key_code, max_key_code; // key codes range
    XDisplayKeycodes(display, &min_key_code, &max_key_code);
  
    for (int code = min_key_code; code <= max_key_code; ++code) {
      const char *keysym  = XKeysymToString(XKeycodeToKeysym(display, code, 0));
      key_map[code]       = keysym ? keysym : "NoSymbol";

      keysym              = XKeysymToString(XKeycodeToKeysym(display, code, 1));
      key_map_upper[code] = keysym ? keysym : "NoSymbol";

      if (key_map[code] == "Caps_Lock")
        caps_set.insert(code);
      else if (key_map[code] == "Shift_L"   || key_map[code] == "Shift_R")
        shift_set.insert(code);
      else if (key_map[code] == "Control_L" || key_map[code] == "Control_R")
        ctrl_set.insert(code);
      else if (key_map[code] == "Alt_L"     || key_map[code] == "Alt_R")
        alt_set.insert(code);
      else if (key_map[code] == "Meta_L"    || key_map[code] == "Meta_R")
        meta_set.insert(code);

      std::map<std::string,std::string>::const_iterator loc
        = names_map.find(key_map[code]);
      if (loc != names_map.end())
        key_map[code] = loc->second;

      loc = names_map.find(key_map_upper[code]);
      if (loc != names_map.end())
        key_map_upper[code] = loc->second;
    }
  }
}

int
main(int argc, const char *const *argv)
{
  // sleep time between keyboard queries (1 ms)
  const timespec sleeptime = { 0, 1000000 };

  // disable C/C++ streams synchronization
  std::ios_base::sync_with_stdio(false);

  /*
   * register signal handler
   */
  if (std::signal(SIGINT, &quitter) == SIG_ERR
      || std::signal(SIGTERM, &quitter) == SIG_ERR) {
    std::cerr << "Could not register signal handlers.\n";
    std::exit(1);
  }

  /*
   * check number of arguments
   */
  if (argc != 2) {
    std::cerr << "Format: " << argv[0] << " display-spec\n";
    std::exit(1);
  }
  
  /*
   * open the display
   */
  if (! (display = XOpenDisplay(argv[1]))) {
    std::cerr << "Can't open display " << argv[1] << '\n';
    std::exit(1);
  }

  /*
   * fill up keyboard mapping
   */
  fill_mappings();

  /*
   * fill relevant key buffers
   */
  char keys[32];                  // buffer for reading keys in
  char lastkeys[32];              // previous keys state  

  std::fill(lastkeys, lastkeys + sizeof(lastkeys), 0);
		
  /*
   * query keyboard in loop
   */
  bool last_is_nav  = false;    // navigation key indicator
  bool last_is_char = false;    // spaces adjustment
  
  while (cont) {
    nanosleep(&sleeptime, 0);   // avoid busy waiting
    
    XQueryKeymap(display, keys);

    // read modifiers (caps lock is ignored)
    bool shift = false;
    bool ctrl  = false;
    bool alt   = false;
    bool meta  = false;

    for (unsigned i = 0; i < sizeof(keys); ++i)
      for (unsigned j = 0, test = 1; j < 8; ++j, test *= 2)
        if (keys[i] & test) {
          const int code = i*8+j;

          if (shift_set.find(code) != shift_set.end())
            shift = true;

          if (ctrl_set.find(code) != ctrl_set.end())
            ctrl = true;

          if (alt_set.find(code) != alt_set.end())
            alt = true;

          if (meta_set.find(code) != meta_set.end())
            meta = true;
        }
    
    // print changed keys
    for (unsigned i = 0; i < sizeof(keys); ++i)
      if (keys[i] != lastkeys[i]) {
        // check which key got changed
        for (unsigned j = 0, test = 1; j < 8; ++j, test *= 2)
          // if the key was pressed, and it wasn't before, print this
          if ((keys[i] & test) && 
              ((keys[i] & test) != (lastkeys[i] & test))) {
            const int code = i*8+j;
            std::string key = key_map[code];
            const bool key_is_nav = (key == "Nav");

            // only print navigation keys once
            if (! (last_is_nav && key_is_nav) && key.size() > 0) {
              // change key according to modifiers
              if (! key_is_nav) {
                if (shift)
                  key = key_map_upper[code];

                if (meta)
                  key = "M-" + key;

                if (alt)
                  key = "A-" + key;

                if (ctrl)
                  key = "C-" + key;
              }
              
              switch (key.size()) {
              case 1:
                std::cout << key << std::flush;
                last_is_char = (key != "\n");
                
                break;

              default:
                if (last_is_char) {
                  std::cout << ' ';
                  last_is_char = false;
                }
                
                std::cout << '[' << key << "] " << std::flush;
                break;
              }
              
              last_is_nav = key_is_nav;
            }
          }
      
        lastkeys[i] = keys[i];
      }
  }

  XCloseDisplay(display);
  std::cout << std::endl;
}
