#pragma warning(push, 0)
#include "glad/glad.h"
#include "sdl/SDL.h"
#pragma warning(pop)

#include <iostream>
#include <chrono>

#include "rendering/Camera.h"
#include "rendering/Renderer.h"
#include "scene/RobotScene1.h"
#include "scene/RobotScene2.h"
#include "scene/CrowdScene3.h"
#include "scene/CrowdScene4.h"
#include "scene/CrowdScene5.h"

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
	bool toggleJointLimits = false;
	float mouseX = 0.0f;
	float mouseY = 0.0f;
	bool mouseMoved = false;
	bool mousePressed = false;
	bool mouseDepressed = false;
	bool toggleShowNodes = false;
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
		if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_3)
			scene = 3;
		if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_4)
			scene = 4;
		if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_5)
			scene = 5;
		if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_j)
			input.toggleJointLimits = true;
		if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_n)
			input.toggleShowNodes = true;
		if (windowEvent.type == SDL_MOUSEMOTION) {
			input.mouseMoved = true;
			input.mouseX = (float)(windowEvent.motion.x) / screenDetails.width;
			input.mouseY = (float)(windowEvent.motion.y) / screenDetails.height;
		}
		if (windowEvent.type == SDL_MOUSEBUTTONDOWN && windowEvent.button.button == SDL_BUTTON_LEFT) {
			input.mousePressed = true;
		}
		if (windowEvent.type == SDL_MOUSEBUTTONUP && windowEvent.button.button == SDL_BUTTON_LEFT) {
			input.mouseDepressed = true;
		}
	}

	return scene;
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
	//Camera camera(1, 1, Pos3F(0, 0, 15), Vec3F(0, 0, -1), Vec3F(0, 1, 0));
	camera.SetAspect((float)screenDetails.width, (float)screenDetails.height);

	// Main Loop
	SDL_Event windowEvent;

	auto lastFrameTime = std::chrono::high_resolution_clock::now();
	const std::chrono::duration<float> targetFrameTime(0.01);

	UserInput input;
	RobotScene1 scene1;
	RobotScene2 scene2;
	CrowdScene3 scene3;
	CrowdScene4 scene4;
	CrowdScene5 scene5;

	IScene* scene;
	scene = &scene1;

	IKArm::IgnoreJointLimits = true;
	input.toggleJointLimits = true; // So that it outputs to console on load

	std::cout << "Hit Left-Control to Progress Simulation\n";

	while (!input.quit) {
		// Keyboard events
		int newSceneId = GetInput(input, windowEvent);

		if (newSceneId != -1) {
			camera.SetAspect((float)screenDetails.width, (float)screenDetails.height);
			if (newSceneId == 1 || newSceneId == 2) {
				camera = Camera(1, 1, Pos3F(0, 0, 3), Vec3F(0, 0, -1), Vec3F(0, 1, 0));
			}
			else {
				camera = Camera(1, 1, Pos3F(0, 0, 15), Vec3F(0, 0, -1), Vec3F(0, 1, 0));
			}
			
			if (newSceneId == 1) {
				scene1 = RobotScene1();
				scene = &scene1;
			}
			else if (newSceneId == 2) {
				scene2 = RobotScene2();
				scene = &scene2;
			}
			else if (newSceneId == 3) {
				scene3 = CrowdScene3();
				scene = &scene3;
			}
			else if (newSceneId == 4) {
				scene4 = CrowdScene4();
				scene = &scene4;
			}
			else if (newSceneId == 5) {
				scene5 = CrowdScene5();
				scene = &scene5;
			}
		}

		if (input.toggleJointLimits) {
			IKArm::IgnoreJointLimits = !IKArm::IgnoreJointLimits;
			input.toggleJointLimits = false;
			std::cout << "Using Joint Limits: " << !IKArm::IgnoreJointLimits << std::endl;
		}

		if (input.toggleShowNodes) {
			if (CrowdMap::RenderLines) {
				CrowdMap::RenderLines = false;
			}
			else if (CrowdMap::RenderNodes) {
				CrowdMap::RenderNodes = false;
			}
			else {
				CrowdMap::RenderLines = true;
				CrowdMap::RenderNodes = true;
			}
			input.toggleShowNodes = false;
		}

		if (input.mouseMoved) {
			input.mouseMoved = false;
			scene->MouseMove(input.mouseX, input.mouseY);
		}

		if (input.mousePressed) {
			scene->MouseDown(input.mouseX, input.mouseY);
			input.mousePressed = false;
		}

		if (input.mouseDepressed) {
			scene->MouseUp();
			input.mouseDepressed = false;
		}

		auto thisFrameTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float>dt = thisFrameTime - lastFrameTime;

		renderer.StartFrame();
		renderer.SetCamera(camera);
		float frameTime = dt.count();

		if (input.lctrl) {
			input.lctrl = false;
			scene->Update(frameTime);
			
		}

		//MoveCamera(input, camera, frameTime);
		renderer.SetCamera(camera);

		//scene.Update(frameTime);
		scene->Render(renderer);
		//crowdSim.Update(frameTime);
		//crowdSim.Render(renderer);

		renderer.FinalizeFrame();

		lastFrameTime = thisFrameTime;

		SDL_GL_SwapWindow(window); //Double buffering
	}

	return 0;
}