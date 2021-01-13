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
#include "ComponentTransform.h"
#include "ComponentCamera.h"

ModuleCamera::ModuleCamera() : posX(0.0f), posY(1.0f), posZ(-10.0f) {

	gameObject = new GameObject(nullptr, "EditorCamera");

	transform = (ComponentTransform*)gameObject->GetComponentOfType(ComponentType::CTTransform);
	if (!transform) {
		transform = (ComponentTransform*)gameObject->CreateComponent(ComponentType::CTTransform);
	}
	transform->SetPosition(float3(posX, posY, posZ));
}

ModuleCamera::~ModuleCamera() {
}

bool ModuleCamera::Init() {
	camera = (ComponentCamera*)gameObject->CreateComponent(ComponentType::CTCamera);

	camera->GetFrustum().SetKind(FrustumSpaceGL, FrustumRightHanded);
	camera->GetFrustum().SetViewPlaneDistances(nearPlane, farPlane);
	camera->GetFrustum().SetHorizontalFovAndAspectRatio(DEGTORAD * fov, App->window->width / (float)App->window->height);
	camera->GetFrustum().SetPos(float3(posX, posY, posZ));
	camera->GetFrustum().SetFront(float3(float3::unitZ));
	camera->GetFrustum().SetUp(float3(float3::unitY));

	FocusOnSelected();

	return true;
}

update_status ModuleCamera::PreUpdate() {
	return UPDATE_CONTINUE;
}

update_status ModuleCamera::Update() {
	UpdateCamera();

	return UPDATE_CONTINUE;
}

update_status ModuleCamera::PostUpdate() {
	return UPDATE_CONTINUE;
}

bool ModuleCamera::CleanUp() {
	RELEASE(gameObject);
	return true;
}

void ModuleCamera::SetFOV(float fov) {
	Frustum frustum = camera->GetFrustum();
	frustum.SetHorizontalFovAndAspectRatio(DEGTORAD * fov, frustum.AspectRatio());
	this->fov = fov;
}

void ModuleCamera::SetAspectRatio(float aspectRatio) {
	camera->GetFrustum().SetHorizontalFovAndAspectRatio(camera->GetFrustum().HorizontalFov(), aspectRatio);
}

void ModuleCamera::SetPlanes(float zNear, float zFar) {
	camera->GetFrustum().SetViewPlaneDistances(zNear, zFar);
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
	Frustum frustum = camera->GetFrustum();
	frustum.SetPos(center + frustum.Front().Neg() * distance * 2);
	LookAt(center);
}

void ModuleCamera::OrbitCamera(float xOffset, float yOffset) {
	Frustum frustum = camera->GetFrustum();

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

	transform->SetPosition(camFocusVector + center);

	LookAt(center);
}

void ModuleCamera::LookAt(const float3& point) {

	float3 direction = (point - camera->GetFrustum().Pos()).Normalized();
	RotateCamera(float3x3::LookAt(camera->GetFrustum().Front(), direction, camera->GetFrustum().Up(), float3::unitY));
	pitch = 0.0f;
}

float4x4 ModuleCamera::GetProjectionMatrix() const {
	return camera->GetFrustum().ProjectionMatrix();
}

float4x4 ModuleCamera::GetViewMatrix() const {

	return float4x4(camera->GetFrustum().ViewMatrix());
}

void ModuleCamera::GetPlanes(float* zNear, float* zFar) const {
	Frustum frustum = camera->GetFrustum();
	*zNear = frustum.NearPlaneDistance();
	*zFar = frustum.FarPlaneDistance();
}

void ModuleCamera::UpdateCamera() {

	float speed = cameraSpeed;
	float cameraRotationSpeed = angleSpeed;


	ProcessKeyboardInput(App->GetDeltaTime(), speed, cameraRotationSpeed);
	ProcessMouseInput(App->GetDeltaTime());
}

void ModuleCamera::RotateCamera(const float3x3& rotationMatrix) {
	vec oldFront = camera->GetFrustum().Front().Normalized();
	camera->GetFrustum().SetFront(rotationMatrix * oldFront);

	vec oldUp = camera->GetFrustum().Up().Normalized();
	camera->GetFrustum().SetUp(rotationMatrix * oldUp);
}

void ModuleCamera::ProcessKeyboardInput(float deltaTime, float speed, float cameraRotationSpeed) {

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT) {
		speed *= 2;
		cameraRotationSpeed *= 2;
	}

	float movementSpeed = speed * deltaTime;

	if (App->input->GetKey(SDL_SCANCODE_F)) FocusOnSelected();
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) != KeyState::KEY_REPEAT) {
		float3 motion = float3::zero;
		if (App->input->GetKey(SDL_SCANCODE_W)) {
			motion += camera->GetFrustum().Front();
		}
		if (App->input->GetKey(SDL_SCANCODE_S)) {
			motion -= camera->GetFrustum().Front();
		}

		if (App->input->GetKey(SDL_SCANCODE_A)) {
			motion -= camera->GetFrustum().WorldRight();
		}
		if (App->input->GetKey(SDL_SCANCODE_D)) {
			motion += camera->GetFrustum().WorldRight();
		}
		if (App->input->GetKey(SDL_SCANCODE_Q)) {
			motion += camera->GetFrustum().Up();
		}
		if (App->input->GetKey(SDL_SCANCODE_E)) {
			motion -= camera->GetFrustum().Up();
		}

		transform->SetPosition(camera->GetFrustum().Pos() + motion * movementSpeed);


	}

	//Rotation
	if (App->input->GetKey(SDL_SCANCODE_LEFT)) {
		yaw += cameraRotationSpeed * deltaTime;
		float3x3 rotationMatrix = camera->GetFrustum().ViewMatrix().RotatePart();
		rotationMatrix = rotationMatrix.RotateY(DEGTORAD * deltaTime * cameraRotationSpeed);

		RotateCamera(rotationMatrix);
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT)) {
		yaw -= cameraRotationSpeed * deltaTime;
		float3x3 rotationMatrix = camera->GetFrustum().ViewMatrix().RotatePart();
		rotationMatrix = rotationMatrix.RotateY(DEGTORAD * -deltaTime * cameraRotationSpeed);

		RotateCamera(rotationMatrix);
	}

	if (App->input->GetKey(SDL_SCANCODE_UP)) {
		float oldPitch = pitch;
		if (pitch > 89.0f) pitch = 89.0f;
		pitch += cameraRotationSpeed * deltaTime;
		float3x3 rotationMatrix = camera->GetFrustum().ViewMatrix().RotatePart();
		rotationMatrix = rotationMatrix.RotateAxisAngle(camera->GetFrustum().WorldRight(), DEGTORAD * (pitch - oldPitch));

		RotateCamera(rotationMatrix);
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN)) {
		float oldPitch = pitch;
		if (pitch < -89.0f) pitch = -89.0f;
		pitch -= cameraRotationSpeed * deltaTime;
		float3x3 rotationMatrix = camera->GetFrustum().ViewMatrix().RotatePart();
		rotationMatrix = rotationMatrix.RotateAxisAngle(camera->GetFrustum().WorldRight(), DEGTORAD * (pitch - oldPitch));

		RotateCamera(rotationMatrix);
	}
}

void ModuleCamera::ProcessMouseInput(float deltaTime) {
	// Guide: https://learnopengl.com/Getting-started/Camera

	iPoint motion = App->input->GetMouseMotion();
	const float sensitivity = 0.1f;
	float xOffset = motion.x * angleSpeed * deltaTime;
	float yOffset = motion.y * angleSpeed * deltaTime;


	if (App->input->GetKey(SDL_SCANCODE_LALT)) {
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT) {
			OrbitCamera(xOffset, yOffset);
		}

		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT) {
			float totalMotion = motion.x + motion.y;
			transform->SetPosition(transform->localPosition + camera->GetFrustum().Front() * totalMotion * 2.0 * App->GetDeltaTime());
		}

	} else {
		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT) {
			if (xOffset != 0) {
				yaw += xOffset;
				float3x3 rotationMatrix = GetFrustum().ViewMatrix().RotatePart();
				rotationMatrix = rotationMatrix.RotateY(DEGTORAD * -xOffset);
				RotateCamera(rotationMatrix);
			}

			if (yOffset != 0) {
				float oldPitch = pitch;
				if (pitch > 89.0f) pitch = 89.0f;
				if (pitch < -89.0f) pitch = -89.0f;
				pitch += yOffset;
				float3x3 rotationMatrix = GetFrustum().ViewMatrix().RotatePart();
				rotationMatrix = rotationMatrix.RotateAxisAngle(GetFrustum().WorldRight(), (DEGTORAD * -(pitch - oldPitch)));
				RotateCamera(rotationMatrix);
			}
		}
	}
	int wheelYOffset = App->input->GetMouseWheel();

	if (wheelYOffset != 0) {
		fov -= wheelYOffset * zoomSpeed;
		if (fov < 1.0f) fov = 1.0f;
		if (fov > MaxFOV) fov = MaxFOV;
		SetFOV(fov);
	}
}


float3 ModuleCamera::GetUpVector() const { return camera->GetFrustum().Up(); }
float3 ModuleCamera::GetRightVector() const { return camera->GetFrustum().WorldRight(); }
float3 ModuleCamera::GetFrontVector() const { return camera->GetFrustum().Front(); }
float3 ModuleCamera::GetCameraPosition() const { return camera->GetFrustum().Pos(); }
float ModuleCamera::GetAspectRatio() const { return camera->GetFrustum().AspectRatio(); }
float ModuleCamera::GetFOV() const { return fov; }
const Frustum& ModuleCamera::GetFrustum() { return camera->GetFrustum(); }