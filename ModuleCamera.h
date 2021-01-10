#ifndef __ModuleCamera_H__
#define __ModuleCamera_H__
#include "MathGeoLib/Geometry/Frustum.h"
#include "MathGeoLib/Math/float3x3.h"
#include "Globals.h"
#include "Module.h"
#include "Point.h"

#define MaxRotSpeed 30.0f
#define MaxMovSpeed 10.0f
#define MaxZoomSpeed 5.0f
#define MaxZNear 100.0f
#define MaxZFar 200.0f
#define MaxFOV 90.0f

class ModuleCamera : public Module {
public:
	ModuleCamera();

	~ModuleCamera();

	bool Init();

	update_status PreUpdate();

	update_status Update();

	update_status PostUpdate();

	bool CleanUp();

	void SetFOV(float fov);

	void SetAspectRatio(float aspectRatio);

	void setPlanes(float zNear, float zFar);

	void FocusOnSelected();

	void orbitCamera(float xOfsset, float yOffset);

	void LookAt(const float3& point);

	float4x4 getProjectionMatrix() const;
	float4x4 getViewMatrix() const;
	void getPlanes(float* zNear, float* zFar) const;
	float3 getUpVector() const { return frustum.Up(); }
	float3 getRightVector() const { return frustum.WorldRight(); }
	float3 getFrontVector() const { return frustum.Front(); }
	float3 getCameraPosition() const { return frustum.Pos(); }
	float getAspectRatio() const { return frustum.AspectRatio(); }
	float getFOV() const { return fov; }
	const Frustum& GetFrustum() { return frustum; }
private:
	void updateCamera();
	void rotateCamera(const float3x3& rotationMatrix);
	void processKeyboardInput(float deltaTime, float speed, float cameraRotationSpeed);
	void processMouseInput(float deltaTime);

public:
	float cameraSpeed = 5.0f;
	float angleSpeed = 20.0f;
	float zoomSpeed = 5.0f;

private:
	Frustum frustum;
	float posX, posY, posZ;
	float yaw = 0.0f;
	float pitch = 0.0f;
	float fov = 90.0f;
	float nearPlane = 0.1f;
	float farPlane = 200.0f;
	iPoint mouseLastPos = { 0,0 };
};

#endif // __ModuleCamera_H__

