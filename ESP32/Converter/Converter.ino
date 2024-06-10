#include "ModelLoader.hpp"
#include "SD.h"
#include "SPI.h"
#include "Protocol.h"

JsonObject j_son;
Model* ImpModel;
Image* ImpImage;
Sound* ImpSound;
DynamicJsonDocument jsonDoc(2048);

#define PATH "/sd/raven"

volatile byte altAnim = LOW;

#define  SD_CS_PIN  5
#define  SD_MOSI_PIN  23
#define  SD_MISO_PIN  19
#define  SD_SCK_PIN  18

#define ANIM_SWITCH_PIN 25
#define RDY_PIN 14

#define DEBOUNCE_TIME 200
volatile uint32_t lastTime = 0;

void IRAM_ATTR AnimSwitch(){ 
  uint32_t currentTime = millis();
  if(Ready && (currentTime-lastTime)>DEBOUNCE_TIME){
    altAnim=HIGH;   
    lastTime = currentTime;
  }
}

void setup() {  
  pinMode(RDY_PIN,OUTPUT);
  digitalWrite(RDY_PIN,LOW);

  pinMode(ANIM_SWITCH_PIN,INPUT);
  attachInterrupt(digitalPinToInterrupt(ANIM_SWITCH_PIN), AnimSwitch, CHANGE);

  delay(WAIT);

  Serial.begin(115200);
  SD.begin();  

  j_son = jsonDoc.to<JsonObject>();
  jsonDoc.clear();
  
}

void PostImage(Image & img){
 SerializeImage(img,j_son);
 serializeJson(j_son,Serial);
 Img_Sent = true;
 delete[] img.Data;
 j_son.clear();
 jsonDoc.clear();
 
}
void PostSound(Sound & sfx){
 SerializeSound(sfx,j_son);
 serializeJson(j_son,Serial);
 Sfx_Sent = true;
 delete[] sfx.Data;
 j_son.clear();
 jsonDoc.clear();
 ImpModel = new Model(PATH);
}
void PostMesh(Model & Owner){
 SerializeMesh(Owner.Meshes[MeshIndex],j_son);
 serializeJson(j_son,Serial);
 Owner.Meshes[MeshIndex].Vertices.clear();
 Owner.Meshes[MeshIndex].Indices.clear();
 Owner.Meshes[MeshIndex].UVs.clear();
 Owner.Meshes[MeshIndex].Normals.clear();
 MeshIndex++;
 j_son.clear();
}
void PostBone(Model & Owner){
 SerializeBone(Owner.Bones[BoneIndex],j_son);
 serializeJson(j_son,Serial);
 Owner.Bones[BoneIndex].Children.clear();
 BoneIndex++;
 j_son.clear();
 jsonDoc.clear();
}
void PostTrack(Model & Owner){
 SerializeAnimation(Owner.Tracks[TrackIndex],j_son);
 serializeJson(j_son,Serial);
 Owner.Tracks[TrackIndex].Translations.clear();
 Owner.Tracks[TrackIndex].Rotations.clear();
 Owner.Tracks[TrackIndex].Scales.clear();
 Owner.Tracks[TrackIndex].T_Times.clear();
 Owner.Tracks[TrackIndex].R_Times.clear();
 Owner.Tracks[TrackIndex].S_Times.clear();
 TrackIndex++;
 j_son.clear();
 jsonDoc.clear();
}
void PostResponse(String Response){
  j_son["Type"]="Command";
  j_son["Value"]=Response;
  serializeJson(j_son,Serial);
  j_son.clear();
  jsonDoc.clear();
}

void loop() {
  

  if(!Ready){
    if(Serial.available()){

       delay(S_WAIT);
      
      String Response = Serial.readStringUntil('\n');

      if(Response.equals(NEXT)){        

        if(!Img_Sent){
          ImpImage = new Image(PATH);
          PostImage(*ImpImage);
        }
        else if(!Sfx_Sent){
          ImpSound = new Sound(PATH);
          PostSound(*ImpSound);
        }
        else if(MeshIndex < ImpModel->Meshes.size()){
          PostMesh(*ImpModel);
        }
        else if (BoneIndex < ImpModel->Bones.size()){
          PostBone(*ImpModel);
        }
        else if (TrackIndex < ImpModel->Tracks.size()){
          PostTrack(*ImpModel);
        }
        else{
          Ready=true;
          PostResponse(RDY);
          delay(WAIT);
          digitalWrite(RDY_PIN,HIGH);
        }
       
        Serial.write('\n');

      }
    }
  }
   
  else{
    if(altAnim){
      PostResponse(SWITCH);
      altAnim=LOW;
      Serial.write('\n');
    }  
  }

}



