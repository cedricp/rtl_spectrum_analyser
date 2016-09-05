#ifndef GRAPH_CONTAINER_H
#define GRAPH_CONTAINER_H

#include <FL/Fl_Double_Window.H>
#include <vector>

class Fl_Scrollbar;
class Gl_graph_widget;

class Graph_container : public Fl_Double_Window
{
	Gl_graph_widget* m_graph_view;
	Fl_Scrollbar* m_scroll_bar;
public:
	Graph_container(int X,int Y,int W,int H,const char*L=0);
	void reset();
	void set_buffer(std::vector<float>* buffer);
	void set_window(float startx, float stopx, float starty, float stopy);
protected:
	void resize(int X,int Y,int W,int H);
};


#endif
