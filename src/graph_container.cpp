/*
 * (c) 2016 Cedric PAILLE
 * Simple Spectrum Analyzer for RTL Dongle
 *
 */

#include "graph_container.h"
#include "graph_widget.h"

#include <FL/Fl.H>
#include <FL/Fl_Scrollbar.H>

#include <math.h>

Graph_container::Graph_container(int X,int Y,int W,int H,const char*L) : Fl_Double_Window(X, Y, W, H){
	m_scroll_bar = new Fl_Scrollbar(10,h()-30, w()-20, 10);
	m_scroll_bar->type(FL_HORIZONTAL);
	m_scroll_bar->slider_size(.5);
	m_scroll_bar->bounds(0,100);
	m_graph_view = new Gl_graph_widget(10, 10, w()-20, h()-40, m_scroll_bar);
	m_graph_view->set_data_window(88000000, 108000000, -80, 20);
	end();
}

void
Graph_container::set_window(float startx, float stopx, float starty, float stopy)
{
	m_graph_view->set_data_window(startx, stopx, starty, stopy);
}

float
Graph_container::get_power_at_cursor()
{
	return m_graph_view->get_power_at_cursor();
}

void
Graph_container::resize(int X,int Y,int W,int H) {
	Fl_Window::resize(X,Y,W,H);
	m_graph_view->size(W-20,H-40);
	m_scroll_bar->size(w()-20, 10);
	m_scroll_bar->position(10,h()-30);
	redraw();
}

void
Graph_container::reset()
{
	m_graph_view->reset();
}

void
Graph_container::set_buffer(std::vector<float>* buffer)
{
	m_graph_view->set_buffer(buffer);
}
