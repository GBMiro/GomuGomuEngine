#include "ModuleEditor.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_opengl3.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "MonitorWindow.h"
#include "ConfigurationWindow.h"
#include "Model.h"
#include "Leaks.h"

ModuleEditor::ModuleEditor() {
	windows.push_back(monitor = new MonitorWindow("Monitor", 0));
	windows.push_back(configuration = new ConfigurationWindow("Configuration", 1));
}

ModuleEditor::~ModuleEditor() {
	for (unsigned i = 0; i < windows.size(); ++i) {
		delete windows[i];
	}
}

bool ModuleEditor::Init()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Needed to be able to take windows outside engine
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->getContext());
	ImGui_ImplOpenGL3_Init();

	return true;
}

update_status ModuleEditor::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update()
{
	showMenus();
	Draw();
	ImGui::ShowDemoWindow();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate()
{
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)	{ // Needed to be able to take windows outside engine
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}

	return UPDATE_CONTINUE;
}

bool ModuleEditor::cleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}

void ModuleEditor::showMenus() {

	static bool showConfiguration = true;
	static bool showCameraWindow, showMetricsWindow, showLogWindow, showPropertiesWindow = false;

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Quit", "ESC")) {}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Windows")) {
			if (ImGui::MenuItem("Camera", NULL, &showCameraWindow));
			if (ImGui::MenuItem("Metrics", NULL, &monitor->active));
			if (ImGui::MenuItem("Log", NULL, &showLogWindow));
			if (ImGui::MenuItem("Configuration", NULL, &configuration->active));
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("About")) {
			if (ImGui::MenuItem("Visit github", "url")) {}
			if (ImGui::MenuItem("ABout GomuGomuEngine", "Open mini window")) {}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void ModuleEditor::Draw() {
	for (unsigned i = 0; i < windows.size(); ++i) {
		windows[i]->Draw();
	}
}

void ModuleEditor::registerFPS(float deltaTime) {
	monitor->addData(deltaTime);
}