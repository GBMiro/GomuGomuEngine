#include "ModuleEditor.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_opengl3.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "Leaks.h"

ModuleEditor::ModuleEditor() {
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
	showMenus();
	//ImGui::ShowDemoWindow();
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
	static bool showCameraWindow, showMetricsWindow, showLogWindow = true;

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Quit", "ESC")) {}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Windows")) {
			if (ImGui::MenuItem("Camera", NULL, &showCameraWindow));
			if (ImGui::MenuItem("Metrics", NULL, &showMetricsWindow));
			if (ImGui::MenuItem("Log", NULL, &showLogWindow));
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("About")) {
			if (ImGui::MenuItem("Visit github", "url")) {}
			if (ImGui::MenuItem("ABout GomuGomuEngine", "Open mini window")) {}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	// Window settings
	static bool fullscreen, windowBorderless = false;
	static bool windowResizable = true;
	static int width = SCREEN_WIDTH;
	static int height = SCREEN_HEIGHT;
	static float brightness = App->window->getBrightness();

	if (showConfiguration) {
		ImGui::Begin("Configuration");
		if (ImGui::CollapsingHeader("Input")) {}
		if (ImGui::CollapsingHeader("Renderer")) {}
		if (ImGui::CollapsingHeader("Textures")) {}
		if (ImGui::CollapsingHeader("Window")) {
			if (ImGui::Checkbox("Fullscreen", &fullscreen)) App->window->setFlag(SDL_WINDOW_FULLSCREEN, fullscreen); ImGui::SameLine();
			if (ImGui::Checkbox("Resizable", &windowResizable)) App->window->setFlag(SDL_WINDOW_RESIZABLE, windowResizable); ImGui::SameLine();
			if (ImGui::Checkbox("Borderless", &windowBorderless)) App->window->setFlag(SDL_WINDOW_BORDERLESS, windowBorderless);
			if (ImGui::SliderInt("Screen width", &width, 100, 1920)) App->window->setWindowSize(width, height);
			if (ImGui::SliderInt("Screen height", &height, 100, 1080)) App->window->setWindowSize(width, height);
			if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f, "%.3f")) App->window->setBrightness(brightness);
		}
		if (ImGui::CollapsingHeader("Metrics")) {}
		ImGui::End();
	}

	if (showLogWindow) {
		ImGui::Begin("Log", &showLogWindow);
		ImGui::Text("This is the log window");
		ImGui::End();
	}

	if (showMetricsWindow) {
		ImGui::Begin("Metrics", &showMetricsWindow);
		ImGui::Text("This will show metrics");
		ImGui::End();
	}

	if (showCameraWindow) {
		ImGui::Begin("Camera", &showCameraWindow);
		ImGui::Text("This will show camera options");
		ImGui::End();
	}
}
