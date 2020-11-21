#pragma once
#include <windows.h>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define FULLSCREEN false
#define VSYNC true
#define TITLE "Gomu Gomu Engine"

#define DEGTORAD M_PI / 180.0