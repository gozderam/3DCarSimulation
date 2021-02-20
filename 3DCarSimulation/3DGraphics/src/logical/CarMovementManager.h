#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class CarMovementManager
{
private:
	glm::vec3 position = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 direction = glm::vec3(0.0, 0.0, 0.0);
	float angle = 3.14; // in the -z direction (rotation)

	float relativeSpotlightAngle_horizontal = 0.0;
	float relativeSpotlightAngle_vertical = 0.0;

public:
	glm::mat4 GetRotateMatrix() { return glm::rotate(angle, glm::vec3(0, 1, 0)); }
	glm::mat4 GetTranslateMatrix() { return glm::translate(glm::vec3(position.x, position.y, position.z)); }
	glm::mat4 GetScaleMatrix() { return glm::scale(glm::vec3(1.0f, 1.0f, 1.0f)); }
	glm::mat4 GetModelMatrix() { return  GetTranslateMatrix() * GetRotateMatrix() * GetScaleMatrix(); }

	void SetCarDirection(glm::vec3 dir) { direction = dir; }
	glm::vec3 GetCarDirection() { return direction; }

	void SetCarPosition(glm::vec3 pos) { position = pos; }
	glm::vec3 GetCarPosition() { return position; }

	void IncreaseCarRotationAngle(float angleToAdd) { angle += angleToAdd; }
	float GetCarRotationAngle() { return angle; }

	void IncreaseSpotlightRelativeAngleHorizontal(float angleToAdd) 
	{
		
		if ((relativeSpotlightAngle_horizontal + angleToAdd) <= 3.14 / 4 && (relativeSpotlightAngle_horizontal + angleToAdd) >= -3.14 / 4)
			relativeSpotlightAngle_horizontal += angleToAdd;
	}

	void IncreaseSpotlightRelativeAngleVertical(float angleToAdd)
	{
		if ((relativeSpotlightAngle_vertical + angleToAdd) <= 3.14 / 4  && (relativeSpotlightAngle_vertical + angleToAdd) >= 0) 
			relativeSpotlightAngle_vertical += angleToAdd;
	}

	glm::vec3 GetLeftSpotlightLocation()
	{
		glm::vec3 leftSpotlight = glm::normalize(glm::vec3(glm::sin(angle + 0.3), 0, glm::cos(angle + 0.3)));
		return glm::vec3(position.x, position.y + 0.5, position.z) + 2.0f * leftSpotlight;
	}

	glm::vec3 GetRightSpotlightLocation()
	{
		glm::vec3 rightSpotlight = glm::normalize(glm::vec3(glm::sin(angle - 0.3), 0, glm::cos(angle - 0.3)));
		return glm::vec3(position.x, position.y + 0.5, position.z) + 2.0f * rightSpotlight;
	}

	glm::vec3 GetSpotlightsDirection()
	{
		return glm::vec3(sin(angle + relativeSpotlightAngle_horizontal), sin(relativeSpotlightAngle_vertical), cos(angle + relativeSpotlightAngle_horizontal));
	}

	void UpdateDirection()
	{
		direction = glm::normalize(glm::vec3(glm::sin(angle), 0, glm::cos(angle)));
	}

	void IncreasePosition(float factor)
	{
		UpdateDirection();
		glm::vec3 toAdd = factor * glm::normalize(direction);
		position += toAdd;
	}
};