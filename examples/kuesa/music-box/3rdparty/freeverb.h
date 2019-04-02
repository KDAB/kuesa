/* ------------------------------------------------------------
author: "Grame"
copyright: "(c) GRAME 2006"
license: "BSD"
name: "freeverb"
version: "1.0"
Code generated with Faust 2.15.11 (https://faust.grame.fr)
Compilation options: -vec -lv 0 -vs 32 -ftz 0 -mcd 16
------------------------------------------------------------ */

#ifndef __mydsp_H__
#define __mydsp_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

#include <algorithm>
#include <cmath>

#ifndef FAUSTCLASS
#define FAUSTCLASS mydsp
#endif
#ifdef __APPLE__
#define exp10f __exp10f
#define exp10 __exp10
#endif

class mydsp
{

private:
    FAUSTFLOAT fHslider0;
    float fRec9_perm[4];
    FAUSTFLOAT fHslider1;
    float fYec0[2048];
    int fYec0_idx;
    int fYec0_idx_save;
    float fRec8_perm[4];
    float fRec11_perm[4];
    float fYec1[2048];
    int fYec1_idx;
    int fYec1_idx_save;
    float fRec10_perm[4];
    float fRec13_perm[4];
    float fYec2[2048];
    int fYec2_idx;
    int fYec2_idx_save;
    float fRec12_perm[4];
    float fRec15_perm[4];
    float fYec3[2048];
    int fYec3_idx;
    int fYec3_idx_save;
    float fRec14_perm[4];
    float fRec17_perm[4];
    float fYec4[2048];
    int fYec4_idx;
    int fYec4_idx_save;
    float fRec16_perm[4];
    float fRec19_perm[4];
    float fYec5[2048];
    int fYec5_idx;
    int fYec5_idx_save;
    float fRec18_perm[4];
    float fRec21_perm[4];
    float fYec6[2048];
    int fYec6_idx;
    int fYec6_idx_save;
    float fRec20_perm[4];
    float fRec23_perm[4];
    float fYec7[2048];
    int fYec7_idx;
    int fYec7_idx_save;
    float fRec22_perm[4];
    float fYec8[1024];
    int fYec8_idx;
    int fYec8_idx_save;
    float fRec6_perm[4];
    float fYec9[512];
    int fYec9_idx;
    int fYec9_idx_save;
    float fRec4_perm[4];
    float fYec10[512];
    int fYec10_idx;
    int fYec10_idx_save;
    float fRec2_perm[4];
    float fYec11[512];
    int fYec11_idx;
    int fYec11_idx_save;
    float fRec0_perm[4];
    FAUSTFLOAT fHslider2;
    float fRec33_perm[4];
    float fYec12[2048];
    int fYec12_idx;
    int fYec12_idx_save;
    float fRec32_perm[4];
    float fRec35_perm[4];
    float fYec13[2048];
    int fYec13_idx;
    int fYec13_idx_save;
    float fRec34_perm[4];
    float fRec37_perm[4];
    float fYec14[2048];
    int fYec14_idx;
    int fYec14_idx_save;
    float fRec36_perm[4];
    float fRec39_perm[4];
    float fYec15[2048];
    int fYec15_idx;
    int fYec15_idx_save;
    float fRec38_perm[4];
    float fRec41_perm[4];
    float fYec16[2048];
    int fYec16_idx;
    int fYec16_idx_save;
    float fRec40_perm[4];
    float fRec43_perm[4];
    float fYec17[2048];
    int fYec17_idx;
    int fYec17_idx_save;
    float fRec42_perm[4];
    float fRec45_perm[4];
    float fYec18[2048];
    int fYec18_idx;
    int fYec18_idx_save;
    float fRec44_perm[4];
    float fRec47_perm[4];
    float fYec19[2048];
    int fYec19_idx;
    int fYec19_idx_save;
    float fRec46_perm[4];
    float fYec20[1024];
    int fYec20_idx;
    int fYec20_idx_save;
    float fRec30_perm[4];
    float fYec21[512];
    int fYec21_idx;
    int fYec21_idx_save;
    float fRec28_perm[4];
    float fYec22[512];
    int fYec22_idx;
    int fYec22_idx_save;
    float fRec26_perm[4];
    float fYec23[512];
    int fYec23_idx;
    int fYec23_idx_save;
    float fRec24_perm[4];
    int fSamplingFreq;

public:
    void setDamp(float v) { fHslider0 = v; }
    void setRoomSize(float v) { fHslider1 = v; }
    //void setStereoSpread(float v) { fHslider2 = v; }
    void setDryWet(float v) { fHslider2 = v; }

    virtual int getNumInputs()
    {
        return 2;
    }
    virtual int getNumOutputs()
    {
        return 2;
    }
    virtual int getInputRate(int channel)
    {
        int rate;
        switch (channel) {
        case 0: {
            rate = 0;
            break;
        }
        case 1: {
            rate = 0;
            break;
        }
        default: {
            rate = -1;
            break;
        }
        }
        return rate;
    }
    virtual int getOutputRate(int channel)
    {
        int rate;
        switch (channel) {
        case 0: {
            rate = 1;
            break;
        }
        case 1: {
            rate = 1;
            break;
        }
        default: {
            rate = -1;
            break;
        }
        }
        return rate;
    }

    static void classInit(int samplingFreq)
    {
    }

    virtual void instanceConstants(int samplingFreq)
    {
        fSamplingFreq = samplingFreq;
    }

    virtual void instanceResetUserInterface()
    {
        fHslider0 = FAUSTFLOAT(0.5f);
        fHslider1 = FAUSTFLOAT(0.90000000000000002f);
        fHslider2 = FAUSTFLOAT(0.90000000000000002f);
    }

    virtual void instanceClear()
    {
        for (int l0 = 0; (l0 < 4); l0 = (l0 + 1)) {
            fRec9_perm[l0] = 0.0f;
        }
        for (int l1 = 0; (l1 < 2048); l1 = (l1 + 1)) {
            fYec0[l1] = 0.0f;
        }
        fYec0_idx = 0;
        fYec0_idx_save = 0;
        for (int l2 = 0; (l2 < 4); l2 = (l2 + 1)) {
            fRec8_perm[l2] = 0.0f;
        }
        for (int l3 = 0; (l3 < 4); l3 = (l3 + 1)) {
            fRec11_perm[l3] = 0.0f;
        }
        for (int l4 = 0; (l4 < 2048); l4 = (l4 + 1)) {
            fYec1[l4] = 0.0f;
        }
        fYec1_idx = 0;
        fYec1_idx_save = 0;
        for (int l5 = 0; (l5 < 4); l5 = (l5 + 1)) {
            fRec10_perm[l5] = 0.0f;
        }
        for (int l6 = 0; (l6 < 4); l6 = (l6 + 1)) {
            fRec13_perm[l6] = 0.0f;
        }
        for (int l7 = 0; (l7 < 2048); l7 = (l7 + 1)) {
            fYec2[l7] = 0.0f;
        }
        fYec2_idx = 0;
        fYec2_idx_save = 0;
        for (int l8 = 0; (l8 < 4); l8 = (l8 + 1)) {
            fRec12_perm[l8] = 0.0f;
        }
        for (int l9 = 0; (l9 < 4); l9 = (l9 + 1)) {
            fRec15_perm[l9] = 0.0f;
        }
        for (int l10 = 0; (l10 < 2048); l10 = (l10 + 1)) {
            fYec3[l10] = 0.0f;
        }
        fYec3_idx = 0;
        fYec3_idx_save = 0;
        for (int l11 = 0; (l11 < 4); l11 = (l11 + 1)) {
            fRec14_perm[l11] = 0.0f;
        }
        for (int l12 = 0; (l12 < 4); l12 = (l12 + 1)) {
            fRec17_perm[l12] = 0.0f;
        }
        for (int l13 = 0; (l13 < 2048); l13 = (l13 + 1)) {
            fYec4[l13] = 0.0f;
        }
        fYec4_idx = 0;
        fYec4_idx_save = 0;
        for (int l14 = 0; (l14 < 4); l14 = (l14 + 1)) {
            fRec16_perm[l14] = 0.0f;
        }
        for (int l15 = 0; (l15 < 4); l15 = (l15 + 1)) {
            fRec19_perm[l15] = 0.0f;
        }
        for (int l16 = 0; (l16 < 2048); l16 = (l16 + 1)) {
            fYec5[l16] = 0.0f;
        }
        fYec5_idx = 0;
        fYec5_idx_save = 0;
        for (int l17 = 0; (l17 < 4); l17 = (l17 + 1)) {
            fRec18_perm[l17] = 0.0f;
        }
        for (int l18 = 0; (l18 < 4); l18 = (l18 + 1)) {
            fRec21_perm[l18] = 0.0f;
        }
        for (int l19 = 0; (l19 < 2048); l19 = (l19 + 1)) {
            fYec6[l19] = 0.0f;
        }
        fYec6_idx = 0;
        fYec6_idx_save = 0;
        for (int l20 = 0; (l20 < 4); l20 = (l20 + 1)) {
            fRec20_perm[l20] = 0.0f;
        }
        for (int l21 = 0; (l21 < 4); l21 = (l21 + 1)) {
            fRec23_perm[l21] = 0.0f;
        }
        for (int l22 = 0; (l22 < 2048); l22 = (l22 + 1)) {
            fYec7[l22] = 0.0f;
        }
        fYec7_idx = 0;
        fYec7_idx_save = 0;
        for (int l23 = 0; (l23 < 4); l23 = (l23 + 1)) {
            fRec22_perm[l23] = 0.0f;
        }
        for (int l24 = 0; (l24 < 1024); l24 = (l24 + 1)) {
            fYec8[l24] = 0.0f;
        }
        fYec8_idx = 0;
        fYec8_idx_save = 0;
        for (int l25 = 0; (l25 < 4); l25 = (l25 + 1)) {
            fRec6_perm[l25] = 0.0f;
        }
        for (int l26 = 0; (l26 < 512); l26 = (l26 + 1)) {
            fYec9[l26] = 0.0f;
        }
        fYec9_idx = 0;
        fYec9_idx_save = 0;
        for (int l27 = 0; (l27 < 4); l27 = (l27 + 1)) {
            fRec4_perm[l27] = 0.0f;
        }
        for (int l28 = 0; (l28 < 512); l28 = (l28 + 1)) {
            fYec10[l28] = 0.0f;
        }
        fYec10_idx = 0;
        fYec10_idx_save = 0;
        for (int l29 = 0; (l29 < 4); l29 = (l29 + 1)) {
            fRec2_perm[l29] = 0.0f;
        }
        for (int l30 = 0; (l30 < 512); l30 = (l30 + 1)) {
            fYec11[l30] = 0.0f;
        }
        fYec11_idx = 0;
        fYec11_idx_save = 0;
        for (int l31 = 0; (l31 < 4); l31 = (l31 + 1)) {
            fRec0_perm[l31] = 0.0f;
        }
        for (int l32 = 0; (l32 < 4); l32 = (l32 + 1)) {
            fRec33_perm[l32] = 0.0f;
        }
        for (int l33 = 0; (l33 < 2048); l33 = (l33 + 1)) {
            fYec12[l33] = 0.0f;
        }
        fYec12_idx = 0;
        fYec12_idx_save = 0;
        for (int l34 = 0; (l34 < 4); l34 = (l34 + 1)) {
            fRec32_perm[l34] = 0.0f;
        }
        for (int l35 = 0; (l35 < 4); l35 = (l35 + 1)) {
            fRec35_perm[l35] = 0.0f;
        }
        for (int l36 = 0; (l36 < 2048); l36 = (l36 + 1)) {
            fYec13[l36] = 0.0f;
        }
        fYec13_idx = 0;
        fYec13_idx_save = 0;
        for (int l37 = 0; (l37 < 4); l37 = (l37 + 1)) {
            fRec34_perm[l37] = 0.0f;
        }
        for (int l38 = 0; (l38 < 4); l38 = (l38 + 1)) {
            fRec37_perm[l38] = 0.0f;
        }
        for (int l39 = 0; (l39 < 2048); l39 = (l39 + 1)) {
            fYec14[l39] = 0.0f;
        }
        fYec14_idx = 0;
        fYec14_idx_save = 0;
        for (int l40 = 0; (l40 < 4); l40 = (l40 + 1)) {
            fRec36_perm[l40] = 0.0f;
        }
        for (int l41 = 0; (l41 < 4); l41 = (l41 + 1)) {
            fRec39_perm[l41] = 0.0f;
        }
        for (int l42 = 0; (l42 < 2048); l42 = (l42 + 1)) {
            fYec15[l42] = 0.0f;
        }
        fYec15_idx = 0;
        fYec15_idx_save = 0;
        for (int l43 = 0; (l43 < 4); l43 = (l43 + 1)) {
            fRec38_perm[l43] = 0.0f;
        }
        for (int l44 = 0; (l44 < 4); l44 = (l44 + 1)) {
            fRec41_perm[l44] = 0.0f;
        }
        for (int l45 = 0; (l45 < 2048); l45 = (l45 + 1)) {
            fYec16[l45] = 0.0f;
        }
        fYec16_idx = 0;
        fYec16_idx_save = 0;
        for (int l46 = 0; (l46 < 4); l46 = (l46 + 1)) {
            fRec40_perm[l46] = 0.0f;
        }
        for (int l47 = 0; (l47 < 4); l47 = (l47 + 1)) {
            fRec43_perm[l47] = 0.0f;
        }
        for (int l48 = 0; (l48 < 2048); l48 = (l48 + 1)) {
            fYec17[l48] = 0.0f;
        }
        fYec17_idx = 0;
        fYec17_idx_save = 0;
        for (int l49 = 0; (l49 < 4); l49 = (l49 + 1)) {
            fRec42_perm[l49] = 0.0f;
        }
        for (int l50 = 0; (l50 < 4); l50 = (l50 + 1)) {
            fRec45_perm[l50] = 0.0f;
        }
        for (int l51 = 0; (l51 < 2048); l51 = (l51 + 1)) {
            fYec18[l51] = 0.0f;
        }
        fYec18_idx = 0;
        fYec18_idx_save = 0;
        for (int l52 = 0; (l52 < 4); l52 = (l52 + 1)) {
            fRec44_perm[l52] = 0.0f;
        }
        for (int l53 = 0; (l53 < 4); l53 = (l53 + 1)) {
            fRec47_perm[l53] = 0.0f;
        }
        for (int l54 = 0; (l54 < 2048); l54 = (l54 + 1)) {
            fYec19[l54] = 0.0f;
        }
        fYec19_idx = 0;
        fYec19_idx_save = 0;
        for (int l55 = 0; (l55 < 4); l55 = (l55 + 1)) {
            fRec46_perm[l55] = 0.0f;
        }
        for (int l56 = 0; (l56 < 1024); l56 = (l56 + 1)) {
            fYec20[l56] = 0.0f;
        }
        fYec20_idx = 0;
        fYec20_idx_save = 0;
        for (int l57 = 0; (l57 < 4); l57 = (l57 + 1)) {
            fRec30_perm[l57] = 0.0f;
        }
        for (int l58 = 0; (l58 < 512); l58 = (l58 + 1)) {
            fYec21[l58] = 0.0f;
        }
        fYec21_idx = 0;
        fYec21_idx_save = 0;
        for (int l59 = 0; (l59 < 4); l59 = (l59 + 1)) {
            fRec28_perm[l59] = 0.0f;
        }
        for (int l60 = 0; (l60 < 512); l60 = (l60 + 1)) {
            fYec22[l60] = 0.0f;
        }
        fYec22_idx = 0;
        fYec22_idx_save = 0;
        for (int l61 = 0; (l61 < 4); l61 = (l61 + 1)) {
            fRec26_perm[l61] = 0.0f;
        }
        for (int l62 = 0; (l62 < 512); l62 = (l62 + 1)) {
            fYec23[l62] = 0.0f;
        }
        fYec23_idx = 0;
        fYec23_idx_save = 0;
        for (int l63 = 0; (l63 < 4); l63 = (l63 + 1)) {
            fRec24_perm[l63] = 0.0f;
        }
    }

    virtual void init(int samplingFreq)
    {
        classInit(samplingFreq);
        instanceInit(samplingFreq);
    }

    virtual void instanceInit(int samplingFreq)
    {
        instanceConstants(samplingFreq);
        instanceResetUserInterface();
        instanceClear();
    }

    virtual mydsp *clone()
    {
        return new mydsp();
    }

    virtual int getSampleRate()
    {
        return fSamplingFreq;
    }

    virtual void compute(int count, FAUSTFLOAT **inputs, FAUSTFLOAT **outputs)
    {
        FAUSTFLOAT *input0_ptr = inputs[0];
        FAUSTFLOAT *input1_ptr = inputs[1];
        FAUSTFLOAT *output0_ptr = outputs[0];
        FAUSTFLOAT *output1_ptr = outputs[1];
        float fSlow0 = (0.400000006f * float(fHslider0));
        float fSlow1 = (1.0f - fSlow0);
        float fRec9_tmp[36];
        float *fRec9 = &fRec9_tmp[4];
        float fSlow2 = ((0.280000001f * float(fHslider1)) + 0.699999988f);
        float fZec0[32];
        float fRec8_tmp[36];
        float *fRec8 = &fRec8_tmp[4];
        float fRec11_tmp[36];
        float *fRec11 = &fRec11_tmp[4];
        float fRec10_tmp[36];
        float *fRec10 = &fRec10_tmp[4];
        float fRec13_tmp[36];
        float *fRec13 = &fRec13_tmp[4];
        float fRec12_tmp[36];
        float *fRec12 = &fRec12_tmp[4];
        float fRec15_tmp[36];
        float *fRec15 = &fRec15_tmp[4];
        float fRec14_tmp[36];
        float *fRec14 = &fRec14_tmp[4];
        float fRec17_tmp[36];
        float *fRec17 = &fRec17_tmp[4];
        float fRec16_tmp[36];
        float *fRec16 = &fRec16_tmp[4];
        float fRec19_tmp[36];
        float *fRec19 = &fRec19_tmp[4];
        float fRec18_tmp[36];
        float *fRec18 = &fRec18_tmp[4];
        float fRec21_tmp[36];
        float *fRec21 = &fRec21_tmp[4];
        float fRec20_tmp[36];
        float *fRec20 = &fRec20_tmp[4];
        float fRec23_tmp[36];
        float *fRec23 = &fRec23_tmp[4];
        float fRec22_tmp[36];
        float *fRec22 = &fRec22_tmp[4];
        float fZec1[32];
        float fRec6_tmp[36];
        float *fRec6 = &fRec6_tmp[4];
        float fRec7[32];
        float fRec4_tmp[36];
        float *fRec4 = &fRec4_tmp[4];
        float fRec5[32];
        float fRec2_tmp[36];
        float *fRec2 = &fRec2_tmp[4];
        float fRec3[32];
        float fRec0_tmp[36];
        float *fRec0 = &fRec0_tmp[4];
        float fRec1[32];
        float fSlow3 = float(fHslider2);
        float fSlow4 = (1.0f - fSlow3);
        float fRec33_tmp[36];
        float *fRec33 = &fRec33_tmp[4];
        float fRec32_tmp[36];
        float *fRec32 = &fRec32_tmp[4];
        float fRec35_tmp[36];
        float *fRec35 = &fRec35_tmp[4];
        float fRec34_tmp[36];
        float *fRec34 = &fRec34_tmp[4];
        float fRec37_tmp[36];
        float *fRec37 = &fRec37_tmp[4];
        float fRec36_tmp[36];
        float *fRec36 = &fRec36_tmp[4];
        float fRec39_tmp[36];
        float *fRec39 = &fRec39_tmp[4];
        float fRec38_tmp[36];
        float *fRec38 = &fRec38_tmp[4];
        float fRec41_tmp[36];
        float *fRec41 = &fRec41_tmp[4];
        float fRec40_tmp[36];
        float *fRec40 = &fRec40_tmp[4];
        float fRec43_tmp[36];
        float *fRec43 = &fRec43_tmp[4];
        float fRec42_tmp[36];
        float *fRec42 = &fRec42_tmp[4];
        float fRec45_tmp[36];
        float *fRec45 = &fRec45_tmp[4];
        float fRec44_tmp[36];
        float *fRec44 = &fRec44_tmp[4];
        float fRec47_tmp[36];
        float *fRec47 = &fRec47_tmp[4];
        float fRec46_tmp[36];
        float *fRec46 = &fRec46_tmp[4];
        float fZec2[32];
        float fRec30_tmp[36];
        float *fRec30 = &fRec30_tmp[4];
        float fRec31[32];
        float fRec28_tmp[36];
        float *fRec28 = &fRec28_tmp[4];
        float fRec29[32];
        float fRec26_tmp[36];
        float *fRec26 = &fRec26_tmp[4];
        float fRec27[32];
        float fRec24_tmp[36];
        float *fRec24 = &fRec24_tmp[4];
        float fRec25[32];
        int vindex = 0;
        /* Main loop */
        for (vindex = 0; (vindex <= (count - 32)); vindex = (vindex + 32)) {
            FAUSTFLOAT *input0 = &input0_ptr[vindex];
            FAUSTFLOAT *input1 = &input1_ptr[vindex];
            FAUSTFLOAT *output0 = &output0_ptr[vindex];
            FAUSTFLOAT *output1 = &output1_ptr[vindex];
            int vsize = 32;
            /* Vectorizable loop 0 */
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fZec0[i] = (0.0149999997f * (float(input0[i]) + float(input1[i])));
            }
            /* Recursive loop 1 */
            /* Pre code */
            for (int j0 = 0; (j0 < 4); j0 = (j0 + 1)) {
                fRec9_tmp[j0] = fRec9_perm[j0];
            }
            fYec0_idx = ((fYec0_idx + fYec0_idx_save) & 2047);
            for (int j2 = 0; (j2 < 4); j2 = (j2 + 1)) {
                fRec8_tmp[j2] = fRec8_perm[j2];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec9[i] = ((fSlow0 * fRec9[(i - 1)]) + (fSlow1 * fRec8[(i - 1)]));
                fYec0[((i + fYec0_idx) & 2047)] = ((fSlow2 * fRec9[i]) + fZec0[i]);
                fRec8[i] = fYec0[(((i + fYec0_idx) - 1116) & 2047)];
            }
            /* Post code */
            fYec0_idx_save = vsize;
            for (int j1 = 0; (j1 < 4); j1 = (j1 + 1)) {
                fRec9_perm[j1] = fRec9_tmp[(vsize + j1)];
            }
            for (int j3 = 0; (j3 < 4); j3 = (j3 + 1)) {
                fRec8_perm[j3] = fRec8_tmp[(vsize + j3)];
            }
            /* Recursive loop 2 */
            /* Pre code */
            for (int j4 = 0; (j4 < 4); j4 = (j4 + 1)) {
                fRec11_tmp[j4] = fRec11_perm[j4];
            }
            fYec1_idx = ((fYec1_idx + fYec1_idx_save) & 2047);
            for (int j6 = 0; (j6 < 4); j6 = (j6 + 1)) {
                fRec10_tmp[j6] = fRec10_perm[j6];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec11[i] = ((fSlow0 * fRec11[(i - 1)]) + (fSlow1 * fRec10[(i - 1)]));
                fYec1[((i + fYec1_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec11[i]));
                fRec10[i] = fYec1[(((i + fYec1_idx) - 1188) & 2047)];
            }
            /* Post code */
            fYec1_idx_save = vsize;
            for (int j5 = 0; (j5 < 4); j5 = (j5 + 1)) {
                fRec11_perm[j5] = fRec11_tmp[(vsize + j5)];
            }
            for (int j7 = 0; (j7 < 4); j7 = (j7 + 1)) {
                fRec10_perm[j7] = fRec10_tmp[(vsize + j7)];
            }
            /* Recursive loop 3 */
            /* Pre code */
            for (int j8 = 0; (j8 < 4); j8 = (j8 + 1)) {
                fRec13_tmp[j8] = fRec13_perm[j8];
            }
            fYec2_idx = ((fYec2_idx + fYec2_idx_save) & 2047);
            for (int j10 = 0; (j10 < 4); j10 = (j10 + 1)) {
                fRec12_tmp[j10] = fRec12_perm[j10];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec13[i] = ((fSlow0 * fRec13[(i - 1)]) + (fSlow1 * fRec12[(i - 1)]));
                fYec2[((i + fYec2_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec13[i]));
                fRec12[i] = fYec2[(((i + fYec2_idx) - 1277) & 2047)];
            }
            /* Post code */
            fYec2_idx_save = vsize;
            for (int j9 = 0; (j9 < 4); j9 = (j9 + 1)) {
                fRec13_perm[j9] = fRec13_tmp[(vsize + j9)];
            }
            for (int j11 = 0; (j11 < 4); j11 = (j11 + 1)) {
                fRec12_perm[j11] = fRec12_tmp[(vsize + j11)];
            }
            /* Recursive loop 4 */
            /* Pre code */
            for (int j12 = 0; (j12 < 4); j12 = (j12 + 1)) {
                fRec15_tmp[j12] = fRec15_perm[j12];
            }
            fYec3_idx = ((fYec3_idx + fYec3_idx_save) & 2047);
            for (int j14 = 0; (j14 < 4); j14 = (j14 + 1)) {
                fRec14_tmp[j14] = fRec14_perm[j14];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec15[i] = ((fSlow0 * fRec15[(i - 1)]) + (fSlow1 * fRec14[(i - 1)]));
                fYec3[((i + fYec3_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec15[i]));
                fRec14[i] = fYec3[(((i + fYec3_idx) - 1356) & 2047)];
            }
            /* Post code */
            fYec3_idx_save = vsize;
            for (int j13 = 0; (j13 < 4); j13 = (j13 + 1)) {
                fRec15_perm[j13] = fRec15_tmp[(vsize + j13)];
            }
            for (int j15 = 0; (j15 < 4); j15 = (j15 + 1)) {
                fRec14_perm[j15] = fRec14_tmp[(vsize + j15)];
            }
            /* Recursive loop 5 */
            /* Pre code */
            for (int j16 = 0; (j16 < 4); j16 = (j16 + 1)) {
                fRec17_tmp[j16] = fRec17_perm[j16];
            }
            fYec4_idx = ((fYec4_idx + fYec4_idx_save) & 2047);
            for (int j18 = 0; (j18 < 4); j18 = (j18 + 1)) {
                fRec16_tmp[j18] = fRec16_perm[j18];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec17[i] = ((fSlow0 * fRec17[(i - 1)]) + (fSlow1 * fRec16[(i - 1)]));
                fYec4[((i + fYec4_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec17[i]));
                fRec16[i] = fYec4[(((i + fYec4_idx) - 1422) & 2047)];
            }
            /* Post code */
            fYec4_idx_save = vsize;
            for (int j17 = 0; (j17 < 4); j17 = (j17 + 1)) {
                fRec17_perm[j17] = fRec17_tmp[(vsize + j17)];
            }
            for (int j19 = 0; (j19 < 4); j19 = (j19 + 1)) {
                fRec16_perm[j19] = fRec16_tmp[(vsize + j19)];
            }
            /* Recursive loop 6 */
            /* Pre code */
            for (int j20 = 0; (j20 < 4); j20 = (j20 + 1)) {
                fRec19_tmp[j20] = fRec19_perm[j20];
            }
            fYec5_idx = ((fYec5_idx + fYec5_idx_save) & 2047);
            for (int j22 = 0; (j22 < 4); j22 = (j22 + 1)) {
                fRec18_tmp[j22] = fRec18_perm[j22];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec19[i] = ((fSlow0 * fRec19[(i - 1)]) + (fSlow1 * fRec18[(i - 1)]));
                fYec5[((i + fYec5_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec19[i]));
                fRec18[i] = fYec5[(((i + fYec5_idx) - 1491) & 2047)];
            }
            /* Post code */
            fYec5_idx_save = vsize;
            for (int j21 = 0; (j21 < 4); j21 = (j21 + 1)) {
                fRec19_perm[j21] = fRec19_tmp[(vsize + j21)];
            }
            for (int j23 = 0; (j23 < 4); j23 = (j23 + 1)) {
                fRec18_perm[j23] = fRec18_tmp[(vsize + j23)];
            }
            /* Recursive loop 7 */
            /* Pre code */
            for (int j24 = 0; (j24 < 4); j24 = (j24 + 1)) {
                fRec21_tmp[j24] = fRec21_perm[j24];
            }
            fYec6_idx = ((fYec6_idx + fYec6_idx_save) & 2047);
            for (int j26 = 0; (j26 < 4); j26 = (j26 + 1)) {
                fRec20_tmp[j26] = fRec20_perm[j26];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec21[i] = ((fSlow0 * fRec21[(i - 1)]) + (fSlow1 * fRec20[(i - 1)]));
                fYec6[((i + fYec6_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec21[i]));
                fRec20[i] = fYec6[(((i + fYec6_idx) - 1557) & 2047)];
            }
            /* Post code */
            fYec6_idx_save = vsize;
            for (int j25 = 0; (j25 < 4); j25 = (j25 + 1)) {
                fRec21_perm[j25] = fRec21_tmp[(vsize + j25)];
            }
            for (int j27 = 0; (j27 < 4); j27 = (j27 + 1)) {
                fRec20_perm[j27] = fRec20_tmp[(vsize + j27)];
            }
            /* Recursive loop 8 */
            /* Pre code */
            for (int j28 = 0; (j28 < 4); j28 = (j28 + 1)) {
                fRec23_tmp[j28] = fRec23_perm[j28];
            }
            fYec7_idx = ((fYec7_idx + fYec7_idx_save) & 2047);
            for (int j30 = 0; (j30 < 4); j30 = (j30 + 1)) {
                fRec22_tmp[j30] = fRec22_perm[j30];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec23[i] = ((fSlow0 * fRec23[(i - 1)]) + (fSlow1 * fRec22[(i - 1)]));
                fYec7[((i + fYec7_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec23[i]));
                fRec22[i] = fYec7[(((i + fYec7_idx) - 1617) & 2047)];
            }
            /* Post code */
            fYec7_idx_save = vsize;
            for (int j29 = 0; (j29 < 4); j29 = (j29 + 1)) {
                fRec23_perm[j29] = fRec23_tmp[(vsize + j29)];
            }
            for (int j31 = 0; (j31 < 4); j31 = (j31 + 1)) {
                fRec22_perm[j31] = fRec22_tmp[(vsize + j31)];
            }
            /* Recursive loop 9 */
            /* Pre code */
            for (int j40 = 0; (j40 < 4); j40 = (j40 + 1)) {
                fRec33_tmp[j40] = fRec33_perm[j40];
            }
            fYec12_idx = ((fYec12_idx + fYec12_idx_save) & 2047);
            for (int j42 = 0; (j42 < 4); j42 = (j42 + 1)) {
                fRec32_tmp[j42] = fRec32_perm[j42];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec33[i] = ((fSlow0 * fRec33[(i - 1)]) + (fSlow1 * fRec32[(i - 1)]));
                fYec12[((i + fYec12_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec33[i]));
                fRec32[i] = fYec12[(((i + fYec12_idx) - 1139) & 2047)];
            }
            /* Post code */
            fYec12_idx_save = vsize;
            for (int j41 = 0; (j41 < 4); j41 = (j41 + 1)) {
                fRec33_perm[j41] = fRec33_tmp[(vsize + j41)];
            }
            for (int j43 = 0; (j43 < 4); j43 = (j43 + 1)) {
                fRec32_perm[j43] = fRec32_tmp[(vsize + j43)];
            }
            /* Recursive loop 10 */
            /* Pre code */
            for (int j44 = 0; (j44 < 4); j44 = (j44 + 1)) {
                fRec35_tmp[j44] = fRec35_perm[j44];
            }
            fYec13_idx = ((fYec13_idx + fYec13_idx_save) & 2047);
            for (int j46 = 0; (j46 < 4); j46 = (j46 + 1)) {
                fRec34_tmp[j46] = fRec34_perm[j46];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec35[i] = ((fSlow0 * fRec35[(i - 1)]) + (fSlow1 * fRec34[(i - 1)]));
                fYec13[((i + fYec13_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec35[i]));
                fRec34[i] = fYec13[(((i + fYec13_idx) - 1211) & 2047)];
            }
            /* Post code */
            fYec13_idx_save = vsize;
            for (int j45 = 0; (j45 < 4); j45 = (j45 + 1)) {
                fRec35_perm[j45] = fRec35_tmp[(vsize + j45)];
            }
            for (int j47 = 0; (j47 < 4); j47 = (j47 + 1)) {
                fRec34_perm[j47] = fRec34_tmp[(vsize + j47)];
            }
            /* Recursive loop 11 */
            /* Pre code */
            for (int j48 = 0; (j48 < 4); j48 = (j48 + 1)) {
                fRec37_tmp[j48] = fRec37_perm[j48];
            }
            fYec14_idx = ((fYec14_idx + fYec14_idx_save) & 2047);
            for (int j50 = 0; (j50 < 4); j50 = (j50 + 1)) {
                fRec36_tmp[j50] = fRec36_perm[j50];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec37[i] = ((fSlow0 * fRec37[(i - 1)]) + (fSlow1 * fRec36[(i - 1)]));
                fYec14[((i + fYec14_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec37[i]));
                fRec36[i] = fYec14[(((i + fYec14_idx) - 1300) & 2047)];
            }
            /* Post code */
            fYec14_idx_save = vsize;
            for (int j49 = 0; (j49 < 4); j49 = (j49 + 1)) {
                fRec37_perm[j49] = fRec37_tmp[(vsize + j49)];
            }
            for (int j51 = 0; (j51 < 4); j51 = (j51 + 1)) {
                fRec36_perm[j51] = fRec36_tmp[(vsize + j51)];
            }
            /* Recursive loop 12 */
            /* Pre code */
            for (int j52 = 0; (j52 < 4); j52 = (j52 + 1)) {
                fRec39_tmp[j52] = fRec39_perm[j52];
            }
            fYec15_idx = ((fYec15_idx + fYec15_idx_save) & 2047);
            for (int j54 = 0; (j54 < 4); j54 = (j54 + 1)) {
                fRec38_tmp[j54] = fRec38_perm[j54];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec39[i] = ((fSlow0 * fRec39[(i - 1)]) + (fSlow1 * fRec38[(i - 1)]));
                fYec15[((i + fYec15_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec39[i]));
                fRec38[i] = fYec15[(((i + fYec15_idx) - 1379) & 2047)];
            }
            /* Post code */
            fYec15_idx_save = vsize;
            for (int j53 = 0; (j53 < 4); j53 = (j53 + 1)) {
                fRec39_perm[j53] = fRec39_tmp[(vsize + j53)];
            }
            for (int j55 = 0; (j55 < 4); j55 = (j55 + 1)) {
                fRec38_perm[j55] = fRec38_tmp[(vsize + j55)];
            }
            /* Recursive loop 13 */
            /* Pre code */
            for (int j56 = 0; (j56 < 4); j56 = (j56 + 1)) {
                fRec41_tmp[j56] = fRec41_perm[j56];
            }
            fYec16_idx = ((fYec16_idx + fYec16_idx_save) & 2047);
            for (int j58 = 0; (j58 < 4); j58 = (j58 + 1)) {
                fRec40_tmp[j58] = fRec40_perm[j58];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec41[i] = ((fSlow0 * fRec41[(i - 1)]) + (fSlow1 * fRec40[(i - 1)]));
                fYec16[((i + fYec16_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec41[i]));
                fRec40[i] = fYec16[(((i + fYec16_idx) - 1445) & 2047)];
            }
            /* Post code */
            fYec16_idx_save = vsize;
            for (int j57 = 0; (j57 < 4); j57 = (j57 + 1)) {
                fRec41_perm[j57] = fRec41_tmp[(vsize + j57)];
            }
            for (int j59 = 0; (j59 < 4); j59 = (j59 + 1)) {
                fRec40_perm[j59] = fRec40_tmp[(vsize + j59)];
            }
            /* Recursive loop 14 */
            /* Pre code */
            for (int j60 = 0; (j60 < 4); j60 = (j60 + 1)) {
                fRec43_tmp[j60] = fRec43_perm[j60];
            }
            fYec17_idx = ((fYec17_idx + fYec17_idx_save) & 2047);
            for (int j62 = 0; (j62 < 4); j62 = (j62 + 1)) {
                fRec42_tmp[j62] = fRec42_perm[j62];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec43[i] = ((fSlow0 * fRec43[(i - 1)]) + (fSlow1 * fRec42[(i - 1)]));
                fYec17[((i + fYec17_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec43[i]));
                fRec42[i] = fYec17[(((i + fYec17_idx) - 1514) & 2047)];
            }
            /* Post code */
            fYec17_idx_save = vsize;
            for (int j61 = 0; (j61 < 4); j61 = (j61 + 1)) {
                fRec43_perm[j61] = fRec43_tmp[(vsize + j61)];
            }
            for (int j63 = 0; (j63 < 4); j63 = (j63 + 1)) {
                fRec42_perm[j63] = fRec42_tmp[(vsize + j63)];
            }
            /* Recursive loop 15 */
            /* Pre code */
            for (int j64 = 0; (j64 < 4); j64 = (j64 + 1)) {
                fRec45_tmp[j64] = fRec45_perm[j64];
            }
            fYec18_idx = ((fYec18_idx + fYec18_idx_save) & 2047);
            for (int j66 = 0; (j66 < 4); j66 = (j66 + 1)) {
                fRec44_tmp[j66] = fRec44_perm[j66];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec45[i] = ((fSlow0 * fRec45[(i - 1)]) + (fSlow1 * fRec44[(i - 1)]));
                fYec18[((i + fYec18_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec45[i]));
                fRec44[i] = fYec18[(((i + fYec18_idx) - 1580) & 2047)];
            }
            /* Post code */
            fYec18_idx_save = vsize;
            for (int j65 = 0; (j65 < 4); j65 = (j65 + 1)) {
                fRec45_perm[j65] = fRec45_tmp[(vsize + j65)];
            }
            for (int j67 = 0; (j67 < 4); j67 = (j67 + 1)) {
                fRec44_perm[j67] = fRec44_tmp[(vsize + j67)];
            }
            /* Recursive loop 16 */
            /* Pre code */
            for (int j68 = 0; (j68 < 4); j68 = (j68 + 1)) {
                fRec47_tmp[j68] = fRec47_perm[j68];
            }
            fYec19_idx = ((fYec19_idx + fYec19_idx_save) & 2047);
            for (int j70 = 0; (j70 < 4); j70 = (j70 + 1)) {
                fRec46_tmp[j70] = fRec46_perm[j70];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec47[i] = ((fSlow0 * fRec47[(i - 1)]) + (fSlow1 * fRec46[(i - 1)]));
                fYec19[((i + fYec19_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec47[i]));
                fRec46[i] = fYec19[(((i + fYec19_idx) - 1640) & 2047)];
            }
            /* Post code */
            fYec19_idx_save = vsize;
            for (int j69 = 0; (j69 < 4); j69 = (j69 + 1)) {
                fRec47_perm[j69] = fRec47_tmp[(vsize + j69)];
            }
            for (int j71 = 0; (j71 < 4); j71 = (j71 + 1)) {
                fRec46_perm[j71] = fRec46_tmp[(vsize + j71)];
            }
            /* Vectorizable loop 17 */
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fZec1[i] = (((((((fRec8[i] + fRec10[i]) + fRec12[i]) + fRec14[i]) + fRec16[i]) + fRec18[i]) + fRec20[i]) + fRec22[i]);
            }
            /* Vectorizable loop 18 */
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fZec2[i] = (((((((fRec32[i] + fRec34[i]) + fRec36[i]) + fRec38[i]) + fRec40[i]) + fRec42[i]) + fRec44[i]) + fRec46[i]);
            }
            /* Recursive loop 19 */
            /* Pre code */
            fYec8_idx = ((fYec8_idx + fYec8_idx_save) & 1023);
            for (int j32 = 0; (j32 < 4); j32 = (j32 + 1)) {
                fRec6_tmp[j32] = fRec6_perm[j32];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fYec8[((i + fYec8_idx) & 1023)] = (fZec1[i] + (0.5f * fRec6[(i - 1)]));
                fRec6[i] = fYec8[(((i + fYec8_idx) - 556) & 1023)];
                fRec7[i] = (fRec6[(i - 1)] - fZec1[i]);
            }
            /* Post code */
            fYec8_idx_save = vsize;
            for (int j33 = 0; (j33 < 4); j33 = (j33 + 1)) {
                fRec6_perm[j33] = fRec6_tmp[(vsize + j33)];
            }
            /* Recursive loop 20 */
            /* Pre code */
            fYec20_idx = ((fYec20_idx + fYec20_idx_save) & 1023);
            for (int j72 = 0; (j72 < 4); j72 = (j72 + 1)) {
                fRec30_tmp[j72] = fRec30_perm[j72];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fYec20[((i + fYec20_idx) & 1023)] = (fZec2[i] + (0.5f * fRec30[(i - 1)]));
                fRec30[i] = fYec20[(((i + fYec20_idx) - 579) & 1023)];
                fRec31[i] = (fRec30[(i - 1)] - fZec2[i]);
            }
            /* Post code */
            fYec20_idx_save = vsize;
            for (int j73 = 0; (j73 < 4); j73 = (j73 + 1)) {
                fRec30_perm[j73] = fRec30_tmp[(vsize + j73)];
            }
            /* Recursive loop 21 */
            /* Pre code */
            fYec9_idx = ((fYec9_idx + fYec9_idx_save) & 511);
            for (int j34 = 0; (j34 < 4); j34 = (j34 + 1)) {
                fRec4_tmp[j34] = fRec4_perm[j34];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fYec9[((i + fYec9_idx) & 511)] = (fRec7[i] + (0.5f * fRec4[(i - 1)]));
                fRec4[i] = fYec9[(((i + fYec9_idx) - 441) & 511)];
                fRec5[i] = (fRec4[(i - 1)] - fRec7[i]);
            }
            /* Post code */
            fYec9_idx_save = vsize;
            for (int j35 = 0; (j35 < 4); j35 = (j35 + 1)) {
                fRec4_perm[j35] = fRec4_tmp[(vsize + j35)];
            }
            /* Recursive loop 22 */
            /* Pre code */
            fYec21_idx = ((fYec21_idx + fYec21_idx_save) & 511);
            for (int j74 = 0; (j74 < 4); j74 = (j74 + 1)) {
                fRec28_tmp[j74] = fRec28_perm[j74];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fYec21[((i + fYec21_idx) & 511)] = (fRec31[i] + (0.5f * fRec28[(i - 1)]));
                fRec28[i] = fYec21[(((i + fYec21_idx) - 464) & 511)];
                fRec29[i] = (fRec28[(i - 1)] - fRec31[i]);
            }
            /* Post code */
            fYec21_idx_save = vsize;
            for (int j75 = 0; (j75 < 4); j75 = (j75 + 1)) {
                fRec28_perm[j75] = fRec28_tmp[(vsize + j75)];
            }
            /* Recursive loop 23 */
            /* Pre code */
            fYec10_idx = ((fYec10_idx + fYec10_idx_save) & 511);
            for (int j36 = 0; (j36 < 4); j36 = (j36 + 1)) {
                fRec2_tmp[j36] = fRec2_perm[j36];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fYec10[((i + fYec10_idx) & 511)] = (fRec5[i] + (0.5f * fRec2[(i - 1)]));
                fRec2[i] = fYec10[(((i + fYec10_idx) - 341) & 511)];
                fRec3[i] = (fRec2[(i - 1)] - fRec5[i]);
            }
            /* Post code */
            fYec10_idx_save = vsize;
            for (int j37 = 0; (j37 < 4); j37 = (j37 + 1)) {
                fRec2_perm[j37] = fRec2_tmp[(vsize + j37)];
            }
            /* Recursive loop 24 */
            /* Pre code */
            fYec22_idx = ((fYec22_idx + fYec22_idx_save) & 511);
            for (int j76 = 0; (j76 < 4); j76 = (j76 + 1)) {
                fRec26_tmp[j76] = fRec26_perm[j76];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fYec22[((i + fYec22_idx) & 511)] = (fRec29[i] + (0.5f * fRec26[(i - 1)]));
                fRec26[i] = fYec22[(((i + fYec22_idx) - 364) & 511)];
                fRec27[i] = (fRec26[(i - 1)] - fRec29[i]);
            }
            /* Post code */
            fYec22_idx_save = vsize;
            for (int j77 = 0; (j77 < 4); j77 = (j77 + 1)) {
                fRec26_perm[j77] = fRec26_tmp[(vsize + j77)];
            }
            /* Recursive loop 25 */
            /* Pre code */
            fYec11_idx = ((fYec11_idx + fYec11_idx_save) & 511);
            for (int j38 = 0; (j38 < 4); j38 = (j38 + 1)) {
                fRec0_tmp[j38] = fRec0_perm[j38];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fYec11[((i + fYec11_idx) & 511)] = (fRec3[i] + (0.5f * fRec0[(i - 1)]));
                fRec0[i] = fYec11[(((i + fYec11_idx) - 225) & 511)];
                fRec1[i] = (fRec0[(i - 1)] - fRec3[i]);
            }
            /* Post code */
            fYec11_idx_save = vsize;
            for (int j39 = 0; (j39 < 4); j39 = (j39 + 1)) {
                fRec0_perm[j39] = fRec0_tmp[(vsize + j39)];
            }
            /* Recursive loop 26 */
            /* Pre code */
            fYec23_idx = ((fYec23_idx + fYec23_idx_save) & 511);
            for (int j78 = 0; (j78 < 4); j78 = (j78 + 1)) {
                fRec24_tmp[j78] = fRec24_perm[j78];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fYec23[((i + fYec23_idx) & 511)] = (fRec27[i] + (0.5f * fRec24[(i - 1)]));
                fRec24[i] = fYec23[(((i + fYec23_idx) - 248) & 511)];
                fRec25[i] = (fRec24[(i - 1)] - fRec27[i]);
            }
            /* Post code */
            fYec23_idx_save = vsize;
            for (int j79 = 0; (j79 < 4); j79 = (j79 + 1)) {
                fRec24_perm[j79] = fRec24_tmp[(vsize + j79)];
            }
            /* Vectorizable loop 27 */
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                output0[i] = FAUSTFLOAT(((fSlow3 * fRec1[i]) + (fSlow4 * float(input0[i]))));
            }
            /* Vectorizable loop 28 */
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                output1[i] = FAUSTFLOAT(((fSlow3 * fRec25[i]) + (fSlow4 * float(input1[i]))));
            }
        }
        /* Remaining frames */
        if (vindex < count) {
            FAUSTFLOAT *input0 = &input0_ptr[vindex];
            FAUSTFLOAT *input1 = &input1_ptr[vindex];
            FAUSTFLOAT *output0 = &output0_ptr[vindex];
            FAUSTFLOAT *output1 = &output1_ptr[vindex];
            int vsize = (count - vindex);
            /* Vectorizable loop 0 */
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fZec0[i] = (0.0149999997f * (float(input0[i]) + float(input1[i])));
            }
            /* Recursive loop 1 */
            /* Pre code */
            for (int j0 = 0; (j0 < 4); j0 = (j0 + 1)) {
                fRec9_tmp[j0] = fRec9_perm[j0];
            }
            fYec0_idx = ((fYec0_idx + fYec0_idx_save) & 2047);
            for (int j2 = 0; (j2 < 4); j2 = (j2 + 1)) {
                fRec8_tmp[j2] = fRec8_perm[j2];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec9[i] = ((fSlow0 * fRec9[(i - 1)]) + (fSlow1 * fRec8[(i - 1)]));
                fYec0[((i + fYec0_idx) & 2047)] = ((fSlow2 * fRec9[i]) + fZec0[i]);
                fRec8[i] = fYec0[(((i + fYec0_idx) - 1116) & 2047)];
            }
            /* Post code */
            fYec0_idx_save = vsize;
            for (int j1 = 0; (j1 < 4); j1 = (j1 + 1)) {
                fRec9_perm[j1] = fRec9_tmp[(vsize + j1)];
            }
            for (int j3 = 0; (j3 < 4); j3 = (j3 + 1)) {
                fRec8_perm[j3] = fRec8_tmp[(vsize + j3)];
            }
            /* Recursive loop 2 */
            /* Pre code */
            for (int j4 = 0; (j4 < 4); j4 = (j4 + 1)) {
                fRec11_tmp[j4] = fRec11_perm[j4];
            }
            fYec1_idx = ((fYec1_idx + fYec1_idx_save) & 2047);
            for (int j6 = 0; (j6 < 4); j6 = (j6 + 1)) {
                fRec10_tmp[j6] = fRec10_perm[j6];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec11[i] = ((fSlow0 * fRec11[(i - 1)]) + (fSlow1 * fRec10[(i - 1)]));
                fYec1[((i + fYec1_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec11[i]));
                fRec10[i] = fYec1[(((i + fYec1_idx) - 1188) & 2047)];
            }
            /* Post code */
            fYec1_idx_save = vsize;
            for (int j5 = 0; (j5 < 4); j5 = (j5 + 1)) {
                fRec11_perm[j5] = fRec11_tmp[(vsize + j5)];
            }
            for (int j7 = 0; (j7 < 4); j7 = (j7 + 1)) {
                fRec10_perm[j7] = fRec10_tmp[(vsize + j7)];
            }
            /* Recursive loop 3 */
            /* Pre code */
            for (int j8 = 0; (j8 < 4); j8 = (j8 + 1)) {
                fRec13_tmp[j8] = fRec13_perm[j8];
            }
            fYec2_idx = ((fYec2_idx + fYec2_idx_save) & 2047);
            for (int j10 = 0; (j10 < 4); j10 = (j10 + 1)) {
                fRec12_tmp[j10] = fRec12_perm[j10];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec13[i] = ((fSlow0 * fRec13[(i - 1)]) + (fSlow1 * fRec12[(i - 1)]));
                fYec2[((i + fYec2_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec13[i]));
                fRec12[i] = fYec2[(((i + fYec2_idx) - 1277) & 2047)];
            }
            /* Post code */
            fYec2_idx_save = vsize;
            for (int j9 = 0; (j9 < 4); j9 = (j9 + 1)) {
                fRec13_perm[j9] = fRec13_tmp[(vsize + j9)];
            }
            for (int j11 = 0; (j11 < 4); j11 = (j11 + 1)) {
                fRec12_perm[j11] = fRec12_tmp[(vsize + j11)];
            }
            /* Recursive loop 4 */
            /* Pre code */
            for (int j12 = 0; (j12 < 4); j12 = (j12 + 1)) {
                fRec15_tmp[j12] = fRec15_perm[j12];
            }
            fYec3_idx = ((fYec3_idx + fYec3_idx_save) & 2047);
            for (int j14 = 0; (j14 < 4); j14 = (j14 + 1)) {
                fRec14_tmp[j14] = fRec14_perm[j14];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec15[i] = ((fSlow0 * fRec15[(i - 1)]) + (fSlow1 * fRec14[(i - 1)]));
                fYec3[((i + fYec3_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec15[i]));
                fRec14[i] = fYec3[(((i + fYec3_idx) - 1356) & 2047)];
            }
            /* Post code */
            fYec3_idx_save = vsize;
            for (int j13 = 0; (j13 < 4); j13 = (j13 + 1)) {
                fRec15_perm[j13] = fRec15_tmp[(vsize + j13)];
            }
            for (int j15 = 0; (j15 < 4); j15 = (j15 + 1)) {
                fRec14_perm[j15] = fRec14_tmp[(vsize + j15)];
            }
            /* Recursive loop 5 */
            /* Pre code */
            for (int j16 = 0; (j16 < 4); j16 = (j16 + 1)) {
                fRec17_tmp[j16] = fRec17_perm[j16];
            }
            fYec4_idx = ((fYec4_idx + fYec4_idx_save) & 2047);
            for (int j18 = 0; (j18 < 4); j18 = (j18 + 1)) {
                fRec16_tmp[j18] = fRec16_perm[j18];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec17[i] = ((fSlow0 * fRec17[(i - 1)]) + (fSlow1 * fRec16[(i - 1)]));
                fYec4[((i + fYec4_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec17[i]));
                fRec16[i] = fYec4[(((i + fYec4_idx) - 1422) & 2047)];
            }
            /* Post code */
            fYec4_idx_save = vsize;
            for (int j17 = 0; (j17 < 4); j17 = (j17 + 1)) {
                fRec17_perm[j17] = fRec17_tmp[(vsize + j17)];
            }
            for (int j19 = 0; (j19 < 4); j19 = (j19 + 1)) {
                fRec16_perm[j19] = fRec16_tmp[(vsize + j19)];
            }
            /* Recursive loop 6 */
            /* Pre code */
            for (int j20 = 0; (j20 < 4); j20 = (j20 + 1)) {
                fRec19_tmp[j20] = fRec19_perm[j20];
            }
            fYec5_idx = ((fYec5_idx + fYec5_idx_save) & 2047);
            for (int j22 = 0; (j22 < 4); j22 = (j22 + 1)) {
                fRec18_tmp[j22] = fRec18_perm[j22];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec19[i] = ((fSlow0 * fRec19[(i - 1)]) + (fSlow1 * fRec18[(i - 1)]));
                fYec5[((i + fYec5_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec19[i]));
                fRec18[i] = fYec5[(((i + fYec5_idx) - 1491) & 2047)];
            }
            /* Post code */
            fYec5_idx_save = vsize;
            for (int j21 = 0; (j21 < 4); j21 = (j21 + 1)) {
                fRec19_perm[j21] = fRec19_tmp[(vsize + j21)];
            }
            for (int j23 = 0; (j23 < 4); j23 = (j23 + 1)) {
                fRec18_perm[j23] = fRec18_tmp[(vsize + j23)];
            }
            /* Recursive loop 7 */
            /* Pre code */
            for (int j24 = 0; (j24 < 4); j24 = (j24 + 1)) {
                fRec21_tmp[j24] = fRec21_perm[j24];
            }
            fYec6_idx = ((fYec6_idx + fYec6_idx_save) & 2047);
            for (int j26 = 0; (j26 < 4); j26 = (j26 + 1)) {
                fRec20_tmp[j26] = fRec20_perm[j26];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec21[i] = ((fSlow0 * fRec21[(i - 1)]) + (fSlow1 * fRec20[(i - 1)]));
                fYec6[((i + fYec6_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec21[i]));
                fRec20[i] = fYec6[(((i + fYec6_idx) - 1557) & 2047)];
            }
            /* Post code */
            fYec6_idx_save = vsize;
            for (int j25 = 0; (j25 < 4); j25 = (j25 + 1)) {
                fRec21_perm[j25] = fRec21_tmp[(vsize + j25)];
            }
            for (int j27 = 0; (j27 < 4); j27 = (j27 + 1)) {
                fRec20_perm[j27] = fRec20_tmp[(vsize + j27)];
            }
            /* Recursive loop 8 */
            /* Pre code */
            for (int j28 = 0; (j28 < 4); j28 = (j28 + 1)) {
                fRec23_tmp[j28] = fRec23_perm[j28];
            }
            fYec7_idx = ((fYec7_idx + fYec7_idx_save) & 2047);
            for (int j30 = 0; (j30 < 4); j30 = (j30 + 1)) {
                fRec22_tmp[j30] = fRec22_perm[j30];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec23[i] = ((fSlow0 * fRec23[(i - 1)]) + (fSlow1 * fRec22[(i - 1)]));
                fYec7[((i + fYec7_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec23[i]));
                fRec22[i] = fYec7[(((i + fYec7_idx) - 1617) & 2047)];
            }
            /* Post code */
            fYec7_idx_save = vsize;
            for (int j29 = 0; (j29 < 4); j29 = (j29 + 1)) {
                fRec23_perm[j29] = fRec23_tmp[(vsize + j29)];
            }
            for (int j31 = 0; (j31 < 4); j31 = (j31 + 1)) {
                fRec22_perm[j31] = fRec22_tmp[(vsize + j31)];
            }
            /* Recursive loop 9 */
            /* Pre code */
            for (int j40 = 0; (j40 < 4); j40 = (j40 + 1)) {
                fRec33_tmp[j40] = fRec33_perm[j40];
            }
            fYec12_idx = ((fYec12_idx + fYec12_idx_save) & 2047);
            for (int j42 = 0; (j42 < 4); j42 = (j42 + 1)) {
                fRec32_tmp[j42] = fRec32_perm[j42];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec33[i] = ((fSlow0 * fRec33[(i - 1)]) + (fSlow1 * fRec32[(i - 1)]));
                fYec12[((i + fYec12_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec33[i]));
                fRec32[i] = fYec12[(((i + fYec12_idx) - 1139) & 2047)];
            }
            /* Post code */
            fYec12_idx_save = vsize;
            for (int j41 = 0; (j41 < 4); j41 = (j41 + 1)) {
                fRec33_perm[j41] = fRec33_tmp[(vsize + j41)];
            }
            for (int j43 = 0; (j43 < 4); j43 = (j43 + 1)) {
                fRec32_perm[j43] = fRec32_tmp[(vsize + j43)];
            }
            /* Recursive loop 10 */
            /* Pre code */
            for (int j44 = 0; (j44 < 4); j44 = (j44 + 1)) {
                fRec35_tmp[j44] = fRec35_perm[j44];
            }
            fYec13_idx = ((fYec13_idx + fYec13_idx_save) & 2047);
            for (int j46 = 0; (j46 < 4); j46 = (j46 + 1)) {
                fRec34_tmp[j46] = fRec34_perm[j46];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec35[i] = ((fSlow0 * fRec35[(i - 1)]) + (fSlow1 * fRec34[(i - 1)]));
                fYec13[((i + fYec13_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec35[i]));
                fRec34[i] = fYec13[(((i + fYec13_idx) - 1211) & 2047)];
            }
            /* Post code */
            fYec13_idx_save = vsize;
            for (int j45 = 0; (j45 < 4); j45 = (j45 + 1)) {
                fRec35_perm[j45] = fRec35_tmp[(vsize + j45)];
            }
            for (int j47 = 0; (j47 < 4); j47 = (j47 + 1)) {
                fRec34_perm[j47] = fRec34_tmp[(vsize + j47)];
            }
            /* Recursive loop 11 */
            /* Pre code */
            for (int j48 = 0; (j48 < 4); j48 = (j48 + 1)) {
                fRec37_tmp[j48] = fRec37_perm[j48];
            }
            fYec14_idx = ((fYec14_idx + fYec14_idx_save) & 2047);
            for (int j50 = 0; (j50 < 4); j50 = (j50 + 1)) {
                fRec36_tmp[j50] = fRec36_perm[j50];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec37[i] = ((fSlow0 * fRec37[(i - 1)]) + (fSlow1 * fRec36[(i - 1)]));
                fYec14[((i + fYec14_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec37[i]));
                fRec36[i] = fYec14[(((i + fYec14_idx) - 1300) & 2047)];
            }
            /* Post code */
            fYec14_idx_save = vsize;
            for (int j49 = 0; (j49 < 4); j49 = (j49 + 1)) {
                fRec37_perm[j49] = fRec37_tmp[(vsize + j49)];
            }
            for (int j51 = 0; (j51 < 4); j51 = (j51 + 1)) {
                fRec36_perm[j51] = fRec36_tmp[(vsize + j51)];
            }
            /* Recursive loop 12 */
            /* Pre code */
            for (int j52 = 0; (j52 < 4); j52 = (j52 + 1)) {
                fRec39_tmp[j52] = fRec39_perm[j52];
            }
            fYec15_idx = ((fYec15_idx + fYec15_idx_save) & 2047);
            for (int j54 = 0; (j54 < 4); j54 = (j54 + 1)) {
                fRec38_tmp[j54] = fRec38_perm[j54];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec39[i] = ((fSlow0 * fRec39[(i - 1)]) + (fSlow1 * fRec38[(i - 1)]));
                fYec15[((i + fYec15_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec39[i]));
                fRec38[i] = fYec15[(((i + fYec15_idx) - 1379) & 2047)];
            }
            /* Post code */
            fYec15_idx_save = vsize;
            for (int j53 = 0; (j53 < 4); j53 = (j53 + 1)) {
                fRec39_perm[j53] = fRec39_tmp[(vsize + j53)];
            }
            for (int j55 = 0; (j55 < 4); j55 = (j55 + 1)) {
                fRec38_perm[j55] = fRec38_tmp[(vsize + j55)];
            }
            /* Recursive loop 13 */
            /* Pre code */
            for (int j56 = 0; (j56 < 4); j56 = (j56 + 1)) {
                fRec41_tmp[j56] = fRec41_perm[j56];
            }
            fYec16_idx = ((fYec16_idx + fYec16_idx_save) & 2047);
            for (int j58 = 0; (j58 < 4); j58 = (j58 + 1)) {
                fRec40_tmp[j58] = fRec40_perm[j58];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec41[i] = ((fSlow0 * fRec41[(i - 1)]) + (fSlow1 * fRec40[(i - 1)]));
                fYec16[((i + fYec16_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec41[i]));
                fRec40[i] = fYec16[(((i + fYec16_idx) - 1445) & 2047)];
            }
            /* Post code */
            fYec16_idx_save = vsize;
            for (int j57 = 0; (j57 < 4); j57 = (j57 + 1)) {
                fRec41_perm[j57] = fRec41_tmp[(vsize + j57)];
            }
            for (int j59 = 0; (j59 < 4); j59 = (j59 + 1)) {
                fRec40_perm[j59] = fRec40_tmp[(vsize + j59)];
            }
            /* Recursive loop 14 */
            /* Pre code */
            for (int j60 = 0; (j60 < 4); j60 = (j60 + 1)) {
                fRec43_tmp[j60] = fRec43_perm[j60];
            }
            fYec17_idx = ((fYec17_idx + fYec17_idx_save) & 2047);
            for (int j62 = 0; (j62 < 4); j62 = (j62 + 1)) {
                fRec42_tmp[j62] = fRec42_perm[j62];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec43[i] = ((fSlow0 * fRec43[(i - 1)]) + (fSlow1 * fRec42[(i - 1)]));
                fYec17[((i + fYec17_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec43[i]));
                fRec42[i] = fYec17[(((i + fYec17_idx) - 1514) & 2047)];
            }
            /* Post code */
            fYec17_idx_save = vsize;
            for (int j61 = 0; (j61 < 4); j61 = (j61 + 1)) {
                fRec43_perm[j61] = fRec43_tmp[(vsize + j61)];
            }
            for (int j63 = 0; (j63 < 4); j63 = (j63 + 1)) {
                fRec42_perm[j63] = fRec42_tmp[(vsize + j63)];
            }
            /* Recursive loop 15 */
            /* Pre code */
            for (int j64 = 0; (j64 < 4); j64 = (j64 + 1)) {
                fRec45_tmp[j64] = fRec45_perm[j64];
            }
            fYec18_idx = ((fYec18_idx + fYec18_idx_save) & 2047);
            for (int j66 = 0; (j66 < 4); j66 = (j66 + 1)) {
                fRec44_tmp[j66] = fRec44_perm[j66];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec45[i] = ((fSlow0 * fRec45[(i - 1)]) + (fSlow1 * fRec44[(i - 1)]));
                fYec18[((i + fYec18_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec45[i]));
                fRec44[i] = fYec18[(((i + fYec18_idx) - 1580) & 2047)];
            }
            /* Post code */
            fYec18_idx_save = vsize;
            for (int j65 = 0; (j65 < 4); j65 = (j65 + 1)) {
                fRec45_perm[j65] = fRec45_tmp[(vsize + j65)];
            }
            for (int j67 = 0; (j67 < 4); j67 = (j67 + 1)) {
                fRec44_perm[j67] = fRec44_tmp[(vsize + j67)];
            }
            /* Recursive loop 16 */
            /* Pre code */
            for (int j68 = 0; (j68 < 4); j68 = (j68 + 1)) {
                fRec47_tmp[j68] = fRec47_perm[j68];
            }
            fYec19_idx = ((fYec19_idx + fYec19_idx_save) & 2047);
            for (int j70 = 0; (j70 < 4); j70 = (j70 + 1)) {
                fRec46_tmp[j70] = fRec46_perm[j70];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fRec47[i] = ((fSlow0 * fRec47[(i - 1)]) + (fSlow1 * fRec46[(i - 1)]));
                fYec19[((i + fYec19_idx) & 2047)] = (fZec0[i] + (fSlow2 * fRec47[i]));
                fRec46[i] = fYec19[(((i + fYec19_idx) - 1640) & 2047)];
            }
            /* Post code */
            fYec19_idx_save = vsize;
            for (int j69 = 0; (j69 < 4); j69 = (j69 + 1)) {
                fRec47_perm[j69] = fRec47_tmp[(vsize + j69)];
            }
            for (int j71 = 0; (j71 < 4); j71 = (j71 + 1)) {
                fRec46_perm[j71] = fRec46_tmp[(vsize + j71)];
            }
            /* Vectorizable loop 17 */
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fZec1[i] = (((((((fRec8[i] + fRec10[i]) + fRec12[i]) + fRec14[i]) + fRec16[i]) + fRec18[i]) + fRec20[i]) + fRec22[i]);
            }
            /* Vectorizable loop 18 */
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fZec2[i] = (((((((fRec32[i] + fRec34[i]) + fRec36[i]) + fRec38[i]) + fRec40[i]) + fRec42[i]) + fRec44[i]) + fRec46[i]);
            }
            /* Recursive loop 19 */
            /* Pre code */
            fYec8_idx = ((fYec8_idx + fYec8_idx_save) & 1023);
            for (int j32 = 0; (j32 < 4); j32 = (j32 + 1)) {
                fRec6_tmp[j32] = fRec6_perm[j32];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fYec8[((i + fYec8_idx) & 1023)] = (fZec1[i] + (0.5f * fRec6[(i - 1)]));
                fRec6[i] = fYec8[(((i + fYec8_idx) - 556) & 1023)];
                fRec7[i] = (fRec6[(i - 1)] - fZec1[i]);
            }
            /* Post code */
            fYec8_idx_save = vsize;
            for (int j33 = 0; (j33 < 4); j33 = (j33 + 1)) {
                fRec6_perm[j33] = fRec6_tmp[(vsize + j33)];
            }
            /* Recursive loop 20 */
            /* Pre code */
            fYec20_idx = ((fYec20_idx + fYec20_idx_save) & 1023);
            for (int j72 = 0; (j72 < 4); j72 = (j72 + 1)) {
                fRec30_tmp[j72] = fRec30_perm[j72];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fYec20[((i + fYec20_idx) & 1023)] = (fZec2[i] + (0.5f * fRec30[(i - 1)]));
                fRec30[i] = fYec20[(((i + fYec20_idx) - 579) & 1023)];
                fRec31[i] = (fRec30[(i - 1)] - fZec2[i]);
            }
            /* Post code */
            fYec20_idx_save = vsize;
            for (int j73 = 0; (j73 < 4); j73 = (j73 + 1)) {
                fRec30_perm[j73] = fRec30_tmp[(vsize + j73)];
            }
            /* Recursive loop 21 */
            /* Pre code */
            fYec9_idx = ((fYec9_idx + fYec9_idx_save) & 511);
            for (int j34 = 0; (j34 < 4); j34 = (j34 + 1)) {
                fRec4_tmp[j34] = fRec4_perm[j34];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fYec9[((i + fYec9_idx) & 511)] = (fRec7[i] + (0.5f * fRec4[(i - 1)]));
                fRec4[i] = fYec9[(((i + fYec9_idx) - 441) & 511)];
                fRec5[i] = (fRec4[(i - 1)] - fRec7[i]);
            }
            /* Post code */
            fYec9_idx_save = vsize;
            for (int j35 = 0; (j35 < 4); j35 = (j35 + 1)) {
                fRec4_perm[j35] = fRec4_tmp[(vsize + j35)];
            }
            /* Recursive loop 22 */
            /* Pre code */
            fYec21_idx = ((fYec21_idx + fYec21_idx_save) & 511);
            for (int j74 = 0; (j74 < 4); j74 = (j74 + 1)) {
                fRec28_tmp[j74] = fRec28_perm[j74];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fYec21[((i + fYec21_idx) & 511)] = (fRec31[i] + (0.5f * fRec28[(i - 1)]));
                fRec28[i] = fYec21[(((i + fYec21_idx) - 464) & 511)];
                fRec29[i] = (fRec28[(i - 1)] - fRec31[i]);
            }
            /* Post code */
            fYec21_idx_save = vsize;
            for (int j75 = 0; (j75 < 4); j75 = (j75 + 1)) {
                fRec28_perm[j75] = fRec28_tmp[(vsize + j75)];
            }
            /* Recursive loop 23 */
            /* Pre code */
            fYec10_idx = ((fYec10_idx + fYec10_idx_save) & 511);
            for (int j36 = 0; (j36 < 4); j36 = (j36 + 1)) {
                fRec2_tmp[j36] = fRec2_perm[j36];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fYec10[((i + fYec10_idx) & 511)] = (fRec5[i] + (0.5f * fRec2[(i - 1)]));
                fRec2[i] = fYec10[(((i + fYec10_idx) - 341) & 511)];
                fRec3[i] = (fRec2[(i - 1)] - fRec5[i]);
            }
            /* Post code */
            fYec10_idx_save = vsize;
            for (int j37 = 0; (j37 < 4); j37 = (j37 + 1)) {
                fRec2_perm[j37] = fRec2_tmp[(vsize + j37)];
            }
            /* Recursive loop 24 */
            /* Pre code */
            fYec22_idx = ((fYec22_idx + fYec22_idx_save) & 511);
            for (int j76 = 0; (j76 < 4); j76 = (j76 + 1)) {
                fRec26_tmp[j76] = fRec26_perm[j76];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fYec22[((i + fYec22_idx) & 511)] = (fRec29[i] + (0.5f * fRec26[(i - 1)]));
                fRec26[i] = fYec22[(((i + fYec22_idx) - 364) & 511)];
                fRec27[i] = (fRec26[(i - 1)] - fRec29[i]);
            }
            /* Post code */
            fYec22_idx_save = vsize;
            for (int j77 = 0; (j77 < 4); j77 = (j77 + 1)) {
                fRec26_perm[j77] = fRec26_tmp[(vsize + j77)];
            }
            /* Recursive loop 25 */
            /* Pre code */
            fYec11_idx = ((fYec11_idx + fYec11_idx_save) & 511);
            for (int j38 = 0; (j38 < 4); j38 = (j38 + 1)) {
                fRec0_tmp[j38] = fRec0_perm[j38];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fYec11[((i + fYec11_idx) & 511)] = (fRec3[i] + (0.5f * fRec0[(i - 1)]));
                fRec0[i] = fYec11[(((i + fYec11_idx) - 225) & 511)];
                fRec1[i] = (fRec0[(i - 1)] - fRec3[i]);
            }
            /* Post code */
            fYec11_idx_save = vsize;
            for (int j39 = 0; (j39 < 4); j39 = (j39 + 1)) {
                fRec0_perm[j39] = fRec0_tmp[(vsize + j39)];
            }
            /* Recursive loop 26 */
            /* Pre code */
            fYec23_idx = ((fYec23_idx + fYec23_idx_save) & 511);
            for (int j78 = 0; (j78 < 4); j78 = (j78 + 1)) {
                fRec24_tmp[j78] = fRec24_perm[j78];
            }
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                fYec23[((i + fYec23_idx) & 511)] = (fRec27[i] + (0.5f * fRec24[(i - 1)]));
                fRec24[i] = fYec23[(((i + fYec23_idx) - 248) & 511)];
                fRec25[i] = (fRec24[(i - 1)] - fRec27[i]);
            }
            /* Post code */
            fYec23_idx_save = vsize;
            for (int j79 = 0; (j79 < 4); j79 = (j79 + 1)) {
                fRec24_perm[j79] = fRec24_tmp[(vsize + j79)];
            }
            /* Vectorizable loop 27 */
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                output0[i] = FAUSTFLOAT(((fSlow3 * fRec1[i]) + (fSlow4 * float(input0[i]))));
            }
            /* Vectorizable loop 28 */
            /* Compute code */
            for (int i = 0; (i < vsize); i = (i + 1)) {
                output1[i] = FAUSTFLOAT(((fSlow3 * fRec25[i]) + (fSlow4 * float(input1[i]))));
            }
        }
    }
};

#endif
