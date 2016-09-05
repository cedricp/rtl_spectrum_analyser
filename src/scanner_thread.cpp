/*
 * (c) 2016 Cedric PAILLE
 * Simple Spectrum Analyzer for RTL Dongle
 *
 */

#include <FL/Fl.H>
#include "scanner_thread.h"
#include <vector>
#include <stdio.h>

static bool terminate = false;
static Scanner_settings scan_settings;
static pthread_t tid;
static bool ui_ready = false;
static bool ui_draw_finished = true;

void set_ui_ready()
{
	ui_ready = true;
}

void ui_draw_complete(bool complete)
{
	ui_draw_finished = complete;
}

Scanner_settings& get_scanner_settings()
{
	return scan_settings;
}

Scanner_settings::Scanner_settings()
{
	lower_freq = 88000000;
	upper_freq = 108000000;
	step_freq = 10000;
	crop = 0.2;
	rtl_dev_index = 0;
	direct_sampling = false;
	offset_tuning = false;
	ppm_correction = 0;
	gain = RTL_GAIN_AUTO;
	params_changed = true;
	window_type = WINDOW_TYPE_HAMMING;
}

void terminate_scanner_thread(){
	terminate = true;
}

void* scanner_thread(void* user_data)
{
	int status;
	Scanner scanner;

	while (ui_ready == false){
		usleep(2000);
	}

	while (terminate == false){
		status = scanner.init_scanner(scan_settings.lower_freq, scan_settings.upper_freq, scan_settings.step_freq,
									  scan_settings.crop, scan_settings.rtl_dev_index, scan_settings.direct_sampling,
									  scan_settings.direct_sampling, scan_settings.gain, scan_settings.ppm_correction,
									  scan_settings.window_type);

		if (status != SCANNER_OK){
			printf("Scanning issue, check RTL dongle connection or scan parameters...(%s)\n", scanner.get_error(status).c_str());
			usleep(1000000);
			continue;
		}

		scan_settings.params_changed = false;
		std::vector<Scan_result> ffts;
		while(scan_settings.params_changed == false){
			status = scanner.scan(&scan_settings.params_changed);
			if (status != SCANNER_OK)
				break;
			status = scanner.compute_ffts(ffts, &scan_settings.params_changed);
			if (status != SCANNER_OK)
				break;
			std::vector<float>* vecbuf = new std::vector<float>;
			for (int i = 0; i < ffts.size(); ++i){
				Scan_result& scan = ffts[i];
				for (int j = 0; j < scan.buffer.size(); ++j){
					vecbuf->push_back(scan.buffer[j]);
				}
			}

			while(ui_draw_finished == false){
				usleep(200);
				if (terminate)
					break;
			}

			ui_draw_finished = false;
			Fl::awake((void*)vecbuf);

			if (terminate)
				break;
		}
	}
}

void
start_scanner_thread()
{
	int err = pthread_create(&tid, NULL, scanner_thread, NULL);
}

void
join_scanner_thread()
{
	pthread_join(tid, NULL);
}
