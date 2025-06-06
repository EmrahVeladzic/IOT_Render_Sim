#include <string>
#include <vector>
#include <fstream>
#include "ModelLoader.hpp"

extern std::vector<String>allowedAssets;


Image::Image(const char* fileName){
  uint8_t tempUByte;
  uint16_t tempUShort;
  std::vector<uint16_t>CLUT;

  std::string imgFile = std::string(fileName) + ".RPF";

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
  std::string sfxFile = std::string(fileName) + ".WL";

  uint8_t sh_val = 0;

  int16_t out = 0;

  uint8_t tempUByte;
  uint16_t tempUShort;
  uint32_t tempUInt;
  int32_t tempInt;
 

  std::ifstream SfxStream;
  SfxStream.open(sfxFile,std::ios::binary);

  SfxStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

  SfxStream.read(reinterpret_cast<char*>(&this->ChannelCount), sizeof(tempUByte));
  SfxStream.read(reinterpret_cast<char*>(&this->ClampBits), sizeof(tempUByte));

  SfxStream.read(reinterpret_cast<char*>(&tempUInt), sizeof(tempUInt));
  this->SampleCount=(tempUInt*SAMPLES_PER_BLOCK);

  SfxStream.read(reinterpret_cast<char*>(&tempUShort), sizeof(tempUShort));

  this->SampleRate=tempUShort;
 

  this->Data = new int16_t [this->SampleCount];



  for(size_t i =0; i< (size_t)tempUInt ; i++){


    SfxStream.read(reinterpret_cast<char*>(&sh_val), sizeof(tempUByte));

    sh_val>>=4;

    SfxStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

    for(size_t j = 0; j< (size_t)SAMPLES_PER_BLOCK;j+=2){
      
      SfxStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));
      
      tempInt = (int32_t)((tempUByte>>4)&0xF);

      if(tempInt>7){
        tempInt-=16;
      }

      tempInt*= (1<<sh_val);

      out = (int16_t)(tempInt&0xFFFF);

      this->Data[(i*SAMPLES_PER_BLOCK)+j]=out;

     tempInt = (int32_t)((tempUByte)&0xF);

      if(tempInt>7){
        tempInt-=16;
      }

      tempInt*= (1<<sh_val);

      out = (int16_t)(tempInt&0xFFFF);

      this->Data[(i*SAMPLES_PER_BLOCK)+j+1]=out;


    }
    

  }

  

  SfxStream.close();

}

Model::Model(const char* fileName){

  uint8_t scalingFactor;
  uint8_t tempUByte;

  uint8_t x_beg;
  uint8_t y_beg;

  uint8_t x_end;
  uint8_t y_end;

  int32_t tempInt;

  uint16_t tempUShort;

  float tempFloat;

  Mesh tempMsh;
  Bone tempBn;
  Animation tempAnim;


  std::string astFile = std::string(fileName) + ".AST";
 
  std::ifstream AstStream;
  AstStream.open(astFile,std::ios::binary);

  AstStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));
  AstStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

  AstStream.read(reinterpret_cast<char*>(&scalingFactor), sizeof(tempUByte));


  AstStream.read(reinterpret_cast<char*>(&x_beg), sizeof(tempUByte));
  AstStream.read(reinterpret_cast<char*>(&y_beg), sizeof(tempUByte));

  AstStream.read(reinterpret_cast<char*>(&x_end), sizeof(tempUByte));
  AstStream.read(reinterpret_cast<char*>(&y_end), sizeof(tempUByte));

	uint8_t x_diff = (uint8_t)(x_end - x_beg);
	uint8_t y_diff = (uint8_t)(y_end - y_beg);

  AstStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

  uint8_t mshCount = tempUByte;

  for(uint8_t i = 0; i<mshCount; i++){

      AstStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

  tempMsh.Bone = tempUByte;

  
  AstStream.read(reinterpret_cast<char*>(&tempUShort), sizeof(tempUShort));
  
  for(uint16_t j =0; j< tempUShort; j++){

    AstStream.read(reinterpret_cast<char*>(&tempInt), sizeof(tempInt));

    tempMsh.Vertices.push_back(FIX_TO_F(tempInt,scalingFactor));

  }

  uint16_t indCount;

  AstStream.read(reinterpret_cast<char*>(&indCount), sizeof(tempUShort));
  
  for(uint16_t j =0; j< indCount; j++){

    AstStream.read(reinterpret_cast<char*>(&tempUShort), sizeof(tempUShort));

    tempMsh.Indices.push_back(tempUShort);


  }

  AstStream.read(reinterpret_cast<char*>(&tempUShort), sizeof(tempUShort));
  
  for(uint16_t j =0; j< tempUShort; j+=2){

    AstStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));
	
    tempMsh.UVs.push_back(float(tempUByte)/float(x_diff) - 0.5f/(float(x_diff)+1.0f));

    AstStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));	  

   tempMsh.UVs.push_back(float(tempUByte)/float(y_diff) + 0.5f/(float(y_diff)+1.0f));


  }

  AstStream.read(reinterpret_cast<char*>(&tempUShort), sizeof(tempUShort));
  
  for(uint16_t j =0; j< tempUShort; j++){

    AstStream.read(reinterpret_cast<char*>(&tempInt), sizeof(tempInt));


    tempMsh.Normals.push_back(FIX_TO_F(tempInt,scalingFactor));



  }


  this->Meshes.push_back(tempMsh);

  tempMsh.Vertices.clear();
  tempMsh.Indices.clear();
  tempMsh.UVs.clear();
  tempMsh.Normals.clear();
  

  }

  AstStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

  uint8_t boneCount = tempUByte;

  AstStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

  uint8_t root_index = tempUByte;

  AstStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

  uint8_t animCount = tempUByte;

  AstStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

  uint8_t targetFPS = tempUByte;


  for(uint8_t i = 0; i< boneCount; i++){

    tempBn.Root=root_index==i;

    AstStream.read(reinterpret_cast<char*>(&tempInt), sizeof(tempInt));

    tempBn.T_Init[0]=(FIX_TO_F(tempInt,scalingFactor));

    AstStream.read(reinterpret_cast<char*>(&tempInt), sizeof(tempInt));

    tempBn.T_Init[1]=(FIX_TO_F(tempInt,scalingFactor));

    AstStream.read(reinterpret_cast<char*>(&tempInt), sizeof(tempInt));

    tempBn.T_Init[2]=(FIX_TO_F(tempInt,scalingFactor));

    AstStream.read(reinterpret_cast<char*>(&tempInt), sizeof(tempInt));

    tempBn.R_Init[0]=(FIX_TO_F(tempInt,scalingFactor));

    AstStream.read(reinterpret_cast<char*>(&tempInt), sizeof(tempInt));

    tempBn.R_Init[1]=(FIX_TO_F(tempInt,scalingFactor));

    AstStream.read(reinterpret_cast<char*>(&tempInt), sizeof(tempInt));

    tempBn.R_Init[2]=(FIX_TO_F(tempInt,scalingFactor));

    AstStream.read(reinterpret_cast<char*>(&tempInt), sizeof(tempInt));

    tempBn.R_Init[3]=(FIX_TO_F(tempInt,scalingFactor));

    AstStream.read(reinterpret_cast<char*>(&tempInt), sizeof(tempInt));

    tempBn.S_Init[0]=(FIX_TO_F(tempInt,scalingFactor));

    AstStream.read(reinterpret_cast<char*>(&tempInt), sizeof(tempInt));

    tempBn.S_Init[1]=(FIX_TO_F(tempInt,scalingFactor));

    AstStream.read(reinterpret_cast<char*>(&tempInt), sizeof(tempInt));

    tempBn.S_Init[2]=(FIX_TO_F(tempInt,scalingFactor));

    AstStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

    uint8_t ChildrenCount = tempUByte;

    for(uint8_t j = 0; j< ChildrenCount; j++){

      AstStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

      tempBn.Children.push_back(tempUByte);

    }

    for(uint8_t j = 0; j< animCount;j++){

      tempAnim.Bone=i;

        AstStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

        uint8_t TRS_Count =tempUByte;

        for(uint8_t k = 0; k< TRS_Count; k++){

          AstStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

          tempFloat = (float(tempUByte)) / (float(targetFPS));

          if (k > 0) {

            while (tempFloat < tempAnim.T_Times[k - 1])
            {
              tempFloat += 1.0f;
            }

          }

          tempAnim.T_Times.push_back(tempFloat);

          for(int l = 0; l<3; l++){            
            
            AstStream.read(reinterpret_cast<char*>(&tempInt), sizeof(tempInt));

            tempAnim.Translations.push_back(FIX_TO_F(tempInt,scalingFactor));

          }

        }

        AstStream.read(reinterpret_cast<char*>(&TRS_Count), sizeof(tempUByte));
        
        for(uint8_t k = 0; k< TRS_Count; k++){

          AstStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

          tempFloat = (float(tempUByte)) / (float(targetFPS));

          if (k > 0) {

            while (tempFloat < tempAnim.R_Times[k - 1])
            {
              tempFloat += 1.0f;
            }

          }

          tempAnim.R_Times.push_back(tempFloat);

          for(int l = 0; l<4; l++){            
            
            AstStream.read(reinterpret_cast<char*>(&tempInt), sizeof(tempInt));

            tempAnim.Rotations.push_back(FIX_TO_F(tempInt,scalingFactor));

          }

        }

        AstStream.read(reinterpret_cast<char*>(&TRS_Count), sizeof(tempUByte));
        
        for(uint8_t k = 0; k< TRS_Count; k++){

          AstStream.read(reinterpret_cast<char*>(&tempUByte), sizeof(tempUByte));

          tempFloat = (float(tempUByte)) / (float(targetFPS));

          if (k > 0) {

            while (tempFloat < tempAnim.S_Times[k - 1])
            {
              tempFloat += 1.0f;
            }

          }

          tempAnim.S_Times.push_back(tempFloat);

          for(int l = 0; l<3; l++){            
            
            AstStream.read(reinterpret_cast<char*>(&tempInt), sizeof(tempInt));

            tempAnim.Scales.push_back(FIX_TO_F(tempInt,scalingFactor));

          }

        }






      this->Tracks.push_back(tempAnim);
      tempAnim.T_Times.clear();
      tempAnim.R_Times.clear();
      tempAnim.S_Times.clear();
      tempAnim.Translations.clear();
      tempAnim.Rotations.clear();
      tempAnim.Scales.clear();
    }



    this->Bones.push_back(tempBn);
    tempBn.Children.clear();


  }




  AstStream.close();

}


JsonObject  SerializeImage(Image& Img, JsonObject & json){
  json["Type"]="Texture";

  json["Width"]=Img.Width;
  json["Height"]=Img.Height;
  json["Data"].to<JsonArray>();
  for(size_t i = 0; i < (Img.Width*Img.Height);i++){
    json["Data"].add(Img.Data[i]);
  }
  return json;
}
JsonObject  SerializeSound(Sound& Sfx, JsonObject & json){
  json["Type"]="SFX";

  json["SampleRate"]=Sfx.SampleRate;
  json["SampleCount"]=Sfx.SampleCount;
  json["Data"].to<JsonArray>();
  for(size_t i = 0; i < Sfx.SampleCount;i++){
    json["Data"].add(Sfx.Data[i]);
  }
 return json;
}
JsonObject  SerializeMesh(Mesh& Msh, JsonObject & json){
  json["Type"]="Mesh";

  json["Bone"]=Msh.Bone;
 json["Vertices"].to<JsonArray>();
  for(size_t i = 0; i < Msh.Vertices.size(); i++){
    json["Vertices"].add(Msh.Vertices[i]);
  }
 json["Indices"].to<JsonArray>();
  for(size_t i = 0; i < Msh.Indices.size(); i++){
    json["Indices"].add(Msh.Indices[i]);
  }
 json["UVs"].to<JsonArray>();
  for(size_t i = 0; i < Msh.UVs.size(); i++){
    json["UVs"].add(Msh.UVs[i]);
  }
 json["Normals"].to<JsonArray>();
  for(size_t i = 0; i < Msh.Normals.size(); i++){
    json["Normals"].add(Msh.Normals[i]);
  }
  return json;
}
JsonObject  SerializeAnimation(Animation& Anim, JsonObject & json){
  json["Type"]="Animation";

 json["Translations"].to<JsonArray>();
  for(size_t i =0; i < Anim.Translations.size(); i++){
    json["Translations"].add(Anim.Translations[i]);
  }
 json["Rotations"].to<JsonArray>();
  for(size_t i =0; i < Anim.Rotations.size(); i++){
    json["Rotations"].add(Anim.Rotations[i]);
  }
 json["Scales"].to<JsonArray>();
  for(size_t i =0; i < Anim.Scales.size(); i++){
    json["Scales"].add(Anim.Scales[i]);
  }
 
 json["T_Times"].to<JsonArray>();
  for(size_t i =0; i < Anim.T_Times.size(); i++){
    json["T_Times"].add(Anim.T_Times[i]);
  }
   json["R_Times"].to<JsonArray>();
  for(size_t i =0; i < Anim.R_Times.size(); i++){
    json["R_Times"].add(Anim.R_Times[i]);
  }
   json["S_Times"].to<JsonArray>();
  for(size_t i =0; i < Anim.S_Times.size(); i++){
    json["S_Times"].add(Anim.S_Times[i]);
  }

  json["Bone"]=Anim.Bone;

  return json;
}
JsonObject  SerializeBone(Bone& Bone, JsonObject & json){
  json["Type"]="Bone";
  json["Root"]=Bone.Root;
   json["T_Init"].to<JsonArray>();
  for(size_t i =0; i <3; i++){
    json["T_Init"].add(Bone.T_Init[i]);
  }
  json["R_Init"].to<JsonArray>();
  for(size_t i =0; i <4; i++){
    json["R_Init"].add(Bone.R_Init[i]);
  }
   json["S_Init"].to<JsonArray>();
  for(size_t i =0; i <3; i++){
    json["S_Init"].add(Bone.S_Init[i]);
  }
  json["Children"].to<JsonArray>();
  for(size_t i =0; i <Bone.Children.size(); i++){
    json["Children"].add(Bone.Children[i]);
  }

  return json;
}

JsonObject GetList(JsonObject & json){

json["Type"]="List";
json["Assets"].to<JsonArray>();
std::ifstream listStream;

std::string listMember;

listStream.open(((std::string)PATH+(std::string)LIST_FILE).c_str());
while(!listStream.eof()){

listStream>>listMember;

allowedAssets.push_back(String(listMember.c_str()));

json["Assets"].add(listMember);

}
listStream.close();



return json;
}

JsonObject Identify(JsonObject & json){

  json["ID"]=DEVICE;

  return json;
}
