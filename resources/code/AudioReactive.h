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

	Uint32 wav_offset; // where we are in the current buffer

	// fftw output
		fftw_complex Lsignal[4096];
		fftw_complex Lresult[4096];
		
		fftw_complex Rsignal[4096];
		fftw_complex Rresult[4096];

		fftw_plan Lplan = fftw_plan_dft_1d(4096,
										  Lsignal,
										  Lresult,
										  FFTW_FORWARD,
										  FFTW_ESTIMATE);
		
		fftw_plan Rplan = fftw_plan_dft_1d(4096,
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
