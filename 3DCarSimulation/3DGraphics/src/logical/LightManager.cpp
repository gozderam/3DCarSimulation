#include "LightManager.h"

void LightManager::AddStationaryLight(StationaryLight light)
{
	stationaryLights.push_back(light);
}


void LightManager::AddSpotlight(Spotlight light)
{
	spotlights.push_back(light);
}

void LightManager::SetSpotlightsDirection(glm::vec3 direction)
{
	for (int i = 0; i < spotlights.size(); i++)
	{
		spotlights[i].SetDirection(direction);
	}
}

void LightManager::UpdateDayNightIfNeeded()
{
	if (!isDayNightTimeStopped)
	{
		if (day_n_nightFactor >= 1 || day_n_nightFactor <= 0.01)
		{
			day_n_nightStep = -day_n_nightStep;
		}
		day_n_nightFactor += day_n_nightStep;
	}
}

void LightManager::StartOrStopDayTime()
{
	isDayNightTimeStopped=!isDayNightTimeStopped;
}

float LightManager::GetDayNightFactor()
{
	return day_n_nightFactor;
}


void LightManager::EnableOrDisableFog()
{
	isFogEnabled = !isFogEnabled;
	isFogChanging = true;
}
void LightManager::UpdateFogIfNeeded()
{
	if (isFogEnabled)
	{
		if (isFogChanging)
		{
			fogFactor += fogStep;
			if (fogFactor >= 1)
			{
				fogFactor = 1;
				isFogChanging = false;
			}
		}
	}
	else
	{
		if (isFogChanging)
		{
			fogFactor -= fogStep;
			if (fogFactor <= 0)
			{
				fogFactor = 0;
				isFogChanging = false;
			}
		}
	}
		
}