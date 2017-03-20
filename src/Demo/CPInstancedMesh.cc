/*
  CPInstancedMesh.h cortizopo@esat-alumni.com
*/

#include "Demo/CPInstancedMesh.h"

namespace Demo
{
  ///////////////////////////////////////////////////////////////////

  CPInstancedMesh::CPInstancedMesh()
  {

  }

  ///////////////////////////////////////////////////////////////////

  CPInstancedMesh::~CPInstancedMesh()
  {
    delete instancedMesh;
    delete instanceMaterial;
    delete instancedMatSet;
  }

  ///////////////////////////////////////////////////////////////////

  bool CPInstancedMesh::Init(std::vector<glm::mat4>& iMatrix,SE::SMeshConstructorProcedural meshConst)
  {
    if(iMatrix.size() == 0)
    {
      return false;
    }
    //Save the positions of the instanced mesh
    //instancedPositions = positionsl;
    instancedMatrix.insert(instancedMatrix.end(), iMatrix.begin(), iMatrix.end());

    //Initialize mesh
    rendMan = SE::SRenderManager::GetInstance();

    instancedMesh = new SE::SMesh;
    instancedMesh->Init(meshConst,true);

    rendMan->BindVertexArray(instancedMesh->GetVao());
    {
      //Generate and upload data to the model buffer
      rendMan->GenBuffer(1, instPosVBO);
      rendMan->BindBuffer(SE::kArray, instPosVBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4)*instancedMatrix.size(), &instancedMatrix[0][0], GL_STATIC_DRAW);

      //Enables the model matrix (4*glm::vec4)
	    for (unsigned int i = 0; i < 4; i++)
	    {
		    rendMan->SetVertAttribPoint(4+i, 4, SE::KFloat, false, sizeof(glm::mat4), i * sizeof(glm::vec4));
		    glVertexAttribDivisor(4+i, 1);
	    }
    }
    rendMan->UnbindVertexArray();

    //Initialize material
    instanceMaterial = new SE::SMaterial;
    instancedMatSet = new CPVegetationMatSet
      (
      "../data/shaders/vegetation.vs", "../data/shaders/vegetation.fs"
       );
    instanceMaterial->Init(instancedMatSet);
    
    return true;
  }

  ///////////////////////////////////////////////////////////////////

  void CPInstancedMesh::Render(bool shadowPass)
  {
    if(instancedMatrix.size() == 0)
    {
      return;
    }

    glDisable(GL_CULL_FACE);
    if(!shadowPass)
    {
      instanceMaterial->Use();
    }
    rendMan->BindVertexArray(instancedMesh->GetVao());
    {
      glBindBuffer(GL_ARRAY_BUFFER, instPosVBO);
      //glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4)*instancedMatrix.size(), &instancedMatrix[0][0], GL_STATIC_DRAW);
      glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, instancedMatrix.size());
    }
    rendMan->UnbindVertexArray();
    glEnable(GL_CULL_FACE);
  }

  ///////////////////////////////////////////////////////////////////
}