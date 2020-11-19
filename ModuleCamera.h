#ifndef __ModuleCamera_H__
#define __ModuleCamera_H__
#include "MathGeoLib/Geometry/Frustum.h"
#include "MathGeoLib/Math/float3x3.h"
#include "Globals.h"
#include "Module.h"
#include "Point.h"

class ModuleCamera : public Module
{
public:
	ModuleCamera();

	~ModuleCamera();

	bool Init();

	update_status PreUpdate();

	update_status Update();

	update_status PostUpdate();

	bool CleanUp();

	void updateProjectionMatrix();

	void SetFOV(float aspectRatio);

	void SetAspectRatio();

	void SetPlaneDistance();

	void SetPosition();

	void SetOrientation();

	void LookAt();

	float4x4 getProjectionMatrix();
	float4x4 getViewMatrix();


	void updateCamera();
	void rotateCamera(float3x3 &rotationMatrix);
	void processKeyboardInput(float deltaTime, float speed, float cameraRotationSpeed);
	void processMouseInput(float deltaTime);

private:
	Frustum frustum;
	float posX, posY, posZ = 0.0f;
	float cameraSpeed = 5.0f;
	float angleSpeed = 30.0f;
	float deltaTime;
	float lastFrame;
	float yaw, pitch = 0.0f;
	iPoint mouseLastPos = { 0,0 };
};

#endif // __ModuleCamera_H__

