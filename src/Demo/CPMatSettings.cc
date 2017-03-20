/*
  CPMatSettings.cc cortizopo@esat-alumni.com
*/

#include "Demo/CPMatSettings.h"

namespace Demo
{
  //////////////////////Terrain Material settings

  CPTerrainMatSet::CPTerrainMatSet(const char* vs, const char* fs, std::string gp, std::string grap, std::string sp, std::string cp, std::string gnp, std::string cnp)
  {
    SMaterialSettings::SMaterialSettings(vs, fs);
    vsp = vs;
    fsp = fs;

    resMang = SE::SResourceManager::GetInstance();
    renderMang = SE::SRenderManager::GetInstance();

    grassTexPath = gp;
    gravelTexPath = grap;
    snowTexPath = sp;
    cliffTexPath = cp;

    grassNormPath = gnp;
    cliffNormPath = cnp;

    resMang->AddTexture(grassTexPath);
    resMang->AddTexture(gravelTexPath);
    resMang->AddTexture(snowTexPath);
    resMang->AddTexture(cliffTexPath);

    resMang->AddTexture(grassNormPath);   
    resMang->AddTexture(cliffNormPath);
  }

  //////////////////////

  CPTerrainMatSet::~CPTerrainMatSet()
  {

  }

  //////////////////////

  void CPTerrainMatSet::GetLocations(int pid)
  {
   grassTexLocation = renderMang->GetUniform(pid, "uGrassTexture");
   gravelTexLocation = renderMang->GetUniform(pid, "uGravelTexture");
   snowTexLocation = renderMang->GetUniform(pid, "uSnowTexture");
   cliffTexLocation = renderMang->GetUniform(pid, "uCliffTexture");
  
   grassNormLocation = renderMang->GetUniform(pid, "uGrassNormal");
   cliffNormLocation = renderMang->GetUniform(pid, "uCliffNormal");

   camLocation = renderMang->GetUniform(pid, "uCamPos");

   //Set the clipping planes layout to bind = 1
   glUniformBlockBinding(pid, glGetUniformBlockIndex(pid, "ClipPlanes"), 1);
  }

  //////////////////////

  void CPTerrainMatSet::SetLocations(int pid)
  {
    renderMang->SetUniform(pid, "uCamPos", SE::kFloat3, &camP->x, camLocation);
    //Text unit 0
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D,resMang->GetTexture(grassTexPath)->GetId());
    glUniform1i(grassTexLocation, 0);
    //Text unit 1
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, resMang->GetTexture(gravelTexPath)->GetId());
    glUniform1i(gravelTexLocation, 1);
    //Text unit 2
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, resMang->GetTexture(snowTexPath)->GetId());
    glUniform1i(snowTexLocation, 2);
    //Text unit 3
    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_2D, resMang->GetTexture(cliffTexPath)->GetId());
    glUniform1i(cliffTexLocation, 3);
    //Texture unit 4
    glActiveTexture(GL_TEXTURE0 + 4);
    glBindTexture(GL_TEXTURE_2D, resMang->GetTexture(grassNormPath)->GetId());
    glUniform1i(grassNormLocation , 4);
    //Texture unit 5
    glActiveTexture(GL_TEXTURE0 + 5);
    glBindTexture(GL_TEXTURE_2D, resMang->GetTexture(cliffNormPath)->GetId());
    glUniform1i(cliffNormLocation, 5);
  }

  //////////////////////Water Material Settings
  CPWaterMatSet::CPWaterMatSet(const char* vs, const char* fs)
  {
    SMaterialSettings::SMaterialSettings(vs, fs);
    vsp = vs;
    fsp = fs;

    resMang = SE::SResourceManager::GetInstance();
    renderMang = SE::SRenderManager::GetInstance();

    resMang->AddTexture(wdudvPath);
    resMang->AddTexture(wnormPath);
    resMang->AddTexture(foamPath);
  }

  //////////////////////

  CPWaterMatSet::~CPWaterMatSet()
  {

  }

  //////////////////////

  void CPWaterMatSet::GetLocations(int pid)
  { 
    camLoc = renderMang->GetUniform(pid, "uCamPos");
    timeLoc = renderMang->GetUniform(pid, "uTimeFactor");

    wnormLoc = renderMang->GetUniform(pid, "uNorm");
    wdudvTexLoc = renderMang->GetUniform(pid, "uDudv");

    reflectTexLoc = renderMang->GetUniform(pid, "uReflectionTex");
    refractTexLoc = renderMang->GetUniform(pid, "uRefractionTex");
    depthTexLoc = renderMang->GetUniform(pid, "uDepthTex");
    foamLoc = renderMang->GetUniform(pid, "uFoamTex");
  }

  //////////////////////

  void CPWaterMatSet::SetLocations(int pid)
  {
    renderMang->SetUniform(pid, "uCamPos", SE::kFloat3, &camP->x, camLoc);
    float time = (float)renderMang->GetTime();
    renderMang->SetUniform(pid, "uTime", SE::KFloat, &time, timeLoc);
    //Text unit 0(reflection texture)
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, reflectTex);
    glUniform1i(reflectTexLoc, 0);
    //Text unit 1(refraction texture)
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, refractTex);
    glUniform1i(refractTexLoc, 1);
    //Tex unit 2 (dudv map)
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, resMang->GetTexture(wdudvPath)->GetId());
    glUniform1i(wdudvTexLoc, 2);
    //Tex unit 3 (normal map)
    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_2D, resMang->GetTexture(wnormPath)->GetId());
    glUniform1i(wnormLoc, 3);
    //Tex unit 4 (depth texture)
    glActiveTexture(GL_TEXTURE0 + 4);
    glBindTexture(GL_TEXTURE_2D,depthTex);
    glUniform1i(depthTexLoc, 4);
    //Tex unit 5 (foam texture)
    glActiveTexture(GL_TEXTURE0 + 5);
    glBindTexture(GL_TEXTURE_2D, resMang->GetTexture(foamPath)->GetId());
    glUniform1i(foamLoc, 5);

  }

  //////////////////////Vegetation Material Settings

  CPVegetationMatSet::CPVegetationMatSet(const char* vs, const char* fs)
  {
    SMaterialSettings::SMaterialSettings(vs, fs);
    vsp = vs;
    fsp = fs;

    resMang = SE::SResourceManager::GetInstance();
    renderMang = SE::SRenderManager::GetInstance();

    resMang->AddTexture(grassBilPath);
    resMang->AddTexture(gradientPath);
  }

  //////////////////////

  CPVegetationMatSet::~CPVegetationMatSet()
  {

  }

  //////////////////////

  void CPVegetationMatSet::GetLocations(int pid)
  {
    grassBilLoc = renderMang->GetUniform(pid, "uGrassBilTex");
    gradientLoc = renderMang->GetUniform(pid, "uGradientTex");
    timeLoc = renderMang->GetUniform(pid, "uTime");
  }

  //////////////////////

  void CPVegetationMatSet::SetLocations(int pid)
  {
    renderMang->SetTexture(0, SE::k2DTexture, resMang->GetTexture(grassBilPath)->GetId(), grassBilLoc);
    renderMang->SetTexture(1, SE::k2DTexture, resMang->GetTexture(gradientPath)->GetId(), gradientLoc);
    float time = renderMang->GetTime();
    renderMang->SetUniform(pid, "uTime", SE::KFloat, &time, timeLoc);
  }

  //////////////////////Screen mat settings for scattering 1st pass

  CPMScatterPass1MatSet::CPMScatterPass1MatSet()
  {

  }

  //////////////////////

  CPMScatterPass1MatSet::CPMScatterPass1MatSet(const char* vs, const char* fs)
  {
    SMaterialSettings::SMaterialSettings(vs, fs);
    vsp = vs;
    fsp = fs;
  }

  //////////////////////

  CPMScatterPass1MatSet::~CPMScatterPass1MatSet()
  {

  }

  //////////////////////

  void CPMScatterPass1MatSet::GetLocations(int pid)
  {
    programId = pid;
    SE::SBaseScreenMaterialSet::GetLocations(pid);
  }

  //////////////////////

  void CPMScatterPass1MatSet::SetLocations(int pid)
  {
    SE::SBaseScreenMaterialSet::SetLocations(pid);
    glm::vec4 sPos(*sunPosition);
    sunPosLoc = glGetUniformLocation(pid,"uSunPosition");
    glUniform4f(sunPosLoc,sPos.x,sPos.y,sPos.z,sPos.w);
  }

  //////////////////////

  void CPMScatterPass1MatSet::ShowGui()
  {
    static int samples = 192;
    static float exposure = 1.0;
    static float decay = 1.0;
    static float density = 1.0;
    static float weight = 0.026f;
    /*
    ImGui::Begin("Fancy stuff");
    {
      ImGui::Text("Light scattering");
      ImGui::Separator();

      ImGui::SliderFloat("Exposure", &exposure, 0.0f, 1.0f);
      ImGui::SliderFloat("Decay", &decay, 0.0f, 1.0f);
      ImGui::SliderFloat("Density", &density, 0.0f, 1.0f);
      ImGui::SliderFloat("Weight", &weight, 0.0f, 1.0f);
      ImGui::SliderInt("Samples", &samples,0,500);
    }
    ImGui::End();
    */

    glUniform1f(glGetUniformLocation(programId, "uExposure"), exposure);
    glUniform1f(glGetUniformLocation(programId, "uDecay"), decay);
    glUniform1f(glGetUniformLocation(programId, "uDensity"), density);
    glUniform1f(glGetUniformLocation(programId, "uWeight"), weight);
    glUniform1i(glGetUniformLocation(programId, "uSamples"), samples);
  }

  //////////////////////Screen mat settings for scattering 3rd pass

  CPMScatteringPass3MatSet::CPMScatteringPass3MatSet()
  {

  }

  //////////////////////

  CPMScatteringPass3MatSet::CPMScatteringPass3MatSet(const char* vs, const char* fs)
  {
    SMaterialSettings::SMaterialSettings(vs, fs);
    vsp = vs;
    fsp = fs;
  }

  //////////////////////

  CPMScatteringPass3MatSet::~CPMScatteringPass3MatSet()
  {

  }

  //////////////////////

  void CPMScatteringPass3MatSet::GetLocations(int pid)
  {
    SE::SBaseScreenMaterialSet::GetLocations(pid);
    stdImgLoc = glGetUniformLocation(pid,"uStandarTexture");
  }

  //////////////////////

  void CPMScatteringPass3MatSet::SetLocations(int pid)
  {
    SE::SBaseScreenMaterialSet::SetLocations(pid);
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, stdImg);
    glUniform1i(stdImgLoc, 2);
  }

  //////////////////////Screen mat settings for blur

  CPMBlurMatSet::CPMBlurMatSet()
  {

  }

  //////////////////////

  CPMBlurMatSet::CPMBlurMatSet(const char* vs, const char* fs)
  {
    SMaterialSettings::SMaterialSettings(vs, fs);
    vsp = vs;
    fsp = fs;
  }

  //////////////////////

  CPMBlurMatSet::~CPMBlurMatSet()
  {

  }

  //////////////////////

  void CPMBlurMatSet::GetLocations(int pid)
  {
    SE::SBaseScreenMaterialSet::GetLocations(pid);
    doVerticalBlurLoc = glGetUniformLocation(pid,"uBlurVertical");
  }

  //////////////////////

  void CPMBlurMatSet::SetLocations(int pid)
  {
    SE::SBaseScreenMaterialSet::SetLocations(pid);
    glUniform1i(doVerticalBlurLoc,doVerticalBlur);
  }

  //////////////////////Screen mat settings for bloom second pass

  CPMBloom2MatSet::CPMBloom2MatSet()
  {

  }

  //////////////////////

  CPMBloom2MatSet::CPMBloom2MatSet(const char* vs, const char* fs)
  {
    SMaterialSettings::SMaterialSettings(vs, fs);
    vsp = vs;
    fsp = fs;
  }

  //////////////////////

  CPMBloom2MatSet::~CPMBloom2MatSet()
  {

  }

  //////////////////////

  void CPMBloom2MatSet::GetLocations(int pid)
  {
    SE::SBaseScreenMaterialSet::GetLocations(pid);
    stdImgLoc = glGetUniformLocation(pid,"uStandarTexture");
  }

  //////////////////////

  void CPMBloom2MatSet::SetLocations(int pid)
  {
    SE::SBaseScreenMaterialSet::SetLocations(pid);
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, stdImg);
    glUniform1i(stdImgLoc, 2);
  }

  //////////////////////

}
