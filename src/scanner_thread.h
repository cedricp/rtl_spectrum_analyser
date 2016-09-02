#ifndef SCANNER_THREAD_H
#define SCANNER_THREAD_H

#include "scanner.h"

#define RTL_GAIN_AUTO -1

struct Scanner_settings{
	Scanner_settings();
	int lower_freq, upper_freq, step_freq;
	double crop;
	int rtl_dev_index;
	bool direct_sampling;
	bool offset_tuning;
	int gain;
	int ppm_correction;
	window_types window_type;
	bool params_changed;
};

Scanner_settings* get_scanner_settings();
void* scanner_thread(void* user_data);
void start_scanner_thread();
void join_scanner_thread();

void terminate_scanner_thread();

#endif
