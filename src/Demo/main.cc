/*
  main.cc cortizopo@esat-alumni.com
  Entry point of the demo
*/

#include "Demo/main.h"

#include "Engine/SRenderManager.h"
#include "Engine/SGameObject.h"
#include "Engine/SResourceManager.h"
#include "Engine/SLight.h"
#include "Engine/SMaterial.h"

#include "Demo/CPMatSettings.h"
#include "Demo/CPScene.h"

#include "gtc/random.hpp"

#include "PerlinNoise.h"

/////////////////////////////////

using namespace SE;
using namespace Demo;

//Global variables
SRenderManager* renderManager;
CPScene* scene;
SCamera* mainCamera;
CPTerrainMatSet* terrainMatSet;
SGameObject* water;
CPWaterMatSet* waterMatSet;

SPhysicalMatSet* palmMatSet;


/////////////////////////////////

void Init();
void Input();
void Update();
void Draw();
void CleanUp();

//Terrain stuff (pos x,pos y,terrain id)
unsigned int GetHeight(int, int, int);
unsigned char* hData00; //0
unsigned char* hData01; //1
unsigned char* hData10; //2
unsigned char* hData11; //3
int hX, hY,hN;
const float hScale = 0.15f;//0.15 old
glm::vec3 g_terPosition(0.0f, -19.5f, 0.0f);
glm::vec3 g_terrScale(1.0f,1.0f,1.0f);
unsigned int g_terrId = -1;

//size z,size x,quad size,z offset,x offset,use plane
enum BuildMode{kPerlin,kPlane,kHeightMap};
SMeshConstructorProcedural BuildPlaneData(unsigned int, unsigned int, float,float,float,BuildMode,int id);
std::vector<float> CalcSmoothNormal(int,int,float,BuildMode,int id);
glm::vec3 CalcQuadNorm(float, float, float, BuildMode,int id);
std::vector<float> CalcTangent(float, float, float,BuildMode,int id);
float PoliInterpolate(float, float);
void GenerateGrass(std::vector<float>& pos);
void InitInstGrass();

//Grass instanced matrix
std::vector<glm::mat4> g_grassInstMat;

/////////////////////////////////

int main()
{
  Init();
  while (true)
  {
    renderManager->Events();
    renderManager->MainRender();
    /////////////////////////////
    Input();
    Update();
    Draw();
    /////////////////////////////
    renderManager->Swap();
  }
  CleanUp();
  return 0;
}

/////////////////////////////////

void Init()
{
  //Initialize and cache the render manager
  renderManager = SRenderManager::GetInstance();
  renderManager->Init(1024, 600, "cpDemo");
  renderManager->SetClearColor(glm::vec3(1.0));

  //Enable clipping planes
  glEnable(GL_CLIP_DISTANCE0);

  //Init resoruce manager
  SResourceManager::GetInstance();

  //Load the terrain hmap
  hData00 = renderManager->LoadTextureFromFile("../data/img/terrain/canyonhm.bmp", hX, hY,hN);
  //hData01 = renderManager->LoadTextureFromFile("../data/img/terrain/TiledTerrain/tMap01.bmp", hX, hY,hN);
  //hData10 = renderManager->LoadTextureFromFile("../data/img/terrain/TiledTerrain/tMap10.bmp", hX, hY,hN);
  //hData11 = renderManager->LoadTextureFromFile("../data/img/terrain/TiledTerrain/tMap11.bmp", hX, hY,hN);

  //Camera
  mainCamera = new SCamera;
  mainCamera->Init(60.0f, 0.01f, 500.0f);
  mainCamera->GetPosition()->y = 3.0f;
  mainCamera->GetPosition()->z = 8.0f;

  //Scene
  scene = new CPScene;
  scene->SetCamera(*mainCamera);
  scene->Init();
  unsigned int cid;


  //Build the terrain mat settings
  terrainMatSet = new CPTerrainMatSet
    (
    "../data/shaders/terrain.vs", "../data/shaders/terrain.fs",
    "../data/img/terrain/dirt.png",
    "../data/img/terrain/dirt.png",
    "../data/img/terrain/dirt.png",
    "../data/img/terrain/cliff.png",
    "../data/img/terrain/dirtn.png",
    "../data/img/terrain/cliffn.png"
    );
  terrainMatSet->camP = mainCamera->GetPosition();

  //Terrain
  g_terrId = scene->AddGameObject();
  scene->goList[g_terrId]->AddComponent<STransform>();
  scene->goList[g_terrId]->transform->SetPosition(g_terPosition);
  scene->goList[g_terrId]->transform->SetScale(g_terrScale);
  scene->goList[g_terrId]->AddComponent<SMesh>();
  scene->goList[g_terrId]->mesh->Init
    (
    BuildPlaneData(128, 128, 1.0f,
                   0 * 256,
                   0 * 256,
                   kHeightMap,
                   0)
    );
  scene->goList[g_terrId]->AddComponent<SMaterial>();
  scene->goList[g_terrId]->material->Init(terrainMatSet);
  scene->goList[g_terrId]->myName = "Terrain";
  InitInstGrass();
 

  //Build water material settings
  waterMatSet = new CPWaterMatSet
    (
    "../data/shaders/water.vs", "../data/shaders/water.fs"
    );
  waterMatSet->camP = mainCamera->GetPosition();
  waterMatSet->reflectTex = scene->GetReflectTex();
  waterMatSet->refractTex = scene->GetRefractTex();
  waterMatSet->depthTex = scene->GetDepthTex();

  //Water game object
  water = new SGameObject();
  scene->water = water;
  water->AddComponent<STransform>();
  water->AddComponent<SMesh>();
  water->AddComponent<SMaterial>();
  water->transform->SetPosition(glm::vec3(0.0f, 2.0f, 0.0f));
  water->mesh->Init(SMeshConstructorProcedural(BuildPlaneData(1,1,1024.0f,0,0,kPlane,0)));
  water->material->Init(waterMatSet);
  water->myName = "Water";

  //Init palm
  /*
  palmMatSet = new SPhysicalMatSet
    (
    "../data/shaders/seriousPS.vs", "../data/shaders/seriousPS.fs",
    "../data/img/palmtree/diffus.png", "../data/img/palmtree/gloss.png", "../data/img/palmtree/specular.png", "../data/img/palmtree/normal.png"
    );
  palmMatSet->refCamPos = mainCamera->GetPosition();  

  cid = scene->AddGameObject();
  scene->goList[cid]->AddComponent<STransform>();
  scene->goList[cid]->AddComponent<SMaterial>();
  scene->goList[cid]->AddComponent<SMesh>(SMeshConstructor(kTriangles, "../data/imported/palmtree.sm"));
  scene->goList[cid]->transform->SetPosition(glm::vec3(15,-1,15));
  scene->goList[cid]->transform->SetScale(glm::vec3(0.03));
  scene->goList[cid]->material->Init(palmMatSet);
  scene->goList[cid]->myName = "Palm";

  
  //Another palm
  cId = scene->AddGameObject();
  scene->goList[cid]->AddComponent<STransform>();
  scene->goList[cid]->AddComponent<SMaterial>();
  scene->goList[cid]->AddComponent<SMesh>(SMeshConstructor(kTriangles, "../data/imported/palmtree.sm"));
  scene->goList[cid]->transform->SetPosition(glm::vec3(16, -1, 35));
  scene->goList[cid]->transform->SetScale(glm::vec3(0.03));
  scene->goList[cid]->transform->Rotate(glm::vec3(0, 40, 0));
  scene->goList[cid]->material->Init(palmMatSet);
  scene->goList[cid]->myName = "Palmo";

  //Another palm
  cId = scene->AddGameObject();
  scene->goList[cid]->AddComponent<STransform>();
  scene->goList[cid]->AddComponent<SMaterial>();
  scene->goList[cid]->AddComponent<SMesh>(SMeshConstructor(kTriangles, "../data/imported/palmtree.sm"));
  scene->goList[cid]->transform->SetPosition(glm::vec3(18, -2, 45));
  scene->goList[cid]->transform->SetScale(glm::vec3(0.03));
  scene->goList[cid]->transform->Rotate(glm::vec3(10, 70, 0));
  scene->goList[cid]->material->Init(palmMatSet);
  scene->goList[cid]->myName = "Palmi";
  */

  //Sun light
  cid = scene->AddGameObject();
  scene->goList[cid]->AddComponent<STransform>();
  scene->goList[cid]->AddComponent<SLight>();
  scene->goList[cid]->transform->SetPosition(glm::vec3(900, 800, 2048));
  scene->goList[cid]->light->Init(SE::SLightInfo(SE::kSun, glm::vec3(0, -0.5f, -1), glm::vec3(1.0f)));
  scene->goList[cid]->myName = "Sun Light";
}

/////////////////////////////////

void Input()
{
  if(SInput::GetInstance()->GetKey(kKeyR))
  {
    scene->goList[g_terrId]->material->Reload();
    water->material->Reload();
    scene->ReloadShaders();
  }
}

/////////////////////////////////

void Update()
{
  scene->Update();
}

/////////////////////////////////

void Draw()
{
  scene->Render();
}

/////////////////////////////////

void CleanUp()
{
  renderManager->ShutDown();
  delete scene;
  delete mainCamera;
}

/////////////////////////////////


SMeshConstructorProcedural BuildPlaneData(unsigned int sZ, unsigned sX,float qSize,float xOff,float zOff,BuildMode bMode,int id)
{
  //Init model data
  SBinaryModelData modelData;

  std::vector<float>positions;
  std::vector<float>uvs;
  std::vector<float>normals;
  std::vector<float>tangents;

  //Vertex data
  for (float z = zOff*qSize; z < (sZ+zOff)*qSize; z+=qSize)
  {
    for (float x = xOff*qSize; x < (sX+xOff)*qSize; x+=qSize)
    {
      //Find each quad vertex
      glm::vec3 bl, br, tr, tl;
      float cx, cz, cxq, czq;
      cx = x;
      cz = z;
      cxq = (float)cx+qSize;
      czq = (float)cz+qSize;

      if (bMode == kPerlin)
      {
        bl = glm::vec3(cx, octave_noise_2d(octaves, persistance, scale, cx, cz)*mult, cz);
        br = glm::vec3((float)x + qSize, octave_noise_2d(octaves, persistance, scale, (float)x + qSize, z)*mult, z);
        tr = glm::vec3((float)x + qSize, octave_noise_2d(octaves, persistance, scale, (float)x + qSize, (float)z + qSize)*mult, (float)z + qSize);
        tl = glm::vec3(x, octave_noise_2d(octaves, persistance, scale, x, (float)z + qSize)*mult, (float)z + qSize);
      }
      if (bMode == kPlane)
      {
        bl = glm::vec3(cx, 0, cz);
        br = glm::vec3(cxq, 0, cz);
        tr = glm::vec3(cxq, 0, czq);
        tl = glm::vec3(x, 0, czq);
      }
      else if (bMode == kHeightMap)
      {
        bl = glm::vec3(cx, GetHeight(x, z,id)*hScale, cz);
        br = glm::vec3(cxq, GetHeight(x + 1, z,id)*hScale, cz);
        tr = glm::vec3(cxq, GetHeight(x + 1, z + 1,id)*hScale, czq);
        tl = glm::vec3(cx, GetHeight(x, z + 1,id)*hScale, czq);
      }
     
      //Positions
      //0
      positions.push_back(bl.x);
      positions.push_back(bl.y);
      positions.push_back(bl.z);
      //1              
      positions.push_back(br.x);
      positions.push_back(br.y);
      positions.push_back(br.z);
      //2
      positions.push_back(tr.x);
      positions.push_back(tr.y);
      positions.push_back(tr.z);
      //3
      positions.push_back(tl.x);
      positions.push_back(tl.y);
      positions.push_back(tl.z);

      //Uvs
      uvs.push_back(0);
      uvs.push_back(0);

      uvs.push_back(1);
      uvs.push_back(0);

      uvs.push_back(1);
      uvs.push_back(1);

      uvs.push_back(0);
      uvs.push_back(1);

      //Normals
      std::vector<float> n = CalcSmoothNormal(x, z, qSize,bMode,id);
      normals.insert(normals.end(), n.begin(), n.end());

      //Tangents
      std::vector<float> t = CalcTangent(x, z, qSize,bMode,id);
      tangents.insert(tangents.end(), t.begin(), t.end());
    }
  }

  std::vector<unsigned int> indices;
  unsigned int cnt = 0;
  
  //Indices
  for (unsigned int i = 0; i < sX*sZ; i++)
  {
    indices.push_back(2+cnt);
    indices.push_back(1+cnt);
    indices.push_back(0+cnt);

    indices.push_back(3+cnt);
    indices.push_back(2+cnt);
    indices.push_back(0+cnt); 

    cnt += 4;
  }

  //Build the mesh constructor
  modelData.indices = indices;
  modelData.indicesSize = indices.size();
  modelData.vertices = positions;
  modelData.vertices.insert(modelData.vertices.end(), uvs.begin(), uvs.end());
  modelData.vertices.insert(modelData.vertices.end(), normals.begin(), normals.end());
  modelData.vertices.insert(modelData.vertices.end(), tangents.begin(), tangents.end());
  modelData.vertexSize = positions.size();
  modelData.normalSize = normals.size();
  modelData.UVSize = uvs.size();
  modelData.tangentsSize = tangents.size();
  

  //Generate grass
  GenerateGrass(positions);

  SMeshConstructorProcedural dataToReturn(kTriangles, modelData);
  return dataToReturn;
}

/////////////////////////////////

std::vector<float> CalcSmoothNormal(int x,int z,float qSize,BuildMode bMode,int id)
{
  glm::vec3 bln, brn, trn, tln;

  bln = CalcQuadNorm(x, z, qSize, bMode,id);
  brn = CalcQuadNorm(x + 1, z, qSize, bMode,id);
  trn = CalcQuadNorm(x + 1, z + 1, qSize, bMode,id);
  tln = CalcQuadNorm(x , z + 1, qSize, bMode,id);

  std::vector<float> retNorm;

  retNorm.push_back(bln.x);
  retNorm.push_back(bln.y);
  retNorm.push_back(bln.z);

  retNorm.push_back(brn.x);
  retNorm.push_back(brn.y);
  retNorm.push_back(brn.z);

  retNorm.push_back(trn.x);
  retNorm.push_back(trn.y);
  retNorm.push_back(trn.z);

  retNorm.push_back(tln.x);
  retNorm.push_back(tln.y);
  retNorm.push_back(tln.z);

  return retNorm;
}

/////////////////////////////////

glm::vec3 CalcQuadNorm(float x, float z, float qSize, BuildMode bMode,int id)
{
  glm::vec3 fN(0, 1, 0);
  if (bMode == kHeightMap)
  {
    //Todo add qsize to -
    float hL = GetHeight(x - 1, z,id)*hScale;
    float hR = GetHeight(x + 1, z,id)*hScale;
    float hB = GetHeight(x, z - 1,id)*hScale;
    float hT = GetHeight(x, z + 1,id)*hScale;
    fN = glm::vec3(hL - hR, 2.0f, hB - hT);
  }
  else if (bMode == kPerlin)
  {
    float hL = octave_noise_2d(octaves, persistance, scale, x - qSize, z)*mult;
    float hR = octave_noise_2d(octaves, persistance, scale, x + qSize, z)*mult;
    float hB = octave_noise_2d(octaves, persistance, scale, x, z - qSize)*mult;
    float hT = octave_noise_2d(octaves, persistance, scale, x, z + qSize)*mult;
    fN = glm::vec3(hL - hR, 2.0f, hB - hT);
    fN = glm::mix(glm::vec3(0, 1.0f, 0), fN, 0.6f);
  }

  return glm::normalize(fN);
}


/////////////////////////////////

std::vector<float> CalcTangent(float x, float z, float qSize,BuildMode bMode,int id)
{
  //Cur quad
  float cx = x;
  float cz = z;
  
  glm::vec3 bl, br, tr, tl;

  //From noise data
  if (bMode == kPerlin)
  {
    bl = glm::vec3(cx, octave_noise_2d(octaves, persistance, scale, cx, cz)*mult, cz);
    br = glm::vec3(cx + qSize, octave_noise_2d(octaves, persistance, scale, cx + qSize, cz)*mult, cz);
    tr = glm::vec3(cx + qSize, octave_noise_2d(octaves, persistance, scale, cx + qSize, cz + qSize)*mult, cz + qSize);
    tl = glm::vec3(cx, octave_noise_2d(octaves, persistance, scale, cx, cz + qSize)*mult, cz + qSize);
  }
  //From heightmap data
  else if (bMode == kHeightMap)
  {
    bl = glm::vec3(cx, GetHeight(x, z,id)*hScale, cz);
    br = glm::vec3(cx + qSize, GetHeight(x + qSize, z,id)*hScale, cz);
    tr = glm::vec3(cx + qSize, GetHeight(x + qSize, z + qSize,id)*hScale, cz + qSize);
    tl = glm::vec3(cx, GetHeight(x, z + qSize,id)*hScale, cz + qSize);
  }

  glm::vec2 uv0(0, 0);
  glm::vec2 uv1(1, 0);
  glm::vec2 uv2(1, 1);
  glm::vec2 uv3(0, 1);

  glm::vec3 edge0 = br - bl;
  glm::vec3 edge1 = tr - bl;
  
  glm::vec2 duv0 = uv1 - uv0;
  glm::vec2 duv1 = uv2 - uv0;

  float k = 1.0f / (duv0.x*duv1.y - duv1.x*duv0.y);

  //This will be the tangent for the first quad(bl-br-tr)
  //maybe we should calc for the top quad(bl-tr-tl)
  glm::vec3 tan(0.0f);
  tan.x = k*(duv1.y*edge0.x - duv0.y*edge1.x);
  tan.y = k*(duv1.y*edge0.y - duv0.y*edge1.y);
  tan.z = k*(duv1.y*edge0.z - duv0.y*edge1.z);
  tan = glm::normalize(tan);

  //Build the returned vector
  std::vector<float> retTang;
  retTang.push_back(tan.x);
  retTang.push_back(tan.y);
  retTang.push_back(tan.z);

  retTang.push_back(tan.x);
  retTang.push_back(tan.y);
  retTang.push_back(tan.z);

  retTang.push_back(tan.x);
  retTang.push_back(tan.y);
  retTang.push_back(tan.z);

  retTang.push_back(tan.x);
  retTang.push_back(tan.y);
  retTang.push_back(tan.z);


  return retTang;
}

/////////////////////////////////

unsigned int GetHeight(int x, int y,int id)
{
  /*
    Notes:
    int foo[filas][columnas]
    int foo[alto][ancho]
  */

  if (x > hX)
  {
    x = hX;
  }
  if (x < 0)
  {
    x = 0;
  }
  if (y > hY)
  {
    y = hY;
  }
  if (y < 0)
  {
    y = 0;
  }
  
  int idx = x*(hY*hN) + y*hN;

  switch (id)
  {
  case 0:
    return (unsigned int)hData00[idx];
    break;
  case 1:
    return (unsigned int)hData01[idx];
    break;
  case 2:
    return (unsigned int)hData10[idx];
    break;
  case 3:
    return (unsigned int)hData11[idx];
    break;  
  }
}

/////////////////////////////////

void GenerateGrass(std::vector<float>& pos)
{
  for (unsigned int i = 0; i < pos.size(); i+=3)
  {
    glm::vec3 curPos(pos[i], pos[i+1], pos[i+2]);
    curPos += g_terPosition;
    curPos *= g_terrScale;
    if (curPos.y > 13.0f  && curPos.y < 20.5f)
    {
      float n = octave_noise_2d(8.0f, 0.2f, 0.1f, curPos.x, curPos.z)+1.0f;
      //printf("Cur noise val:%f\n", n);
      if (n > 1.0f)
      {
        float randScale = glm::linearRand(0.15f,0.5f);
        float randXRot = glm::linearRand(-20.0f, 20.0f);
        float randYRot = glm::linearRand(0.0f, 359.0f);
        glm::mat4 curModel;
        curPos.y -= 0.1f; //add an offset
        curModel = glm::translate(curModel, curPos);
        curModel = glm::rotate(curModel, glm::radians(randXRot), glm::vec3(1.0f, 0.0f, 0.0f));
        curModel = glm::rotate(curModel, glm::radians(randYRot), glm::vec3(0.0f,1.0f,0.0f));
        curModel = glm::scale(curModel, glm::vec3(randScale));
        g_grassInstMat.push_back(curModel);
      }
    }
  }
}

/////////////////////////////////

void InitInstGrass()
{
    SE::SBinaryModelData mDat;
    mDat.vertices =
    {
      //Positions
      0.0f, 0.0f, 0.0f,
      1.0f, 0.0f, 0.0f,
      1.0f, 1.0f, 0.0f,
      0.0f, 1.0f, 0.0f,
      //Uvs
      0.0f, 1.0f,
      1.0f, 1.0f,
      1.0f, 0.0f,
      0.0f, 0.0f,
      //Normals
      0.0f, 0.0f, -1.0f,
      0.0f, 0.0f, -1.0f,
      0.0f, 0.0f, -1.0f,
      0.0f, 0.0f, -1.0f
    };
    mDat.vertexSize = 12;
    mDat.UVSize = 8;
    mDat.normalSize = 12;
    mDat.indices =
    {
      0, 1, 2,
      0, 2, 3
    };
    mDat.indicesSize = 6;

    SE::SMeshConstructorProcedural gMeshConst(SE::kTriangles, mDat);
    scene->grassInstance->Init(g_grassInstMat, gMeshConst);
}

/////////////////////////////////



/*

int id = 0;
  for (unsigned int i = 0; i < 2; i++)
  {
    for (unsigned int j = 0; j < 2; j++)
    {
      //Initialize objects
      unsigned int tId = scene->AddGameObject();
      scene->goList[tId]->AddComponent<STransform>();
      scene->goList[tId]->transform->SetPosition(glm::vec3(0,0,0));
      scene->goList[tId]->transform->SetScale(glm::vec3(0.5f));
      scene->goList[tId]->AddComponent<SMesh>();
      scene->goList[tId]->mesh->Init
        (
          BuildPlaneData(128,128, 1.0f,i*128,j*128,kHeightMap,id)
        );
      scene->goList[tId]->AddComponent<SMaterial>();
      scene->goList[tId]->material->Init(terrainMatSet);
      std::string name = "TerrainChunk" + std::to_string(i) + "," + std::to_string(j);
      scene->goList[tId]->myName = name;
      id++;
    }
  }

*/