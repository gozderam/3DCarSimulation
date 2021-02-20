#pragma once
#include <glm\ext\vector_float3.hpp>
#include <glm\fwd.hpp>

enum class CameraType
{
	STATIONARY_WATCHING_SCENE = 1,
	STATIONARY_WATCHING_CAR = 2,
	MOVING_WITH_CAR = 3,
};

class CameraManager
{
private:
	const glm::vec3 stationaryCamPos = glm::vec3(0, 20, -2);
	const glm::vec3 stationaryLookAt = glm::vec3(0, 10, -30);
	const glm::vec3 movingCamPosSubstract = glm::vec3(0, 5, 0);

	CameraType type = CameraType::STATIONARY_WATCHING_SCENE;
	glm::vec3 cameraPosition = stationaryCamPos;
	glm::vec3 lookAt = stationaryLookAt;

private:
	void SetStationaryWatchingScene();
	void SetStationaryWatchingCar(glm::vec3 carPos);
	void SetMovingWithCar(glm::vec3 carPos, glm::vec3 carDirection);

public:
	void ChangeCamera(CameraType type, glm::vec3 carPos, glm::vec3 carDirection);
	
	void UpdateIfNeeded(glm::vec3 carPos, glm::vec3 carDirection);
	glm::mat4 GetViewMatrix();

	glm::vec3 GetCameraPosition() { return this->cameraPosition; }

};