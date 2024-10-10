#ifndef PROTOCOL
#define PROTOCOL

#define ID "R_ID"
#define NEXT "R_REQ"
#define LIST "R_LIST"
#define RDY "R_READY"
#define SWITCH "R_SWITCH"
#define RESET "R_RESET"



#define WAIT 1000
#define S_WAIT 100

volatile bool Ready = false;

volatile bool Img_Sent = false;
volatile bool Sfx_Sent = false;
volatile uint8_t BoneIndex = 0;
volatile uint8_t MeshIndex = 0;
volatile uint8_t TrackIndex = 0;



#endif