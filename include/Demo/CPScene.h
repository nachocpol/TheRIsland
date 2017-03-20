/*
  CPScene.h cortizopo@esat-alumni.com
*/

#ifndef CPSCENE_H
#define CPSCENE_H

#include "Engine/SScene.h"
#include "Engine/SFrameBuffer.h"
#include "CPSkyBox.h"
#include "CPInstancedMesh.h"

namespace Demo
{
  class CPScene : public SE::SScene
  {
  public:
    CPScene();
    ~CPScene();

    void Render()override;

    int GetReflectTex(){ return wreflectionFbo->GetColorTex(); }
    
    int GetRefractTex(){ return wrefractionFbo->GetColorTex(); }

    int GetDepthTex(){ return wrefractionFbo->GetDeptStencilhTex(); }

    //Water game object
    SE::SGameObject* water;

    //Grass instance mesh
    CPInstancedMesh* grassInstance = nullptr;

  private:
    /*
      Rewrote the render gui method.
    */
    virtual void RenderGUI();

    /*
      Returns the position of the sun in normalized device.
      @return Returns the position of the sun in normalized device.
    */
    glm::vec4 GetSunNormDevice();

    //Final fbo
    SE::SFrameBuffer* finalFbo = nullptr;
    SE::SBaseScreenMaterialSet* finalFboMs = nullptr;

    //Water fbos
    SE::SFrameBuffer* wreflectionFbo = nullptr;
    SE::SFrameBuffer* wrefractionFbo = nullptr;

    //Vignetting
    SE::SFrameBuffer* vignetFbo = nullptr;
    SE::SBaseScreenMaterialSet* vignetMs = nullptr;

    //Scattering
    //1:Render objects as black,light as a white point,bg as pink
    SE::SFrameBuffer* scatPass1Fbo = nullptr;
    CPMScatterPass1MatSet* scatPass1Ms = nullptr;
    //2:Extend the rays from the light point
    SE::SFrameBuffer* scatPass2Fbo = nullptr;
    CPMScatterPass1MatSet* scatPass2Ms = nullptr;
    //3:Mix images
    SE::SFrameBuffer* scatPass3Fbo = nullptr;
    CPMScatteringPass3MatSet* scatPass3Ms = nullptr;

    //Blur
    SE::SFrameBuffer* blurFbo = nullptr;
    CPMBlurMatSet* blurMs = nullptr;
    SE::SFrameBuffer* blurTmpFbo = nullptr;
    SE::SBaseScreenMaterialSet* blurTmpMs =  nullptr;

    //Bloom
    SE::SFrameBuffer* bloom1Fbo = nullptr;
    SE::SBaseScreenMaterialSet* bloom1Ms = nullptr;
    SE::SFrameBuffer* bloom2Fbo = nullptr;
    CPMBloom2MatSet* bloom2Ms = nullptr;

    //Skybox
    CPSkyBox* skyBox = nullptr;

    //Imgui water params
    float wOff = 2.0f;
    float wClipOff = 0.1f;
    float wClipPlane = 2.0f;

    //Fog ubo and data
    unsigned int fogUbo = -1;
    glm::vec4 fogColorEnabled; //x,y,z:color a:enabled
    glm::vec4 fogStartEndMode; //x:start y:end z:mode
  };
}
#endif