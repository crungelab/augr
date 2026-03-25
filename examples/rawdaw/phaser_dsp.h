/* ------------------------------------------------------------
author: "JOS, revised by RM"
name: "phaser"
version: "0.0"
Code generated with Faust 2.81.10 (https://faust.grame.fr)
Compilation options: -lang cpp -ct 1 -cn PhaserDsp -scn augr::FaustDsp -es 1 -mcd 16 -mdd 1024 -mdy 33 -single -ftz 0
------------------------------------------------------------ */

#ifndef  __PhaserDsp_H__
#define  __PhaserDsp_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif 

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <math.h>

#ifndef FAUSTCLASS 
#define FAUSTCLASS PhaserDsp
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

static float PhaserDsp_faustpower4_f(float value) {
	return value * value * value * value;
}
static float PhaserDsp_faustpower3_f(float value) {
	return value * value * value;
}
static float PhaserDsp_faustpower2_f(float value) {
	return value * value;
}

class PhaserDsp : public augr::FaustDsp {
	
 private:
	
	FAUSTFLOAT fCheckbox0;
	FAUSTFLOAT fCheckbox1;
	FAUSTFLOAT fCheckbox2;
	FAUSTFLOAT fHslider0;
	int iVec0[2];
	FAUSTFLOAT fHslider1;
	int fSampleRate;
	float fConst0;
	float fConst1;
	float fRec1[2];
	float fRec2[2];
	FAUSTFLOAT fHslider2;
	FAUSTFLOAT fHslider3;
	FAUSTFLOAT fHslider4;
	float fConst2;
	FAUSTFLOAT fHslider5;
	float fConst3;
	FAUSTFLOAT fHslider6;
	FAUSTFLOAT fHslider7;
	float fRec6[3];
	float fRec5[3];
	float fRec4[3];
	float fRec3[3];
	float fRec0[2];
	float fRec11[3];
	float fRec10[3];
	float fRec9[3];
	float fRec8[3];
	float fRec7[2];
	
 public:
	PhaserDsp() {
	}
	
	void metadata(Meta* m) { 
		m->declare("author", "JOS, revised by RM");
		m->declare("basics.lib/bypass2:author", "Julius Smith");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "1.22.0");
		m->declare("compile_options", "-lang cpp -ct 1 -cn PhaserDsp -scn augr::FaustDsp -es 1 -mcd 16 -mdd 1024 -mdy 33 -single -ftz 0");
		m->declare("demos.lib/name", "Faust Demos Library");
		m->declare("demos.lib/phaser2_demo:author", "Julius O. Smith III");
		m->declare("demos.lib/phaser2_demo:licence", "MIT");
		m->declare("demos.lib/version", "1.2.0");
		m->declare("description", "Phaser demo application.");
		m->declare("filename", "phaser.dsp");
		m->declare("filters.lib/fir:author", "Julius O. Smith III");
		m->declare("filters.lib/fir:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/fir:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/iir:author", "Julius O. Smith III");
		m->declare("filters.lib/iir:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/iir:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/lowpass0_highpass1", "MIT-style STK-4.3 license");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/nlf2:author", "Julius O. Smith III");
		m->declare("filters.lib/nlf2:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/nlf2:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/tf2:author", "Julius O. Smith III");
		m->declare("filters.lib/tf2:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf2:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/version", "1.7.1");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.9.0");
		m->declare("name", "phaser");
		m->declare("oscillators.lib/name", "Faust Oscillator Library");
		m->declare("oscillators.lib/version", "1.6.0");
		m->declare("phaflangers.lib/name", "Faust Phaser and Flanger Library");
		m->declare("phaflangers.lib/version", "1.1.0");
		m->declare("platform.lib/name", "Generic Platform Library");
		m->declare("platform.lib/version", "1.3.0");
		m->declare("routes.lib/name", "Faust Signal Routing Library");
		m->declare("routes.lib/version", "1.2.0");
		m->declare("version", "0.0");
	}

	virtual int getNumInputs() {
		return 2;
	}
	virtual int getNumOutputs() {
		return 2;
	}
	
	static void classInit(int sample_rate) {
	}
	
	virtual void instanceConstants(int sample_rate) {
		fSampleRate = sample_rate;
		fConst0 = std::min<float>(1.92e+05f, std::max<float>(1.0f, static_cast<float>(fSampleRate)));
		fConst1 = 6.2831855f / fConst0;
		fConst2 = 1.0f / fConst0;
		fConst3 = 3.1415927f / fConst0;
	}
	
	virtual void instanceResetUserInterface() {
		fCheckbox0 = static_cast<FAUSTFLOAT>(0.0f);
		fCheckbox1 = static_cast<FAUSTFLOAT>(0.0f);
		fCheckbox2 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider0 = static_cast<FAUSTFLOAT>(1.0f);
		fHslider1 = static_cast<FAUSTFLOAT>(0.5f);
		fHslider2 = static_cast<FAUSTFLOAT>(1e+02f);
		fHslider3 = static_cast<FAUSTFLOAT>(8e+02f);
		fHslider4 = static_cast<FAUSTFLOAT>(1.5f);
		fHslider5 = static_cast<FAUSTFLOAT>(1e+03f);
		fHslider6 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider7 = static_cast<FAUSTFLOAT>(0.0f);
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; l0 < 2; l0 = l0 + 1) {
			iVec0[l0] = 0;
		}
		for (int l1 = 0; l1 < 2; l1 = l1 + 1) {
			fRec1[l1] = 0.0f;
		}
		for (int l2 = 0; l2 < 2; l2 = l2 + 1) {
			fRec2[l2] = 0.0f;
		}
		for (int l3 = 0; l3 < 3; l3 = l3 + 1) {
			fRec6[l3] = 0.0f;
		}
		for (int l4 = 0; l4 < 3; l4 = l4 + 1) {
			fRec5[l4] = 0.0f;
		}
		for (int l5 = 0; l5 < 3; l5 = l5 + 1) {
			fRec4[l5] = 0.0f;
		}
		for (int l6 = 0; l6 < 3; l6 = l6 + 1) {
			fRec3[l6] = 0.0f;
		}
		for (int l7 = 0; l7 < 2; l7 = l7 + 1) {
			fRec0[l7] = 0.0f;
		}
		for (int l8 = 0; l8 < 3; l8 = l8 + 1) {
			fRec11[l8] = 0.0f;
		}
		for (int l9 = 0; l9 < 3; l9 = l9 + 1) {
			fRec10[l9] = 0.0f;
		}
		for (int l10 = 0; l10 < 3; l10 = l10 + 1) {
			fRec9[l10] = 0.0f;
		}
		for (int l11 = 0; l11 < 3; l11 = l11 + 1) {
			fRec8[l11] = 0.0f;
		}
		for (int l12 = 0; l12 < 2; l12 = l12 + 1) {
			fRec7[l12] = 0.0f;
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
	
	virtual PhaserDsp* clone() {
		return new PhaserDsp();
	}
	
	virtual int getSampleRate() {
		return fSampleRate;
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->declare(0, "tooltip", "Reference:         https://ccrma.stanford.edu/~jos/pasp/Flanging.html");
		ui_interface->openVerticalBox("PHASER2");
		ui_interface->declare(0, "0", "");
		ui_interface->openHorizontalBox("0x00");
		ui_interface->declare(&fCheckbox0, "0", "");
		ui_interface->declare(&fCheckbox0, "tooltip", "When this is checked, the phaser         has no effect");
		ui_interface->addCheckButton("Bypass", &fCheckbox0);
		ui_interface->declare(&fCheckbox1, "1", "");
		ui_interface->addCheckButton("Invert Internal Phaser Sum", &fCheckbox1);
		ui_interface->declare(&fCheckbox2, "2", "");
		ui_interface->addCheckButton("Vibrato Mode", &fCheckbox2);
		ui_interface->closeBox();
		ui_interface->declare(0, "1", "");
		ui_interface->openHorizontalBox("0x00");
		ui_interface->declare(&fHslider1, "1", "");
		ui_interface->declare(&fHslider1, "style", "knob");
		ui_interface->declare(&fHslider1, "unit", "Hz");
		ui_interface->addHorizontalSlider("Speed", &fHslider1, FAUSTFLOAT(0.5f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1e+01f), FAUSTFLOAT(0.001f));
		ui_interface->declare(&fHslider0, "2", "");
		ui_interface->declare(&fHslider0, "style", "knob");
		ui_interface->addHorizontalSlider("Notch Depth (Intensity)", &fHslider0, FAUSTFLOAT(1.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.001f));
		ui_interface->declare(&fHslider6, "3", "");
		ui_interface->declare(&fHslider6, "style", "knob");
		ui_interface->addHorizontalSlider("Feedback Gain", &fHslider6, FAUSTFLOAT(0.0f), FAUSTFLOAT(-0.999f), FAUSTFLOAT(0.999f), FAUSTFLOAT(0.001f));
		ui_interface->closeBox();
		ui_interface->declare(0, "2", "");
		ui_interface->openHorizontalBox("0x00");
		ui_interface->declare(&fHslider5, "1", "");
		ui_interface->declare(&fHslider5, "scale", "log");
		ui_interface->declare(&fHslider5, "style", "knob");
		ui_interface->declare(&fHslider5, "unit", "Hz");
		ui_interface->addHorizontalSlider("Notch width", &fHslider5, FAUSTFLOAT(1e+03f), FAUSTFLOAT(1e+01f), FAUSTFLOAT(5e+03f), FAUSTFLOAT(1.0f));
		ui_interface->declare(&fHslider2, "2", "");
		ui_interface->declare(&fHslider2, "scale", "log");
		ui_interface->declare(&fHslider2, "style", "knob");
		ui_interface->declare(&fHslider2, "unit", "Hz");
		ui_interface->addHorizontalSlider("Min Notch1 Freq", &fHslider2, FAUSTFLOAT(1e+02f), FAUSTFLOAT(2e+01f), FAUSTFLOAT(5e+03f), FAUSTFLOAT(1.0f));
		ui_interface->declare(&fHslider3, "3", "");
		ui_interface->declare(&fHslider3, "scale", "log");
		ui_interface->declare(&fHslider3, "style", "knob");
		ui_interface->declare(&fHslider3, "unit", "Hz");
		ui_interface->addHorizontalSlider("Max Notch1 Freq", &fHslider3, FAUSTFLOAT(8e+02f), FAUSTFLOAT(2e+01f), FAUSTFLOAT(1e+04f), FAUSTFLOAT(1.0f));
		ui_interface->declare(&fHslider4, "4", "");
		ui_interface->declare(&fHslider4, "style", "knob");
		ui_interface->addHorizontalSlider("Notch Freq Ratio: NotchFreq(n+1)/NotchFreq(n)", &fHslider4, FAUSTFLOAT(1.5f), FAUSTFLOAT(1.1f), FAUSTFLOAT(4.0f), FAUSTFLOAT(0.001f));
		ui_interface->closeBox();
		ui_interface->declare(0, "3", "");
		ui_interface->openHorizontalBox("0x00");
		ui_interface->declare(&fHslider7, "unit", "dB");
		ui_interface->addHorizontalSlider("Phaser Output Level", &fHslider7, FAUSTFLOAT(0.0f), FAUSTFLOAT(-6e+01f), FAUSTFLOAT(1e+01f), FAUSTFLOAT(0.1f));
		ui_interface->closeBox();
		ui_interface->closeBox();
	}
	
	virtual void compute(int count, FAUSTFLOAT** RESTRICT inputs, FAUSTFLOAT** RESTRICT outputs) {
		FAUSTFLOAT* input0 = inputs[0];
		FAUSTFLOAT* input1 = inputs[1];
		FAUSTFLOAT* output0 = outputs[0];
		FAUSTFLOAT* output1 = outputs[1];
		int iSlow0 = static_cast<int>(static_cast<float>(fCheckbox0));
		float fSlow1 = 0.5f * ((static_cast<int>(static_cast<float>(fCheckbox2))) ? 2.0f : static_cast<float>(fHslider0));
		float fSlow2 = ((static_cast<int>(static_cast<float>(fCheckbox1))) ? -fSlow1 : fSlow1);
		float fSlow3 = fConst1 * static_cast<float>(fHslider1);
		float fSlow4 = std::cos(fSlow3);
		float fSlow5 = std::sin(fSlow3);
		float fSlow6 = static_cast<float>(fHslider2);
		float fSlow7 = 3.1415927f * (fSlow6 - std::max<float>(fSlow6, static_cast<float>(fHslider3)));
		float fSlow8 = 6.2831855f * fSlow6;
		float fSlow9 = static_cast<float>(fHslider4);
		float fSlow10 = fConst2 * PhaserDsp_faustpower4_f(fSlow9);
		float fSlow11 = fConst2 * PhaserDsp_faustpower3_f(fSlow9);
		float fSlow12 = fConst2 * PhaserDsp_faustpower2_f(fSlow9);
		float fSlow13 = fConst2 * fSlow9;
		float fSlow14 = std::exp(-(fConst3 * static_cast<float>(fHslider5)));
		float fSlow15 = PhaserDsp_faustpower2_f(fSlow14);
		float fSlow16 = 2.0f * fSlow14;
		float fSlow17 = static_cast<float>(fHslider6);
		float fSlow18 = std::pow(1e+01f, 0.05f * static_cast<float>(fHslider7));
		float fSlow19 = 1.0f - fSlow1;
		for (int i0 = 0; i0 < count; i0 = i0 + 1) {
			iVec0[0] = 1;
			fRec1[0] = fSlow5 * fRec2[1] + fSlow4 * fRec1[1];
			fRec2[0] = static_cast<float>(1 - iVec0[1]) + fSlow4 * fRec2[1] - fSlow5 * fRec1[1];
			float fTemp0 = fSlow8 - fSlow7 * (1.0f - fRec1[0]);
			float fTemp1 = fRec3[1] * std::cos(fSlow10 * fTemp0);
			float fTemp2 = fRec4[1] * std::cos(fSlow11 * fTemp0);
			float fTemp3 = fRec5[1] * std::cos(fSlow12 * fTemp0);
			float fTemp4 = fRec6[1] * std::cos(fSlow13 * fTemp0);
			float fTemp5 = static_cast<float>(input0[i0]);
			float fTemp6 = ((iSlow0) ? 0.0f : fTemp5);
			fRec6[0] = fSlow18 * fTemp6 + fSlow17 * fRec0[1] + fSlow16 * fTemp4 - fSlow15 * fRec6[2];
			fRec5[0] = fRec6[2] + fSlow15 * (fRec6[0] - fRec5[2]) - fSlow16 * (fTemp4 - fTemp3);
			fRec4[0] = fRec5[2] + fSlow15 * (fRec5[0] - fRec4[2]) - fSlow16 * (fTemp3 - fTemp2);
			fRec3[0] = fRec4[2] + fSlow15 * (fRec4[0] - fRec3[2]) - fSlow16 * (fTemp2 - fTemp1);
			fRec0[0] = fRec3[2] + fSlow15 * fRec3[0] - fSlow16 * fTemp1;
			output0[i0] = static_cast<FAUSTFLOAT>(((iSlow0) ? fTemp5 : fSlow18 * fTemp6 * fSlow19 + fRec0[0] * fSlow2));
			float fTemp7 = fSlow8 - fSlow7 * (1.0f - fRec2[0]);
			float fTemp8 = fRec8[1] * std::cos(fSlow10 * fTemp7);
			float fTemp9 = fRec9[1] * std::cos(fSlow11 * fTemp7);
			float fTemp10 = fRec10[1] * std::cos(fSlow12 * fTemp7);
			float fTemp11 = fRec11[1] * std::cos(fSlow13 * fTemp7);
			float fTemp12 = static_cast<float>(input1[i0]);
			float fTemp13 = ((iSlow0) ? 0.0f : fTemp12);
			fRec11[0] = fSlow18 * fTemp13 + fSlow17 * fRec7[1] + fSlow16 * fTemp11 - fSlow15 * fRec11[2];
			fRec10[0] = fRec11[2] + fSlow15 * (fRec11[0] - fRec10[2]) - fSlow16 * (fTemp11 - fTemp10);
			fRec9[0] = fRec10[2] + fSlow15 * (fRec10[0] - fRec9[2]) - fSlow16 * (fTemp10 - fTemp9);
			fRec8[0] = fRec9[2] + fSlow15 * (fRec9[0] - fRec8[2]) - fSlow16 * (fTemp9 - fTemp8);
			fRec7[0] = fRec8[2] + fSlow15 * fRec8[0] - fSlow16 * fTemp8;
			output1[i0] = static_cast<FAUSTFLOAT>(((iSlow0) ? fTemp12 : fSlow18 * fTemp13 * fSlow19 + fRec7[0] * fSlow2));
			iVec0[1] = iVec0[0];
			fRec1[1] = fRec1[0];
			fRec2[1] = fRec2[0];
			fRec6[2] = fRec6[1];
			fRec6[1] = fRec6[0];
			fRec5[2] = fRec5[1];
			fRec5[1] = fRec5[0];
			fRec4[2] = fRec4[1];
			fRec4[1] = fRec4[0];
			fRec3[2] = fRec3[1];
			fRec3[1] = fRec3[0];
			fRec0[1] = fRec0[0];
			fRec11[2] = fRec11[1];
			fRec11[1] = fRec11[0];
			fRec10[2] = fRec10[1];
			fRec10[1] = fRec10[0];
			fRec9[2] = fRec9[1];
			fRec9[1] = fRec9[0];
			fRec8[2] = fRec8[1];
			fRec8[1] = fRec8[0];
			fRec7[1] = fRec7[0];
		}
	}

};

#endif
