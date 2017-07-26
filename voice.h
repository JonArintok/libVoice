
typedef struct {
	uint32_t shape; // index in shape array
	float    amp;   // multiply shape output sample
	float    pos;   // position in shape, 0 to 1
	float    spd;   // 1.0 results in 1 shape sample per output sample
} osc;

enum {
	vo_wave,   // looped
	vo_ampMod, // looped,  multiply wave amp
	vo_spdMod, // looped,  multiply wave speed
	vo_ampEnv, // clamped, multiply wave amp
	vo_spdEnv, // clamped, multiply wave speed
	vo_oscPerVoice
};
typedef osc voice[vo_oscPerVoice];


int initVoices(int initVoiceCount, int initShapeCount);
int closeVoices(void);

void unpauseAudio(void);
void pauseAudio(void);
void setGlobalVolume(float v);

void shapeFromMem(int shapeIndex, int sampleCount, float *mem);
void shapeFromSin(int shapeIndex, int sampleCount);
void shapeFromSaw(int shapeIndex, int sampleCount);

void setOscShape(int voiceIndex, int voicePart, int shapeIndex);
void setOscAmp  (int voiceIndex, int voicePart, float amp);
void setOscPos  (int voiceIndex, int voicePart, float pos);
void setOscSpd  (int voiceIndex, int voicePart, float spd);
void setOsc     (int voiceIndex, int voicePart, osc o);
void setVoice   (int voiceIndex, voice v);

void buildSineWave(float *data, int sampleCount);
double freqFromPitch(double pitch);
