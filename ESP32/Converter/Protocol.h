#ifndef PROTOCOL
#define PROTOCOL

#define NEXT "R_REQ"

#define RDY "R_READY"
#define SWITCH "R_SWITCH"

#define WAIT 1000
#define S_WAIT 100

volatile bool Ready = false;

volatile bool Img_Sent = false;
volatile bool Sfx_Sent = false;
volatile uint8_t BoneIndex = 0;
volatile uint8_t MeshIndex = 0;
volatile uint8_t TrackIndex = 0;



#endif