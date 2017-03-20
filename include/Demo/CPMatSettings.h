/*
  CPMatSettings.h cortizopo@esat-alumni.com
*/

#ifndef CPMATSETTINGS_H
#define CPMATSETTINGS_H

#include "Engine/SMaterialSettings.h"
#include "Engine/SRenderManager.h"
#include "Engine/SResourceManager.h"

namespace Demo
{
  //Terrain material settings
  class CPTerrainMatSet :public SE::SMaterialSettings
  {
  public:
    CPTerrainMatSet();
    CPTerrainMatSet(const char* vs, const char* fs,
                   std::string gp,std::string grap,std::string sp,std::string cp,
                   std::string gnp,std::string cnp);
    ~CPTerrainMatSet();
    void GetLocations(int pid);
    void SetLocations(int pid);

    glm::vec3* camP;

  private:
    SE::SRenderManager* renderMang = nullptr;
    SE::SResourceManager* resMang = nullptr;

    std::string grassTexPath;
    std::string gravelTexPath;
    std::string snowTexPath;
    std::string cliffTexPath;

    std::string grassNormPath;
    std::string cliffNormPath;

    //Texture diffuse
    int grassTexLocation;
    int gravelTexLocation;
    int snowTexLocation;
    int cliffTexLocation;

    //Texture normal
    int grassNormLocation;
    int cliffNormLocation;

    //Camera
    int camLocation;
  };

  //Water material settings
  class CPWaterMatSet :public SE::SMaterialSettings
  {
  public:
    CPWaterMatSet();
    CPWaterMatSet(const char* vs, const char* fs);
    ~CPWaterMatSet();

    void GetLocations(int pid);
    void SetLocations(int pid);

    glm::vec3* camP;

    int reflectTex;
    int refractTex;
    int depthTex;

  private:
    SE::SRenderManager* renderMang = nullptr;
    SE::SResourceManager* resMang = nullptr;

    std::string wdudvPath = "../data/img/water/wdudv.png";
    std::string wnormPath = "../data/img/water/wnorm.png";
    std::string foamPath =  "../data/img/water/wfoam.jpg";

    int camLoc;
    int reflectTexLoc;
    int refractTexLoc;
    int depthTexLoc;
    int wdudvTexLoc;
    int wnormLoc;
    int timeLoc;
    int foamLoc;
  };

  //Instanced vegetation material settings
  class CPVegetationMatSet :public SE::SMaterialSettings
  {
  public:
    CPVegetationMatSet();
    CPVegetationMatSet(const char* vs, const char* fs);
    ~CPVegetationMatSet();

    void GetLocations(int pid);
    void SetLocations(int pid);

  private:
    SE::SRenderManager* renderMang = nullptr;
    SE::SResourceManager* resMang = nullptr;

    std::string grassBilPath = "../data/img/vegetation/grassbil.png";
    std::string gradientPath = "../data/img/vegetation/gradient.png";

    int grassBilLoc = -1;
    int gradientLoc = -1;
    int timeLoc = -1;
  };

  //Screen material settings for the scattering first pass
  class CPMScatterPass1MatSet: public SE::SBaseScreenMaterialSet
  {
  public:
    CPMScatterPass1MatSet();
    CPMScatterPass1MatSet(const char* vs, const char* fs);
    ~CPMScatterPass1MatSet();
    void GetLocations(int pid);
    void SetLocations(int pid);
    void ShowGui();

    glm::vec4* sunPosition;

  private:
    unsigned int sunPosLoc = -1;
    //TO-DO:nope nope noe
    unsigned int programId = -1;
  };

  //Screen material settings for last scattering pass
  class CPMScatteringPass3MatSet :public SE::SBaseScreenMaterialSet
  {
  public:
    CPMScatteringPass3MatSet();
    CPMScatteringPass3MatSet(const char* vs, const char* fs);
    ~CPMScatteringPass3MatSet();
    void GetLocations(int pid);
    void SetLocations(int pid);

    unsigned int stdImg = -1;

  private:
    unsigned int stdImgLoc = -1;
  };

  //Screen material settings for blur
  class CPMBlurMatSet :public SE::SBaseScreenMaterialSet
  {
  public:
    CPMBlurMatSet();
    CPMBlurMatSet(const char* vs, const char* fs);
    ~CPMBlurMatSet();
    void GetLocations(int pid);
    void SetLocations(int pid);

    bool doVerticalBlur = true;

  private:
    unsigned int doVerticalBlurLoc = -1;
  };

  //Screen material settings for bloom second pass
  class CPMBloom2MatSet :public SE::SBaseScreenMaterialSet
  {
  public:
    CPMBloom2MatSet();
    CPMBloom2MatSet(const char* vs, const char* fs);
    ~CPMBloom2MatSet();
    void GetLocations(int pid);
    void SetLocations(int pid);

    unsigned int stdImg = -1;

  private:
    unsigned int stdImgLoc = -1;
  };
}
#endif