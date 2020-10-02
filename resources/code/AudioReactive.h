#ifndef RTTNW
#define RTTNW

#include "includes.h"


// class Waveform
// {
// public:
// 	Waveform();
// 	~Waveform();

// 	void display();
		
// 	// parameters for position, accessible from above
// 	// parameters for color, establishing the gradient

// 	void feed_new_data(std::vector<Uint8> data);
		
// private:
// 	// OpenGL specific data
// 	GLuint shader;
// 	GLuint vao;
// 	GLuint vbo;
	
// };


// #define FFT_SIZE 8192
#define FFT_SIZE 16384


class AudioReactive
{
public:

	AudioReactive();
	~AudioReactive();

	bool pquit;

	// audio specifics
	SDL_AudioSpec wav_spec;
	SDL_AudioDeviceID dev;
	Uint32 wav_length;
	Uint8 *wav_buffer;
	Uint8 *wav_buffer_display;

	int16_t * data; 

	Uint32 wav_offset; // where we are in the current buffer
	Uint32 wav_start_time; // acquired w/ SDL_GetTicks()

	// fftw output
		fftw_complex Lsignal[FFT_SIZE];
		fftw_complex Lresult[FFT_SIZE];
		
		fftw_complex Rsignal[FFT_SIZE];
		fftw_complex Rresult[FFT_SIZE];

		fftw_plan Lplan = fftw_plan_dft_1d(FFT_SIZE,
										  Lsignal,
										  Lresult,
										  FFTW_FORWARD,
										  FFTW_ESTIMATE);
		
		fftw_plan Rplan = fftw_plan_dft_1d(FFT_SIZE,
										  Rsignal,
										  Rresult,
										  FFTW_FORWARD,
										  FFTW_ESTIMATE);	
private:

	SDL_Window * window;
	SDL_GLContext GLcontext;

	ImVec4 clear_color;

	GLuint display_shader;
	GLuint display_vao;
	GLuint display_vbo;

	// std::vector<Waveform> waveforms; // this holds all your waveforms, initially emtpy

	void create_window();
	void gl_setup();
	void draw_everything();

	void quit();



	
};

#endif
