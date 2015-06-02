# Screen
A simple interface for SDL graphics

## Screen class
Instantiate a Screen and use its drawing functions to draw graphics. Each Screen
that you instantiate corresponds to a single window. You can have multiple
Screens instantiated at a single time.

### Creating a Screen

To create instantiate a Screen:

    Screen<SOFT, CLIPPED> scr(int size_x, int size_y, const char *name, bool full_screen, bool vsync);

The parameters are as follows:

SOFT - A boolean that should be true if you want to run the software renderer
CLIPPED - A boolean that should be true if you want your draw calls to be
          clipped (set it to true to be safe)
name - The name for the window that the Screen will draw to
(All other parameters should be self-explanatory)

To simplify instantiation, the following typedefs are provided:

    // A clipped software screen
    typedef Screen<true, true> SoftScreen;
    // A software screen that isn't clipped
    typedef Screen<true, false> PerfSoftScreen;
    // A clipped hardware screen
    typedef Screen<false, true> HardScreen;

The hardware renderer can't write out its buffer to a file as the software
renderer can. In addition, the hardware renderer is likely to be slower if you
only ever make calls to draw individual pixels. To make the most out of the
hardware renderer, use the more abstract drawing functions.

### Drawing with the Screen

Once you have a Screen scr, call the drawing functions on it write to a
back-buffer:

    scr.cls(); // Fill all pixels with black
    scr.set_color(255, 0, 0); // Sets the default color to red
    scr.draw_pixel(100, 100); // Sets the pixel (100, 100) to the default color
    scr.draw_pixel(200, 100, {0, 0, 255}); // Sets the pixel (200, 100) to blue

This will not immediately show up on your computer's screen. In order to show
the contents of the back-buffer on your screen, use the commit function:

    scr.commit(); // scr's contents are now visible

Because the Screen is double-buffered, you should draw to the whole screen
before every commit. If you don't all pixels that weren't drawn to will flip
back to the state they were on the frame before the last commit.

## Authors

Zack Hricz
    
