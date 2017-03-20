/*
  CPSkyBox.cc cortizopo@esat-alumni.com
*/

#include "Demo/CPSkyBox.h"

#include <vector>

namespace Demo
{
  //////////////////////

  CPSkyBox::CPSkyBox()
  {

  }

  //////////////////////

  CPSkyBox::~CPSkyBox()
  {
    delete skyBoxMesh;
  }

  //////////////////////

  void CPSkyBox::Init()
  {    
    skyBoxMesh = new SE::SMesh();
    skyBoxMesh->Init(SE::SMeshConstructor(SE::kTriangles, "../data/imported/cube.sm"));

    skyBoxMs = new SE::SMaterialSettings("../data/shaders/SkyBox.vs", "../data/shaders/SkyBox.fs");
    skyBoxMaterial = new SE::SMaterial();
    skyBoxMaterial->Init(skyBoxMs);

    std::vector<std::string> textures;
    textures.push_back("../data/img/cubemaps/TropicalSunnyDay/right.png");
    textures.push_back("../data/img/cubemaps/TropicalSunnyDay/left.png");
    textures.push_back("../data/img/cubemaps/TropicalSunnyDay/top.png");
    textures.push_back("../data/img/cubemaps/TropicalSunnyDay/bottom.png");
    textures.push_back("../data/img/cubemaps/TropicalSunnyDay/back.png");
    textures.push_back("../data/img/cubemaps/TropicalSunnyDay/front.png");

    cubeMap.Init(textures);
  }

  //////////////////////

  void CPSkyBox::Draw()
  {
    glCullFace(GL_FRONT);
    glDepthMask(GL_FALSE);

    skyBoxMaterial->Use();

    //Set the skybox
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.GetId());
    glUniform1i(glGetUniformLocation(skyBoxMaterial->GetId(),"uEnvMap"), 0);

    skyBoxMesh->MeshRender();

    glDepthMask(GL_TRUE);
    glCullFace(GL_BACK);
  }

  //////////////////////
}