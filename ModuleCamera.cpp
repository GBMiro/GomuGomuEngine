#include "ModuleCamera.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleEditor.h"
#include "MathGeoLib/Geometry/AABB.h"
#include "MathGeoLib/Geometry/Sphere.h"
#include "GL/glew.h"
#include "SDL/include/SDL.h"
#include "GameObject.h"
#include "Leaks.h"

ModuleCamera::ModuleCamera() : posX(0.0f), posY(1.0f), posZ(-10.0f) {

}

ModuleCamera::~ModuleCamera() {
}

bool ModuleCamera::Init() {

	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetViewPlaneDistances(nearPlane, farPlane);
	frustum.SetHorizontalFovAndAspectRatio(DEGTORAD * fov, App->window->width / (float)App->window->height);
	frustum.SetPos(float3(posX, posY, posZ));
	frustum.SetFront(float3(float3::unitZ));
	frustum.SetUp(float3(float3::unitY));

	FocusOnSelected();

	return true;
}

update_status ModuleCamera::PreUpdate() {
	return UPDATE_CONTINUE;
}

update_status ModuleCamera::Update() {
	updateCamera();

	return UPDATE_CONTINUE;
}

update_status ModuleCamera::PostUpdate() {
	return UPDATE_CONTINUE;
}

bool ModuleCamera::CleanUp() {
	return true;
}

void ModuleCamera::SetFOV(float fov) {
	frustum.SetHorizontalFovAndAspectRatio(DEGTORAD * fov, frustum.AspectRatio());
	this->fov = fov;
}

void ModuleCamera::SetAspectRatio(float aspectRatio) {
	frustum.SetHorizontalFovAndAspectRatio(frustum.HorizontalFov(), aspectRatio);
}

void ModuleCamera::setPlanes(float zNear, float zFar) {
	frustum.SetViewPlaneDistances(zNear, zFar);
	nearPlane = zNear;
	farPlane = zFar;
}

void ModuleCamera::FocusOnSelected() {
	GameObject* selection = App->editor->GetGameObjectSelected();
	float distance = 0;
	vec center = vec(0, 0, 0);
	if (selection) {
		AABB aaBB = selection->GetAABB();
		distance = (aaBB.MinimalEnclosingSphere().Diameter() / 2) / Abs(sin(DEGTORAD * fov / 2));
		center = aaBB.CenterPoint();
	}
	frustum.SetPos(center + frustum.Front().Neg() * distance * 2);
	LookAt(center);
}

void ModuleCamera::orbitCamera(float xOffset, float yOffset) {

	float3 up = frustum.Up().Normalized();
	float3 right = frustum.WorldRight().Normalized();

	vec center = vec(0, 0, 0);
	GameObject* selection = App->editor->GetGameObjectSelected();
	if (selection) center = selection->GetAABB().CenterPoint();

	float3 camFocusVector = frustum.Pos() - center;
	float3x3 rotationMatrixY = frustum.ViewMatrix().RotatePart();
	rotationMatrixY = rotationMatrixY.RotateAxisAngle(up, DEGTORAD * xOffset);
	float3x3 rotationMatrixX = frustum.ViewMatrix().RotatePart();
	rotationMatrixX = rotationMatrixX.RotateAxisAngle(right, DEGTORAD * yOffset);
	camFocusVector = camFocusVector * rotationMatrixX;
	camFocusVector = camFocusVector * rotationMatrixY;

	frustum.SetPos(camFocusVector + center);
	LookAt(center);
}

void ModuleCamera::LookAt(const float3& point) {
	float3 direction = (point - frustum.Pos()).Normalized();
	rotateCamera(float3x3::LookAt(frustum.Front(), direction, frustum.Up(), float3::unitY));
	pitch = 0.0f;
}

float4x4 ModuleCamera::getProjectionMatrix() const {
	return frustum.ProjectionMatrix();
}

float4x4 ModuleCamera::getViewMatrix() const {
	return float4x4(frustum.ViewMatrix());
}

void ModuleCamera::getPlanes(float* zNear, float* zFar) const {
	*zNear = frustum.NearPlaneDistance();
	*zFar = frustum.FarPlaneDistance();
}

void ModuleCamera::updateCamera() {

	float speed = cameraSpeed;
	float cameraRotationSpeed = angleSpeed;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT)) {
		speed *= 2;
		cameraRotationSpeed *= 2;
	}
	processKeyboardInput(App->GetDeltaTime(), speed, cameraRotationSpeed);
	processMouseInput(App->GetDeltaTime());
}

void ModuleCamera::rotateCamera(const float3x3& rotationMatrix) {
	vec oldFront = frustum.Front().Normalized();
	frustum.SetFront(rotationMatrix * oldFront);

	vec oldUp = frustum.Up().Normalized();
	frustum.SetUp(rotationMatrix * oldUp);
}

void ModuleCamera::processKeyboardInput(float deltaTime, float speed, float cameraRotationSpeed) {
	float movementSpeed = speed * deltaTime;
	if (App->input->GetKey(SDL_SCANCODE_F)) FocusOnSelected();
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT) {
		if (App->input->GetKey(SDL_SCANCODE_W)) frustum.SetPos(frustum.Pos() + frustum.Front() * movementSpeed);
		if (App->input->GetKey(SDL_SCANCODE_S)) frustum.SetPos(frustum.Pos() - frustum.Front() * movementSpeed);
		if (App->input->GetKey(SDL_SCANCODE_A)) frustum.SetPos(frustum.Pos() - frustum.WorldRight() * movementSpeed);
		if (App->input->GetKey(SDL_SCANCODE_D)) frustum.SetPos(frustum.Pos() + frustum.WorldRight() * movementSpeed);
		if (App->input->GetKey(SDL_SCANCODE_Q)) frustum.SetPos(frustum.Pos() + float3::unitY * movementSpeed);
		if (App->input->GetKey(SDL_SCANCODE_E)) frustum.SetPos(frustum.Pos() - float3::unitY * movementSpeed);
	}

	//Rotation
	if (App->input->GetKey(SDL_SCANCODE_LEFT)) {
		yaw += cameraRotationSpeed * deltaTime;
		float3x3 rotationMatrix = frustum.ViewMatrix().RotatePart();
		rotationMatrix = rotationMatrix.RotateY(DEGTORAD * deltaTime * cameraRotationSpeed);

		rotateCamera(rotationMatrix);
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT)) {
		yaw -= cameraRotationSpeed * deltaTime;
		float3x3 rotationMatrix = frustum.ViewMatrix().RotatePart();
		rotationMatrix = rotationMatrix.RotateY(DEGTORAD * -deltaTime * cameraRotationSpeed);

		rotateCamera(rotationMatrix);
	}

	if (App->input->GetKey(SDL_SCANCODE_UP)) {
		float oldPitch = pitch;
		if (pitch > 89.0f) pitch = 89.0f;
		pitch += cameraRotationSpeed * deltaTime;
		float3x3 rotationMatrix = frustum.ViewMatrix().RotatePart();
		rotationMatrix = rotationMatrix.RotateAxisAngle(frustum.WorldRight(), DEGTORAD * (pitch - oldPitch));

		rotateCamera(rotationMatrix);
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN)) {
		float oldPitch = pitch;
		if (pitch < -89.0f) pitch = -89.0f;
		pitch -= cameraRotationSpeed * deltaTime;
		float3x3 rotationMatrix = frustum.ViewMatrix().RotatePart();
		rotationMatrix = rotationMatrix.RotateAxisAngle(frustum.WorldRight(), DEGTORAD * (pitch - oldPitch));

		rotateCamera(rotationMatrix);
	}
}

void ModuleCamera::processMouseInput(float deltaTime) {
	// Guide: https://learnopengl.com/Getting-started/Camera

	iPoint motion = App->input->GetMouseMotion();
	const float sensitivity = 0.1f;
	float xOffset = motion.x * angleSpeed * deltaTime;
	float yOffset = motion.y * angleSpeed * deltaTime;

	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT) {
		if (xOffset != 0) {
			yaw += xOffset;
			float3x3 rotationMatrix = frustum.ViewMatrix().RotatePart();
			rotationMatrix = rotationMatrix.RotateY(DEGTORAD * -xOffset);
			rotateCamera(rotationMatrix);
		}

		if (yOffset != 0) {
			float oldPitch = pitch;
			if (pitch > 89.0f) pitch = 89.0f;
			if (pitch < -89.0f) pitch = -89.0f;
			pitch += yOffset;
			float3x3 rotationMatrix = frustum.ViewMatrix().RotatePart();
			rotationMatrix = rotationMatrix.RotateAxisAngle(frustum.WorldRight(), (DEGTORAD * -(pitch - oldPitch)));
			rotateCamera(rotationMatrix);
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_LALT) && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT)) orbitCamera(xOffset, yOffset);

	int wheelYOffset = App->input->GetMouseWheel();

	if (wheelYOffset != 0) {
		fov -= wheelYOffset * zoomSpeed;
		if (fov < 1.0f) fov = 1.0f;
		if (fov > MaxFOV) fov = MaxFOV;
		SetFOV(fov);
	}
}
