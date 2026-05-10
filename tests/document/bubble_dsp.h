/* ------------------------------------------------------------
copyright: "(c) 2017: Yann Orlarey, GRAME"
license: "MIT"
name: "bubble"
Code generated with Faust 2.81.10 (https://faust.grame.fr)
Compilation options: -lang cpp -ct 1 -cn BubbleDsp -scn augr::FaustDsp -es 1 -mcd 16 -mdd 1024 -mdy 33 -single -ftz 0
------------------------------------------------------------ */

#ifndef  __BubbleDsp_H__
#define  __BubbleDsp_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif 

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <math.h>

#ifndef FAUSTCLASS 
#define FAUSTCLASS BubbleDsp
#endif

#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

#if defined(_WIN32)
#define RESTRICT __restrict
#else
#define RESTRICT __restrict__
#endif

class BubbleDspSIG0 {
	
  private:
	
	int iVec0[2];
	int iRec0[2];
	
  public:
	
	int getNumInputsBubbleDspSIG0() {
		return 0;
	}
	int getNumOutputsBubbleDspSIG0() {
		return 1;
	}
	
	void instanceInitBubbleDspSIG0(int sample_rate) {
		for (int l0 = 0; l0 < 2; l0 = l0 + 1) {
			iVec0[l0] = 0;
		}
		for (int l1 = 0; l1 < 2; l1 = l1 + 1) {
			iRec0[l1] = 0;
		}
	}
	
	void fillBubbleDspSIG0(int count, float* table) {
		for (int i1 = 0; i1 < count; i1 = i1 + 1) {
			iVec0[0] = 1;
			iRec0[0] = (iVec0[1] + iRec0[1]) % 65536;
			table[i1] = std::sin(9.58738e-05f * static_cast<float>(iRec0[0]));
			iVec0[1] = iVec0[0];
			iRec0[1] = iRec0[0];
		}
	}

};

static BubbleDspSIG0* newBubbleDspSIG0() { return (BubbleDspSIG0*)new BubbleDspSIG0(); }
static void deleteBubbleDspSIG0(BubbleDspSIG0* dsp) { delete dsp; }

static float ftbl0BubbleDspSIG0[65536];

class BubbleDsp : public augr::FaustDsp {
	
 private:
	
	int iVec1[2];
	FAUSTFLOAT fButton0;
	float fVec2[2];
	int iRec2[2];
	FAUSTFLOAT fHslider0;
	int fSampleRate;
	float fConst0;
	float fConst1;
	float fConst2;
	float fRec1[2];
	float fRec3[2];
	FAUSTFLOAT fVslider0;
	FAUSTFLOAT fVslider1;
	float fConst3;
	float fRec13[2];
	FAUSTFLOAT fVslider2;
	float fConst4;
	int IOTA0;
	float fVec3[8192];
	int iConst5;
	int iConst6;
	float fRec12[2];
	float fRec15[2];
	float fVec4[8192];
	int iConst7;
	int iConst8;
	float fRec14[2];
	float fRec17[2];
	float fVec5[8192];
	int iConst9;
	int iConst10;
	float fRec16[2];
	float fRec19[2];
	float fVec6[8192];
	int iConst11;
	int iConst12;
	float fRec18[2];
	float fRec21[2];
	float fVec7[8192];
	int iConst13;
	int iConst14;
	float fRec20[2];
	float fRec23[2];
	float fVec8[8192];
	int iConst15;
	int iConst16;
	float fRec22[2];
	float fRec25[2];
	float fVec9[8192];
	int iConst17;
	int iConst18;
	float fRec24[2];
	float fRec27[2];
	float fVec10[8192];
	int iConst19;
	int iConst20;
	float fRec26[2];
	float fVec11[2048];
	int iConst21;
	int iConst22;
	float fRec10[2];
	float fVec12[2048];
	int iConst23;
	int iConst24;
	float fRec8[2];
	float fVec13[2048];
	int iConst25;
	int iConst26;
	float fRec6[2];
	float fVec14[1024];
	int iConst27;
	int iConst28;
	float fRec4[2];
	float fRec37[2];
	float fVec15[8192];
	FAUSTFLOAT fVslider3;
	float fConst29;
	float fRec36[2];
	float fRec39[2];
	float fVec16[8192];
	float fRec38[2];
	float fRec41[2];
	float fVec17[8192];
	float fRec40[2];
	float fRec43[2];
	float fVec18[8192];
	float fRec42[2];
	float fRec45[2];
	float fVec19[8192];
	float fRec44[2];
	float fRec47[2];
	float fVec20[8192];
	float fRec46[2];
	float fRec49[2];
	float fVec21[8192];
	float fRec48[2];
	float fRec51[2];
	float fVec22[8192];
	float fRec50[2];
	float fVec23[2048];
	float fRec34[2];
	float fVec24[2048];
	float fRec32[2];
	float fVec25[2048];
	float fRec30[2];
	float fVec26[2048];
	float fRec28[2];
	
 public:
	BubbleDsp() {
	}
	
	void metadata(Meta* m) { 
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "1.22.0");
		m->declare("compile_options", "-lang cpp -ct 1 -cn BubbleDsp -scn augr::FaustDsp -es 1 -mcd 16 -mdd 1024 -mdy 33 -single -ftz 0");
		m->declare("copyright", "(c) 2017: Yann Orlarey, GRAME");
		m->declare("delays.lib/name", "Faust Delay Library");
		m->declare("delays.lib/version", "1.2.0");
		m->declare("demos.lib/freeverb_demo:author", " Romain Michon");
		m->declare("demos.lib/freeverb_demo:licence", "LGPL");
		m->declare("demos.lib/name", "Faust Demos Library");
		m->declare("demos.lib/version", "1.2.0");
		m->declare("description", "Production of a water drop bubble sound.");
		m->declare("filename", "bubble.dsp");
		m->declare("filters.lib/allpass_comb:author", "Julius O. Smith III");
		m->declare("filters.lib/allpass_comb:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/allpass_comb:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/lowpass0_highpass1", "MIT-style STK-4.3 license");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/version", "1.7.1");
		m->declare("license", "MIT");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.9.0");
		m->declare("name", "bubble");
		m->declare("oscillators.lib/name", "Faust Oscillator Library");
		m->declare("oscillators.lib/version", "1.6.0");
		m->declare("platform.lib/name", "Generic Platform Library");
		m->declare("platform.lib/version", "1.3.0");
		m->declare("reverbs.lib/mono_freeverb:author", "Romain Michon");
		m->declare("reverbs.lib/name", "Faust Reverb Library");
		m->declare("reverbs.lib/stereo_freeverb:author", "Romain Michon");
		m->declare("reverbs.lib/version", "1.4.0");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "1.6.0");
	}

	virtual int getNumInputs() {
		return 0;
	}
	virtual int getNumOutputs() {
		return 2;
	}
	
	static void classInit(int sample_rate) {
		BubbleDspSIG0* sig0 = newBubbleDspSIG0();
		sig0->instanceInitBubbleDspSIG0(sample_rate);
		sig0->fillBubbleDspSIG0(65536, ftbl0BubbleDspSIG0);
		deleteBubbleDspSIG0(sig0);
	}
	
	virtual void instanceConstants(int sample_rate) {
		fSampleRate = sample_rate;
		fConst0 = std::min<float>(1.92e+05f, std::max<float>(1.0f, static_cast<float>(fSampleRate)));
		fConst1 = 0.075f / fConst0;
		fConst2 = 1.0f / fConst0;
		fConst3 = 1.764e+04f / fConst0;
		fConst4 = 12348.0f / fConst0;
		iConst5 = static_cast<int>(0.025306122f * fConst0);
		iConst6 = std::max<int>(0, iConst5 + -1);
		iConst7 = static_cast<int>(0.026938776f * fConst0);
		iConst8 = std::max<int>(0, iConst7 + -1);
		iConst9 = static_cast<int>(0.028956916f * fConst0);
		iConst10 = std::max<int>(0, iConst9 + -1);
		iConst11 = static_cast<int>(0.030748298f * fConst0);
		iConst12 = std::max<int>(0, iConst11 + -1);
		iConst13 = static_cast<int>(0.0322449f * fConst0);
		iConst14 = std::max<int>(0, iConst13 + -1);
		iConst15 = static_cast<int>(0.033809524f * fConst0);
		iConst16 = std::max<int>(0, iConst15 + -1);
		iConst17 = static_cast<int>(0.035306122f * fConst0);
		iConst18 = std::max<int>(0, iConst17 + -1);
		iConst19 = static_cast<int>(0.036666665f * fConst0);
		iConst20 = std::max<int>(0, iConst19 + -1);
		iConst21 = static_cast<int>(0.0126077095f * fConst0);
		iConst22 = std::min<int>(1024, std::max<int>(0, iConst21 + -1));
		iConst23 = static_cast<int>(0.01f * fConst0);
		iConst24 = std::min<int>(1024, std::max<int>(0, iConst23 + -1));
		iConst25 = static_cast<int>(0.0077324263f * fConst0);
		iConst26 = std::min<int>(1024, std::max<int>(0, iConst25 + -1));
		iConst27 = static_cast<int>(0.0051020407f * fConst0);
		iConst28 = std::min<int>(1024, std::max<int>(0, iConst27 + -1));
		fConst29 = 0.0010430838f * fConst0;
	}
	
	virtual void instanceResetUserInterface() {
		fButton0 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider0 = static_cast<FAUSTFLOAT>(6e+02f);
		fVslider0 = static_cast<FAUSTFLOAT>(0.3333f);
		fVslider1 = static_cast<FAUSTFLOAT>(0.5f);
		fVslider2 = static_cast<FAUSTFLOAT>(0.5f);
		fVslider3 = static_cast<FAUSTFLOAT>(0.5f);
	}
	
	virtual void instanceClear() {
		for (int l2 = 0; l2 < 2; l2 = l2 + 1) {
			iVec1[l2] = 0;
		}
		for (int l3 = 0; l3 < 2; l3 = l3 + 1) {
			fVec2[l3] = 0.0f;
		}
		for (int l4 = 0; l4 < 2; l4 = l4 + 1) {
			iRec2[l4] = 0;
		}
		for (int l5 = 0; l5 < 2; l5 = l5 + 1) {
			fRec1[l5] = 0.0f;
		}
		for (int l6 = 0; l6 < 2; l6 = l6 + 1) {
			fRec3[l6] = 0.0f;
		}
		for (int l7 = 0; l7 < 2; l7 = l7 + 1) {
			fRec13[l7] = 0.0f;
		}
		IOTA0 = 0;
		for (int l8 = 0; l8 < 8192; l8 = l8 + 1) {
			fVec3[l8] = 0.0f;
		}
		for (int l9 = 0; l9 < 2; l9 = l9 + 1) {
			fRec12[l9] = 0.0f;
		}
		for (int l10 = 0; l10 < 2; l10 = l10 + 1) {
			fRec15[l10] = 0.0f;
		}
		for (int l11 = 0; l11 < 8192; l11 = l11 + 1) {
			fVec4[l11] = 0.0f;
		}
		for (int l12 = 0; l12 < 2; l12 = l12 + 1) {
			fRec14[l12] = 0.0f;
		}
		for (int l13 = 0; l13 < 2; l13 = l13 + 1) {
			fRec17[l13] = 0.0f;
		}
		for (int l14 = 0; l14 < 8192; l14 = l14 + 1) {
			fVec5[l14] = 0.0f;
		}
		for (int l15 = 0; l15 < 2; l15 = l15 + 1) {
			fRec16[l15] = 0.0f;
		}
		for (int l16 = 0; l16 < 2; l16 = l16 + 1) {
			fRec19[l16] = 0.0f;
		}
		for (int l17 = 0; l17 < 8192; l17 = l17 + 1) {
			fVec6[l17] = 0.0f;
		}
		for (int l18 = 0; l18 < 2; l18 = l18 + 1) {
			fRec18[l18] = 0.0f;
		}
		for (int l19 = 0; l19 < 2; l19 = l19 + 1) {
			fRec21[l19] = 0.0f;
		}
		for (int l20 = 0; l20 < 8192; l20 = l20 + 1) {
			fVec7[l20] = 0.0f;
		}
		for (int l21 = 0; l21 < 2; l21 = l21 + 1) {
			fRec20[l21] = 0.0f;
		}
		for (int l22 = 0; l22 < 2; l22 = l22 + 1) {
			fRec23[l22] = 0.0f;
		}
		for (int l23 = 0; l23 < 8192; l23 = l23 + 1) {
			fVec8[l23] = 0.0f;
		}
		for (int l24 = 0; l24 < 2; l24 = l24 + 1) {
			fRec22[l24] = 0.0f;
		}
		for (int l25 = 0; l25 < 2; l25 = l25 + 1) {
			fRec25[l25] = 0.0f;
		}
		for (int l26 = 0; l26 < 8192; l26 = l26 + 1) {
			fVec9[l26] = 0.0f;
		}
		for (int l27 = 0; l27 < 2; l27 = l27 + 1) {
			fRec24[l27] = 0.0f;
		}
		for (int l28 = 0; l28 < 2; l28 = l28 + 1) {
			fRec27[l28] = 0.0f;
		}
		for (int l29 = 0; l29 < 8192; l29 = l29 + 1) {
			fVec10[l29] = 0.0f;
		}
		for (int l30 = 0; l30 < 2; l30 = l30 + 1) {
			fRec26[l30] = 0.0f;
		}
		for (int l31 = 0; l31 < 2048; l31 = l31 + 1) {
			fVec11[l31] = 0.0f;
		}
		for (int l32 = 0; l32 < 2; l32 = l32 + 1) {
			fRec10[l32] = 0.0f;
		}
		for (int l33 = 0; l33 < 2048; l33 = l33 + 1) {
			fVec12[l33] = 0.0f;
		}
		for (int l34 = 0; l34 < 2; l34 = l34 + 1) {
			fRec8[l34] = 0.0f;
		}
		for (int l35 = 0; l35 < 2048; l35 = l35 + 1) {
			fVec13[l35] = 0.0f;
		}
		for (int l36 = 0; l36 < 2; l36 = l36 + 1) {
			fRec6[l36] = 0.0f;
		}
		for (int l37 = 0; l37 < 1024; l37 = l37 + 1) {
			fVec14[l37] = 0.0f;
		}
		for (int l38 = 0; l38 < 2; l38 = l38 + 1) {
			fRec4[l38] = 0.0f;
		}
		for (int l39 = 0; l39 < 2; l39 = l39 + 1) {
			fRec37[l39] = 0.0f;
		}
		for (int l40 = 0; l40 < 8192; l40 = l40 + 1) {
			fVec15[l40] = 0.0f;
		}
		for (int l41 = 0; l41 < 2; l41 = l41 + 1) {
			fRec36[l41] = 0.0f;
		}
		for (int l42 = 0; l42 < 2; l42 = l42 + 1) {
			fRec39[l42] = 0.0f;
		}
		for (int l43 = 0; l43 < 8192; l43 = l43 + 1) {
			fVec16[l43] = 0.0f;
		}
		for (int l44 = 0; l44 < 2; l44 = l44 + 1) {
			fRec38[l44] = 0.0f;
		}
		for (int l45 = 0; l45 < 2; l45 = l45 + 1) {
			fRec41[l45] = 0.0f;
		}
		for (int l46 = 0; l46 < 8192; l46 = l46 + 1) {
			fVec17[l46] = 0.0f;
		}
		for (int l47 = 0; l47 < 2; l47 = l47 + 1) {
			fRec40[l47] = 0.0f;
		}
		for (int l48 = 0; l48 < 2; l48 = l48 + 1) {
			fRec43[l48] = 0.0f;
		}
		for (int l49 = 0; l49 < 8192; l49 = l49 + 1) {
			fVec18[l49] = 0.0f;
		}
		for (int l50 = 0; l50 < 2; l50 = l50 + 1) {
			fRec42[l50] = 0.0f;
		}
		for (int l51 = 0; l51 < 2; l51 = l51 + 1) {
			fRec45[l51] = 0.0f;
		}
		for (int l52 = 0; l52 < 8192; l52 = l52 + 1) {
			fVec19[l52] = 0.0f;
		}
		for (int l53 = 0; l53 < 2; l53 = l53 + 1) {
			fRec44[l53] = 0.0f;
		}
		for (int l54 = 0; l54 < 2; l54 = l54 + 1) {
			fRec47[l54] = 0.0f;
		}
		for (int l55 = 0; l55 < 8192; l55 = l55 + 1) {
			fVec20[l55] = 0.0f;
		}
		for (int l56 = 0; l56 < 2; l56 = l56 + 1) {
			fRec46[l56] = 0.0f;
		}
		for (int l57 = 0; l57 < 2; l57 = l57 + 1) {
			fRec49[l57] = 0.0f;
		}
		for (int l58 = 0; l58 < 8192; l58 = l58 + 1) {
			fVec21[l58] = 0.0f;
		}
		for (int l59 = 0; l59 < 2; l59 = l59 + 1) {
			fRec48[l59] = 0.0f;
		}
		for (int l60 = 0; l60 < 2; l60 = l60 + 1) {
			fRec51[l60] = 0.0f;
		}
		for (int l61 = 0; l61 < 8192; l61 = l61 + 1) {
			fVec22[l61] = 0.0f;
		}
		for (int l62 = 0; l62 < 2; l62 = l62 + 1) {
			fRec50[l62] = 0.0f;
		}
		for (int l63 = 0; l63 < 2048; l63 = l63 + 1) {
			fVec23[l63] = 0.0f;
		}
		for (int l64 = 0; l64 < 2; l64 = l64 + 1) {
			fRec34[l64] = 0.0f;
		}
		for (int l65 = 0; l65 < 2048; l65 = l65 + 1) {
			fVec24[l65] = 0.0f;
		}
		for (int l66 = 0; l66 < 2; l66 = l66 + 1) {
			fRec32[l66] = 0.0f;
		}
		for (int l67 = 0; l67 < 2048; l67 = l67 + 1) {
			fVec25[l67] = 0.0f;
		}
		for (int l68 = 0; l68 < 2; l68 = l68 + 1) {
			fRec30[l68] = 0.0f;
		}
		for (int l69 = 0; l69 < 2048; l69 = l69 + 1) {
			fVec26[l69] = 0.0f;
		}
		for (int l70 = 0; l70 < 2; l70 = l70 + 1) {
			fRec28[l70] = 0.0f;
		}
	}
	
	virtual void init(int sample_rate) {
		classInit(sample_rate);
		instanceInit(sample_rate);
	}
	
	virtual void instanceInit(int sample_rate) {
		instanceConstants(sample_rate);
		instanceResetUserInterface();
		instanceClear();
	}
	
	virtual BubbleDsp* clone() {
		return new BubbleDsp();
	}
	
	virtual int getSampleRate() {
		return fSampleRate;
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("bubble");
		ui_interface->openHorizontalBox("Freeverb");
		ui_interface->declare(0, "0", "");
		ui_interface->openVerticalBox("0x00");
		ui_interface->declare(&fVslider1, "0", "");
		ui_interface->declare(&fVslider1, "style", "knob");
		ui_interface->declare(&fVslider1, "tooltip", "Somehow control the         density of the reverb.");
		ui_interface->addVerticalSlider("Damp", &fVslider1, FAUSTFLOAT(0.5f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.025f));
		ui_interface->declare(&fVslider2, "1", "");
		ui_interface->declare(&fVslider2, "style", "knob");
		ui_interface->declare(&fVslider2, "tooltip", "The room size         between 0 and 1 with 1 for the largest room.");
		ui_interface->addVerticalSlider("RoomSize", &fVslider2, FAUSTFLOAT(0.5f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.025f));
		ui_interface->declare(&fVslider3, "2", "");
		ui_interface->declare(&fVslider3, "style", "knob");
		ui_interface->declare(&fVslider3, "tooltip", "Spatial         spread between 0 and 1 with 1 for maximum spread.");
		ui_interface->addVerticalSlider("Stereo Spread", &fVslider3, FAUSTFLOAT(0.5f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.01f));
		ui_interface->closeBox();
		ui_interface->declare(&fVslider0, "1", "");
		ui_interface->declare(&fVslider0, "tooltip", "The amount of reverb applied to the signal         between 0 and 1 with 1 for the maximum amount of reverb.");
		ui_interface->addVerticalSlider("Wet", &fVslider0, FAUSTFLOAT(0.3333f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.025f));
		ui_interface->closeBox();
		ui_interface->openVerticalBox("bubble");
		ui_interface->addHorizontalSlider("freq", &fHslider0, FAUSTFLOAT(6e+02f), FAUSTFLOAT(1.5e+02f), FAUSTFLOAT(2e+03f), FAUSTFLOAT(1.0f));
		ui_interface->closeBox();
		ui_interface->addButton("drop", &fButton0);
		ui_interface->closeBox();
	}
	
	virtual void compute(int count, FAUSTFLOAT** RESTRICT inputs, FAUSTFLOAT** RESTRICT outputs) {
		FAUSTFLOAT* output0 = outputs[0];
		FAUSTFLOAT* output1 = outputs[1];
		float fSlow0 = static_cast<float>(fButton0);
		float fSlow1 = static_cast<float>(fHslider0);
		float fSlow2 = 0.0014f * std::pow(fSlow1, 1.5f) + 0.043f * fSlow1;
		float fSlow3 = fConst1 * fSlow2;
		float fSlow4 = fConst2 * fSlow1;
		float fSlow5 = fConst2 * fSlow2;
		float fSlow6 = static_cast<float>(fVslider0);
		float fSlow7 = 1.0f - fSlow6;
		float fSlow8 = 0.2f * fSlow6;
		float fSlow9 = fConst3 * static_cast<float>(fVslider1);
		float fSlow10 = 1.0f - fSlow9;
		float fSlow11 = fConst4 * static_cast<float>(fVslider2) + 0.7f;
		int iSlow12 = static_cast<int>(fConst29 * static_cast<float>(fVslider3)) + -1;
		int iSlow13 = std::max<int>(0, iConst5 + iSlow12);
		int iSlow14 = std::max<int>(0, iConst7 + iSlow12);
		int iSlow15 = std::max<int>(0, iConst9 + iSlow12);
		int iSlow16 = std::max<int>(0, iConst11 + iSlow12);
		int iSlow17 = std::max<int>(0, iConst13 + iSlow12);
		int iSlow18 = std::max<int>(0, iConst15 + iSlow12);
		int iSlow19 = std::max<int>(0, iConst17 + iSlow12);
		int iSlow20 = std::max<int>(0, iConst19 + iSlow12);
		int iSlow21 = std::min<int>(1024, std::max<int>(0, iConst21 + iSlow12));
		int iSlow22 = std::min<int>(1024, std::max<int>(0, iConst23 + iSlow12));
		int iSlow23 = std::min<int>(1024, std::max<int>(0, iConst25 + iSlow12));
		int iSlow24 = std::min<int>(1024, std::max<int>(0, iConst27 + iSlow12));
		for (int i0 = 0; i0 < count; i0 = i0 + 1) {
			iVec1[0] = 1;
			fVec2[0] = fSlow0;
			iRec2[0] = ((fSlow0 > fVec2[1]) ? 0 : iRec2[1]) + 1;
			float fTemp0 = static_cast<float>(iRec2[0]);
			float fTemp1 = ((1 - iVec1[1]) ? 0.0f : fRec1[1] + fSlow4 * (fSlow3 * fTemp0 + 1.0f));
			fRec1[0] = fTemp1 - std::floor(fTemp1);
			fRec3[0] = 0.01f * std::exp(-(fSlow5 * fTemp0)) + 0.99f * fRec3[1];
			float fTemp2 = fRec3[0] * ftbl0BubbleDspSIG0[std::max<int>(0, std::min<int>(static_cast<int>(65536.0f * fRec1[0]), 65535))];
			float fTemp3 = fSlow7 * fTemp2;
			float fTemp4 = fSlow8 * fTemp2;
			fRec13[0] = fSlow9 * fRec13[1] + fSlow10 * fRec12[1];
			fVec3[IOTA0 & 8191] = fSlow11 * fRec13[0] + fTemp4;
			fRec12[0] = fVec3[(IOTA0 - iConst6) & 8191];
			fRec15[0] = fSlow9 * fRec15[1] + fSlow10 * fRec14[1];
			fVec4[IOTA0 & 8191] = fTemp4 + fSlow11 * fRec15[0];
			fRec14[0] = fVec4[(IOTA0 - iConst8) & 8191];
			fRec17[0] = fSlow9 * fRec17[1] + fSlow10 * fRec16[1];
			fVec5[IOTA0 & 8191] = fTemp4 + fSlow11 * fRec17[0];
			fRec16[0] = fVec5[(IOTA0 - iConst10) & 8191];
			fRec19[0] = fSlow9 * fRec19[1] + fSlow10 * fRec18[1];
			fVec6[IOTA0 & 8191] = fTemp4 + fSlow11 * fRec19[0];
			fRec18[0] = fVec6[(IOTA0 - iConst12) & 8191];
			fRec21[0] = fSlow9 * fRec21[1] + fSlow10 * fRec20[1];
			fVec7[IOTA0 & 8191] = fTemp4 + fSlow11 * fRec21[0];
			fRec20[0] = fVec7[(IOTA0 - iConst14) & 8191];
			fRec23[0] = fSlow9 * fRec23[1] + fSlow10 * fRec22[1];
			fVec8[IOTA0 & 8191] = fTemp4 + fSlow11 * fRec23[0];
			fRec22[0] = fVec8[(IOTA0 - iConst16) & 8191];
			fRec25[0] = fSlow9 * fRec25[1] + fSlow10 * fRec24[1];
			fVec9[IOTA0 & 8191] = fTemp4 + fSlow11 * fRec25[0];
			fRec24[0] = fVec9[(IOTA0 - iConst18) & 8191];
			fRec27[0] = fSlow9 * fRec27[1] + fSlow10 * fRec26[1];
			fVec10[IOTA0 & 8191] = fTemp4 + fSlow11 * fRec27[0];
			fRec26[0] = fVec10[(IOTA0 - iConst20) & 8191];
			float fTemp5 = fRec26[1] + fRec24[1] + fRec22[1] + fRec20[1] + fRec18[1] + fRec16[1] + fRec14[1] + 0.5f * fRec10[1] + fRec12[1];
			fVec11[IOTA0 & 2047] = fTemp5;
			fRec10[0] = fVec11[(IOTA0 - iConst22) & 2047];
			float fRec11 = -(0.5f * fTemp5);
			float fTemp6 = fRec10[1] + fRec11 + 0.5f * fRec8[1];
			fVec12[IOTA0 & 2047] = fTemp6;
			fRec8[0] = fVec12[(IOTA0 - iConst24) & 2047];
			float fRec9 = -(0.5f * fTemp6);
			float fTemp7 = fRec8[1] + fRec9 + 0.5f * fRec6[1];
			fVec13[IOTA0 & 2047] = fTemp7;
			fRec6[0] = fVec13[(IOTA0 - iConst26) & 2047];
			float fRec7 = -(0.5f * fTemp7);
			float fTemp8 = fRec6[1] + fRec7 + 0.5f * fRec4[1];
			fVec14[IOTA0 & 1023] = fTemp8;
			fRec4[0] = fVec14[(IOTA0 - iConst28) & 1023];
			float fRec5 = -(0.5f * fTemp8);
			output0[i0] = static_cast<FAUSTFLOAT>(fRec5 + fRec4[1] + fTemp3);
			fRec37[0] = fSlow9 * fRec37[1] + fSlow10 * fRec36[1];
			fVec15[IOTA0 & 8191] = fTemp4 + fSlow11 * fRec37[0];
			fRec36[0] = fVec15[(IOTA0 - iSlow13) & 8191];
			fRec39[0] = fSlow9 * fRec39[1] + fSlow10 * fRec38[1];
			fVec16[IOTA0 & 8191] = fTemp4 + fSlow11 * fRec39[0];
			fRec38[0] = fVec16[(IOTA0 - iSlow14) & 8191];
			fRec41[0] = fSlow9 * fRec41[1] + fSlow10 * fRec40[1];
			fVec17[IOTA0 & 8191] = fTemp4 + fSlow11 * fRec41[0];
			fRec40[0] = fVec17[(IOTA0 - iSlow15) & 8191];
			fRec43[0] = fSlow9 * fRec43[1] + fSlow10 * fRec42[1];
			fVec18[IOTA0 & 8191] = fTemp4 + fSlow11 * fRec43[0];
			fRec42[0] = fVec18[(IOTA0 - iSlow16) & 8191];
			fRec45[0] = fSlow9 * fRec45[1] + fSlow10 * fRec44[1];
			fVec19[IOTA0 & 8191] = fTemp4 + fSlow11 * fRec45[0];
			fRec44[0] = fVec19[(IOTA0 - iSlow17) & 8191];
			fRec47[0] = fSlow9 * fRec47[1] + fSlow10 * fRec46[1];
			fVec20[IOTA0 & 8191] = fTemp4 + fSlow11 * fRec47[0];
			fRec46[0] = fVec20[(IOTA0 - iSlow18) & 8191];
			fRec49[0] = fSlow9 * fRec49[1] + fSlow10 * fRec48[1];
			fVec21[IOTA0 & 8191] = fTemp4 + fSlow11 * fRec49[0];
			fRec48[0] = fVec21[(IOTA0 - iSlow19) & 8191];
			fRec51[0] = fSlow9 * fRec51[1] + fSlow10 * fRec50[1];
			fVec22[IOTA0 & 8191] = fTemp4 + fSlow11 * fRec51[0];
			fRec50[0] = fVec22[(IOTA0 - iSlow20) & 8191];
			float fTemp9 = fRec50[1] + fRec48[1] + fRec46[1] + fRec44[1] + fRec42[1] + fRec40[1] + fRec38[1] + 0.5f * fRec34[1] + fRec36[1];
			fVec23[IOTA0 & 2047] = fTemp9;
			fRec34[0] = fVec23[(IOTA0 - iSlow21) & 2047];
			float fRec35 = -(0.5f * fTemp9);
			float fTemp10 = fRec34[1] + fRec35 + 0.5f * fRec32[1];
			fVec24[IOTA0 & 2047] = fTemp10;
			fRec32[0] = fVec24[(IOTA0 - iSlow22) & 2047];
			float fRec33 = -(0.5f * fTemp10);
			float fTemp11 = fRec32[1] + fRec33 + 0.5f * fRec30[1];
			fVec25[IOTA0 & 2047] = fTemp11;
			fRec30[0] = fVec25[(IOTA0 - iSlow23) & 2047];
			float fRec31 = -(0.5f * fTemp11);
			float fTemp12 = fRec30[1] + fRec31 + 0.5f * fRec28[1];
			fVec26[IOTA0 & 2047] = fTemp12;
			fRec28[0] = fVec26[(IOTA0 - iSlow24) & 2047];
			float fRec29 = -(0.5f * fTemp12);
			output1[i0] = static_cast<FAUSTFLOAT>(fRec28[1] + fRec29 + fTemp3);
			iVec1[1] = iVec1[0];
			fVec2[1] = fVec2[0];
			iRec2[1] = iRec2[0];
			fRec1[1] = fRec1[0];
			fRec3[1] = fRec3[0];
			fRec13[1] = fRec13[0];
			IOTA0 = IOTA0 + 1;
			fRec12[1] = fRec12[0];
			fRec15[1] = fRec15[0];
			fRec14[1] = fRec14[0];
			fRec17[1] = fRec17[0];
			fRec16[1] = fRec16[0];
			fRec19[1] = fRec19[0];
			fRec18[1] = fRec18[0];
			fRec21[1] = fRec21[0];
			fRec20[1] = fRec20[0];
			fRec23[1] = fRec23[0];
			fRec22[1] = fRec22[0];
			fRec25[1] = fRec25[0];
			fRec24[1] = fRec24[0];
			fRec27[1] = fRec27[0];
			fRec26[1] = fRec26[0];
			fRec10[1] = fRec10[0];
			fRec8[1] = fRec8[0];
			fRec6[1] = fRec6[0];
			fRec4[1] = fRec4[0];
			fRec37[1] = fRec37[0];
			fRec36[1] = fRec36[0];
			fRec39[1] = fRec39[0];
			fRec38[1] = fRec38[0];
			fRec41[1] = fRec41[0];
			fRec40[1] = fRec40[0];
			fRec43[1] = fRec43[0];
			fRec42[1] = fRec42[0];
			fRec45[1] = fRec45[0];
			fRec44[1] = fRec44[0];
			fRec47[1] = fRec47[0];
			fRec46[1] = fRec46[0];
			fRec49[1] = fRec49[0];
			fRec48[1] = fRec48[0];
			fRec51[1] = fRec51[0];
			fRec50[1] = fRec50[0];
			fRec34[1] = fRec34[0];
			fRec32[1] = fRec32[0];
			fRec30[1] = fRec30[0];
			fRec28[1] = fRec28[0];
		}
	}

};

#endif
