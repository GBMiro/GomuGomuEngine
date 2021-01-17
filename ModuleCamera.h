#ifndef __ModuleCamera_H__
#define __ModuleCamera_H__
#include "MathGeoLib/Geometry/Frustum.h"
#include "MathGeoLib/Math/float3x3.h"
#include "Globals.h"
#include "Module.h"
#include "Point.h"

#define MaxRotSpeed 30.0f
#define MaxMovSpeed 40.0f
#define MaxZoomSpeed 5.0f
#define MaxZNear 100.0f
#define MaxZFar 200000.0f
#define MaxFOV 90.0f

class ComponentTransform;
class ComponentCamera;
class GameObject;

class ModuleCamera : public Module {
private:
	GameObject* gameObject = nullptr;
	ComponentTransform* transform = nullptr;
	ComponentCamera* camera = nullptr;
	float posX, posY, posZ;
	float yaw = 0.0f;
	float pitch = 0.0f;
	float fov = 90.0f;
	float nearPlane = 0.1f;
	float farPlane = 200.0f;
	iPoint mouseLastPos = { 0,0 };
	float cameraSpeed = 10.0f;
	float angleSpeed = 20.0f;
	float zoomSpeed = 5.0f;

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

	void SetPlanes(float zNear, float zFar);

	void FocusOnSelected();

	void OrbitCamera(float xOfsset, float yOffset);

	void LookAt(const float3& point);

	const float4x4& GetProjectionMatrix() const;
	const float4x4& GetViewMatrix() const;
	void GetPlanes(float* zNear, float* zFar) const;
	const float3& GetUpVector() const;
	const float3& GetRightVector() const;
	const float3& GetFrontVector() const;
	const float3& GetCameraPosition() const;
	float GetAspectRatio() const;
	float GetFOV() const;
	const Frustum& GetFrustum();

	float GetCameraSpeed()const { return cameraSpeed; }
	float GetAngleSpeed()const { return angleSpeed; }
	float GetZoomSpeed()const { return zoomSpeed; }

	void SetCameraSpeed(float newVal) { cameraSpeed = newVal; }
	void SetAngleSpeed(float newVal) { angleSpeed = newVal; }
	void SetZoomSpeed(float newVal) { zoomSpeed = newVal; }


private:
	void UpdateCamera();
	void RotateCamera(const float3x3& rotationMatrix);
	void ProcessKeyboardInput(float deltaTime, float speed, float cameraRotationSpeed);
	void ProcessMouseInput(float deltaTime);
};

#endif // __ModuleCamera_H__

