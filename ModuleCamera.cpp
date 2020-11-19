#include "ModuleCamera.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "GL/glew.h"
#include "SDL/include/SDL.h"
#include "Leaks.h"

ModuleCamera::ModuleCamera()
{
	deltaTime = lastFrame =  0.0f;
	yaw = pitch = 0.0f;
}

ModuleCamera::~ModuleCamera()
{
}

bool ModuleCamera::Init()
{
	posX = 0.0f;
	posY = 1.0f;
	posZ = 4.0f;

	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetViewPlaneDistances(0.1f, 200.0f);
	frustum.SetHorizontalFovAndAspectRatio(DEGTORAD * 90.0f, 1.3f);
	frustum.SetPos(float3(posX, posY, posZ));//float3(1.5, 1, 2)); //Where the camera is
	frustum.SetFront(float3(-float3::unitZ));// float3(-1, -1, -1)); //Where the camera is looking
	frustum.SetUp(float3(float3::unitY));//float3(0, 1, 0)); //Where the up camera vector is pointing. This shows if camera is upside down or in another position
	
	//updateProjectionMatrix();

	return true;
}

update_status ModuleCamera::PreUpdate()
{
	return UPDATE_CONTINUE;
}

update_status ModuleCamera::Update()
{
	updateCamera();

	//float4x4 viewMatrix = frustum.ViewMatrix();

	//glMatrixMode(GL_MODELVIEW);
	//glLoadMatrixf(*(viewMatrix.Transposed()).v);

	return UPDATE_CONTINUE;
}

update_status ModuleCamera::PostUpdate()
{
	return UPDATE_CONTINUE;
}

bool ModuleCamera::CleanUp()
{
	return true;
}

void ModuleCamera::updateProjectionMatrix()
{
	float4x4 projectionGL = frustum.ProjectionMatrix().Transposed(); //<-- Important to transpose!
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(*projectionGL.v);
}

void ModuleCamera::SetFOV(float aspectRatio)
{
	frustum.SetHorizontalFovAndAspectRatio(frustum.HorizontalFov(), aspectRatio);
	//updateProjectionMatrix();
}

float4x4 ModuleCamera::getProjectionMatrix()
{
	return frustum.ProjectionMatrix();
}

float4x4 ModuleCamera::getViewMatrix()
{
	return float4x4(frustum.ViewMatrix());
}

void ModuleCamera::updateCamera()
{
	float currentFrame = SDL_GetPerformanceCounter(); //Should I use SDL_GetTicks()?
	deltaTime = ((currentFrame - lastFrame) * 1000 / SDL_GetPerformanceFrequency()) / 1000;
	lastFrame = currentFrame;
	
	float speed = cameraSpeed;
	float cameraRotationSpeed = angleSpeed;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT)) {
		speed *= 2;
		cameraRotationSpeed *= 2;
	}
	processKeyboardInput(deltaTime, speed, cameraRotationSpeed);
	processMouseInput(deltaTime);	
}

void ModuleCamera::rotateCamera(float3x3 &rotationMatrix)
{
	vec oldFront = frustum.Front().Normalized();
	frustum.SetFront(rotationMatrix * oldFront);

	vec oldUp = frustum.Up().Normalized();
	frustum.SetUp(rotationMatrix * oldUp);
}

void ModuleCamera::processKeyboardInput(float deltaTime, float speed, float cameraRotationSpeed) 
{
	float movementSpeed = speed * deltaTime;

	if (App->input->GetKey(SDL_SCANCODE_W)) frustum.SetPos(frustum.Pos() + frustum.Front() * movementSpeed);
	if (App->input->GetKey(SDL_SCANCODE_S)) frustum.SetPos(frustum.Pos() - frustum.Front() * movementSpeed);
	if (App->input->GetKey(SDL_SCANCODE_A)) frustum.SetPos(frustum.Pos() - frustum.WorldRight() * movementSpeed);
	if (App->input->GetKey(SDL_SCANCODE_D)) frustum.SetPos(frustum.Pos() + frustum.WorldRight() * movementSpeed);
	if (App->input->GetKey(SDL_SCANCODE_Q)) frustum.SetPos(frustum.Pos() + float3(float3::unitY) * movementSpeed);
	if (App->input->GetKey(SDL_SCANCODE_E)) frustum.SetPos(frustum.Pos() - float3(float3::unitY) * movementSpeed);

	//Rotation
	if (App->input->GetKey(SDL_SCANCODE_LEFT)) {
		yaw += cameraRotationSpeed * deltaTime; // Reset yaw when doing 360º
		float3x3 rotationMatrix = frustum.ViewMatrix().RotatePart();
		rotationMatrix = rotationMatrix.RotateY(DEGTORAD * deltaTime * cameraRotationSpeed);

		rotateCamera(rotationMatrix);
		LOG("Keyboard -> yaw: %f, pitch: %f", yaw, pitch);
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT)) {
		yaw -= cameraRotationSpeed * deltaTime; // Reset yaw when doing 360º
		float3x3 rotationMatrix = frustum.ViewMatrix().RotatePart();
		rotationMatrix = rotationMatrix.RotateY(DEGTORAD * -deltaTime * cameraRotationSpeed);

		rotateCamera(rotationMatrix);
		LOG("Keyboard -> yaw: %f, pitch: %f", yaw, pitch);
	}

	if (App->input->GetKey(SDL_SCANCODE_UP)) {
		float oldPitch = pitch;
		if (pitch > 89.0f) pitch = 89.0f;
		pitch += cameraRotationSpeed * deltaTime;
		float3x3 rotationMatrix = frustum.ViewMatrix().RotatePart();
		rotationMatrix = rotationMatrix.RotateAxisAngle(frustum.WorldRight(), DEGTORAD  * (pitch - oldPitch));

		rotateCamera(rotationMatrix);
		LOG("Keyboard -> yaw: %f, pitch: %f", yaw, pitch);
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN)) {
		float oldPitch = pitch;
		if (pitch < -89.0f) pitch = -89.0f;
		pitch -= cameraRotationSpeed * deltaTime;
		float3x3 rotationMatrix = frustum.ViewMatrix().RotatePart();
		rotationMatrix = rotationMatrix.RotateAxisAngle(frustum.WorldRight(), DEGTORAD * (pitch - oldPitch));

		rotateCamera(rotationMatrix);
		LOG("Keyboard -> yaw: %f, pitch: %f", yaw, pitch);
	}
}

void ModuleCamera::processMouseInput(float deltaTime)
{
	// Guide: https://learnopengl.com/Getting-started/Camera

	iPoint motion = App->input->GetMouseMotion();
	const float sensitivity = 0.1f;
	float xOffset = -motion.x * angleSpeed * deltaTime;
	float yOffset = -motion.y * angleSpeed * deltaTime;

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
		if (xOffset != 0) {
			yaw += xOffset;
			float3x3 rotationMatrix = frustum.ViewMatrix().RotatePart();
			rotationMatrix = rotationMatrix.RotateY(DEGTORAD * xOffset);
			rotateCamera(rotationMatrix);
			LOG("Mouse -> yaw: %f, pitch: %f", yaw, pitch);
		}

		if (yOffset != 0) {
			float oldPitch = pitch;
			if (pitch > 89.0f) pitch = 89.0f;
			if (pitch < -89.0f) pitch = -89.0f;
			pitch += yOffset;
			float3x3 rotationMatrix = frustum.ViewMatrix().RotatePart();
			rotationMatrix = rotationMatrix.RotateAxisAngle(frustum.WorldRight(), (DEGTORAD * (pitch - oldPitch)));
			rotateCamera(rotationMatrix);
			LOG("Mouse -> yaw: %f, pitch: %f", yaw, pitch);
		}
	}

	int wheelYOffset = App->input->GetMouseWheel();

	if (wheelYOffset != 0) {
		float3 newPos = frustum.Pos() + frustum.Front() * wheelYOffset * cameraSpeed * deltaTime;
		frustum.SetPos(newPos);
	}
}
