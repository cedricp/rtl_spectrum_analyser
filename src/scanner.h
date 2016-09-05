#ifndef SCANNER_H
#define SCANNER_H
#include <unistd.h>
#include <stdint.h>
#include "rtldev.h"

/* 3000 is enough for 3GHz b/w worst case */
#define MAX_TUNES	3000

enum window_types {
	WINDOW_TYPE_RECTANGLE,
	WINDOW_TYPE_HAMMING,
	WINDOW_TYPE_BLACKMAN,
	WINDOW_TYPE_BACKMAN_HARRIS,
	WINDOW_TYPE_HANN_POISSON,
	WINDOW_TYPE_YOUSSEF,
	WINDOW_TYPE_KAISER,
	WINDOW_TYPE_BARTLETT
};

#define SCANNER_OK		    		 0
#define SCANNER_DEVICE_ERROR 		-1
#define SCANNER_DEVICE_CONNECTION	-2
#define SCANNER_NOK					-3
#define SCANNER_MEMORY_ERROR		-4

#define RTL_GAIN_AUTO -10000

struct Scan_result
{
	int length;
	int freq_start;
	int freq_stop;
	float freq_step;
	int num_samples;
	std::vector<float> buffer;
};

struct scan_info{
	int 	num_frequency_hops;
	int 	dongle_bw_hz;
	int 	downsampling;
	double 	cropping_percent;
	int 	total_fft_bins;
	int 	logged_fft_bins;
	double 	fft_bin_size_hz;
	int 	buffer_size_bytes;
	double 	buffer_size_ms;
};

class Scanner{
	struct tuning_state
	/* one per tuning range */
	{
		int 	freq;
		int 	rate;
		int 	bin_e;
		long 	*avg;  /* length == 2^bin_e */
		int 	samples;
		int 	downsample;
		int 	downsample_passes;  /* for the recursive filter */
		double 	crop;
		/* having the iq buffer here is wasteful, but will avoid contention */
		uint8_t *buf8;
		int 	buf_len;
	};
	int 		m_nwave, m_log2_nwave;
	int16_t		*m_sinewave;
	int16_t 	*m_fft_buf;
	int 		*m_window_coefs;
	int 		m_boxcar;
	int 		m_comp_fir_size;
	int		 	m_peak_hold;
	int		 	m_tune_count;
	tuning_state m_tunes[MAX_TUNES];
	scan_info	 m_scan_info;
	Rtl_dev		 m_rtl_device;
	int			 m_must_stop;

	void make_sine_table(int size);
	int  fix_fft(int16_t iq[], int m);
	void rms_power(struct tuning_state *ts);
	int  frequency_range(double crop, int upper, int lower, int max_size);
	void fifth_order(int16_t *data, int length);
	void remove_dc(int16_t *data, int length);
	void generic_fir(int16_t *data, int length, int *fir);
	void downsample_iq(int16_t *data, int length);
	int  must_stop();
	void destroy_tunes_memory();
	int  compute_fft(Scan_result& res, tuning_state* ts);
	void set_gain(int gain);
	void set_auto_gain();
public:
	Scanner();
	~Scanner();
	int  init_scanner(int lower_frep, int upper_freq, int bin_len, double crop, int rtl_dev_index, bool direct_sampling,
					  bool offset_tuning, int gain, int ppm_error, window_types win_type);
	int scan(bool* params_changed);
	const scan_info& get_scan_info(){return m_scan_info;}
	int compute_ffts(std::vector<Scan_result>& res, bool* params_changed);
	Rtl_dev& get_rtl_device(){return m_rtl_device;}
	std::string get_error(int s);
};

#endif
