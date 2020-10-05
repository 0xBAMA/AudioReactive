#include "AudioReactive.h"
// This contains the lower level code


Waveform::Waveform()
{
    // static variable allows multiple instantiations of this waveform display
    static int current_texture_unit{0}; // constructing with zero will initialize the count
    
    // create the vao
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // create the vbo
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
  
    // initalize textures
    my_front = current_texture_unit;
    my_back = current_texture_unit+1;

    GLuint my_front_loc, my_back_loc;
    glGenTextures(1, &my_front_loc);
    glGenTextures(1, &my_back_loc);


    // going to use 16 bit values for the texture
    glBindTexture(GL_TEXTURE_2D, my_front_loc);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindImageTexture(my_front, my_front_loc, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, 8196, 8196, 0, GL_RGBA, GL_FLOAT, NULL);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glBindTexture(GL_TEXTURE_2D, my_back_loc);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindImageTexture(my_back, my_back_loc, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, 8196, 8196, 0, GL_RGBA, GL_FLOAT, NULL);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // we used two
    current_texture_unit += 2;
    
    // compile shaders (display + compute)
    shader_display = Shader("resources/code/shaders/display.vs.glsl", "resources/code/shaders/display.fs.glsl").Program;
    // shader_compute = CShader("resources/shaders/compute.cs.glsl").Program;

    // using the display shader initially
    glUseProgram(shader_display);
    
    // generate geometry
    generate_points();
    
    // buffer to GPU
    int num_bytes_points = sizeof(glm::vec3) * points.size();

    glBufferData(GL_ARRAY_BUFFER, num_bytes_points, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, num_bytes_points, &points[0]);
    
    // vertex attributes (position only)
    GLuint vPosition = glGetAttribLocation(shader_display, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, ((GLvoid*) (0))); 
}

Waveform::~Waveform()
{
    
}

void Waveform::generate_points()
{
    // surface
    glm::vec3 a, b, c, d;

    float scale = 0.5f;

    a = glm::vec3(-1.0f*scale, 0.1, -1.0f*scale);
    b = glm::vec3(-1.0f*scale, 0.1, 1.0f*scale);
    c = glm::vec3(1.0f*scale, 0.1, -1.0f*scale);
    d = glm::vec3(1.0f*scale, 0.1, 1.0f*scale);

    subd_square(a, b, c, d, 9);

    num_points = points.size();
    cout << "surface is " << num_points << " points" << endl;

  // skirts
    // generate points
    subd_skirts(a, b, 9);
    subd_skirts(b, d, 9);
    subd_skirts(d, c, 9);
    subd_skirts(c, a, 9);
    
    cout << "plus " << points.size()-num_points << " points for the skirts" << endl;
    cout << "for a total of " << points.size() << " points" << endl;

    num_points = points.size();
}

void Waveform::subd_square(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, int levels, int current)
{
    if(current == levels)
    {//add points
        // triangle 1 ABC
        points.push_back(a);
        points.push_back(b);
        points.push_back(c);
        //triangle 2 BCD
        points.push_back(b);
        points.push_back(c);
        points.push_back(d);

        //middle
        // points.push_back((a+b+c+d)/4.0f);
    }
    else
    { //recurse
        glm::vec3 center = (a + b + c + d) / 4.0f;    //center of the square

        glm::vec3 bdmidp = (b + d) / 2.0f;            //midpoint between b and d
        glm::vec3 abmidp = (a + b) / 2.0f;            //midpoint between a and b
        glm::vec3 cdmidp = (c + d) / 2.0f;            //midpoint between c and d
        glm::vec3 acmidp = (a + c) / 2.0f;            //midpoint between a and c

        subd_square(abmidp, b, center, bdmidp, levels, current+1);
        subd_square(a, abmidp, acmidp, center, levels, current+1);
        subd_square(center, bdmidp, cdmidp, d, levels, current+1);
        subd_square(acmidp, center, c, cdmidp, levels, current+1);
    } 
}

void Waveform::subd_skirts(glm::vec3 a, glm::vec3 b, int levels, int current)
{
    // direction doesn't matter, we're not doing anything that depends on front/back facing
    //  we only need slices along the horizontal axis
    if(current == levels)
    {
        //triangle abc
        points.push_back(a);
        points.push_back(b);
        points.push_back(glm::vec3(a.x, 0.0, a.z));

        //triangle bcd
        points.push_back(b);
        points.push_back(glm::vec3(a.x, 0.0, a.z));
        points.push_back(glm::vec3(b.x, 0.0, b.z));
    }
    else
    {
        glm::vec3 center = (a+b) / 2.0f;
        subd_skirts(a, center, levels, current+1);
        subd_skirts(center, b, levels, current+1);
    }
}

void Waveform::display()
{
    
    // bind vao, vbo
    glBindVertexArray(vao);
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    
    // use shader
    glUseProgram(shader_display);

    // need to use the handle for the front buffer, the current data
    glUniform1i(glGetUniformLocation(shader_display, "data_texture"), my_front);

    theta = 0.0002*SDL_GetTicks();
    // phi = 0.0001*SDL_GetTicks();
    glUniform1f(glGetUniformLocation(shader_display, "theta"), theta);
    glUniform1f(glGetUniformLocation(shader_display, "phi"), phi);

    glUniform1f(glGetUniformLocation(shader_display, "time"), 0.01*SDL_GetTicks());
 
    // draw everything in one go - surface/skirts distinction made in shader
    glDrawArrays(GL_TRIANGLES, 0, num_points);
}

void Waveform::feed_new_data(std::vector<Uint8> data)
{
    // use the compute shader program
    // glUseProgram(shader_compute);

    // the data goes in as a uniform vector of floats

    // the compute shader will need to know which is front/back
    // glUniform1i(glGetUniformLocation(shader_compute, "front_buffer"), my_front);
    // glUniform1i(glGetUniformLocation(shader_compute, "back_buffer"), my_back);

    // dispatch the compute shader
    
    // wait to finish
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT); 
    
    // re generate the mipmap, after the compute has finished
    glGenerateMipmap(GL_TEXTURE_2D);

    // linear data is stored in the texture, we will be doing the logarithmic conversion as part of our sampling 
}







void AudioCallback(void*  userdata, Uint8* stream, int len)
{
    AudioReactive * a = (AudioReactive *) userdata;

        /* Only play if we have data left */
        if ( a->wav_length == 0 )
            return;

        /* Mix as much data as possible */
        len = ( len > (int)a->wav_length ? (int)a->wav_length : len );
        SDL_MixAudio(stream, a->wav_buffer+a->wav_offset, len, SDL_MIX_MAXVOLUME);
        // a->wav_buffer += len;
        a->wav_length -= len;
        a->wav_offset += len;
        
        cout << "Audio callback passing " << len << " bytes" << endl;
}

void AudioReactive::create_window()
{
	if(SDL_Init( SDL_INIT_EVERYTHING ) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
	}

	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 8);

	// this is how you query the screen resolution
	SDL_DisplayMode dm;
	SDL_GetDesktopDisplayMode(0, &dm);

	// pulling these out because I'm going to try to span the whole screen with
	// the window, in a way that's flexible on different resolution screens
	int total_screen_width = dm.w;
	int total_screen_height = dm.h;

	cout << "creating window...";

	// window = SDL_CreateWindow( "OpenGL Window", 0, 0, total_screen_width, total_screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN | SDL_WINDOW_BORDERLESS );
	window = SDL_CreateWindow( "OpenGL Window", 0, 0, total_screen_width, total_screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE );
  	SDL_ShowWindow(window);

	cout << "done." << endl;


	cout << "setting up OpenGL context...";
	// OpenGL 4.3 + GLSL version 430
	const char* glsl_version = "#version 430";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
	GLcontext = SDL_GL_CreateContext( window );

	SDL_GL_MakeCurrent(window, GLcontext);
	SDL_GL_SetSwapInterval(1); // Enable vsync
	// SDL_GL_SetSwapInterval(0); // explicitly disable vsync










	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
	}

	glEnable(GL_DEPTH_TEST);
    glEnable(GL_POINT_SMOOTH);

    glPointSize(3.0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io; // void cast prevents unused variable warning

	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(window, GLcontext);
	ImGui_ImplOpenGL3_Init(glsl_version);

	clear_color = ImVec4(75.0f/255.0f, 75.0f/255.0f, 75.0f/255.0f, 0.5f); // initial value for clear color

	// really excited by the fact imgui has an hsv picker to set this
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear( GL_COLOR_BUFFER_BIT );
	SDL_GL_SwapWindow( window );

	cout << "done." << endl;

	ImVec4* colors = ImGui::GetStyle().Colors;

	colors[ImGuiCol_Text]                   = ImVec4(0.64f, 0.37f, 0.37f, 1.00f);
	colors[ImGuiCol_TextDisabled]           = ImVec4(0.49f, 0.26f, 0.26f, 1.00f);
	colors[ImGuiCol_WindowBg]               = ImVec4(0.17f, 0.00f, 0.00f, 0.94f);
	colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg]                = ImVec4(0.18f, 0.00f, 0.00f, 0.94f);
	colors[ImGuiCol_Border]                 = ImVec4(0.35f, 0.00f, 0.03f, 0.50f);
	colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg]                = ImVec4(0.14f, 0.04f, 0.00f, 1.00f);
	colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.14f, 0.04f, 0.00f, 1.00f);
	colors[ImGuiCol_FrameBgActive]          = ImVec4(0.14f, 0.04f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBg]                = ImVec4(0.14f, 0.04f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgActive]          = ImVec4(0.14f, 0.04f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark]              = ImVec4(0.87f, 0.23f, 0.09f, 1.00f);
	colors[ImGuiCol_SliderGrab]             = ImVec4(0.87f, 0.23f, 0.09f, 1.00f);
	colors[ImGuiCol_SliderGrabActive]       = ImVec4(1.00f, 0.33f, 0.00f, 1.00f);
	colors[ImGuiCol_Button]                 = ImVec4(0.81f, 0.38f, 0.09f, 0.08f);
	colors[ImGuiCol_ButtonHovered]          = ImVec4(0.87f, 0.23f, 0.09f, 1.00f);
	colors[ImGuiCol_ButtonActive]           = ImVec4(1.00f, 0.33f, 0.00f, 1.00f);
	colors[ImGuiCol_Header]                 = ImVec4(0.81f, 0.38f, 0.09f, 0.08f);
	colors[ImGuiCol_HeaderHovered]          = ImVec4(0.87f, 0.23f, 0.09f, 1.00f);
	colors[ImGuiCol_HeaderActive]           = ImVec4(1.00f, 0.33f, 0.00f, 1.00f);
	colors[ImGuiCol_Separator]              = ImVec4(0.81f, 0.38f, 0.09f, 0.08f);
	colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.87f, 0.23f, 0.09f, 1.00f);
	colors[ImGuiCol_SeparatorActive]        = ImVec4(1.00f, 0.33f, 0.00f, 1.00f);
	colors[ImGuiCol_ResizeGrip]             = ImVec4(0.81f, 0.38f, 0.09f, 0.08f);
	colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.87f, 0.23f, 0.09f, 1.00f);
	colors[ImGuiCol_ResizeGripActive]       = ImVec4(1.00f, 0.33f, 0.00f, 1.00f);
	colors[ImGuiCol_Tab]                    = ImVec4(0.81f, 0.38f, 0.09f, 0.08f);
	colors[ImGuiCol_TabHovered]             = ImVec4(0.87f, 0.23f, 0.09f, 1.00f);
	colors[ImGuiCol_TabActive]              = ImVec4(1.00f, 0.33f, 0.00f, 1.00f);
	colors[ImGuiCol_TabUnfocused]           = ImVec4(0.81f, 0.38f, 0.09f, 0.08f);
	colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.81f, 0.38f, 0.09f, 0.08f);
	colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.81f, 0.38f, 0.09f, 0.08f);
	colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);


    // // /* Load the WAV */
    // if (SDL_LoadWAV("test.wav", &wav_spec, &wav_buffer, &wav_length) == NULL)
    // {
    //     fprintf(stderr, "Could not open test.wav: %s\n", SDL_GetError());
    // }
    // else
    // {
    //     cout << "Audio file info (test.wav):" << endl;
    //     cout << "  freq:" << wav_spec.freq << endl;
    //     cout << "  format:" << wav_spec.format << endl;
    //     cout << "  channels:" << (int)wav_spec.channels << endl;
    //     cout << "  samples:" << wav_spec.samples << endl;
    //     cout << "  size:" << wav_spec.size << endl << endl; 

    // }

    // // set up the audio
    // SDL_AudioSpec want, have;
    // SDL_memset(&want, 0, sizeof(want)); /* or SDL_zero(want) */
    // want.freq = wav_spec.freq;
    // // want.format = AUDIO_F32;
    // want.format = AUDIO_S16LSB;
    // // want.format = wav_spec.format;
    // want.channels = wav_spec.channels;
    // want.samples = wav_spec.samples;
    // want.callback = AudioCallback;  // this is what gives the audio device more samples
    // want.userdata = (void*)this;
    // dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
    

    // // querying the situation
    // cout << "Audio Device list:" << endl;
    // int i, count = SDL_GetNumAudioDevices(0);

    // for (i = 0; i < count; ++i)
    // {
    //     SDL_Log("Audio device %d: %s", i, SDL_GetAudioDeviceName(i, 0));
    // }
    
    // cout << endl << endl;
    // cout << "Audio device info:" << endl;
    // cout << "  freq:" << have.freq << endl;
    // cout << "  format:" << have.format << endl;
    // cout << "  channels:" << (int)have.channels << endl;
    // cout << "  samples:" << have.samples << endl;
    // cout << "  size:" << have.size << endl << endl;
    
    // // unpause the device
    // SDL_PauseAudioDevice(dev, 0); 


    // std::string filename("test.wav");
    // std::string filename("groupB.wav");
    std::string filename("dennisMorrow.wav");


    cout << "opening file: " << filename << endl;

    SDL_LoadWAV(filename.c_str(), &wav_spec, &wav_buffer, &wav_length);
    SDL_LoadWAV(filename.c_str(), &wav_spec, &wav_buffer_display, &wav_length);
    dev = SDL_OpenAudioDevice(NULL, 0, &wav_spec, NULL, 0); 

    SDL_QueueAudio(dev, wav_buffer, wav_length);
    SDL_PauseAudioDevice(dev, 0);



    SDL_AudioFormat fmt = wav_spec.format;

    cout << endl << endl;
    if (SDL_AUDIO_ISBIGENDIAN(fmt))
    {
        printf("big endian ");
    }
    else
    {
        printf("little endian ");
    }

    if (SDL_AUDIO_ISSIGNED(fmt))
    {
        printf("signed ");
    }
    else
    {
        printf("unsigned ");
    }

    if (SDL_AUDIO_ISFLOAT(fmt))
    {
        printf("floating point data ");
    }
    else
    {
        printf("integer data ");
    }

    printf("at %d bits per sample\n", (int) SDL_AUDIO_BITSIZE(fmt));


    
    wav_start_time = SDL_GetTicks();
    
    data = (int16_t *)wav_buffer_display;


    
}




void AudioReactive::gl_setup()
{
	// some info on your current platform
	const GLubyte *renderer = glGetString( GL_RENDERER ); // get renderer string
	const GLubyte *version = glGetString( GL_VERSION );		// version as a string
	printf( "Renderer: %s\n", renderer );
	printf( "OpenGL version supported %s\n\n\n", version );



    // create the shader for the triangles to cover the screen
    display_shader = Shader("resources/code/shaders/blit.vs.glsl", "resources/code/shaders/blit.fs.glsl").Program;

    // set up the points for the display
    //  A---------------B
    //  |          .    |
    //  |       .       |
    //  |    .          |
    //  |               |
    //  C---------------D

    // diagonal runs from C to B
    //  A is -1, 1
    //  B is  1, 1
    //  C is -1,-1
    //  D is  1,-1
    std::vector<glm::vec3> points;

    points.clear();
    points.push_back(glm::vec3(-1, 1, 0.5));  //A
    points.push_back(glm::vec3(-1,-1, 0.5));  //C
    points.push_back(glm::vec3( 1, 1, 0.5));  //B

    points.push_back(glm::vec3( 1, 1, 0.5));  //B
    points.push_back(glm::vec3(-1,-1, 0.5));  //C
    points.push_back(glm::vec3( 1,-1, 0.5));  //D

    // vao, vbo
    cout << "  setting up vao, vbo for display geometry...........";
    glGenVertexArrays( 1, &display_vao );
    glBindVertexArray( display_vao );

    glGenBuffers( 1, &display_vbo );
    glBindBuffer( GL_ARRAY_BUFFER, display_vbo );
    cout << "done." << endl;

    // buffer the data
    cout << "  buffering vertex data..............................";
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(), NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * points.size(), &points[0]);
    cout << "done." << endl;

    // set up attributes
    cout << "  setting up attributes in display shader............";
    GLuint points_attrib = glGetAttribLocation(display_shader, "vPosition");
    glEnableVertexAttribArray(points_attrib);
    glVertexAttribPointer(points_attrib, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) (static_cast<const char*>(0) + (0)));
    cout << "done." << endl;

    // this is where I'll be setting up the shaders for the waveforms, as well as the geometry to represent them
    waveforms.resize(2); // all the setup is done in the constructor, so this is going to be running all that twice

}


static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void AudioReactive::draw_everything()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io; // void cast prevents unused variable warning
    //get the screen dimensions and pass in as uniforms


	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);   // from hsv picker
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);                     // clear the background

	// draw the stuff on the GPU

    // texture display
    // glUseProgram(display_shader);
    // glBindVertexArray( display_vao );
    // glBindBuffer( GL_ARRAY_BUFFER, display_vbo );

    // glDrawArrays( GL_TRIANGLES, 0, 6 );


    

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);
	ImGui::NewFrame();

	// show the demo window
	// static bool show_demo_window = true;
	// if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

	// do my own window
	ImGui::SetNextWindowPos(ImVec2(10,10));
	ImGui::SetNextWindowSize(ImVec2(256,385));
	ImGui::Begin("Controls", NULL, 0);



    //do the other widgets
   HelpMarker("shut up, compiler");


   // // //TEMPORARILY REPORTING AUDIO STATUS IN THE MAIN LOOP - this is not going to give me enough information
   // switch (SDL_GetAudioDeviceStatus(dev))
   //  {
   //      case SDL_AUDIO_STOPPED: printf("stopped\n"); break;
   //      case SDL_AUDIO_PLAYING: printf("playing\n"); break;
   //      case SDL_AUDIO_PAUSED: printf("paused\n"); break;
   //      default: printf("???"); break;
   //  }


   // first step is to get the wav_offset value
        wav_offset = 2*wav_spec.freq*(SDL_GetTicks()-wav_start_time)/1000;

        // cout << SDL_GetTicks() << " " << wav_offset << endl;
        for(unsigned int i = 0; i < FFT_SIZE; ++i)
        {
            //these are both purely real signals, so we put it in index 0
            if(wav_offset + 2*i + 1 < wav_length)
            {
                Lsignal[i][0] = 0.0025*data[wav_offset + 2*i];
                Rsignal[i][0] = 0.0025*data[wav_offset + 2*i + 1];
            }
            else
            {
                Lsignal[i][0] = 0;
                Rsignal[i][0] = 0;
            }
        }

        usleep(48000);
        fftw_execute(Lplan);
        fftw_execute(Rplan);

        float lmax = 0;
        float rmax = 0;
        
        int samples_per_band = 128;

        for(int i = 0; i < FFT_SIZE/(2*samples_per_band); ++i)
        {
            fftw_complex Lsum; Lsum[0] = 0; Lsum[1] = 0;
            fftw_complex Rsum; Rsum[0] = 0; Rsum[1] = 0;

            for(int j = 0; j < FFT_SIZE/(2*samples_per_band); ++j)
            {
                Lsum[0] += (Lresult)[samples_per_band*i+j][0];
                Lsum[1] += (Lresult)[samples_per_band*i+j][1];
                
                Rsum[0] += (Rresult)[samples_per_band*i+j][0];
                Rsum[1] += (Rresult)[samples_per_band*i+j][1];
            }
            
            double Lmag = sqrt(Lsum[0] * Lsum[0] +
                               Lsum[1] * Lsum[1]) / FFT_SIZE;
            double Rmag = sqrt(Rsum[0] * Rsum[0] +
                               Rsum[1] * Rsum[1]) / FFT_SIZE;

            if(Lmag > lmax) lmax = Lmag;
            if(Rmag > rmax) rmax = Rmag;

            // cout << "\e[34m";

            // while(Lmag > 0)
            // {
            //     printf("|");
            //     Lmag -= 0.03;
            //     Lmag *= 0.99;
            // }

            // cout << "\e[0m" << endl;
            // cout << "\e[31m"; 
            // while(Rmag > 0)
            // {
            //     printf("|");
            //     Rmag -= 0.03;
            //     Rmag *= 0.99;
            // }

            // cout << "\e[0m" << endl;
        }

        // cout << "lmax is: " << lmax << " and rmax is: " << rmax << endl; // maximum intensity reported across all bands

        // this is where you're going to be drawing the waveform -
        //  at a high level, there are two steps to this process -
        //    1 - update the data on the GPU with the compute shader (takes the appropriate channel's freq data as input e.g. L takes Lresult, R takes Rresult)
        //    2 - the rendering shader references this same data texture and uses it as a height value when processing the surface/skirt geometry
        
        for(auto& x : waveforms)
            x.display();
        
        ImGui::End();
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());   // put imgui data into the framebuffer

        SDL_GL_SwapWindow(window); // swap the double buffers

        // handle events

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            if (event.type == SDL_QUIT)
                pquit = true;

            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                pquit = true;

            if ((event.type == SDL_KEYUP  && event.key.keysym.sym == SDLK_ESCAPE) || (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_X1)) //x1 is browser back on the mouse
                pquit = true;
        }
}


void AudioReactive::quit()
{
  //shutdown everything
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  //destroy window
  SDL_GL_DeleteContext(GLcontext);
  SDL_DestroyWindow(window);
  SDL_Quit();

  cout << "goodbye." << endl;
}
