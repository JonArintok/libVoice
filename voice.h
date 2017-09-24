#include <stdint.h>

extern const double semitoneRatio; // multiply a freq by this to get the freq 1 semitone higher

typedef struct {
	long   shape; // index in shape array
	float  amp;   // multiply shape output sample
	float  shift; // add to shape output sample
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

double freqFromPitch(double pitch);

void shapeFromMem    (int shapeIndex, int sampleCount, const float *mem);
void shapeFromSine   (int shapeIndex, int sampleCount);
void shapeFromSaw    (int shapeIndex, int sampleCount);
void shapeFromPulse  (int shapeIndex, int sampleCount, double pulseWidth);
void shapesFromWavFile(int firstShapeIndex, uint32_t shapeCount, const char *path); // will not write more shapes than there are channels in the wav file

void setOscShape        (int voiceIndex, int voicePart, int shapeIndex);
void setOscAmp          (int voiceIndex, int voicePart, double amp);
void setOscShift        (int voiceIndex, int voicePart, double shift);
void setOscPos          (int voiceIndex, int voicePart, double pos);
void setOscInc          (int voiceIndex, int voicePart, double inc);
void mulOscInc          (int voiceIndex, int voicePart, double n);
void setOscIncFromFreq  (int voiceIndex, int voicePart, double freq); // in Hz
void setOscIncFromFreqAndRestart(int voiceIndex, int voicePart, double freq); // in Hz
void setOscIncFromPeriod(int voiceIndex, int voicePart, double period); // in seconds
void setOscIncFromSpeed (int voiceIndex, int voicePart, double speed); // input samples per output sampes
void setOsc             (int voiceIndex, int voicePart, const osc o);
void setVoice    (int voiceIndex, const voice v);
void setVoicePan (int voiceIndex, double pan); // 0 is centered, -1.0 is all left, 1.0 is all right
void restartVoice(int voiceIndex); // set all osc positions to 0
void enableVoice (int voiceIndex);
void disableVoice(int voiceIndex);
// the following are guaranteed simultaneous
void setOscPoss   (int firstVoiceIndex, int lastVoiceIndex, int voicePart, double pos);
void setOscIncs   (int firstVoiceIndex, int lastVoiceIndex, int voicePart, double inc);
void mulOscIncs   (int firstVoiceIndex, int lastVoiceIndex, int voicePart, double n);
void restartVoices(int firstVoiceIndex, int lastVoiceIndex);
void enableVoices (int firstVoiceIndex, int lastVoiceIndex);
void disableVoices(int firstVoiceIndex, int lastVoiceIndex);
