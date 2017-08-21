
typedef struct {
	long   shape; // index in shape array
	float  shift; // add to shape output sample
	float  amp;   // multiply shape output sample
	double pos;   // position in shape, 0 to 1
	double inc;   // increment pos by this amount for each output sample
} osc;

enum {
	vo_wave,   // looped, negative shape index indicates voice is disabled
	vo_ampMod, // looped,  multiply wave amp
	vo_incMod, // looped,  multiply wave speed
	vo_ampEnv, // clamped, multiply wave amp
	vo_incEnv, // clamped, multiply wave speed
	vo_oscPerVoice
};
typedef osc voice[vo_oscPerVoice];


int initVoices(int initVoiceCount, int initShapeCount);
int closeVoices(void);

void unpauseAudio(void);
void pauseAudio(void);
void setGlobalVolume(float v); // exponential

void shapeFromMem  (int shapeIndex, int sampleCount, float *mem);
void shapeFromSine (int shapeIndex, int sampleCount);
void shapeFromSaw  (int shapeIndex, int sampleCount);
void shapeFromPulse(int shapeIndex, int sampleCount, double pulseWidth);

void setOscShape           (int voiceIndex, int voicePart, int shapeIndex);
void setOscShift           (int voiceIndex, int voicePart, double shift);
void setOscAmp             (int voiceIndex, int voicePart, double amp);
void setOscPos             (int voiceIndex, int voicePart, double pos);
void setOscInc             (int voiceIndex, int voicePart, double inc);
void setOscIncFromLoopFreq (int voiceIndex, int voicePart, double freq);
void setOscIncFromPlaySpeed(int voiceIndex, int voicePart, double speed);
void setOsc                (int voiceIndex, int voicePart, osc o);
void setVoice              (int voiceIndex, voice v);
void enableVoice (int voiceIndex);
void disableVoice(int voiceIndex);

double freqFromPitch(double pitch);
