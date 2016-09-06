/*
 * (c) 2016 Cedric PAILLE
 * Simple Spectrum Analyzer for RTL Dongle
 *
 */

#ifndef GRAPH_WIDGET_H
#define GRAPH_WIDGET_H

#include <FL/Fl_Gl_Window.H>
#include <vector>

typedef std::vector<float> FLT_BUF;

class Tip_widget;
class Fl_Scrollbar;

class Gl_graph_widget : public Fl_Gl_Window {
public:
    Gl_graph_widget(int X,int Y,int W,int H,Fl_Scrollbar* sb, const char*L=0);
    ~Gl_graph_widget();
    void reset();
    void set_brightness(double new_fg, double new_bg);
    void set_data_window(float startx, float stopx, float starty, float stopy);
    FLT_BUF& get_current_buffer(){return *m_data_buffer;}
    void set_buffer(FLT_BUF* buffer){delete m_data_buffer;m_data_buffer = buffer;redraw();}
    static void scroll_callback(Fl_Widget*, void *);
    void set_fill(bool on){m_fill_under = on; redraw();}
    bool get_selection_bounding_box(float &x0, float&y0, float &x1, float &y1);
    float get_power_at_cursor();
private:
    void get_mouse_coordinates(float& x, float& y);
	double fg;                       // foreground brightness
    double bg;                       // background brightness
    Tip_widget *m_tooltip;
    Fl_Scrollbar *m_scroll_bar;

    int 	m_grid_div_x, m_grid_div_y;
    int 	m_width, m_height;
    float 	m_start_x, m_stop_x;
    float 	m_start_y, m_stop_y;
    float 	m_start_x_orig, m_stop_x_orig;
    float 	m_start_y_orig, m_stop_y_orig;
    int 	m_mousex, m_mousey;
    float 	m_frozen_x, m_frozen_y;
    float 	m_selection_x, m_selection_y;
    float	m_cursor_x, m_cursor_y;
    bool	m_cursor_active;
    bool  	m_selection_active;
    int 	m_button;
    int 	m_dragging;
    FLT_BUF* m_data_buffer;
    float 	m_aspect_ratio;
    bool  	m_fill_under;

    int  handle(int e);
    void pan_view(float x, float y);
    void zoom_view(float x, float y);
    void draw_grid(int, int);
    void draw_text(int, int);
    void init_viewport(int W,int H);
    void draw();
    void resize(int X,int Y,int W,int H);
};

#endif
