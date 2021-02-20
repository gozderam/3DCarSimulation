#include <glm/ext/vector_float3.hpp>
#include <vector>

class Light
{
protected:
	glm::vec3 color;
	float power;

public:
	Light(glm::vec3 color, float power)
		: color(color), power(power) { }

	glm::vec3 GetColor() { return color; }
	float GetPower() { return power; }
};

class DirectionalLight : public Light
{
protected:
	glm::vec3 direction;
public:
	DirectionalLight(glm::vec3 color, float power, glm::vec3 direction)
		: Light(color, power), direction(direction) { }

	glm::vec3 GetDirection() { return direction; }
	void SetDirection(glm::vec3 direction) { this->direction = direction; }
};

class StationaryLight : public Light
{
protected:
	glm::vec3 location;
public:
	StationaryLight(glm::vec3 color, float power, glm::vec3 location)
		: Light(color,  power), location(location){ }

	glm::vec3 GetLocation() { return location; }
	void SetLocation(glm::vec3 location) { this->location = location; }
};

class Spotlight : public Light
{
private:
	glm::vec3 location;
	glm::vec3 direction;
public:
	Spotlight(glm::vec3 color, float power, glm::vec3 location, glm::vec3 direction)
		: Light(color, power), location(location), direction(direction) { }

	glm::vec3 GetLocation() { return location; }
	void SetLocation(glm::vec3 location) { this->location = location; }

	glm::vec3 GetDirection() { return direction; }
	void SetDirection(glm::vec3 direction) { this->direction = direction; }
};

class LightManager
{
private:
	DirectionalLight directionalLight;
	std::vector<StationaryLight> stationaryLights;
	std::vector<Spotlight> spotlights;

	bool isDayNightTimeStopped = true;
	float day_n_nightFactor = 1;
    float day_n_nightStep = 0.0009;

	bool isFogEnabled = false;
	bool isFogChanging = false;
	float fogFactor = 0;
	float fogStep = 0.0007;



public:
	LightManager(DirectionalLight directionaLight)
		: directionalLight(directionaLight) { }

	void SetDirectionalLight(DirectionalLight directionalLight) { this->directionalLight = directionalLight; }
	DirectionalLight& GetDirectionalLight() { return directionalLight; }

	void AddStationaryLight(StationaryLight light);
	std::vector<StationaryLight>& GetStationaryLights() { return stationaryLights; }

	void AddSpotlight(Spotlight light);
	void SetSpotlightsDirection(glm::vec3 direction);
	std::vector<Spotlight>& GetSpotlights() { return spotlights; }

	void StartOrStopDayTime();
	void UpdateDayNightIfNeeded();
	float GetDayNightFactor();

	void EnableOrDisableFog();
	void UpdateFogIfNeeded();
	float GetFogFactor() { return fogFactor; }
	bool IsFogEnabled() { return isFogEnabled; }
	bool IsFogChanging() { return isFogChanging; }
};