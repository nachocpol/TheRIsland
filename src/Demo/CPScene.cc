/*
  CPScene.cc cortizopo@esat-alumni.com
*/

#include "Demo/CPScene.h"
#include "Demo/CPMatSettings.h"

namespace Demo
{
  ///////////////////////

  CPScene::CPScene()
  {
    //Init final FBO
    finalFboMs = new SE::SBaseScreenMaterialSet("../data/shaders/BScreen.vs", "../data/shaders/BScreen.fs");
    finalFbo = new SE::SFrameBuffer(SE::SScreenQuadConstructor(),finalFboMs);
    finalFboMs->screenColorTexture = finalFbo->GetColorTex();

    //Init reflection fbo
    wreflectionFbo = new SE::SFrameBuffer(SE::SScreenQuadConstructor(glm::vec2(-1), glm::vec2(-0.5f, -1.0f), glm::vec2(-0.5f, 0.0f), glm::vec2(-1.0f, 0.0f)),finalFboMs);
    
    //Init refraction fbo
    wrefractionFbo = new SE::SFrameBuffer(SE::SScreenQuadConstructor(glm::vec2(-1,0), glm::vec2(-0.5f, 0), glm::vec2(-0.5f, 1.0f), glm::vec2(-1.0f, 1.0f)),finalFboMs);

    //Init vignet FBO
    vignetMs = new SE::SBaseScreenMaterialSet("../data/shaders/vignetting.vs", "../data/shaders/vignetting.fs");
    vignetFbo = new SE::SFrameBuffer(SE::SScreenQuadConstructor(),vignetMs);
    vignetMs->screenColorTexture = vignetFbo->GetColorTex();

    //Init scattering stuff
    //1
    scatPass1Ms = new CPMScatterPass1MatSet("../data/shaders/Scattering1.vs", "../data/shaders/Scattering1.fs");
    scatPass1Fbo = new SE::SFrameBuffer(SE::SScreenQuadConstructor(),scatPass1Ms);
    scatPass1Ms->screenColorTexture = scatPass1Fbo->GetColorTex();
    //2
    scatPass2Ms = new CPMScatterPass1MatSet("../data/shaders/Scattering2.vs", "../data/shaders/Scattering2.fs");
    scatPass2Fbo = new SE::SFrameBuffer(SE::SScreenQuadConstructor(),scatPass2Ms);
    scatPass2Ms->screenColorTexture = scatPass2Fbo->GetColorTex();//We receive the color from the last pass
    //3
    scatPass3Ms = new CPMScatteringPass3MatSet("../data/shaders/Scattering3.vs", "../data/shaders/Scattering3.fs");
    scatPass3Fbo = new SE::SFrameBuffer(SE::SScreenQuadConstructor(),scatPass3Ms);
    scatPass3Ms->screenColorTexture = scatPass3Fbo->GetColorTex();
    scatPass3Ms->stdImg = finalFbo->GetColorTex();

    //Init blur
    blurMs = new CPMBlurMatSet("../data/shaders/Blur.vs","../data/shaders/Blur.fs");
    blurFbo = new SE::SFrameBuffer(SE::SScreenQuadConstructor(),blurMs);
    blurMs->screenColorTexture = blurFbo->GetColorTex();
    //tmp blur holder
    blurTmpMs = new SE::SBaseScreenMaterialSet("../data/shaders/BScreen.vs", "../data/shaders/BScreen.fs");
    blurTmpFbo = new SE::SFrameBuffer(SE::SScreenQuadConstructor(),blurTmpMs);
    blurTmpMs->screenColorTexture = blurTmpFbo->GetColorTex();

    //Init bloom
    //1
    bloom1Ms = new SE::SBaseScreenMaterialSet("../data/shaders/Bloom1.vs","../data/shaders/Bloom1.fs");
    bloom1Fbo = new SE::SFrameBuffer(SE::SScreenQuadConstructor(),bloom1Ms);
    bloom1Ms->screenColorTexture = bloom1Fbo->GetColorTex();
    //2
    bloom2Ms = new CPMBloom2MatSet("../data/shaders/Bloom2.vs","../data/shaders/Bloom2.fs");
    bloom2Fbo = new SE::SFrameBuffer(SE::SScreenQuadConstructor(),bloom2Ms);
    bloom2Ms->screenColorTexture = bloom2Fbo->GetColorTex();

    //Init skybox
    skyBox = new CPSkyBox();
    skyBox->Init();

    //Init instanced grass
    grassInstance = new CPInstancedMesh;

    //Init fog ubo
    glGenBuffers(1,&fogUbo);
    glBindBuffer(GL_UNIFORM_BUFFER,fogUbo);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 2, fogUbo, 0, 2 * sizeof(glm::vec4));

    //Default values
    fogColorEnabled = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
    fogStartEndMode = glm::vec4(10.0f, 500.0f, 0.0f, 0.0f);
  }

  ///////////////////////

  CPScene::~CPScene()
  {
    delete finalFbo;
    delete wreflectionFbo;
    delete wrefractionFbo;
    delete finalFboMs;
    delete skyBox;
    delete grassInstance;
    delete vignetFbo;
    delete vignetMs;
    delete scatPass1Fbo;
    delete scatPass1Ms;
    delete scatPass2Fbo;
    delete scatPass2Ms;
    delete scatPass3Fbo;
    delete scatPass3Ms;
    delete blurFbo;
    delete blurMs;
    delete bloom1Fbo;
    delete bloom1Ms;
  }

  ///////////////////////

  void CPScene::Render()
  {
    float clipPlane = wClipPlane + wClipOff;

    //Upload the draw data to the uniform buffer
    unsigned int sMat4 = sizeof(glm::mat4);
    glBindBuffer(GL_UNIFORM_BUFFER, dDataBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sMat4, glm::value_ptr(*mainCamera->GetView()));
    glBufferSubData(GL_UNIFORM_BUFFER, sMat4, sMat4, glm::value_ptr(*mainCamera->GetProjection()));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //Upload fog data
    unsigned int sVec4 = sizeof(glm::vec4);
    glBindBuffer(GL_UNIFORM_BUFFER,fogUbo);
    glBufferSubData(GL_UNIFORM_BUFFER,0,sVec4,&fogColorEnabled.x);
    glBufferSubData(GL_UNIFORM_BUFFER,sVec4,sVec4,&fogStartEndMode.x);
    glBindBuffer(GL_UNIFORM_BUFFER,0);

    //Render the shadow maps 
    displayList->SetShadowPass(true);
    for(unsigned int i=0;i<lights.size();i++)
    {
      displayList->SetCurLightMatrix(lights[i]->GetLightMatrix());
      lights[i]->EnableLightShadow();
      displayList->RunDl();
      lights[i]->DisableLightShadow();
    }
    displayList->SetShadowPass(false);

    //Setup scene for water reflections rendering
    {
      //Position the camera under water surface
      float offD = wOff * (mainCamera->GetPosition()->y - water->transform->GetPosition()->y);
      mainCamera->GetPosition()->y -= offD;
      *mainCamera->GetPitch() = -*mainCamera->GetPitch();

      //Upload the draw data to the uniform buffer
      glBindBuffer(GL_UNIFORM_BUFFER, dDataBuffer);
      glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(*mainCamera->GetView()));
      glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(*mainCamera->GetProjection()));
      glBindBuffer(GL_UNIFORM_BUFFER, 0);

      //Reflection Fbo
      wreflectionFbo->Enable();
      {
        skyBox->Draw();

        displayList->SetCurrentLights(lights);
        displayList->SetCamera(mainCamera);
        displayList->SetClipPlane(0, glm::vec4(0, 1, 0, -clipPlane));
        grassInstance->Render();
        displayList->RunDl();
      }
      wreflectionFbo->Disable();

      //Reset the camera
      mainCamera->GetPosition()->y += offD;
      *mainCamera->GetPitch() = -*mainCamera->GetPitch();

      //Upload the draw data to the uniform buffer
      glBindBuffer(GL_UNIFORM_BUFFER, dDataBuffer);
      glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(*mainCamera->GetView()));
      glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    //Refraction fbo
    wrefractionFbo->Enable();
    {
      //skyBox->Draw();

      displayList->SetCurrentLights(lights);
      displayList->SetCamera(mainCamera);
      displayList->SetClipPlane(0, glm::vec4(0, -1, 0, clipPlane));
      displayList->RunDl();
      //grassInstance->Render();
    }
    wrefractionFbo->Disable();

    //Basic rendering the scene
    finalFbo->Enable();
    {
      skyBox->Draw();

      displayList->SetCurrentLights(lights);
      displayList->SetCamera(mainCamera);
      displayList->SetClipPlane(0, glm::vec4(0, 1, 0, 99999999));
      displayList->RunDl();

      grassInstance->Render();

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      water->material->Use(*water->transform->GetModel(), lights);
      water->mesh->MeshRender();
      glDisable(GL_BLEND);
    }
    finalFbo->Disable();


    //Give the scatering phases the sun ndp
    glm::vec4 sunND = GetSunNormDevice();
    scatPass1Ms->sunPosition = &sunND;
    scatPass2Ms->sunPosition = &sunND;

    //Render scene to scat fbo
    scatPass1Fbo->Enable();
    {
      displayList->SetCurrentLights(lights);
      displayList->SetCamera(mainCamera);
      displayList->SetClipPlane(0, glm::vec4(0, 1, 0, 99999999));
      displayList->RunDl();

      grassInstance->Render();

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      water->material->Use(*water->transform->GetModel(), lights);
      water->mesh->MeshRender();
      glDisable(GL_BLEND);
    }
    scatPass1Fbo->Disable();
    
    //Second pass of scattering
    scatPass2Fbo->Enable();
    {
      scatPass1Fbo->Draw(false);
    }
    scatPass2Fbo->Disable();

    //Third pass of scattering
    scatPass3Fbo->Enable();
    {
      scatPass2Fbo->Draw(false);
    }
    scatPass3Fbo->Disable();

    /*
    //Bloom first pass
    bloom1Fbo->Enable();
    {
      scatPass3Fbo->Draw(false);
    }
    bloom1Fbo->Disable();

    //Perform blur
    //Vertical blur
    blurMs->doVerticalBlur = true;
    blurFbo->Enable();
    {
      bloom1Fbo->Draw(false);
    }
    blurFbo->Disable();

    //Save result of vertical blur
    blurTmpFbo->Enable();
    {
      blurFbo->Draw(false);
    }
    blurTmpFbo->Disable();

    //Horizontal blur
    blurMs->doVerticalBlur = false;
    blurFbo->Enable();
    {
     blurTmpFbo->Draw(false);
    }
    blurFbo->Disable();

    //Bloom last pass
    bloom2Ms->stdImg = blurFbo->GetColorTex();
    bloom2Fbo->Enable();
    {
      scatPass3Fbo->Draw(false);
    }
    bloom2Fbo->Disable();
    
    bloom2Fbo->Draw();
    */

    //Perform vignetting
    vignetFbo->Enable();
    {
      scatPass3Fbo->Draw(false);
    }
    vignetFbo->Disable();

    vignetFbo->Draw();
 

    //Render the gui
    RenderGUI();
    displayList->CleanDl();
    //TO-DO:change this?
    lights.clear();
  }

  //////////////////////

  void CPScene::RenderGUI()
  {
    SScene::RenderGUI();

    bool showFancyGui = true;
    ImGui::Begin("Fancy stuff",&showFancyGui);
    {
      //Water section
      ImGui::Text("Water");
      ImGui::Separator();

      ImGui::InputFloat("Water Off:",&wOff);

      ImGui::InputFloat("Water Clip Plane:",&wClipPlane);

      ImGui::InputFloat("Water Clip Off:", &wClipOff);

      //Fog section
      static glm::vec3 fogColor = glm::vec3(fogColorEnabled);
      static float fogStart = fogStartEndMode.x;
      static float fogEnd = fogStartEndMode.y;
      static bool fogEnabled = fogColorEnabled.a;

      ImGui::Text("Fog");
      ImGui::Separator();

      ImGui::Checkbox("Fog Enabled:",&fogEnabled);

      ImGui::InputFloat("Fog Start:",&fogStart);

      ImGui::InputFloat("Fog End:",&fogEnd);

      ImGui::InputFloat3("Fog Color:",&fogColor.x);

      fogStartEndMode.x = fogStart;
      fogStartEndMode.y = fogEnd;

      fogColorEnabled.a = (int)fogEnabled;

      fogColorEnabled.x = fogColor.x;
      fogColorEnabled.y = fogColor.y;
      fogColorEnabled.z = fogColor.z;
    }
    ImGui::End();

    //scatPass2Ms->ShowGui();

  }

  ///////////////////////

  glm::vec4 CPScene::GetSunNormDevice()
  {
    glm::mat4 proj = *mainCamera->GetProjection();
    glm::mat4 view = *mainCamera->GetView();
    glm::vec3 position = *lights[0]->GetPosition();

    glm::vec4 result = glm::vec4(0.0f);
    result = proj * view * glm::vec4(position,1.0);
    result = result / result.w;
    return result;
  }

  ///////////////////////
}