#ifndef TIP_WIDGET_H
#define TIP_WIDGET_H

#include <FL/Fl_Menu_Window.H>
#include <FL/Fl_Tooltip.H>
#include <FL/fl_draw.H>
#include <FL/Fl.H>
#include <stdio.h>
#include <string.h>

class Tip_widget : public Fl_Menu_Window {
    char tip[40];
public:
    Tip_widget():Fl_Menu_Window(1,1) {      // will autosize
        strcpy(tip, "X.XX");
        set_override();
        end();
    }
    void draw() {
        draw_box(FL_BORDER_BOX, 0, 0, w(), h(), Fl_Color(175));
        fl_color(FL_BLACK);
        fl_font(labelfont(), 10.);
        fl_draw(tip, 3, 3, w()-6, h()-6, Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_WRAP));
    }
    void value(float f, float g) {
        snprintf(tip, 39, "%.2f/%.2f", f, g);
        // Recalc size of window
        fl_font(labelfont(), 10.);
        int W = w(), H = h();
        fl_measure(tip, W, H, 0);
        W += 8;
        size(W, H);
        redraw();
    }
};

#endif
