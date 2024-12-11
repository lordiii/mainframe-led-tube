#ifndef BEAM_H
#define BEAM_H

union EffectData;
struct LED_Ring;
struct LED_Pixel;

typedef struct FX_Beam {
    LED_Ring *last;
} FX_Beam;

typedef struct FX_SideBeam {
    LED_Pixel *last;
} FX_SideBeam;


void FX_resetBeam(EffectData *effectData);

void FX_resetSideBeam(EffectData *effectData);

bool FX_beam(EffectData *effectData, unsigned long delta);

bool FX_sideBeam(EffectData *effectData, unsigned long delta);


#endif
