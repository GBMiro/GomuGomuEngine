#include "ModuleEditor.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_opengl3.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"

ModuleEditor::ModuleEditor() {
	showAboutWindow = true;
	showLogWindow = false;
	showHardwareWindow = false;
}

ModuleEditor::~ModuleEditor()
{
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
	windows();
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

void ModuleEditor::windows()
{
	if (showAboutWindow) {
		ImGui::Begin("About");
		ImGui::Text("This is a trial");
		ImGui::Checkbox("Hardware Window", &showHardwareWindow);
		ImGui::Checkbox("Log Window", &showLogWindow);
		ImGui::End();
	}

	if (showLogWindow) {
		ImGui::Begin("Log", &showLogWindow);
		ImGui::Text("This is the log window");
		ImGui::End();
	}

	if (showHardwareWindow) {
		ImGui::Begin("Hardware", &showHardwareWindow);
		ImGui::Text("This is the hardware window");
		ImGui::End();
	}
}
