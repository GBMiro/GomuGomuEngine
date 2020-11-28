#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow();

	// Destructor
	virtual ~ModuleWindow();

	// Called before quitting
	bool Init();

	// Called before quitting
	bool CleanUp();

	float getBrightness() const;

	void setFlag(SDL_WindowFlags flag, bool state);

	void setBrightness(float brightness);

	void setWindowSize(int width, int height);

public:
	//The window we'll be rendering to
	SDL_Window* window = NULL;

	//The surface contained by the window
	SDL_Surface* screen_surface = NULL;

	SDL_DisplayMode desktopSize;
	int width;
	int height;
};

#endif // __ModuleWindow_H__