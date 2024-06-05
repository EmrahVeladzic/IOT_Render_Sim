#include "ModelLoader.hpp"
#include "SD.h"
#include "SPI.h"
#include "Protocol.h"
#include <WiFi.h>
#include <HTTPClient.h>

JsonObject j_son;
Model* ImpModel;
DynamicJsonDocument jsonDoc(2048);
WiFiClient client;
HTTPClient http;

const char* ssid     = "Net_437027";
const char* password = "123456789";

#define SD_CS_PIN  5
#define  SD_MOSI_PIN  23
#define  SD_MISO_PIN  19
#define  SD_SCK_PIN  18

void setup() {
  Serial.begin(115200);
  SD.begin();

   WiFi.begin(ssid,password);

  while (WiFi.status() != WL_CONNECTED) {

  }

  ImpModel = new Model("/sd/raven");
  j_son = jsonDoc.to<JsonObject>();
  
  SerializeModel(*ImpModel,j_son);
  serializeJson(j_son,Serial);
  j_son.clear();

  Serial.print("\n");

  

}

void PostImage(Model & Owner){
 SerializeImage(Owner.Texture,j_son);
 serializeJson(j_son,Serial);
 Img_Sent = true;
 j_son.clear();
}
void PostSound(Model & Owner){
 SerializeSound(Owner.SFX,j_son);
 serializeJson(j_son,Serial);
 Sfx_Sent = true;
 j_son.clear();
}
void PostMesh(Model & Owner){
 SerializeMesh(Owner.Meshes[MeshIndex],j_son);
 serializeJson(j_son,Serial);
 MeshIndex++;
 j_son.clear();
}
void PostBone(Model & Owner){
 SerializeBone(Owner.Bones[BoneIndex],j_son);
 serializeJson(j_son,Serial);
 BoneIndex++;
 j_son.clear();
}
void PostTrack(Model & Owner){
 SerializeAnimation(Owner.Tracks[TrackIndex],j_son);
 serializeJson(j_son,Serial);
 TrackIndex++;
 j_son.clear();
}


void loop() {
  if(!Ready){
    if(Serial.available()){

       delay(100);
      
      if(Response==ACK){

        Response=0;

        if(!Img_Sent){
          PostImage(*ImpModel);
        }
        else if(!Sfx_Sent){
          PostSound(*ImpModel);
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
        }
       
        Serial.write('\n');
      }


    }
  }  
 
}
