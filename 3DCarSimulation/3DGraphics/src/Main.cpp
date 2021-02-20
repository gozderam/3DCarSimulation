// opengl 
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>

#include <iostream>

// logical
#include "logical/CameraManager.h"
#include "logical/LightManager.h"

// graphics api
#include "graphics_api/shader_m.h"
#include "graphics_api/model.h"
#include "graphics_api/skybox.h"
#include "graphics_api/deferredShadingManager.h"
#include "logical/CarMovementManager.h"

// settings
enum class ShadingMode { DEFFERED_SHADING, PHONG, GOURAUD};
ShadingMode selectedShadingMode = ShadingMode::DEFFERED_SHADING;
const unsigned int SCR_WIDTH = 1440;
const unsigned int SCR_HEIGHT = 900;
float FoV = 45.0f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool keys[1024];

CameraManager cameraManager;
LightManager lightManager(DirectionalLight(glm::vec3(1, 1, 1), 1, glm::vec3(0, -1, -0.30)));
CarMovementManager carMovementManager;
DeferredShadingManager deferedShadingManager(SCR_WIDTH, SCR_HEIGHT);
Shader* drawGeometryShader;
Shader* lightShader;
Shader* skyboxShader;


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
glm::mat4 GetProjectionMatrix();
void DoMovement(GLFWwindow* window);


int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Car Simulation", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glewInit();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // build and compile shaders
    Shader DS_shaderGeometryPass("res\\shaders\\DS_geometry_pass_v.shader", "res\\shaders\\DS_geometry_pass_f.shader");
    Shader DS_shaderLightingPass("res\\shaders\\DS_lightning_pass_v.shader", "res\\shaders\\DS_lightning_pass_f.shader");
    Shader phong_shader("res\\shaders\\Phong_shader_v.shader", "res\\shaders\\Phong_shader_f.shader");
    Shader gouraud_shader("res\\shaders\\Gouraud_shader_v.shader", "res\\shaders\\Gouraud_shader_f.shader");
    Shader skyboxShader("res\\shaders\\skybox_v.shader", "res\\shaders\\skybox_f.shader");

    Shader* drawGeometryShader = &DS_shaderGeometryPass;
    Shader* lightShader = &DS_shaderLightingPass;

    // load models     
    Model ground("res\\models\\grass\\grass.obj");
    Model building("res\\models\\building\\building.obj");
    Model car("res\\models\\car\\car.obj");
    Model sphere("res\\models\\sphere\\sphere.obj");
    Model road("res\\models\\road\\road.obj");
    Model lamp("res\\models\\lamp\\lamp.obj");

    Skybox skybox;

    // light
    lightManager.AddStationaryLight(StationaryLight(glm::vec3(1, 1, 1), 70, glm::vec3(5.5, 5.5, -30)));
    lightManager.AddStationaryLight(StationaryLight(glm::vec3(1, 1, 1), 70, glm::vec3(-5, 5.5, -60)));
    lightManager.AddSpotlight(Spotlight(glm::vec3(1, 1, 1), 60, glm::vec3(10, 0.0, -32), glm::vec3(18, 2, -32)));
    lightManager.AddSpotlight(Spotlight(glm::vec3(1, 1, 1), 60, glm::vec3(10, 0.0, -32), glm::vec3(18, 2, -32)));

    // WARNING: initaly selectedShadingmode must be DEFFERED_SHADING to ConfiureGBufferFramebuffer
    // TODO: move Phong, DF and Gouraud into abstract class hierarchy
    if(selectedShadingMode == ShadingMode::DEFFERED_SHADING)
        deferedShadingManager.ConfiureGBufferFramebuffer(*lightShader);

    cameraManager.ChangeCamera(CameraType::MOVING_WITH_CAR, carMovementManager.GetCarPosition(), carMovementManager.GetCarDirection());

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // logic for each frame
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        carMovementManager.UpdateDirection();
        DoMovement(window);
        cameraManager.UpdateIfNeeded(carMovementManager.GetCarPosition(), carMovementManager.GetCarDirection());
        lightManager.UpdateDayNightIfNeeded();
        lightManager.UpdateFogIfNeeded();
        switch (selectedShadingMode)
        {
        case ShadingMode::PHONG:
            drawGeometryShader = &phong_shader;
            lightShader = &phong_shader;
            break;
        case ShadingMode::DEFFERED_SHADING:
            drawGeometryShader = &DS_shaderGeometryPass;
            lightShader = &DS_shaderLightingPass;
            break;
        case ShadingMode::GOURAUD:
            drawGeometryShader = &gouraud_shader;
            lightShader = &gouraud_shader;
            break;
        }

        if (selectedShadingMode == ShadingMode::DEFFERED_SHADING)
            deferedShadingManager.StartGeometryPass();

        // setup unifroms
        glm::mat4 model = glm::mat4(1.0f);
        drawGeometryShader->use();
        drawGeometryShader->setMat4("projection", GetProjectionMatrix());
        drawGeometryShader->setMat4("view", cameraManager.GetViewMatrix());
        drawGeometryShader->setInt("useTexture_f", 1);

        // buildings
        drawGeometryShader->setMat4("model", glm::translate(glm::mat4(1.0), glm::vec3(-20, 0.0, -60)));
        building.Draw(*drawGeometryShader);
        drawGeometryShader->setMat4("model", glm::translate(glm::mat4(1.0), glm::vec3(20, 0.0, -50)) * glm::rotate(glm::radians(-70.0f), glm::vec3(0, 1, 0)));
        building.Draw(*drawGeometryShader);
        drawGeometryShader->setMat4("model", glm::translate(glm::mat4(1.0), glm::vec3(0, 0.0, -80)));
        building.Draw(*drawGeometryShader);

        // lamps
        drawGeometryShader->setMat4("model", glm::translate(glm::mat4(1.0), glm::vec3(-5, 0.0, -60)));
        lamp.Draw(*drawGeometryShader);
        drawGeometryShader->setMat4("model", glm::translate(glm::mat4(1.0), glm::vec3(5.5, 0.0, -30)));
        lamp.Draw(*drawGeometryShader);

        // sphere
        drawGeometryShader->setMat4("model", glm::translate(glm::mat4(1.0), glm::vec3(-20, 0.0, -32)));
        sphere.Draw(*drawGeometryShader);

        // car
        drawGeometryShader->setInt("useTexture_f", 0);
        drawGeometryShader->setMat4("model", carMovementManager.GetModelMatrix());
        car.Draw(*drawGeometryShader);
        drawGeometryShader->setInt("useTexture_f", 1);

        //ground 
        drawGeometryShader->setMat4("model", glm::mat4(1.0f));
        ground.Draw(*drawGeometryShader);

        // road
        drawGeometryShader->setMat4("model", glm::mat4(1.0f));
        road.Draw(*drawGeometryShader);

        if (selectedShadingMode == ShadingMode::DEFFERED_SHADING)
        {
            deferedShadingManager.FinishGeometryPass();
            deferedShadingManager.StartLightningPass(*lightShader);
        }
            
        // sunlight
        lightShader->setDirectionalLights("directionalLights_f", std::vector<DirectionalLight> {lightManager.GetDirectionalLight()});

        // statonary Light
        lightShader->setStationaryLights("stationaryLights_f", lightManager.GetStationaryLights());

        // spotlights
        lightManager.GetSpotlights()[0].SetLocation(carMovementManager.GetLeftSpotlightLocation());
        lightManager.GetSpotlights()[1].SetLocation(carMovementManager.GetRightSpotlightLocation());
        lightManager.SetSpotlightsDirection(carMovementManager.GetSpotlightsDirection());

        // other uniforms
        lightShader->setSpotlights("spotlights_f", lightManager.GetSpotlights());
        lightShader->setVec3("viewPos", cameraManager.GetCameraPosition());
        lightShader->setFloat("day_n_night_factor", lightManager.GetDayNightFactor());
        lightShader->setInt("fogEnabled_f", lightManager.IsFogEnabled() || lightManager.IsFogChanging() ? 1 : 0);
        lightShader->setFloat("fogFactor_f", lightManager.GetFogFactor());

        if (selectedShadingMode == ShadingMode::DEFFERED_SHADING)
            deferedShadingManager.FinishLightningPass();

        // draw skybox as last
        skyboxShader.use();
        skyboxShader.setInt("fogEnabled_f", lightManager.IsFogEnabled() || lightManager.IsFogChanging() ? 1 : 0);
        skyboxShader.setFloat("fogFactor_f", lightManager.GetFogFactor());
        skyboxShader.setFloat("day_n_night_factor", lightManager.GetDayNightFactor());
        glm::mat4 view = glm::mat4(glm::mat3(cameraManager.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", GetProjectionMatrix());
        skybox.Draw(skyboxShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}




void DoMovement(GLFWwindow* window)
{
    // spotlghts
    if (keys[GLFW_KEY_UP])
        carMovementManager.IncreaseSpotlightRelativeAngleVertical(0.005);
    if (keys[GLFW_KEY_DOWN])
        carMovementManager.IncreaseSpotlightRelativeAngleVertical(-0.005);
    if (keys[GLFW_KEY_RIGHT])
        carMovementManager.IncreaseSpotlightRelativeAngleHorizontal(-0.005);
    if (keys[GLFW_KEY_LEFT])
        carMovementManager.IncreaseSpotlightRelativeAngleHorizontal(0.005);


    if (keys[GLFW_KEY_W])
    {
        if (keys[GLFW_KEY_D])
            carMovementManager.IncreaseCarRotationAngle(-0.01);
        if (keys[GLFW_KEY_A])
            carMovementManager.IncreaseCarRotationAngle(0.01);
        carMovementManager.IncreasePosition(0.03f);
    }

    if (keys[GLFW_KEY_S])
    {
        if (keys[GLFW_KEY_D])
            carMovementManager.IncreaseCarRotationAngle(0.01);
        if (keys[GLFW_KEY_A])
            carMovementManager.IncreaseCarRotationAngle(-0.01);
        carMovementManager.IncreasePosition(-0.03f);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_3 && action == GLFW_PRESS)
        cameraManager.ChangeCamera(static_cast<CameraType>(key - 48), carMovementManager.GetCarPosition(), carMovementManager.GetCarDirection());
    else if (key == GLFW_KEY_N && action == GLFW_PRESS)
        lightManager.StartOrStopDayTime();
    else if (key == GLFW_KEY_F && action == GLFW_PRESS)
        lightManager.EnableOrDisableFog();
    else if (key == GLFW_KEY_7 && action == GLFW_PRESS)
        selectedShadingMode = ShadingMode::DEFFERED_SHADING;
    else if (key == GLFW_KEY_8 && action == GLFW_PRESS)
        selectedShadingMode = ShadingMode::PHONG;
    else if (key == GLFW_KEY_9 && action == GLFW_PRESS)
        selectedShadingMode = ShadingMode::GOURAUD;
    else if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

glm::mat4 GetProjectionMatrix()
{
    return glm::perspective(
        glm::radians(FoV), // The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
        (float)SCR_WIDTH / (float)SCR_HEIGHT,       // Aspect Ratio. Depends on the size of your window. 
        0.1f,              // Near clipping plane. 
        100.0f             // Far clipping plane. 
    );
}



