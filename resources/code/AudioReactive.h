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
