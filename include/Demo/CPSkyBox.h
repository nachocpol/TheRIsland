/*
  CPSkyBox.h cortizopo@esat-alumni.com
*/

#ifndef CPSKYBOX_H
#define CPSKYBOX_H

#include "Engine/SMesh.h"
#include "Engine/SMaterial.h"
#include "Engine/STextureCubeMap.h"

namespace Demo
{
  class CPSkyBox
  {
  public:
    CPSkyBox();
    ~CPSkyBox();
    void Init();
    void Draw();

  private:
    SE::SMesh* skyBoxMesh = nullptr;
    SE::SMaterialSettings* skyBoxMs = nullptr;
    SE::SMaterial* skyBoxMaterial = nullptr;
    SE::STextureCubeMap cubeMap;

  };
}
#endif