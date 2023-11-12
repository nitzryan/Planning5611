#pragma warning(push, 0)
#include "glad/glad.h"
#include "sdl/SDL.h"
#pragma warning(pop)

#include <iostream>
#include <chrono>

#include "rendering/Camera.h"
#include "rendering/Renderer.h"
#include "scene/TestScene.h"

struct ScreenDetails {
	bool fullscreen;
	int width;
	int height;
};

ScreenDetails screenDetails{ false, 800, 800 };

struct UserInput {
	bool leftPressed = false;
	bool rightPressed = false;
	bool upPressed = false;
	bool downPressed = false;
	bool lctrl = false;
	bool quit = false;
};

int GetInput(UserInput& input, SDL_Event& windowEvent) {
	int scene = -1;
	while (SDL_PollEvent(&windowEvent)) {
		if (windowEvent.type == SDL_QUIT) input.quit = true; //Exit Game Loop
		if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE)
			input.quit = true; //Exit Game Loop
		if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_LEFT)
			input.leftPressed = false;
		if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_RIGHT)
			input.rightPressed = false;
		if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_DOWN)
			input.downPressed = false;
		if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_UP)
			input.upPressed = false;
		if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_LCTRL)
			input.lctrl = false;
		if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_LEFT)
			input.leftPressed = true;
		if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_RIGHT)
			input.rightPressed = true;
		if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_DOWN)
			input.downPressed = true;
		if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_UP)
			input.upPressed = true;
		if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_LCTRL)
			input.lctrl = true;
		if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_1)
			scene = 1;
		if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_2)
			scene = 2;
	}

	return scene;
}

void MoveCamera(const UserInput& input, Camera& camera, float frameTime) {
	if (input.lctrl) {
		float w = 0.6f;
		if (input.rightPressed) {
			camera.RotateY(w, frameTime);
		}
		if (input.leftPressed) {
			camera.RotateY(-w, frameTime);
		}
		if (input.upPressed) {
			camera.RotateX(w, frameTime);
		}
		if (input.downPressed) {
			camera.RotateX(-w, frameTime);
		}
	}
	else {
		if (input.rightPressed) {
			camera.StepX(frameTime);
		}
		if (input.leftPressed) {
			camera.StepX(-frameTime);
		}
		if (input.upPressed) {
			camera.StepZ(frameTime);
		}
		if (input.downPressed) {
			camera.StepZ(-frameTime);
		}
	}
}

int main(int, char**) {
	// Setup SDL
	SDL_Init(SDL_INIT_VIDEO);
	// Get Versions
	SDL_version comp;
	SDL_version linked;
	SDL_VERSION(&comp);
	SDL_VERSION(&linked);
	//Ask SDL to get a recent version of OpenGL (3.2 or greater)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	//Create a window (offsetx, offsety, width, height, flags)
	SDL_Window* window = SDL_CreateWindow("CSCI5611 Proj2", 100, 100,
		screenDetails.width, screenDetails.height, SDL_WINDOW_OPENGL);
	if (!window) {
		std::cout << "Could not create window: %s\n" << SDL_GetError();
		return 1; //Exit as SDL failed
	}
	float aspect = screenDetails.width / (float)screenDetails.height; //aspect ratio needs update on resize
	SDL_GLContext context = SDL_GL_CreateContext(window); //Bind OpenGL to the window

	// Initialize OpenGL
	if (gladLoadGLLoader(SDL_GL_GetProcAddress)) {
	}
	else {
		printf("ERROR: Failed to initialize OpenGL context.\n");
		return -1;
	}

	Renderer renderer = Renderer();
	Camera camera(1, 1, Pos3F(0, 0, 3), Vec3F(0, 0, -1), Vec3F(0, 1, 0));
	camera.SetAspect((float)screenDetails.width, (float)screenDetails.height);

	// Main Loop
	SDL_Event windowEvent;

	auto lastFrameTime = std::chrono::high_resolution_clock::now();
	const std::chrono::duration<float> targetFrameTime(0.01);

	UserInput input;
	TestScene scene;

	while (!input.quit) {
		// Keyboard events
		int newSceneId = GetInput(input, windowEvent);
		if (input.lctrl) {
			input.lctrl = false;
			scene.Step();
		}

		if (newSceneId != -1) {
			camera.SetAspect((float)screenDetails.width, (float)screenDetails.height);
		}

		auto thisFrameTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float>dt = thisFrameTime - lastFrameTime;

		renderer.StartFrame();
		renderer.SetCamera(camera);
		float frameTime = dt.count();

		MoveCamera(input, camera, frameTime);
		renderer.SetCamera(camera);

		scene.Update(frameTime);
		scene.Render(renderer);

		renderer.FinalizeFrame();

		lastFrameTime = thisFrameTime;

		SDL_GL_SwapWindow(window); //Double buffering
	}

	return 0;
}