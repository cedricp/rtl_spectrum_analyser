#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Value_Slider.H>
#include <stdio.h>
#include <math.h>
#include <graph_container.h>
#include <scanner.h>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include "scanner_thread.h"

class MyAppWindow : public Fl_Window {
	Graph_container *mygl;
private:

public:
    MyAppWindow(int W,int H,const char*L=0) : Fl_Window(W,H,L) {
    	Fl_Group* group = new Fl_Group(10,10, W - 10, H -10);
        mygl = new Graph_container(10, 10, group->w()-10, group->h()-10);
        group->end();
        end();
    }

    void reset_view(){
    	mygl->reset();
    }
    void set_buffer(std::vector<float>* buff){
    	mygl->set_buffer(buff);
    }
};

// MAIN
int main() {
	Fl::scheme("plastic");
    MyAppWindow win(500, 300, "RTL Spectrum Analyzer");
    win.resizable(win);
    win.show();

	Fl::lock();

	start_scanner_thread();

	/* run thread */
	while (Fl::wait() > 0) {
		std::vector<float>* buffer = (std::vector<float>*)Fl::thread_message();
		if (buffer) {
			win.set_buffer(buffer);
		}
	}

	printf("Asking scanner thread to terminate...\n");
	terminate_scanner_thread();
	join_scanner_thread();
	printf("Scanner thread finished.\n");
}
    
