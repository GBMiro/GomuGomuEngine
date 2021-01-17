#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;
class ModuleWindow : public Module {
public:
	//The window we'll be rendering to
	SDL_Window* window = NULL;

	//The surface contained by the window
	SDL_Surface* screenSurface = NULL;

	SDL_DisplayMode desktopSize;
private:
	int width;
	int height;
public:

	ModuleWindow();

	// Destructor
	virtual ~ModuleWindow();

	// Called before quitting
	bool Init();

	// Called before quitting
	bool CleanUp();

	float GetBrightness() const;

	void SetFlag(SDL_WindowFlags flag, bool state);

	void SetBrightness(float brightness);

	void SetWindowSize(int width, int height);

	void SetWidth(float newW) { width = newW; }
	void SetHeight(float newH) { height = newH; }

	float GetWidth()const { return width; }
	float GetHeight()const { return height; }

};

#endif // __ModuleWindow_H__