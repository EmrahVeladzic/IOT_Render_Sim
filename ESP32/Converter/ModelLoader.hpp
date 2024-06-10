#ifndef MODEL_LOADER
#define MODEL_LOADER


#define SAMPLES_PER_BLOCK 28
#define DIVISOR 5
#define MIN_INT_4 -8
#define MAX_INT_4 7
#define PAGE_FILE_SIZE 255.0f
const int16_t rounding_table[4] = { 2,3,7,8 };

#include <vector>
#include <fstream>
#include "FixedPointMath.h"
#include <ArduinoJson.h>


struct Image{

uint16_t Width;
uint16_t Height;
uint16_t* Data;

Image(const char* fileName);

};

struct Sound{

uint16_t SampleRate;

uint32_t SampleCount;

int16_t* Data;

Sound(const char* fileName);

};


struct Animation{

std::vector<float>Translations;
std::vector<float>Rotations;
std::vector<float>Scales;

std::vector<float>T_Times;
std::vector<float>R_Times;
std::vector<float>S_Times;

uint8_t Bone;

};

struct Bone{

float T_Init[3];
float R_Init[4];
float S_Init[3];

std::vector<uint8_t> Children;

bool Root;

};

struct Mesh{

std::vector<float>Vertices;
std::vector<uint16_t>Indices;
std::vector<float>UVs;
std::vector<float>Normals;

uint8_t Bone;

};


class Model{
public:

std::vector<Animation>Tracks;
std::vector<Mesh>Meshes;
std::vector<Bone>Bones;

Model(const char* fileName);

};


JsonObject  SerializeImage(Image& Img, JsonObject & json);
JsonObject  SerializeSound(Sound& Sfx, JsonObject  & json);
JsonObject  SerializeMesh(Mesh& Msh, JsonObject & json);
JsonObject  SerializeAnimation(Animation& Anim, JsonObject & json);
JsonObject  SerializeBone(Bone& Bone, JsonObject & json);
JsonObject  SerializeModel(Model& Mdl, JsonObject & json);

#endif
