/*
  CPInstancedMesh.h cortizopo@esat-alumni.com
*/

#ifndef CPINSTANCEDMESH_H
#define CPINSTANCEDMESH_H

#include "Engine/SRenderManager.h"
#include "Engine/SMesh.h"
#include "CPMatSettings.h"

namespace Demo
{
  class CPInstancedMesh
  {
  public:
    CPInstancedMesh();
    ~CPInstancedMesh();
    
    /*
      Initializes the positions for the instanced mesh.
      @param positions The positions
      @param meshConst Struct to construct the mesh.
      @return True if succes creating it.
    */
    bool Init(std::vector<glm::mat4>& iMatrix, SE::SMeshConstructorProcedural meshConst);

    /*
      Renders this instanced mesh.
      @param shadowPass True if perform the shadow pass.
    */
    void Render(bool shadowPass = false);

    SE::SMaterial* instanceMaterial = nullptr;

  private:
    std::vector<glm::mat4> instancedMatrix;
    GLuint instPosVBO = -1;
    SE::SRenderManager* rendMan = nullptr;
    SE::SMesh* instancedMesh = nullptr;
    CPVegetationMatSet* instancedMatSet = nullptr;

  };
}

#endif