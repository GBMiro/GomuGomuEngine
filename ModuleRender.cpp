#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ShadingProgram.h"
#include "ModuleCamera.h"
#include "ComponentCamera.h"
#include "ComponentMeshRenderer.h"
#include "ModuleTextures.h"
#include "ModuleDebugDraw.h"
#include "debug_draw/debugdraw.h"
#include "SDL/include/SDL.h"
#include "GL/glew.h"
#include "MathGeoLib/Geometry/Frustum.h"
#include "ModuleScene.h"
#include "Brofiler/include/Brofiler.h"
#include "Mesh.h"
#include "Quadtree.h"
#include "GameObject.h"
#include "Leaks.h"

ModuleRender::ModuleRender() {
	gridColor = float3(0.f, 0.f, 1.f);
	backgroundColor = float3(0.1f, 0.1f, 0.1f);
	cullingCamera = nullptr;
	frustumCulling = false;
}

// Destructor
ModuleRender::~ModuleRender() {

}



void __stdcall OurOpenGLErrorFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
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
bool ModuleRender::Init() {
	LOG("Creating Renderer context");

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	context = SDL_GL_CreateContext(App->window->window);

	GLenum err = glewInit();

	if (SDL_GL_SetSwapInterval(VSYNC) < 0)
		LOG("Warning: Unable to set VSync! SDL Error: %s", SDL_GetError());

	// ... check for errors
	LOG("Using Glew %s", glewGetString(GLEW_VERSION));

	LOG("Vendor: %s", glGetString(GL_VENDOR));
	LOG("Renderer: %s", glGetString(GL_RENDERER));
	LOG("OpenGL version supported %s", glGetString(GL_VERSION));
	LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	/*
		//OpenGL Debugg
	#ifdef _DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

		glDebugMessageCallback(OurOpenGLErrorFunction, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
	#endif // _DEBUG
	*/

	defaultProgram = new ShadingProgram("./Shaders/default_vertex.glsl", "./Shaders/default_fragment.glsl");
	unLitProgram = new ShadingProgram("./Shaders/unlit.vs", "./Shaders/unlit.fs");
	litProgram = new ShadingProgram("./Shaders/litVertex.glsl", "./Shaders/litFragment.glsl");


	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// create a color attachment texture
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, App->window->width, App->window->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, App->window->width, App->window->height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

update_status ModuleRender::PreUpdate() {
	GLsizei w, h = 0;
	SDL_GetWindowSize(App->window->window, &w, &h);
	glViewport(0, 0, w, h);
	glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 0.1f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glEnable(GL_DEPTH_TEST);
	glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 0.1f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update() {

	//Draw Grid
	dd::xzSquareGrid(-10, 10, 0.0f, 1.0f, gridColor);
	GLsizei h, w;

	SDL_GetWindowSize(App->window->window, &w, &h);

	//DebugDraw call
	App->debugDraw->Draw(App->camera->GetViewMatrix(), App->camera->GetProjectionMatrix(), w, h);

	//Now we pass on to draw on the viewport
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);

	return UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate() {
	BROFILER_CATEGORY("Post Update Render", Profiler::Color::Orchid)
		SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp() {
	LOG("Destroying renderer");
	//Destroy window

	RELEASE(defaultProgram);
	RELEASE(litProgram);
	RELEASE(unLitProgram);
	SDL_GL_DeleteContext(context);
	return true;
}

void ModuleRender::WindowResized(unsigned width, unsigned height) {


	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, App->window->width, App->window->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);


	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, App->window->width, App->window->height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void* ModuleRender::GetContext() const {
	return context;
}

unsigned ModuleRender::GetDefaultProgram() const {
	return defaultProgram->GetID();
}

void ModuleRender::GetGridColor(float* color) const {
	color[0] = gridColor.x;
	color[1] = gridColor.y;
	color[2] = gridColor.z;
}

void ModuleRender::GetBackgroundColor(float* color) const {
	color[0] = backgroundColor.x;
	color[1] = backgroundColor.y;
	color[2] = backgroundColor.z;
}

void ModuleRender::SetGridColor(const float* color) {
	gridColor = float3(color[0], color[1], color[2]);
}

void ModuleRender::SetBackgroundColor(const float* color) {
	backgroundColor = float3(color[0], color[1], color[2]);
}

const float3& ModuleRender::GetDefaultColor() {
	return defaultColor;
}

bool ModuleRender::GetUseToneMapping() {
	return useToneMapping;
}

bool ModuleRender::GetUseGammaCorrection() {
	return useGammaCorrection;
}

void ModuleRender::DefaultColor(float3 newColor) {
	defaultColor = newColor;
}

void ModuleRender::SetUseToneMapping(bool should) {
	useToneMapping = should;
}

void ModuleRender::SetUseGammaCorrection(bool should) {
	useGammaCorrection = should;
}

const ComponentCamera* ModuleRender::GetCullingCamera()const {
	return cullingCamera;
}

void  ModuleRender::SetCullingCamera(ComponentCamera* newCam) {
	cullingCamera = newCam;
	SetFrustumCulling(cullingCamera == nullptr ? false : true);
}

bool  ModuleRender::GetFrustumCulling()const {
	return frustumCulling;

}
void  ModuleRender::SetFrustumCulling(bool use) {
	frustumCulling = use;
}


/// <summary>
/// This method gets all QuadTreeNodes that intersect with the cullingFrustum, GameObjects contained
/// within said QuadtreeNodes are then checked for an intersection between the gameObjects AABBand the frustum
/// </summary>
/// <param name="objs">vector of GameObject* passed by reference</param>
/// <param name="qtn"></param>
void ModuleRender::CheckCullingFrustumIntersectionWithQuadTree(std::vector<GameObject*>& objs, const QuadtreeNode& qtn)const {
	if (cullingCamera->GetFrustum().Intersects(qtn.boundingBox)) {
		for (std::vector<GameObject*>::const_iterator it = qtn.gameObjects.begin(); it != qtn.gameObjects.end(); ++it) {
			if (cullingCamera->GetFrustum().Intersects((*it)->GetAABB())) {
				objs.push_back(*it);
			}
		}
		for (std::vector<QuadtreeNode>::const_iterator it = qtn.childNodes.begin(); it != qtn.childNodes.end(); ++it) {
			CheckCullingFrustumIntersectionWithQuadTree(objs, *it);
		}
	}
}
