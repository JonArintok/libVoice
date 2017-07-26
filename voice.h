
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

void uploadShape(float *shape, int sampleCount, int shapeIndex);

void buildSineWave(float *data, int sampleCount);
double freqFromPitch(double pitch);
