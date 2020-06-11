#include "screen.h"
#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600
#define SDL_MAX_SCANCODE_VALUE 284

void recomputeTransformMatrices();

struct colorRect
{
    unsigned int shaderProgram;
    unsigned int glVAO;
};

static struct colorRect rectData;
static float orthoMatrix[4][4];
static float (transformMatrices[64][32])[4][4];
static ImVec4 clearColor;
static ImVec4 pixelColor;
static bool settingsMenuActive;
static Mix_Chunk *squareWave;

int getCurrentlyPressedKey() // Get a single pressed key. If multiple keys are pressed, key with lowest numerical scancode will be prioritized. Return -1 if no key is pressed.
{
    for(int i = 0; i <= SDL_MAX_SCANCODE_VALUE; i++)
    {
        if(igIsKeyPressed(i, false))
        {
            return i;
        }
    }
    return -1;
}

#define keyBufferSize 200
void showBindingsMenu(bool* open)
{
    static bool first = true;
    static char labels[16][8];
    static char bindingBuffers[16][keyBufferSize];
    int keyPressed;
    if(first)
    {
        for(int i = 0; i < 16; i++)
        {
            sprintf(labels[i],"##Num %X",i);
        }
        first = false;
    }

    if(*open)
    {

        static bool currentSelection[16];
        igBegin("Controls", open,ImGuiWindowFlags_AlwaysAutoResize);
        if(*open)
        {
            igPushItemWidth(100);
            for(int i = 0; i < 16; i++)
            {
                igInputTextWithHint(labels[i],"<Unbound>", bindingBuffers[i], keyBufferSize, ImGuiInputTextFlags_ReadOnly, NULL, NULL);
                if(igIsItemActive())
                {
                    keyPressed = getCurrentlyPressedKey();
                    if(keyPressed != -1)
                    {
                        //printf("Printing keypress %d into item %d\n", keyPressed, i);
                        strcpy(bindingBuffers[i], SDL_GetKeyName(SDL_GetKeyFromScancode(keyPressed)));
                    }
                }
                igSameLine(0,5);
                igText("Numpad %X", i);
            }
            igPopItemWidth();
        }
        igEnd();
    }
}
void showSettingsMenu(bool *open, Chip8State* state)
{
    if(*open)
    {
        ImVec2 buttonSize;
        buttonSize.x = 0;
        buttonSize.y = 0;
        static bool bindingsMenu = false;
        bool freqCollapse = true;
        bool displayCollapse = true;
        igBegin("Settings", open, ImGuiWindowFlags_AlwaysAutoResize);
        igText("Currently Running:");
        igText(state->currentFilePath);
        if(igCollapsingHeaderBoolPtr("CPU Settings", &freqCollapse, 0))
        {
            igInputInt("Frequency", &state->frequency, 10, 100, ImGuiInputTextFlags_CharsDecimal);
            igText("Bit Shift Method");
            igRadioButtonIntPtr("Store to VX", &state->shiftMethod, 0);
            igSameLine(0, 5);
            igRadioButtonIntPtr("Store to VY", &state->shiftMethod, 1);
        }
        if(igCollapsingHeaderBoolPtr("Display Settings", &displayCollapse, 0))
        {
            igColorEdit3("Background color", (float*)&clearColor, 0);
            igColorEdit3("Pixel color", (float*)&pixelColor, 0);
        }
        if(state->paused)
        {
            if(igButton("Unpause", buttonSize))
            {
                state->paused = false;
            }
        } else
        {
            if(igButton("Pause", buttonSize))
            {
                state->paused = true;
            }
        }
        igSameLine(0,5);
        if(igButton("Reload", buttonSize))
        {
            reload(state);
        }
        igSameLine(0,5);
        if(igButton("Open File", buttonSize))
        {
            char* fileName;
            fileName = tinyfd_openFileDialog("Select a chip8 rom",
                    "",
                    0,
                    NULL,
                    NULL,
                    0);
            free(state->currentFilePath);
            state->currentFilePath = fileName;
            reload(state);
        }
        if(igButton("Control Bindings", buttonSize))
        {
            bindingsMenu = true;
        }
        igText("%X",getCurrentlyPressedKey());
        igEnd();
        showBindingsMenu(&bindingsMenu);
    }
}


void print4dmatrix(float matrix[][4])
{
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            printf("%f, ",matrix[i][j]);
        }
        printf("\n");
    }
}

const char* vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"

  	"uniform mat4 transform;\n"

	"void main()\n"
	"{\n"
		"gl_Position = transform * vec4(aPos, 1.0f);\n"
	"}\n\0";
const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec4 inputColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = inputColor;\n"
    "}\n\0";  

void mult4dmatrix(float matA[][4], float matB[][4], float matC[][4])
{
    float temp[4][4] = {0};
    for(int i = 0; i < 4; i++)
    {   
        for(int j = 0; j < 4; j++)
        {   
            for(int k = 0; k < 4; k++)
            {   
                temp[i][j] += matA[i][k] * matB[k][j];
            }   
        }   
    }   

    memcpy(matC, temp, sizeof(temp));
}

void createColorRect()
{

    recomputeTransformMatrices();

    //Compile shader codes
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    int success;
    char infoLog[512];
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("Fragment shader compilation failed:\n%s\n", infoLog);
    }
    unsigned int defaultShaderProgram = glCreateProgram();
    glAttachShader(defaultShaderProgram, vertexShader);
    glAttachShader(defaultShaderProgram, fragmentShader);
    glLinkProgram(defaultShaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    //Set up VBO for basic rect
    float vertices[] =
    {
        -1.0f,  1.0f, 0.0f, // Top left
         1.0f,  1.0f, 0.0f, // Top right
        -1.0f, -1.0f, 0.0f, // Bottom left
         1.0f, -1.0f, 0.0f, // Bottom right
    };

    unsigned int winding[] =
    {
        0, 1, 3,
        0, 3, 2
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(winding), winding, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    rectData.glVAO = VAO;
    rectData.shaderProgram = defaultShaderProgram;


    //Prevent potential further modifications of this vertex array
    glBindVertexArray(0);
}

// Generate orthographic projection matrix for a screen with the given pixel dimensions
void generateOrthoMatrix(float width, float height, float returnMatrix[][4])
{
    float tempMatrix[4][4] = {
        2/(width), 0.0f, 0.0f, -1.0f,
        0.0f, 2/(-height), 0.0f, 1.0f,
        0.0f, 0.0f, -2/(1), -1.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    memcpy(returnMatrix, tempMatrix, sizeof(tempMatrix));
}

void createScaleMatrix(float xFactor, float yFactor, float matrix[][4])
{
    float scale[4][4] = {
        (float)xFactor, 0.0f, 0.0f, 0.0f,
        0.0f, (float)yFactor, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    memcpy(matrix, scale, sizeof(scale));
}

void translateMatrix(float x, float y, float matrix[][4])
{
	float translate[4][4] = {
        1.0f, 0.0f, 0.0f, (float)x,
        0.0f, 1.0f, 0.0f, (float)y,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    mult4dmatrix(translate, matrix, matrix);
}


void recomputeTransformMatrices()
{
    int winX;
    int winY;
    SDL_GetWindowSize(SDL_GL_GetCurrentWindow(), &winX, &winY);
    generateOrthoMatrix(winX, winY, orthoMatrix);
    printf("WindowDims: %d,%d\n", winX, winY);
    int width = winX/64;
    int height = winY/32;
    float* currentMatrix;
    //print4dmatrix(orthoMatrix);
    for(int i = 0; i < 64; i++)
    {
        for(int j = 0; j < 32; j++)
        {
            createScaleMatrix(width/2, height/2, transformMatrices[i][j]);
            translateMatrix(i*width + width/2, j*height + height/2, transformMatrices[i][j]);
            mult4dmatrix(orthoMatrix, transformMatrices[i][j], transformMatrices[i][j]);
        }
    }

}
void drawRect(int x, int y, int width, int height, ImVec4 color)
{
    float scale[4][4] = {
        (float)width/2, 0.0f, 0.0f, 0.0f,
        0.0f, (float)height/2, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
	float transform[4][4] = {
        1.0f, 0.0f, 0.0f, (float)x + width/2,
        0.0f, 1.0f, 0.0f, (float)y + height/2,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    mult4dmatrix(transform, scale, transform);
    mult4dmatrix(orthoMatrix, transform, transform);
    glUseProgram(rectData.shaderProgram);
    unsigned int transformLoc = glGetUniformLocation(rectData.shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_TRUE, (float*)transform);
    unsigned int colorLoc = glGetUniformLocation(rectData.shaderProgram, "inputColor");
    glUniform4f(colorLoc, color.x, color.y, color.z, color.w);
    glBindVertexArray(rectData.glVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void drawPixel(int x, int y)
{
  SDL_Window *window = SDL_GL_GetCurrentWindow();
  SDL_DisplayMode dimensions;
  SDL_GetWindowDisplayMode(window, &dimensions);
  int width = dimensions.w/64;
  int height = dimensions.h/32;
  ImVec4 color;
  color.x = 1.0f;
  color.y = 1.0f;
  color.z = 1.0f;
  color.w = 1.0f;
  drawRect(x*width, y*height, width, height, color);
}

void parseKeyboard()
{
  const uint8_t* stateArray = SDL_GetKeyboardState(NULL);
  clearNumpad();
  if(stateArray[SDL_SCANCODE_Q])
  {
    setNumpadKey(4);
  }
  if(stateArray[SDL_SCANCODE_W])
  {
    setNumpadKey(5);
  }
  if(stateArray[SDL_SCANCODE_E])
  {
    setNumpadKey(6);
  }
  if(stateArray[SDL_SCANCODE_A])
  {
    setNumpadKey(7);
  }
  if(stateArray[SDL_SCANCODE_S])
  {
    setNumpadKey(8);
  }
  if(stateArray[SDL_SCANCODE_D])
  {
    setNumpadKey(9);
  }

}

bool pollEvents()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {

      ImGui_ImplSDL2_ProcessEvent(&event);
      switch (event.type)
      {
        case SDL_QUIT:
            return false;
            break;
        case SDL_WINDOWEVENT:
            if(event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                recomputeTransformMatrices();
            }
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                if(settingsMenuActive == false)
                {
                    settingsMenuActive = true;
                } else
                {
                    settingsMenuActive = false;
                }
            }
            break;
      }
    }
    parseKeyboard();
    return true;
}

int initScreen()
{
    settingsMenuActive = false;
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {   
        SDL_Log("Failed to init: %s\n", SDL_GetError());
        return -1; 
    }
    if(Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
    {
        printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
        return -1;
    }
    clearColor.x = 0.0f;
    clearColor.y = 0.0f;
    clearColor.z = 0.0f;
    clearColor.w = 1.0f;
    pixelColor.x = 1.0f;
    pixelColor.y = 1.0f;
    pixelColor.z = 1.0f;
    pixelColor.w = 1.0f;

    // OpenGL initialization
    // Set OpenGL version
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0); 
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); 
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0); 

    // Perform OpenGL configuration
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); 
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); 
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);

    SDL_Window *window = SDL_CreateWindow(
        "Chip-8 Interpreter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DEFAULT_WIDTH, DEFAULT_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if(window == NULL)
    {   
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return -1;
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(0);

    bool err = gl3wInit() != 0;

    if(err)
    {
        SDL_Log("Failed to initialize OpenGL loader.\n");
        return -1;
    }

    printf("OpenGL and SDL initialized successfully!\n");

    createColorRect();

    //imGui initialization
    igCreateContext(NULL);
    ImGuiIO io =  *igGetIO();
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);
    igStyleColorsDark(NULL);
    SDL_GL_MakeCurrent(window, gl_context);

    //load sound
    squareWave = Mix_LoadWAV("SquareWave.wav");
    if(squareWave == NULL)
    {
        printf("Error loading SquareWave.wav\n");
        return -1;
    }
    return 0;
}

void playSquareWave()
{
    if(!Mix_Playing(1))
    {
        Mix_PlayChannel(1,squareWave, -1);
    }
}

void stopSound()
{
    Mix_HaltChannel(1);
}

void drawScreen(uint8_t screen[][32], ImVec4 color)
{
    glUseProgram(rectData.shaderProgram);
    for(int i = 0; i < 64; i++)
    {
        for(int j = 0; j < 32; j++)
        {
            if(screen[i][j] == 1)
            {
                unsigned int transformLoc = glGetUniformLocation(rectData.shaderProgram, "transform");
                glUniformMatrix4fv(transformLoc, 1, GL_TRUE, (float*)transformMatrices[i][j]);
                unsigned int colorLoc = glGetUniformLocation(rectData.shaderProgram, "inputColor");
                glUniform4f(colorLoc, color.x, color.y, color.z, color.w);
                glBindVertexArray(rectData.glVAO);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }
    }
    glBindVertexArray(0);
}

void renderFrame(Chip8State* state)
{
    if(state->ST > 0)
    {
        playSquareWave();
    } else
    {
        stopSound();
    }
    uint8_t (*screen)[32] = state->screen;
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(SDL_GL_GetCurrentWindow());
    igNewFrame();
    ImGuiIO io = *igGetIO();
    showSettingsMenu(&settingsMenuActive, state);
	igRender();
	//SDL_GL_MakeCurrent(SDL_GL_GetCurrentWindow(), gl_context);
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
	glClear(GL_COLOR_BUFFER_BIT);
    drawScreen(screen, pixelColor);
	ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
	SDL_GL_SwapWindow(SDL_GL_GetCurrentWindow());
}

void cleanupSDL()
{
    Mix_FreeChunk(squareWave);
    SDL_GL_DeleteContext(SDL_GL_GetCurrentContext());
    SDL_DestroyWindow(SDL_GL_GetCurrentWindow());
    SDL_Quit();
}
