#include "CameraManager.h"
#include <glm\ext\matrix_transform.hpp>

void CameraManager::SetStationaryWatchingScene()
{
	this->type = CameraType::STATIONARY_WATCHING_SCENE;
	this->cameraPosition = stationaryCamPos;
	this->lookAt = stationaryLookAt;
}
void CameraManager::SetStationaryWatchingCar(glm::vec3 carPos)
{
	this->type = CameraType::STATIONARY_WATCHING_CAR;
	this->cameraPosition = stationaryCamPos;
	this->lookAt = carPos;
}

void CameraManager::SetMovingWithCar(glm::vec3 carPos, glm::vec3 carDirection)
{
	this->type = CameraType::MOVING_WITH_CAR;
	this->cameraPosition = carPos + movingCamPosSubstract - 8.0f * carDirection;
	this->lookAt = carPos + 10.0f * carDirection;
}

void CameraManager::ChangeCamera(CameraType type, glm::vec3 carPos, glm::vec3 carDirection)
{
	switch (type)
	{
	case CameraType::STATIONARY_WATCHING_SCENE:
		SetStationaryWatchingScene();
		break;
	case CameraType::STATIONARY_WATCHING_CAR:
		SetStationaryWatchingCar(carPos);
		break;
	case CameraType::MOVING_WITH_CAR:
		SetMovingWithCar(carPos, carDirection);
	}
}

void CameraManager::UpdateIfNeeded(glm::vec3 carPos, glm::vec3 carDirection)
{
	switch(type)
	{
	case CameraType::STATIONARY_WATCHING_CAR:
		this->lookAt = carPos;
		break;
	case CameraType::MOVING_WITH_CAR:
		this->cameraPosition =  carPos + movingCamPosSubstract - 8.0f * carDirection;
		this->lookAt = carPos + 10.0f*carDirection;
		break;
	}
}

glm::mat4 CameraManager::GetViewMatrix()
{
	return glm::lookAt(
		cameraPosition, // Camera is here,in World Cooridantes
		lookAt, // and looks here : at the same position, plus "direction"
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
}