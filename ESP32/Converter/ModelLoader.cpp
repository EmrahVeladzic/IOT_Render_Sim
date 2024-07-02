#include <vector>
#include <fstream>
#include "ModelLoader.hpp"



Image::Image(const char* fileName){
  uint8_t tempUByte;
  uint16_t tempUShort;
  std::vector<uint16_t>CLUT;

  std::string imgFile = std::string(fileName) + ".rpf";

  std::ifstream ImgStream;
  ImgStream.open(imgFile,std::ios::binary);
  ImgStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

  ImgStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));
  uint16_t txl_count = tempUByte +1;

  ImgStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));
  this->Width=((uint16_t)tempUByte)+1;

  ImgStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));
  this->Height=((uint16_t)tempUByte)+1;

  for(uint16_t i = 0; i<= txl_count; i++)
    {
      ImgStream.read(reinterpret_cast<char*>(&tempUShort), sizeof(tempUShort));

      CLUT.push_back(tempUShort);
    }
    
  size_t arr_size = (size_t)this->Width * (size_t)this->Height;
  this->Data = new uint16_t[arr_size];

  uint8_t lsb;
  uint8_t msb;
    
  for(size_t i = 0; i < arr_size; i+=2)
  {
    ImgStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

    msb = (tempUByte>>4)&0xF;
    lsb = (tempUByte)&0xF;

    this->Data[i]=CLUT[msb];
     this->Data[i+1]=CLUT[lsb];

  }
    
  CLUT.clear();
  ImgStream.close();

}
  



Sound::Sound(const char* fileName)
{
  std::string sfxFile = std::string(fileName) + ".wl";

  int16_t temp16 = 0;
  uint8_t sh_val = 0;
  int16_t old = 0;
  int16_t older = 0;
  int16_t oldest = 0;

  int16_t out = 0;

  uint8_t tempUByte;
  uint16_t tempUShort;
  uint32_t tempUInt;

 

  std::ifstream SfxStream;
  SfxStream.open(sfxFile,std::ios::binary);

  SfxStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

  SfxStream.read(reinterpret_cast<char*>(&tempUInt), sizeof(tempUInt));
  this->SampleCount=(tempUInt*SAMPLES_PER_BLOCK);

  SfxStream.read(reinterpret_cast<char*>(&tempUShort), sizeof(tempUShort));

  this->SampleRate=tempUShort;


 


  this->Data = new int16_t [this->SampleCount];



  for(uint32_t i =0; i< tempUInt ; i++){


    SfxStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));
    sh_val = (tempUByte >> 4) &0xF;

    SfxStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

      for(uint8_t j =0; j< (uint8_t)SAMPLES_PER_BLOCK; j+=2){

      SfxStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

      temp16 = (int16_t)((tempUByte>>4) &0xF);

      if (temp16 > MAX_INT_4) {
        temp16 = 0 - (temp16 - MAX_INT_4);
      }

      else if (temp16 < MIN_INT_4) {
        temp16 = 0 - (temp16 - MIN_INT_4);
      }

      temp16<<=sh_val;

      temp16-=old;

      out = 0;
      out +=  (int16_t)((int32_t)oldest * rounding_table[0] / DIVISOR);
      out +=  (int16_t)((int32_t)older * rounding_table[1] / DIVISOR);
      out +=  (int16_t)((int32_t)old * rounding_table[2] / DIVISOR);
      out +=  (int16_t)((int32_t)temp16 * rounding_table[3] / DIVISOR);



      this->Data[((i*SAMPLES_PER_BLOCK)+j)]=out;

      oldest=older;
      older=old;
      old=temp16;


      temp16 = (int16_t)(tempUByte &0xF);

      if (temp16 > MAX_INT_4) 
      {
        temp16 = 0 - (temp16 - MAX_INT_4);
      }

      else if (temp16 < MIN_INT_4) 
      {
        temp16 = 0 - (temp16 - MIN_INT_4);
      }

      temp16<<=sh_val;

      temp16-=old;

      out = 0;
      out +=  (int16_t)((int32_t)oldest * rounding_table[0] / DIVISOR);
      out +=  (int16_t)((int32_t)older * rounding_table[1] / DIVISOR);
      out +=  (int16_t)((int32_t)old * rounding_table[2] / DIVISOR);
      out +=  (int16_t)((int32_t)temp16 * rounding_table[3] / DIVISOR);

      this->Data[((i*SAMPLES_PER_BLOCK)+j+1)]=out;

      oldest=older;
      older=old;
      old=temp16;

       

    }

  }

  

  SfxStream.close();

}

Model::Model(const char* fileName){

  fix tempFix;
  uint8_t tempUByte;
  uint16_t tempUShort;
  float tempFloat;
  Mesh tempMesh;
  Bone tempBone;
  Animation tempAnim;


  std::string vtFile = std::string(fileName) + ".vt";
  std::string indFile = std::string(fileName) + ".ind";
  std::string uvFile = std::string(fileName) + ".uv";
  std::string nrmFile = std::string(fileName) + ".nrm";
  std::string fkrFile = std::string(fileName) + ".fkr";
  std::string anmFile = std::string(fileName) + ".anm";
    

  std::ifstream vtStream;

  vtStream.open(vtFile,std::ios::binary);
  vtStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

  uint8_t mshCount = tempUByte;


  vtStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

  uint8_t scalingFactor = tempUByte;

  for (uint8_t i = 0; i < mshCount; i++)
  {
    

    vtStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));
    tempMesh.Bone = (int)tempUByte;
    vtStream.read(reinterpret_cast<char*>(&tempUShort), sizeof(tempUShort));
    
    for (uint16_t j = 0; j < tempUShort; j++)
    {
      vtStream.read(reinterpret_cast<char*>(&tempFix), sizeof(tempFix));

      tempFloat = FIX_TO_F(tempFix,scalingFactor);

      tempMesh.Vertices.push_back(tempFloat);
    }
    this->Meshes.push_back(tempMesh);

    tempMesh.Vertices.clear();
  }

  vtStream.close();

  std::ifstream indStream;

  indStream.open(indFile,std::ios::binary);



  indStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

  mshCount = tempUByte;


  for (uint8_t i = 0; i < mshCount; i++)
  {
    indStream.read(reinterpret_cast<char*>(&tempUShort), sizeof(tempUShort));
    uint16_t indCount = tempUShort;

    for (uint16_t j = 0; j < indCount; j++)
    {
      indStream.read(reinterpret_cast<char*>(&tempUShort), sizeof(tempUShort));
      tempMesh.Indices.push_back(tempUShort);


    }

    this->Meshes[i].Indices.clear();
    this->Meshes[i].Indices = tempMesh.Indices;

    tempMesh.Indices.clear();

  }

  indStream.close();

  std::ifstream uvStream;

uvStream.open(uvFile,std::ios::binary);

uvStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

mshCount = tempUByte;

for (uint8_t i = 0; i < mshCount; i++)
{
	uvStream.read(reinterpret_cast<char*>(&tempUShort), sizeof(tempUShort));

	for (uint16_t j = 0; j < tempUShort; j++)
	{
		uvStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

		tempFloat = (float(tempUByte) / PAGE_FILE_SIZE);

		tempMesh.UVs.push_back(tempFloat);

	}


	this->Meshes[i].UVs.clear();
	this->Meshes[i].UVs = tempMesh.UVs;

	tempMesh.UVs.clear();

}
uvStream.close();


std::ifstream nrmStream;

nrmStream.open(nrmFile,std::ios::binary);

nrmStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

mshCount = tempUByte;

nrmStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

scalingFactor = tempUByte;

for (uint8_t i = 0; i < mshCount; i++)
{

	nrmStream.read(reinterpret_cast<char*>(&tempUShort), sizeof(tempUShort));

	for (uint16_t j = 0; j < tempUShort; j++)
	{

		nrmStream.read(reinterpret_cast<char*>(&tempFix), sizeof(tempFix));

		tempFloat = FIX_TO_F(tempFix,scalingFactor);

		tempMesh.Normals.push_back(tempFloat);

	}


	this->Meshes[i].Normals.clear();
	this->Meshes[i].Normals = tempMesh.Normals;

	tempMesh.Normals.clear();
}


nrmStream.close();


std::ifstream fkrStream;

fkrStream.open(fkrFile,std::ios::binary);

fkrStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

uint8_t boneCount = tempUByte;

fkrStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

scalingFactor = tempUByte;



for (uint8_t i = 0; i < boneCount; i++)
{
	fkrStream.read(reinterpret_cast<char*>(&tempFix), sizeof(tempFix));

	tempBone.T_Init[0] = FIX_TO_F(tempFix, scalingFactor);

	fkrStream.read(reinterpret_cast<char*>(&tempFix), sizeof(tempFix));

	tempBone.T_Init[1] = FIX_TO_F(tempFix, scalingFactor);

	fkrStream.read(reinterpret_cast<char*>(&tempFix), sizeof(tempFix));

	tempBone.T_Init[2] = FIX_TO_F(tempFix, scalingFactor);

	fkrStream.read(reinterpret_cast<char*>(&tempFix), sizeof(tempFix));

	tempBone.R_Init[0] = FIX_TO_F(tempFix, scalingFactor);

	fkrStream.read(reinterpret_cast<char*>(&tempFix), sizeof(tempFix));

	tempBone.R_Init[1] = FIX_TO_F(tempFix, scalingFactor);

	fkrStream.read(reinterpret_cast<char*>(&tempFix), sizeof(tempFix));

	tempBone.R_Init[2] = FIX_TO_F(tempFix, scalingFactor);

	fkrStream.read(reinterpret_cast<char*>(&tempFix), sizeof(tempFix));

	tempBone.R_Init[3]  = FIX_TO_F(tempFix, scalingFactor);

	fkrStream.read(reinterpret_cast<char*>(&tempFix), sizeof(tempFix));

	tempBone.S_Init[0] = FIX_TO_F(tempFix, scalingFactor);

	fkrStream.read(reinterpret_cast<char*>(&tempFix), sizeof(tempFix));

	tempBone.S_Init[1] = FIX_TO_F(tempFix, scalingFactor);

	fkrStream.read(reinterpret_cast<char*>(&tempFix), sizeof(tempFix));

	tempBone.S_Init[2]  = FIX_TO_F(tempFix, scalingFactor);

	fkrStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

	mshCount = tempUByte;

	for (uint8_t j = 0; j < mshCount; j++)
	{
		fkrStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

		tempBone.Children.push_back(tempUByte);
	}


	this->Bones.push_back(tempBone);
	

	tempBone.Children.clear();
}

fkrStream.close();

bool IsRoot;
for (uint8_t i = 0; i < (uint8_t)boneCount; i++)
{
	IsRoot = true;

  for (uint8_t j = 0; j < (uint8_t)boneCount; j++)
  {

    for(uint8_t k = 0; k < (uint8_t)this->Bones[j].Children.size();k++)
    {
      if(this->Bones[j].Children[k]==i){
        IsRoot = false;
        break;
      }
    }
    if(!IsRoot){
      break;
    }
  }

  this->Bones[i].Root=IsRoot;
}

	std::ifstream anmStream;

	anmStream.open(anmFile, std::ios::binary);

	anmStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

	boneCount = tempUByte;

	

	anmStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

	scalingFactor = tempUByte;

	

	anmStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

	uint8_t targetFPS = tempUByte;	



	for (uint8_t i = 0; i < boneCount; i++)
	{
		anmStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

		uint8_t animCount = tempUByte;


		for (uint8_t j = 0; j < animCount; j++) {

			anmStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

			uint8_t TRSCount = tempUByte;

			for (uint8_t k = 0; k < TRSCount; k++)
			{
				anmStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

				tempFloat = (float(tempUByte))/(float(targetFPS));

				if (k > 0) {

					while (tempFloat < tempAnim.T_Times[k-1])
					{
						tempFloat += 1.0f;
					}

				}

				tempAnim.T_Times.push_back(tempFloat);

				anmStream.read(reinterpret_cast<char*>(&tempFix), sizeof(tempFix));

				tempFloat = FIX_TO_F(tempFix, scalingFactor);

				tempAnim.Translations.push_back(tempFloat);

				anmStream.read(reinterpret_cast<char*>(&tempFix), sizeof(tempFix));

				tempFloat = FIX_TO_F(tempFix, scalingFactor);

				tempAnim.Translations.push_back(tempFloat);

				anmStream.read(reinterpret_cast<char*>(&tempFix), sizeof(tempFix));

				tempFloat = FIX_TO_F(tempFix, scalingFactor);				

				tempAnim.Translations.push_back(tempFloat);

			}


			anmStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

			TRSCount = tempUByte;

			for (uint8_t k = 0; k < TRSCount; k++)
			{
				anmStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

				tempFloat = (float(tempUByte)) / (float(targetFPS));

				if (k > 0) {

					while (tempFloat < tempAnim.R_Times[k - 1])
					{
						tempFloat += 1.0f;
					}

				}

				tempAnim.R_Times.push_back(tempFloat);

				anmStream.read(reinterpret_cast<char*>(&tempFix), sizeof(tempFix));

				tempFloat = FIX_TO_F(tempFix, scalingFactor);

				tempAnim.Rotations.push_back(tempFloat);

				anmStream.read(reinterpret_cast<char*>(&tempFix), sizeof(tempFix));

				tempFloat = FIX_TO_F(tempFix, scalingFactor);

				tempAnim.Rotations.push_back(tempFloat);

				anmStream.read(reinterpret_cast<char*>(&tempFix), sizeof(tempFix));

				tempFloat = FIX_TO_F(tempFix, scalingFactor);

				tempAnim.Rotations.push_back(tempFloat);

				anmStream.read(reinterpret_cast<char*>(&tempFix), sizeof(tempFix));

				tempFloat = FIX_TO_F(tempFix, scalingFactor);

				tempAnim.Rotations.push_back(tempFloat);


			}

			anmStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

			TRSCount = tempUByte;

			for (uint8_t k = 0; k < TRSCount; k++)
			{
				anmStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

				tempFloat = (float(tempUByte)) / (float(targetFPS));

				if (k > 0) {

					while (tempFloat < tempAnim.S_Times[k - 1])
					{
						tempFloat += 1.0f;
					}

				}

				tempAnim.S_Times.push_back(tempFloat);

				anmStream.read(reinterpret_cast<char*>(&tempFix), sizeof(tempFix));

				tempFloat = FIX_TO_F(tempFix, scalingFactor);

				tempAnim.Scales.push_back(tempFloat);

				anmStream.read(reinterpret_cast<char*>(&tempFix), sizeof(tempFix));

				tempFloat = FIX_TO_F(tempFix, scalingFactor);

			  tempAnim.Scales.push_back(tempFloat);

				anmStream.read(reinterpret_cast<char*>(&tempFix), sizeof(tempFix));

				tempFloat = FIX_TO_F(tempFix, scalingFactor);

				tempAnim.Scales.push_back(tempFloat);

				

			}			
		
      tempAnim.Bone=i;

			this->Tracks.push_back(tempAnim);
			
		
			tempAnim.Translations.clear();
			tempAnim.Rotations.clear();
			tempAnim.Scales.clear();
			
			tempAnim.T_Times.clear();
			tempAnim.R_Times.clear();
			tempAnim.S_Times.clear();
		}
		
	}

	anmStream.close();

}


JsonObject  SerializeImage(Image& Img, JsonObject & json){
  json["Type"]="Texture";

  json["Width"]=Img.Width;
  json["Height"]=Img.Height;
  json.createNestedArray("Data");
  for(size_t i = 0; i < (Img.Width*Img.Height);i++){
    json["Data"].add(Img.Data[i]);
  }
  return json;
}
JsonObject  SerializeSound(Sound& Sfx, JsonObject & json){
  json["Type"]="SFX";

  json["SampleRate"]=Sfx.SampleRate;
  json["SampleCount"]=Sfx.SampleCount;
  json.createNestedArray("Data");
  for(size_t i = 0; i < Sfx.SampleCount;i++){
    json["Data"].add(Sfx.Data[i]);
  }
 return json;
}
JsonObject  SerializeMesh(Mesh& Msh, JsonObject & json){
  json["Type"]="Mesh";

  json["Bone"]=Msh.Bone;
  json.createNestedArray("Vertices");
  for(size_t i = 0; i < Msh.Vertices.size(); i++){
    json["Vertices"].add(Msh.Vertices[i]);
  }
  json.createNestedArray("Indices");
  for(size_t i = 0; i < Msh.Indices.size(); i++){
    json["Indices"].add(Msh.Indices[i]);
  }
  json.createNestedArray("UVs");
  for(size_t i = 0; i < Msh.UVs.size(); i++){
    json["UVs"].add(Msh.UVs[i]);
  }
  json.createNestedArray("Normals");
  for(size_t i = 0; i < Msh.Normals.size(); i++){
    json["Normals"].add(Msh.Normals[i]);
  }
  return json;
}
JsonObject  SerializeAnimation(Animation& Anim, JsonObject & json){
  json["Type"]="Animation";

  json.createNestedArray("Translations");
  for(size_t i =0; i < Anim.Translations.size(); i++){
    json["Translations"].add(Anim.Translations[i]);
  }
  json.createNestedArray("Rotations");
  for(size_t i =0; i < Anim.Rotations.size(); i++){
    json["Rotations"].add(Anim.Rotations[i]);
  }
  json.createNestedArray("Scales");
  for(size_t i =0; i < Anim.Scales.size(); i++){
    json["Scales"].add(Anim.Scales[i]);
  }
 
  json.createNestedArray("T_Times");
  for(size_t i =0; i < Anim.T_Times.size(); i++){
    json["T_Times"].add(Anim.T_Times[i]);
  }
  json.createNestedArray("R_Times");
  for(size_t i =0; i < Anim.R_Times.size(); i++){
    json["R_Times"].add(Anim.R_Times[i]);
  }
  json.createNestedArray("S_Times");
  for(size_t i =0; i < Anim.S_Times.size(); i++){
    json["S_Times"].add(Anim.S_Times[i]);
  }

  json["Bone"]=Anim.Bone;

  return json;
}
JsonObject  SerializeBone(Bone& Bone, JsonObject & json){
  json["Type"]="Bone";
  json["Root"]=Bone.Root;
  json.createNestedArray("T_Init");
  for(size_t i =0; i <3; i++){
    json["T_Init"].add(Bone.T_Init[i]);
  }
  json.createNestedArray("R_Init");
  for(size_t i =0; i <4; i++){
    json["R_Init"].add(Bone.R_Init[i]);
  }
  json.createNestedArray("S_Init");
  for(size_t i =0; i <3; i++){
    json["S_Init"].add(Bone.S_Init[i]);
  }
  json.createNestedArray("Children");
  for(size_t i =0; i <Bone.Children.size(); i++){
    json["Children"].add(Bone.Children[i]);
  }

  return json;
}


