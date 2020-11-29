#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleProgram.h"
#include "ModuleCamera.h"
#include "ModuleTextures.h"
#include "ModuleDebugDraw.h"
#include "debug_draw/debugdraw.h"
#include "SDL/include/SDL.h"
#include "GL/glew.h"
#include "MathGeoLib/Geometry/Frustum.h"
#include "Model.h"
#include "Leaks.h"

ModuleRender::ModuleRender() {
	gridColor = float3(0.f, 0.f, 1.f);
	backgroundColor = float3(0.1f, 0.1f, 0.1f);
}

// Destructor
ModuleRender::~ModuleRender()
{
}

void __stdcall OurOpenGLErrorFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	const char* tmp_source = "", * tmp_type = "", * tmp_severity = "";
	switch (source) {
	case GL_DEBUG_SOURCE_API: tmp_source = "API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: tmp_source = "Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: tmp_source = "Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY: tmp_source = "Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION: tmp_source = "Application"; break;
	case GL_DEBUG_SOURCE_OTHER: tmp_source = "Other"; break;
	};
	switch (type) {
	case GL_DEBUG_TYPE_ERROR: tmp_type = "Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: tmp_type = "Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: tmp_type = "Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY: tmp_type = "Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE: tmp_type = "Performance"; break;
	case GL_DEBUG_TYPE_MARKER: tmp_type = "Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP: tmp_type = "Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP: tmp_type = "Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER: tmp_type = "Other"; break;
	};
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: tmp_severity = "high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM: tmp_severity = "medium"; break;
	case GL_DEBUG_SEVERITY_LOW: tmp_severity = "low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: tmp_severity = "notification"; break;
	};
	LOG("<Source:%s> <Type:%s> <Severity:%s> <ID:%d> <Message:%s>\n", tmp_source, tmp_type, tmp_severity, id, message);
}

// Called before render is available
bool ModuleRender::Init()
{
	LOG("Creating Renderer context");

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	context = SDL_GL_CreateContext(App->window->window);

	GLenum err = glewInit();

	// ... check for errors
	LOG("Using Glew %s", glewGetString(GLEW_VERSION));

	LOG("Vendor: %s", glGetString(GL_VENDOR));
	LOG("Renderer: %s", glGetString(GL_RENDERER));
	LOG("OpenGL version supported %s", glGetString(GL_VERSION));
	LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	//OpenGL Debugg
#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	glDebugMessageCallback(OurOpenGLErrorFunction, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
#endif // _DEBUG


	char* vtx_shader = App->program->loadShaderSource("./Shaders/default_vertex.glsl");
	char* frg_shader = App->program->loadShaderSource("./Shaders/default_fragment.glsl");

	unsigned vtxId = App->program->compileShader(GL_VERTEX_SHADER, vtx_shader);
	unsigned frgId = App->program->compileShader(GL_FRAGMENT_SHADER, frg_shader);

	programId = App->program->createProgram(vtxId, frgId);
	
	RELEASE(vtx_shader);
	RELEASE(frg_shader);

	App->model->Load("./Resources/Models/BakerHouse.fbx");
	return true;
}

update_status ModuleRender::PreUpdate()
{
	GLsizei w, h = 0;
	SDL_GetWindowSize(App->window->window, &w, &h);
	glViewport(0, 0, w, h);
	glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 0.1f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update()
{
	//OpenGLExercise
	dd::axisTriad(float4x4::identity, 0.1f, 1.0f);
	dd::xzSquareGrid(-10, 10, 0.0f, 1.0f, gridColor);
	float4x4 proj = App->camera->getProjectionMatrix();
	float4x4 view = App->camera->getViewMatrix();
	GLsizei h, w;
	SDL_GetWindowSize(App->window->window, &w, &h);
	App->debugDraw->Draw(view, proj, w, h);

	App->model->Draw();
	return UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{
	SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	LOG("Destroying renderer");
	//Destroy window
	SDL_GL_DeleteContext(context);
	return true;
}

void ModuleRender::WindowResized(unsigned width, unsigned height)
{
}

void* ModuleRender::getContext() const {
	return context;
}

unsigned ModuleRender::getProgram() const {
	return programId;
}

void ModuleRender::getGridColor(float* color) const
{
	color[0] = gridColor.x;
	color[1] = gridColor.y;
	color[2] = gridColor.z;
}

void ModuleRender::getBackgroundColor(float* color) const {
	color[0] = backgroundColor.x;
	color[1] = backgroundColor.y;
	color[2] = backgroundColor.z;
}

void ModuleRender::setGridColor(const float* color) {
	gridColor = float3(color[0], color[1], color[2]);
}

void ModuleRender::setBackgroundColor(const float* color) {
	backgroundColor = float3(color[0], color[1], color[2]);
}


