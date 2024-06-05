#ifndef PROTOCOL
#define PROTOCOL

#define NEXT "NEXT"

extern volatile bool Ready = false;

extern volatile bool Img_Sent = false;
extern volatile bool Sfx_Sent = false;
extern volatile uint8_t BoneIndex = 0;
extern volatile uint8_t MeshIndex = 0;
extern volatile uint8_t TrackIndex = 0;



#endif