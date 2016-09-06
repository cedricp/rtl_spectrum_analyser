/*
 * (c) 2016 Cedric PAILLE
 * Simple Spectrum Analyzer for RTL Dongle
 *
 */

#include <graph_widget.h>
#include <tip_widget.h>
#include <FL/Fl.H>
#include <FL/gl.h>
#include <FL/Fl_Scrollbar.H>
#include <math.h>
#include <stdio.h>
#include <string>

extern void ui_draw_complete(bool);

std::string convert_to_metric(float in, std::string unitname = ""){
	char buff[10];
	bool sign = in < 0.;
	in = fabs(in);
	std::string metric_unit;
	int rank = 0;

	if (in == 0.f){
		return std::string("0")  + metric_unit + unitname;
	}

	if (in > 1.f){
		while(in > 1000.){
			rank++;
			in /= 1000.;
		}
		if (rank == 1){
			metric_unit = "K";
		} else if (rank == 2){
			metric_unit = "M";
		} else if (rank == 3){
			metric_unit = "G";
		} else if (rank == 4){
			metric_unit = "P";
		} else if (rank > 4){
			metric_unit = "?";
		}
	} else if (in < 1.f){
		while(in < 1000){
			rank++;
			in *= 1000.;
		}
		if (rank == 1){
			metric_unit = "m";
		} else if (rank == 2){
			metric_unit = "u";
		} else if (rank == 3){
			metric_unit = "p";
		} else if (rank > 3){
			metric_unit = "?";
		}
	}

	if (sign)
		in *= -1;

	snprintf(buff, 6, "%f", in);

	return std::string(buff) + metric_unit + unitname;
}

Gl_graph_widget::Gl_graph_widget(int X,int Y,int W,int H, Fl_Scrollbar* sb, const char*L) : Fl_Gl_Window(X,Y,W,H,L) {
	fg = 1.0;
	bg = 0.0;
	m_button = -1;
	m_grid_div_x = m_grid_div_y = 8;
	m_tooltip = new Tip_widget();
	m_tooltip->hide();
	m_fill_under = true;
	end();
	m_scroll_bar = sb;
	m_data_buffer = NULL;
	m_selection_active = false;
	m_cursor_active = false;
	sb->callback(scroll_callback, this);
}

Gl_graph_widget::~Gl_graph_widget(){
	m_tooltip->hide();
	delete m_tooltip;
}

void
Gl_graph_widget::reset(){
	m_start_x = m_start_x_orig;
	m_stop_x  = m_stop_x_orig;

	m_start_y = m_start_y_orig;
	m_stop_y  = m_stop_y_orig;
	valid(0);
	redraw();
}

void
Gl_graph_widget::scroll_callback(Fl_Widget*, void *t)
{
	Gl_graph_widget* gw = (Gl_graph_widget*)t;
	float frame_len = gw->m_stop_x - gw->m_start_x;
	float slider_pos = gw->m_scroll_bar->value();

	gw->m_start_x = slider_pos ;
	gw->m_stop_x = slider_pos + frame_len;
	gw->valid(0);
	gw->redraw();
}

int
Gl_graph_widget::handle(int e)
{
	int ret = Fl_Gl_Window::handle(e);
	int old_x = m_mousex;
	int old_y = m_mousey;
	int key = -1;
	bool moving = false;
	bool button_press = false;
	bool button_release = false;
	bool single_click= false;
	switch ( e ) {
		case FL_ENTER:
			ret = 1;
			m_tooltip->show();
			break;
		case FL_LEAVE:
		case FL_HIDE:
			m_tooltip->hide();
			ret = 1;
			break;
		case FL_MOVE:
		case FL_DRAG:
			moving = true;
			m_mousex = (int)Fl::event_x();
			m_mousey = (int)Fl::event_y();
			ret = 1;
			break;
		case FL_PUSH:
			button_press = true;
			m_button = Fl::event_button();
			ret = 1;
			break;
		case FL_RELEASE:
			button_release = true;
			if (!moving && Fl::event_button() == 1){
				single_click = true;
				m_cursor_active = true;
				get_mouse_coordinates(m_cursor_x, m_cursor_y);
			}
			m_button = -1;
			ret = 1;
			break;
		case FL_SHORTCUT:
			key = Fl::event_key();
			ret = 1;
			break;
	}


	float vx, vy;
	get_mouse_coordinates(vx, vy);
	m_tooltip->position(Fl::event_x_root(), Fl::event_y_root()+20);
	m_tooltip->value(vx, vy);

	if (key != -1 && Fl::event_shift() == 0){
		if (key == 65451 && m_grid_div_x < 30){
			m_grid_div_x++;
		}
		if (key == 65453 && m_grid_div_x > 1){
			m_grid_div_x--;
		}
		if (key == 'f'){
			m_fill_under = !m_fill_under;
		}
		if (key == 32 && !m_selection_active){
			reset();
		}
		if (key == 32 && m_selection_active){
			m_start_x = m_frozen_x > m_selection_x ? m_selection_x : m_frozen_x;
			m_stop_x  = m_frozen_x < m_selection_x ? m_selection_x : m_frozen_x;
			m_start_y = m_frozen_y > m_selection_y ? m_selection_y : m_frozen_y;
			m_stop_y  = m_frozen_y < m_selection_y ? m_selection_y : m_frozen_y;
			m_selection_active = false;
			valid(0);
		}
		redraw();
	}

	if (key != -1 && Fl::event_shift()){
		if (key == 65451 && m_grid_div_y < 30){
			m_grid_div_y++;
		}
		if (key == 65453 && m_grid_div_y > 1){
			m_grid_div_y--;
		}
		redraw();
	}

	if (m_button >= 0){
		int delta_x = m_mousex - old_x;
		int delta_y = m_mousey - old_y;
		if(m_button == 2)
			pan_view(-delta_x, delta_y);
		if(m_button == 3)
			zoom_view(delta_x, delta_y);
		if(m_button == 1  && button_press){
			m_selection_active = false;
			get_mouse_coordinates(m_frozen_x, m_frozen_y);
		}
		valid(0);
		redraw();
	}

	if (moving && m_button == 1){
		// User is moving with left mouse button pressed
		get_mouse_coordinates(m_selection_x, m_selection_y);
		m_selection_active = true;
	}

	if (single_click)
		redraw();

	return(ret);
}

void
Gl_graph_widget::get_mouse_coordinates(float& x, float& y)
{
	y = float(m_height - Fl::event_y()) / m_height * (m_stop_y - m_start_y) + m_start_y;
	x = float(Fl::event_x()) / m_width * (m_stop_x - m_start_x) + m_start_x;
}

bool
Gl_graph_widget::get_selection_bounding_box(float &x0, float&y0, float &x1, float &y1)
{
	x0 = m_frozen_x;
	y0 = m_frozen_y;
	x1 = m_selection_x;
	y1 = m_selection_y;
	return m_selection_active;
}

void
Gl_graph_widget::pan_view(float x, float y){
	float frame_len = m_stop_x - m_start_x;
	float new_dx = x * frame_len / float(m_width);
	float new_dy = y * (m_stop_y - m_start_y) / float(m_height);

	if ((m_start_x + new_dx) <= m_start_x_orig){
		float remaining = m_start_x_orig - m_start_x;
		new_dx = remaining < 0. ? remaining : 0.;
	}

	if ((m_stop_x + new_dx) >= m_stop_x_orig){
		float remaining = m_stop_x_orig - m_stop_x;
		new_dx = remaining > 0. ? remaining : 0.;
	}

	if ((m_start_y + new_dy) <= m_start_y_orig){
		float remaining = m_start_y_orig - m_start_y;
		new_dy = remaining < 0. ? remaining : 0.;
	}

	if ((m_stop_y + new_dy) >= m_stop_y_orig){
		float remaining = m_stop_y_orig - m_stop_y;
		new_dy = remaining > 0. ? remaining : 0.;
	}

	m_start_x += new_dx;
	m_start_y += new_dy;
	m_stop_x  += new_dx;
	m_stop_y  += new_dy;

	int scroll_bar_value = m_start_x;
	((Fl_Valuator*)m_scroll_bar)->value(scroll_bar_value);
}

void
Gl_graph_widget::zoom_view(float x, float y){
	m_start_x += x * (m_stop_x - m_start_x) / float(m_width);
	m_stop_x  -= x * (m_stop_x - m_start_x) / float(m_width);

	m_start_y -= y * (m_stop_y - m_start_y) / float(m_height);
	m_stop_y  += y * (m_stop_y - m_start_y) / float(m_height);

	if (m_start_x < m_start_x_orig){
		m_start_x = m_start_x_orig;
	}

	if (m_stop_x > m_stop_x_orig){
		m_stop_x = m_stop_x_orig;
	}

	if (m_start_y < m_start_y_orig){
		m_start_y = m_start_y_orig;
	}

	if (m_stop_y > m_stop_y_orig){
		m_stop_y = m_stop_y_orig;
	}
	int frame_size = m_stop_x - m_start_x;
	float slider_size = float(frame_size) / float(m_stop_x_orig - m_start_x_orig);
	m_scroll_bar->slider_size( slider_size );

	m_scroll_bar->bounds(m_start_x_orig, m_stop_x_orig - frame_size);
}

void
Gl_graph_widget::draw_grid(int numx, int numy){
	float zoom_factor_x = (m_stop_x_orig - m_start_x_orig) /  (m_stop_x - m_start_x);
	float zoom_factor_y = (m_stop_y_orig - m_start_y_orig) /  (m_stop_y - m_start_y);
	numx *= zoom_factor_x;
	numy *= zoom_factor_y;

	const float lengthx = m_stop_x_orig - m_start_x_orig;
	const float lengthy = m_stop_y_orig - m_start_y_orig;

	const float stepx = lengthx / float(numx);
	const float stepy = lengthy / float(numy);

	glLineStipple(1, 0xAAAA);
	glEnable(GL_LINE_STIPPLE);
	glColor3f(.5, 0, 0);
	glBegin(GL_LINES);
	for (int i = 0; i < numx; ++i){
		float x_axis = m_start_x_orig + stepx * i;
		if (x_axis < m_start_x) continue;
		if (x_axis > m_stop_x) break;
		glVertex2f(x_axis, m_start_y_orig);
		glVertex2f(x_axis, m_start_y_orig + lengthy);
	}

	for (int i = 0; i < numy; ++i){
		float y_axis = m_start_y_orig + stepy * i;
		if (y_axis < m_start_y) continue;
		if (y_axis > m_stop_y) break;
		glVertex2f(m_start_x_orig, y_axis);
		glVertex2f(m_start_x_orig + lengthx, y_axis);
	}
	glEnd();
	glDisable(GL_LINE_STIPPLE);


}

void
Gl_graph_widget::draw_text(int numx, int numy){
	float zoom_factor_x = (m_stop_x_orig - m_start_x_orig) /  (m_stop_x - m_start_x);
	float zoom_factor_y = (m_stop_y_orig - m_start_y_orig) /  (m_stop_y - m_start_y);
	const float lengthx = m_stop_x_orig - m_start_x_orig;
	const float lengthy = m_stop_y_orig - m_start_y_orig;
	numx *= zoom_factor_x;
	numy *= zoom_factor_y;
	const float stepx = lengthx / float(numx);
	const float stepy = lengthy / float(numy);
	int mx, my;

	gl_font(FL_HELVETICA, 8);
	glColor3f(0, .6, 0);
	float epsilon_offset = (m_stop_x - m_start_x) *0.01;
	float epsilon_offset_y = (m_stop_y - m_start_y) *0.01;
	for (int i = 0; i < numx; ++i){
		float screen_x = m_start_x_orig + (stepx * i);
		if (screen_x >= m_start_x && screen_x < m_stop_x){
			std::string str = convert_to_metric(screen_x * 1000, "s");
			gl_measure(str.c_str(), mx, my);
			mx /= 2;
			gl_draw(str.c_str(), screen_x - mx, m_start_y + epsilon_offset_y);
		}
	}

	for (int i = 0; i < numy; ++i){
		float screen_y = m_start_y_orig + (stepy * i);
		if (screen_y >= m_start_y && screen_y < m_stop_y){
			std::string str = convert_to_metric(screen_y, "dBm");
			gl_measure(str.c_str(), mx, my);
			my /= 2;
			gl_draw(str.c_str(), m_start_x + epsilon_offset, screen_y - my);
		}
	}
}

void
Gl_graph_widget::init_viewport(int W,int H) {
	m_width = W;
	m_height = H;
	glLoadIdentity();
	glViewport(0,0,W,H);
	m_aspect_ratio = float(W) / float(H);

	glOrtho(m_start_x,m_stop_x,m_start_y ,m_stop_y ,-1,1);
}

void
Gl_graph_widget::draw() {

	if (!valid()){
		valid(1);
		init_viewport(w(), h());
	}

	glClearColor(bg, bg, bg, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	const int data_size = m_data_buffer ? m_data_buffer->size() : 0;

	if (data_size < 2)
		goto FINISH_DRAW;
	{
		const float range 	= m_stop_x_orig - m_start_x_orig;
		const float step 	= range / float(data_size);
		glDisable( GL_LINE_SMOOTH );

		draw_grid(m_grid_div_x, m_grid_div_y);

		if (m_data_buffer != NULL){

			glEnable (GL_BLEND);
			glEnable (GL_SMOOTH);

			float inc = m_start_x_orig;
			const int start = ((m_start_x - m_start_x_orig) / data_size);
			inc += start;
			int cstep = (m_stop_x - m_start_x) / m_width;
			cstep = cstep < 1 ? 1 : cstep;

			if (m_fill_under){
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glBegin(GL_QUADS);
				for (int i = start; i < data_size - 1; i += cstep){
					float avg_data1 = 0.;
					float avg_data2 = 0.;
					for (int j = 0; j < cstep; ++j){
						avg_data1 += (*m_data_buffer)[i+j];
						avg_data2 += (*m_data_buffer)[i+j+1];
					}
					avg_data1 /= cstep;
					avg_data2 /= cstep;

					float offset = m_start_y_orig;
					float scale = 1. / (m_stop_y_orig - m_start_y_orig);
					glColor4f(0, 0, 0, .5);
					glVertex2f(inc, m_start_y);
					glColor4f(0, 0, (avg_data1 - offset) * scale, .5);
					glVertex2f(inc, avg_data1);
					inc += step * cstep;
					glColor4f(0, 0, (avg_data2 - offset) * scale, .5);
					glVertex2f(inc, avg_data2);
					glColor4f(0, 0, 0., .5);
					glVertex2f(inc, m_start_y);
					if (inc > m_stop_x)
						break;
				}
				glEnd();
			}

			glEnable( GL_LINE_SMOOTH );
			glHint( GL_LINE_SMOOTH_HINT, GL_FASTEST );
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glLineWidth(1.5);

			glColor3f(fg, fg, fg);
			glBegin(GL_LINE_STRIP);
			inc = m_start_x_orig;
			inc += start;
			cstep = (m_stop_x - m_start_x) / m_width;
			cstep = cstep < 1 ? 1 : cstep;
			for (int i = start; i < data_size; i += cstep){
				float avg_data = 0.;
				for (int j = 0; j < cstep; ++j){
					avg_data += (*m_data_buffer)[i+j];
				}
				avg_data /= cstep;
			glVertex2f(inc, avg_data);
			inc += step * cstep;
			if (inc - cstep > m_stop_x)
				break;
			}
			glEnd();
		}

		if (m_cursor_active){
			glLineWidth(1.);
			glColor4f(.0, .9, .1, .7);
			glBegin(GL_LINES);
			glVertex2f(m_cursor_x, m_start_y);
			glVertex2f(m_cursor_x, m_stop_y);
			glEnd();
		}

		if (m_selection_active){
			glColor4f(.7, .7, 0, .3);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBegin(GL_QUADS);
			glVertex2f(m_frozen_x, m_frozen_y);
			glVertex2f(m_frozen_x, m_selection_y);
			glVertex2f(m_selection_x, m_selection_y);
			glVertex2f(m_selection_x, m_frozen_y);
			glEnd();
		}
	}
	draw_text(m_grid_div_x, m_grid_div_y);

FINISH_DRAW:
	ui_draw_complete(true);
}

void
Gl_graph_widget::resize(int X,int Y,int W,int H) {
	Fl_Gl_Window::resize(X,Y,W,H);
	init_viewport(W,H);
	redraw();
}

void
Gl_graph_widget::set_brightness(double new_fg, double new_bg)
{
	fg = new_fg;
	bg = new_bg;
	redraw();
}

void
Gl_graph_widget::set_data_window(float startx, float stopx, float starty, float stopy){
	// Divide by 1000 to avoid opengl draw flaws
	m_start_x = m_start_x_orig	= startx / 1000.;
	m_start_y = m_start_y_orig	= starty;
	m_stop_x  = m_stop_x_orig	= stopx / 1000.;
	m_stop_y  = m_stop_y_orig 	= stopy;
	m_scroll_bar->slider_size( 1.f );
	m_scroll_bar->bounds(m_start_x_orig, m_stop_x_orig);
	valid(0);
	redraw();
}

float
Gl_graph_widget::get_power_at_cursor()
{
	if (!m_data_buffer)
		return -100.;
	float buff_len = m_data_buffer->size();
	float graph_width = m_stop_x_orig - m_start_x_orig;
	float ratio = m_cursor_x - m_start_x_orig;
	int pos = (ratio / graph_width) * buff_len;
	if (pos >= 0 && pos < (int)buff_len){
		return m_data_buffer->at(pos);
	}
	return -100.;
}
