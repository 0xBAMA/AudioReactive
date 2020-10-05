#ifndef RTTNW
#define RTTNW

#include "includes.h"


class Waveform
{
public:
	Waveform();
	~Waveform();

	void display();
		
	// parameters for position, accessible from above
	float theta = 0.2;
	float phi = -0.35;
		
	// parameters for color, establishing the gradient

	void feed_new_data(std::vector<Uint8> data);

private:

	void generate_points();
	void subd_square(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, int levels, int current=0); // recursive
	void subd_skirts(glm::vec3 a, glm::vec3 b, int levels, int current = 0); 
		
		
	std::vector<glm::vec3> points;
		
	// OpenGL specific data
	GLuint shader_display;
	GLuint shader_compute;
	GLuint vao;
	GLuint vbo;

	// texture units
	GLuint my_front;
	GLuint my_back;

	// number of points in the surface and skirts
	int num_points;

		
};


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

		// I need to think of a better mechanism to allow for pausing etc
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

	std::vector<Waveform> waveforms; // this holds all your waveforms, initially emtpy

	void create_window();
	void gl_setup();
	void draw_everything();

	void quit();



	
};

#endif
