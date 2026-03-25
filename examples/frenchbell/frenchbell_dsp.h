/* ------------------------------------------------------------
copyright: "(c)Romain Michon, CCRMA (Stanford University), GRAME"
license: "MIT"
name: "FrenchChurchBell"
Code generated with Faust 2.81.10 (https://faust.grame.fr)
Compilation options: -lang cpp -ec -ct 1 -cn FrenchBellDsp -scn augr::FaustDsp -es 1 -mcd 16 -mdd 1024 -mdy 33 -single -ftz 0
------------------------------------------------------------ */

#ifndef  __FrenchBellDsp_H__
#define  __FrenchBellDsp_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif 

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <math.h>

#ifndef FAUSTCLASS 
#define FAUSTCLASS FrenchBellDsp
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

const static float fFrenchBellDspSIG0Wave0[350] = {0.97289f,0.542393f,0.495832f,0.897966f,0.552367f,0.557895f,0.614213f,0.353694f,0.436039f,0.441024f,0.674913f,0.566754f,0.755008f,0.69903f,0.164398f,0.91004f,0.628373f,0.201718f,0.429517f,0.503715f,0.871174f,0.106886f,0.761173f,0.673602f,0.291937f,0.58859f,0.31528f,0.413081f,0.274464f,0.494062f,0.696121f,0.61201f,0.382757f,0.995113f,0.228806f,0.198449f,0.595847f,0.306263f,0.252397f,0.0883567f,0.236086f,1.0f,0.245278f,0.379388f,0.198824f,0.548892f,0.492764f,0.420871f,0.794637f,0.605634f,1.0f,0.604159f,0.399841f,0.799347f,0.507187f,0.50981f,0.477383f,0.310226f,0.426976f,0.437623f,0.735712f,0.630728f,0.625785f,0.651168f,0.277738f,0.850858f,0.578079f,0.645354f,0.3837f,0.370598f,0.782542f,0.181325f,0.614391f,0.740684f,0.342441f,0.586186f,0.286909f,0.405197f,0.259215f,0.566983f,0.748219f,0.655898f,0.36826f,0.940814f,0.336157f,0.413702f,0.561557f,0.402176f,0.117698f,0.329369f,0.254571f,0.870706f,0.260981f,0.274122f,0.206247f,0.6453f,0.400758f,0.363622f,0.636834f,0.584566f,0.975603f,0.616937f,0.295401f,0.650447f,0.464221f,0.465057f,0.312467f,0.238358f,0.383695f,0.399674f,0.753912f,0.662012f,0.504268f,0.599248f,0.378665f,0.725363f,0.493214f,1.0f,0.332836f,0.265889f,0.674073f,0.320401f,0.440018f,0.769782f,0.316419f,0.529968f,0.380509f,0.578676f,0.249013f,0.591384f,0.761717f,0.687057f,0.324437f,0.818864f,0.505369f,0.672485f,0.461783f,0.426198f,0.0678875f,0.435329f,0.347955f,0.708394f,0.293322f,0.328986f,0.258254f,0.893512f,0.320131f,0.433554f,0.459302f,0.542213f,0.817241f,0.51623f,0.205302f,0.467354f,0.388683f,0.388216f,0.171262f,0.150865f,0.29172f,0.311414f,0.658876f,0.570647f,0.383619f,0.502455f,0.364114f,0.532313f,0.352989f,1.0f,0.261984f,0.219591f,0.544031f,0.337199f,0.279173f,0.668303f,0.208439f,0.39923f,0.418674f,0.648618f,0.234133f,0.504729f,0.645347f,0.572851f,0.232828f,0.614292f,0.485272f,0.666264f,0.31657f,0.320355f,0.191421f,0.340131f,0.342069f,0.538371f,0.281131f,0.393115f,0.251394f,0.890725f,0.310644f,0.5037f,0.29909f,0.442478f,0.733128f,0.455217f,0.199322f,0.315699f,0.375856f,0.37649f,0.029145f,0.0200283f,0.279578f,0.3168f,0.655957f,0.546843f,0.349666f,0.470249f,0.353765f,0.286794f,0.180185f,1.0f,0.210831f,0.280133f,0.535853f,0.376488f,0.15367f,0.634745f,0.0510449f,0.485575f,0.593111f,0.917884f,0.380477f,0.422925f,0.599373f,0.311421f,0.135654f,0.359954f,0.295748f,0.474439f,0.353339f,0.116743f,0.454313f,0.112858f,0.35931f,0.483897f,0.301608f,0.577342f,0.262663f,0.794986f,0.54958f,0.808086f,0.152511f,0.439591f,0.535941f,0.308018f,0.419837f,0.579191f,0.250428f,0.25212f,0.10286f,0.288332f,0.599679f,0.665108f,0.636285f,0.495234f,0.251613f,0.208527f,0.0939072f,0.458415f,0.318952f,0.23535f,0.215189f,0.313412f,0.0472787f,0.0386893f,0.0863359f,0.222346f,0.361511f,0.997037f,0.238509f,0.38214f,1.0f,0.203554f,0.472087f,0.509015f,0.206371f,0.441013f,0.794008f,0.971623f,0.796384f,0.55043f,0.68778f,0.554298f,0.0436545f,0.0595577f,0.214685f,0.58147f,0.27732f,0.368466f,0.121382f,0.152739f,0.0782244f,0.190497f,0.316269f,0.180456f,0.405196f,0.518716f,0.159365f,0.165808f,0.194372f,0.614764f,0.553415f,0.644794f,0.44178f,0.44149f,0.221588f,1.0f,0.579371f,0.606339f,0.529167f,0.0214475f,0.417046f,0.663152f,0.894841f,0.475752f,0.168289f,0.46859f,0.304604f,0.666369f,0.308337f,0.295091f,0.768945f,0.350911f,0.49068f,0.516584f,0.400117f,0.820687f,0.545486f,0.709357f,0.509759f,0.472006f,0.152455f,0.981265f,0.459134f,0.698666f,0.247154f,0.623253f,0.255508f,0.830815f,0.088709f,0.126082f,0.0770623f,0.263328f};
class FrenchBellDspSIG0 {
	
  private:
	
	int fFrenchBellDspSIG0Wave0_idx;
	
  public:
	
	int getNumInputsFrenchBellDspSIG0() {
		return 0;
	}
	int getNumOutputsFrenchBellDspSIG0() {
		return 1;
	}
	
	void instanceInitFrenchBellDspSIG0(int sample_rate) {
		fFrenchBellDspSIG0Wave0_idx = 0;
	}
	
	void fillFrenchBellDspSIG0(int count, float* table) {
		for (int i1 = 0; i1 < count; i1 = i1 + 1) {
			table[i1] = fFrenchBellDspSIG0Wave0[fFrenchBellDspSIG0Wave0_idx];
			fFrenchBellDspSIG0Wave0_idx = (1 + fFrenchBellDspSIG0Wave0_idx) % 350;
		}
	}

};

static FrenchBellDspSIG0* newFrenchBellDspSIG0() { return (FrenchBellDspSIG0*)new FrenchBellDspSIG0(); }
static void deleteFrenchBellDspSIG0(FrenchBellDspSIG0* dsp) { delete dsp; }

static float ftbl0FrenchBellDspSIG0[350];
static float FrenchBellDsp_faustpower2_f(float value) {
	return value * value;
}

class FrenchBellDsp : public augr::FaustDsp {
	
 private:
	
	FAUSTFLOAT fEntry0;
	int iSlow0;
	float fSlow1;
	int fSampleRate;
	float fConst0;
	float fConst1;
	float fConst2;
	float fConst3;
	FAUSTFLOAT fButton0;
	float fSlow2;
	float fVec0[2];
	int iRec1[2];
	FAUSTFLOAT fHslider0;
	float fConst4;
	float fSlow3;
	FAUSTFLOAT fHslider1;
	float fConst5;
	float fSlow4;
	float fSlow5;
	float fSlow6;
	float fSlow7;
	float fSlow8;
	float fSlow9;
	float fConst6;
	float fConst7;
	float fConst8;
	float fConst9;
	float fConst10;
	float fConst11;
	float fConst12;
	int iRec4[2];
	float fRec3[3];
	float fConst13;
	float fRec2[3];
	FAUSTFLOAT fHslider2;
	float fSlow10;
	float fRec0[3];
	float fSlow11;
	float fConst14;
	float fConst15;
	float fConst16;
	float fRec5[3];
	float fSlow12;
	float fConst17;
	float fConst18;
	float fConst19;
	float fRec6[3];
	float fSlow13;
	float fConst20;
	float fConst21;
	float fConst22;
	float fRec7[3];
	float fSlow14;
	float fConst23;
	float fConst24;
	float fConst25;
	float fRec8[3];
	float fSlow15;
	float fConst26;
	float fConst27;
	float fConst28;
	float fRec9[3];
	float fSlow16;
	float fConst29;
	float fConst30;
	float fConst31;
	float fRec10[3];
	float fSlow17;
	float fConst32;
	float fConst33;
	float fConst34;
	float fRec11[3];
	float fSlow18;
	float fConst35;
	float fConst36;
	float fConst37;
	float fRec12[3];
	float fSlow19;
	float fConst38;
	float fConst39;
	float fConst40;
	float fRec13[3];
	float fSlow20;
	float fConst41;
	float fConst42;
	float fConst43;
	float fRec14[3];
	float fSlow21;
	float fConst44;
	float fConst45;
	float fConst46;
	float fRec15[3];
	float fSlow22;
	float fConst47;
	float fConst48;
	float fConst49;
	float fRec16[3];
	float fSlow23;
	float fConst50;
	float fConst51;
	float fConst52;
	float fRec17[3];
	float fSlow24;
	float fConst53;
	float fConst54;
	float fConst55;
	float fRec18[3];
	float fSlow25;
	float fConst56;
	float fConst57;
	float fConst58;
	float fRec19[3];
	float fSlow26;
	float fConst59;
	float fConst60;
	float fConst61;
	float fRec20[3];
	float fSlow27;
	float fConst62;
	float fConst63;
	float fConst64;
	float fRec21[3];
	float fSlow28;
	float fConst65;
	float fConst66;
	float fConst67;
	float fRec22[3];
	float fSlow29;
	float fConst68;
	float fConst69;
	float fConst70;
	float fRec23[3];
	float fSlow30;
	float fConst71;
	float fConst72;
	float fConst73;
	float fRec24[3];
	float fSlow31;
	float fConst74;
	float fConst75;
	float fConst76;
	float fRec25[3];
	float fSlow32;
	float fConst77;
	float fConst78;
	float fConst79;
	float fRec26[3];
	float fSlow33;
	float fConst80;
	float fConst81;
	float fConst82;
	float fRec27[3];
	float fSlow34;
	float fConst83;
	float fConst84;
	float fConst85;
	float fRec28[3];
	float fSlow35;
	float fConst86;
	float fConst87;
	float fConst88;
	float fRec29[3];
	float fSlow36;
	float fConst89;
	float fConst90;
	float fConst91;
	float fRec30[3];
	float fSlow37;
	float fConst92;
	float fConst93;
	float fConst94;
	float fRec31[3];
	float fSlow38;
	float fConst95;
	float fConst96;
	float fConst97;
	float fRec32[3];
	float fSlow39;
	float fConst98;
	float fConst99;
	float fConst100;
	float fRec33[3];
	float fSlow40;
	float fConst101;
	float fConst102;
	float fConst103;
	float fRec34[3];
	float fSlow41;
	float fConst104;
	float fConst105;
	float fConst106;
	float fRec35[3];
	float fSlow42;
	float fConst107;
	float fConst108;
	float fConst109;
	float fRec36[3];
	float fSlow43;
	float fConst110;
	float fConst111;
	float fConst112;
	float fRec37[3];
	float fSlow44;
	float fConst113;
	float fConst114;
	float fConst115;
	float fRec38[3];
	float fSlow45;
	float fConst116;
	float fConst117;
	float fConst118;
	float fRec39[3];
	float fSlow46;
	float fConst119;
	float fConst120;
	float fConst121;
	float fRec40[3];
	float fSlow47;
	float fConst122;
	float fConst123;
	float fConst124;
	float fRec41[3];
	float fSlow48;
	float fConst125;
	float fConst126;
	float fConst127;
	float fRec42[3];
	float fSlow49;
	float fConst128;
	float fConst129;
	float fConst130;
	float fRec43[3];
	float fSlow50;
	float fConst131;
	float fConst132;
	float fConst133;
	float fRec44[3];
	float fSlow51;
	float fConst134;
	float fConst135;
	float fConst136;
	float fRec45[3];
	float fSlow52;
	float fConst137;
	float fConst138;
	float fConst139;
	float fRec46[3];
	float fSlow53;
	float fConst140;
	float fConst141;
	float fConst142;
	float fRec47[3];
	float fSlow54;
	float fConst143;
	float fConst144;
	float fConst145;
	float fRec48[3];
	float fSlow55;
	float fConst146;
	float fConst147;
	float fConst148;
	float fRec49[3];
	float fSlow56;
	float fConst149;
	float fConst150;
	float fConst151;
	float fRec50[3];
	float fSlow57;
	float fConst152;
	float fConst153;
	float fConst154;
	float fRec51[3];
	float fSlow58;
	float fConst155;
	float fConst156;
	float fConst157;
	float fRec52[3];
	float fSlow59;
	float fConst158;
	float fConst159;
	float fConst160;
	float fRec53[3];
	
 public:
	FrenchBellDsp() {
	}
	
	void metadata(Meta* m) { 
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "1.22.0");
		m->declare("compile_options", "-lang cpp -ec -ct 1 -cn FrenchBellDsp -scn augr::FaustDsp -es 1 -mcd 16 -mdd 1024 -mdy 33 -single -ftz 0");
		m->declare("copyright", "(c)Romain Michon, CCRMA (Stanford University), GRAME");
		m->declare("description", "French church bell physical model.");
		m->declare("envelopes.lib/ar:author", "Yann Orlarey, Stéphane Letz");
		m->declare("envelopes.lib/author", "GRAME");
		m->declare("envelopes.lib/copyright", "GRAME");
		m->declare("envelopes.lib/license", "LGPL with exception");
		m->declare("envelopes.lib/name", "Faust Envelope Library");
		m->declare("envelopes.lib/version", "1.3.0");
		m->declare("filename", "frenchBell.dsp");
		m->declare("filters.lib/fir:author", "Julius O. Smith III");
		m->declare("filters.lib/fir:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/fir:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/highpass:author", "Julius O. Smith III");
		m->declare("filters.lib/highpass:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/iir:author", "Julius O. Smith III");
		m->declare("filters.lib/iir:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/iir:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/lowpass0_highpass1", "MIT-style STK-4.3 license");
		m->declare("filters.lib/lowpass0_highpass1:author", "Julius O. Smith III");
		m->declare("filters.lib/lowpass:author", "Julius O. Smith III");
		m->declare("filters.lib/lowpass:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/lowpass:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/tf2:author", "Julius O. Smith III");
		m->declare("filters.lib/tf2:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf2:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/tf2s:author", "Julius O. Smith III");
		m->declare("filters.lib/tf2s:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf2s:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/version", "1.7.1");
		m->declare("license", "MIT");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.9.0");
		m->declare("name", "FrenchChurchBell");
		m->declare("noises.lib/name", "Faust Noise Generator Library");
		m->declare("noises.lib/version", "1.5.0");
		m->declare("physmodels.lib/name", "Faust Physical Models Library");
		m->declare("physmodels.lib/version", "1.2.0");
		m->declare("platform.lib/name", "Generic Platform Library");
		m->declare("platform.lib/version", "1.3.0");
	}

	virtual int getNumInputs() {
		return 0;
	}
	virtual int getNumOutputs() {
		return 2;
	}
	
	static void classInit(int sample_rate) {
		FrenchBellDspSIG0* sig0 = newFrenchBellDspSIG0();
		sig0->instanceInitFrenchBellDspSIG0(sample_rate);
		sig0->fillFrenchBellDspSIG0(350, ftbl0FrenchBellDspSIG0);
		deleteFrenchBellDspSIG0(sig0);
	}
	
	virtual void instanceConstants(int sample_rate) {
		fSampleRate = sample_rate;
		fConst0 = std::min<float>(1.92e+05f, std::max<float>(1.0f, static_cast<float>(fSampleRate)));
		fConst1 = std::pow(0.001f, 177.57524f / fConst0);
		fConst2 = FrenchBellDsp_faustpower2_f(fConst1);
		fConst3 = 2.0f * std::cos(15136.131f / fConst0) * fConst1;
		fConst4 = 0.002f * fConst0;
		fConst5 = 3.1415927f / fConst0;
		fConst6 = std::tan(31.415926f / fConst0);
		fConst7 = FrenchBellDsp_faustpower2_f(fConst6);
		fConst8 = 2.0f * (1.0f - 1.0f / fConst7);
		fConst9 = 1.0f / fConst6;
		fConst10 = (fConst9 + -1.4142135f) / fConst6 + 1.0f;
		fConst11 = (fConst9 + 1.4142135f) / fConst6 + 1.0f;
		fConst12 = 1.0f / fConst11;
		fConst13 = 1.0f / (fConst7 * fConst11);
		fConst14 = std::pow(0.001f, 161.98773f / fConst0);
		fConst15 = FrenchBellDsp_faustpower2_f(fConst14);
		fConst16 = 2.0f * std::cos(15107.542f / fConst0) * fConst14;
		fConst17 = std::pow(0.001f, 36.60426f / fConst0);
		fConst18 = FrenchBellDsp_faustpower2_f(fConst17);
		fConst19 = 2.0f * std::cos(14499.204f / fConst0) * fConst17;
		fConst20 = std::pow(0.001f, 33.02221f / fConst0);
		fConst21 = FrenchBellDsp_faustpower2_f(fConst20);
		fConst22 = 2.0f * std::cos(14444.8545f / fConst0) * fConst20;
		fConst23 = std::pow(0.001f, 32.680614f / fConst0);
		fConst24 = FrenchBellDsp_faustpower2_f(fConst23);
		fConst25 = 2.0f * std::cos(14439.263f / fConst0) * fConst23;
		fConst26 = std::pow(0.001f, 31.955881f / fConst0);
		fConst27 = FrenchBellDsp_faustpower2_f(fConst26);
		fConst28 = 2.0f * std::cos(14427.136f / fConst0) * fConst26;
		fConst29 = std::pow(0.001f, 13.995437f / fConst0);
		fConst30 = FrenchBellDsp_faustpower2_f(fConst29);
		fConst31 = 2.0f * std::cos(13911.286f / fConst0) * fConst29;
		fConst32 = std::pow(0.001f, 13.492317f / fConst0);
		fConst33 = FrenchBellDsp_faustpower2_f(fConst32);
		fConst34 = 2.0f * std::cos(13884.96f / fConst0) * fConst32;
		fConst35 = std::pow(0.001f, 7.079305f / fConst0);
		fConst36 = FrenchBellDsp_faustpower2_f(fConst35);
		fConst37 = 2.0f * std::cos(13364.398f / fConst0) * fConst35;
		fConst38 = std::pow(0.001f, 6.5504103f / fConst0);
		fConst39 = FrenchBellDsp_faustpower2_f(fConst38);
		fConst40 = 2.0f * std::cos(13293.838f / fConst0) * fConst38;
		fConst41 = std::pow(0.001f, 5.5839663f / fConst0);
		fConst42 = FrenchBellDsp_faustpower2_f(fConst41);
		fConst43 = 2.0f * std::cos(13142.916f / fConst0) * fConst41;
		fConst44 = std::pow(0.001f, 5.137064f / fConst0);
		fConst45 = FrenchBellDsp_faustpower2_f(fConst44);
		fConst46 = 2.0f * std::cos(13060.795f / fConst0) * fConst44;
		fConst47 = std::pow(0.001f, 4.45922f / fConst0);
		fConst48 = FrenchBellDsp_faustpower2_f(fConst47);
		fConst49 = 2.0f * std::cos(12916.155f / fConst0) * fConst47;
		fConst50 = std::pow(0.001f, 4.4322863f / fConst0);
		fConst51 = FrenchBellDsp_faustpower2_f(fConst50);
		fConst52 = 2.0f * std::cos(12909.81f / fConst0) * fConst50;
		fConst53 = std::pow(0.001f, 4.0347524f / fConst0);
		fConst54 = FrenchBellDsp_faustpower2_f(fConst53);
		fConst55 = 2.0f * std::cos(12809.719f / fConst0) * fConst53;
		fConst56 = std::pow(0.001f, 3.9501925f / fConst0);
		fConst57 = FrenchBellDsp_faustpower2_f(fConst56);
		fConst58 = 2.0f * std::cos(12786.722f / fConst0) * fConst56;
		fConst59 = std::pow(0.001f, 2.2498608f / fConst0);
		fConst60 = FrenchBellDsp_faustpower2_f(fConst59);
		fConst61 = 2.0f * std::cos(12112.097f / fConst0) * fConst59;
		fConst62 = std::pow(0.001f, 2.0767975f / fConst0);
		fConst63 = FrenchBellDsp_faustpower2_f(fConst62);
		fConst64 = 2.0f * std::cos(12005.471f / fConst0) * fConst62;
		fConst65 = std::pow(0.001f, 1.5660667f / fConst0);
		fConst66 = FrenchBellDsp_faustpower2_f(fConst65);
		fConst67 = 2.0f * std::cos(11605.923f / fConst0) * fConst65;
		fConst68 = std::pow(0.001f, 1.3993771f / fConst0);
		fConst69 = FrenchBellDsp_faustpower2_f(fConst68);
		fConst70 = 2.0f * std::cos(11435.837f / fConst0) * fConst68;
		fConst71 = std::pow(0.001f, 1.3371644f / fConst0);
		fConst72 = FrenchBellDsp_faustpower2_f(fConst71);
		fConst73 = 2.0f * std::cos(11365.277f / fConst0) * fConst71;
		fConst74 = std::pow(0.001f, 1.315844f / fConst0);
		fConst75 = FrenchBellDsp_faustpower2_f(fConst74);
		fConst76 = 2.0f * std::cos(11340.081f / fConst0) * fConst74;
		fConst77 = std::pow(0.001f, 0.8697476f / fConst0);
		fConst78 = FrenchBellDsp_faustpower2_f(fConst77);
		fConst79 = 2.0f * std::cos(10642.2705f / fConst0) * fConst77;
		fConst80 = std::pow(0.001f, 0.8623695f / fConst0);
		fConst81 = FrenchBellDsp_faustpower2_f(fConst80);
		fConst82 = 2.0f * std::cos(10626.877f / fConst0) * fConst80;
		fConst83 = std::pow(0.001f, 0.7920422f / fConst0);
		fConst84 = FrenchBellDsp_faustpower2_f(fConst83);
		fConst85 = 2.0f * std::cos(10470.74f / fConst0) * fConst83;
		fConst86 = std::pow(0.001f, 0.78402334f / fConst0);
		fConst87 = FrenchBellDsp_faustpower2_f(fConst86);
		fConst88 = 2.0f * std::cos(10451.765f / fConst0) * fConst86;
		fConst89 = std::pow(0.001f, 0.6248085f / fConst0);
		fConst90 = FrenchBellDsp_faustpower2_f(fConst89);
		fConst91 = 2.0f * std::cos(10011.313f / fConst0) * fConst89;
		fConst92 = std::pow(0.001f, 0.6230969f / fConst0);
		fConst93 = FrenchBellDsp_faustpower2_f(fConst92);
		fConst94 = 2.0f * std::cos(10005.784f / fConst0) * fConst92;
		fConst95 = std::pow(0.001f, 0.4912485f / fConst0);
		fConst96 = FrenchBellDsp_faustpower2_f(fConst95);
		fConst97 = 2.0f * std::cos(9506.836f / fConst0) * fConst95;
		fConst98 = std::pow(0.001f, 0.48983717f / fConst0);
		fConst99 = FrenchBellDsp_faustpower2_f(fConst98);
		fConst100 = 2.0f * std::cos(9500.553f / fConst0) * fConst98;
		fConst101 = std::pow(0.001f, 0.36290723f / fConst0);
		fConst102 = FrenchBellDsp_faustpower2_f(fConst101);
		fConst103 = 2.0f * std::cos(8811.351f / fConst0) * fConst101;
		fConst104 = std::pow(0.001f, 0.36082828f / fConst0);
		fConst105 = FrenchBellDsp_faustpower2_f(fConst104);
		fConst106 = 2.0f * std::cos(8797.465f / fConst0) * fConst104;
		fConst107 = std::pow(0.001f, 0.34939942f / fConst0);
		fConst108 = FrenchBellDsp_faustpower2_f(fConst107);
		fConst109 = 2.0f * std::cos(8719.176f / fConst0) * fConst107;
		fConst110 = std::pow(0.001f, 0.22336319f / fConst0);
		fConst111 = FrenchBellDsp_faustpower2_f(fConst110);
		fConst112 = 2.0f * std::cos(7539.257f / fConst0) * fConst110;
		fConst113 = std::pow(0.001f, 0.22197479f / fConst0);
		fConst114 = FrenchBellDsp_faustpower2_f(fConst113);
		fConst115 = 2.0f * std::cos(7521.538f / fConst0) * fConst113;
		fConst116 = std::pow(0.001f, 0.19992995f / fConst0);
		fConst117 = FrenchBellDsp_faustpower2_f(fConst116);
		fConst118 = 2.0f * std::cos(7218.7515f / fConst0) * fConst116;
		fConst119 = std::pow(0.001f, 0.19845314f / fConst0);
		fConst120 = FrenchBellDsp_faustpower2_f(fConst119);
		fConst121 = 2.0f * std::cos(7196.886f / fConst0) * fConst119;
		fConst122 = std::pow(0.001f, 0.17200719f / fConst0);
		fConst123 = FrenchBellDsp_faustpower2_f(fConst122);
		fConst124 = 2.0f * std::cos(6764.3516f / fConst0) * fConst122;
		fConst125 = std::pow(0.001f, 0.17125909f / fConst0);
		fConst126 = FrenchBellDsp_faustpower2_f(fConst125);
		fConst127 = 2.0f * std::cos(6750.843f / fConst0) * fConst125;
		fConst128 = std::pow(0.001f, 0.17077428f / fConst0);
		fConst129 = FrenchBellDsp_faustpower2_f(fConst128);
		fConst130 = 2.0f * std::cos(6742.0464f / fConst0) * fConst128;
		fConst131 = std::pow(0.001f, 0.16794412f / fConst0);
		fConst132 = FrenchBellDsp_faustpower2_f(fConst131);
		fConst133 = 2.0f * std::cos(6690.0215f / fConst0) * fConst131;
		fConst134 = std::pow(0.001f, 0.16781573f / fConst0);
		fConst135 = FrenchBellDsp_faustpower2_f(fConst134);
		fConst136 = 2.0f * std::cos(6687.634f / fConst0) * fConst134;
		fConst137 = std::pow(0.001f, 0.122413f / fConst0);
		fConst138 = FrenchBellDsp_faustpower2_f(fConst137);
		fConst139 = 2.0f * std::cos(5648.904f / fConst0) * fConst137;
		fConst140 = std::pow(0.001f, 0.1135364f / fConst0);
		fConst141 = FrenchBellDsp_faustpower2_f(fConst140);
		fConst142 = 2.0f * std::cos(5384.476f / fConst0) * fConst140;
		fConst143 = std::pow(0.001f, 0.08849458f / fConst0);
		fConst144 = FrenchBellDsp_faustpower2_f(fConst143);
		fConst145 = 2.0f * std::cos(4460.2886f / fConst0) * fConst143;
		fConst146 = std::pow(0.001f, 0.08845718f / fConst0);
		fConst147 = FrenchBellDsp_faustpower2_f(fConst146);
		fConst148 = 2.0f * std::cos(4458.6553f / fConst0) * fConst146;
		fConst149 = std::pow(0.001f, 0.07575908f / fConst0);
		fConst150 = FrenchBellDsp_faustpower2_f(fConst149);
		fConst151 = 2.0f * std::cos(3843.9019f / fConst0) * fConst149;
		fConst152 = std::pow(0.001f, 0.07513989f / fConst0);
		fConst153 = FrenchBellDsp_faustpower2_f(fConst152);
		fConst154 = 2.0f * std::cos(3810.4502f / fConst0) * fConst152;
		fConst155 = std::pow(0.001f, 0.058786336f / fConst0);
		fConst156 = FrenchBellDsp_faustpower2_f(fConst155);
		fConst157 = 2.0f * std::cos(2766.5178f / fConst0) * fConst155;
		fConst158 = std::pow(0.001f, 0.05868406f / fConst0);
		fConst159 = FrenchBellDsp_faustpower2_f(fConst158);
		fConst160 = 2.0f * std::cos(2758.8022f / fConst0) * fConst158;
	}
	
	virtual void instanceResetUserInterface() {
		fEntry0 = static_cast<FAUSTFLOAT>(0.0f);
		fButton0 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider0 = static_cast<FAUSTFLOAT>(0.5f);
		fHslider1 = static_cast<FAUSTFLOAT>(6.5e+03f);
		fHslider2 = static_cast<FAUSTFLOAT>(1.0f);
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; l0 < 2; l0 = l0 + 1) {
			fVec0[l0] = 0.0f;
		}
		for (int l1 = 0; l1 < 2; l1 = l1 + 1) {
			iRec1[l1] = 0;
		}
		for (int l2 = 0; l2 < 2; l2 = l2 + 1) {
			iRec4[l2] = 0;
		}
		for (int l3 = 0; l3 < 3; l3 = l3 + 1) {
			fRec3[l3] = 0.0f;
		}
		for (int l4 = 0; l4 < 3; l4 = l4 + 1) {
			fRec2[l4] = 0.0f;
		}
		for (int l5 = 0; l5 < 3; l5 = l5 + 1) {
			fRec0[l5] = 0.0f;
		}
		for (int l6 = 0; l6 < 3; l6 = l6 + 1) {
			fRec5[l6] = 0.0f;
		}
		for (int l7 = 0; l7 < 3; l7 = l7 + 1) {
			fRec6[l7] = 0.0f;
		}
		for (int l8 = 0; l8 < 3; l8 = l8 + 1) {
			fRec7[l8] = 0.0f;
		}
		for (int l9 = 0; l9 < 3; l9 = l9 + 1) {
			fRec8[l9] = 0.0f;
		}
		for (int l10 = 0; l10 < 3; l10 = l10 + 1) {
			fRec9[l10] = 0.0f;
		}
		for (int l11 = 0; l11 < 3; l11 = l11 + 1) {
			fRec10[l11] = 0.0f;
		}
		for (int l12 = 0; l12 < 3; l12 = l12 + 1) {
			fRec11[l12] = 0.0f;
		}
		for (int l13 = 0; l13 < 3; l13 = l13 + 1) {
			fRec12[l13] = 0.0f;
		}
		for (int l14 = 0; l14 < 3; l14 = l14 + 1) {
			fRec13[l14] = 0.0f;
		}
		for (int l15 = 0; l15 < 3; l15 = l15 + 1) {
			fRec14[l15] = 0.0f;
		}
		for (int l16 = 0; l16 < 3; l16 = l16 + 1) {
			fRec15[l16] = 0.0f;
		}
		for (int l17 = 0; l17 < 3; l17 = l17 + 1) {
			fRec16[l17] = 0.0f;
		}
		for (int l18 = 0; l18 < 3; l18 = l18 + 1) {
			fRec17[l18] = 0.0f;
		}
		for (int l19 = 0; l19 < 3; l19 = l19 + 1) {
			fRec18[l19] = 0.0f;
		}
		for (int l20 = 0; l20 < 3; l20 = l20 + 1) {
			fRec19[l20] = 0.0f;
		}
		for (int l21 = 0; l21 < 3; l21 = l21 + 1) {
			fRec20[l21] = 0.0f;
		}
		for (int l22 = 0; l22 < 3; l22 = l22 + 1) {
			fRec21[l22] = 0.0f;
		}
		for (int l23 = 0; l23 < 3; l23 = l23 + 1) {
			fRec22[l23] = 0.0f;
		}
		for (int l24 = 0; l24 < 3; l24 = l24 + 1) {
			fRec23[l24] = 0.0f;
		}
		for (int l25 = 0; l25 < 3; l25 = l25 + 1) {
			fRec24[l25] = 0.0f;
		}
		for (int l26 = 0; l26 < 3; l26 = l26 + 1) {
			fRec25[l26] = 0.0f;
		}
		for (int l27 = 0; l27 < 3; l27 = l27 + 1) {
			fRec26[l27] = 0.0f;
		}
		for (int l28 = 0; l28 < 3; l28 = l28 + 1) {
			fRec27[l28] = 0.0f;
		}
		for (int l29 = 0; l29 < 3; l29 = l29 + 1) {
			fRec28[l29] = 0.0f;
		}
		for (int l30 = 0; l30 < 3; l30 = l30 + 1) {
			fRec29[l30] = 0.0f;
		}
		for (int l31 = 0; l31 < 3; l31 = l31 + 1) {
			fRec30[l31] = 0.0f;
		}
		for (int l32 = 0; l32 < 3; l32 = l32 + 1) {
			fRec31[l32] = 0.0f;
		}
		for (int l33 = 0; l33 < 3; l33 = l33 + 1) {
			fRec32[l33] = 0.0f;
		}
		for (int l34 = 0; l34 < 3; l34 = l34 + 1) {
			fRec33[l34] = 0.0f;
		}
		for (int l35 = 0; l35 < 3; l35 = l35 + 1) {
			fRec34[l35] = 0.0f;
		}
		for (int l36 = 0; l36 < 3; l36 = l36 + 1) {
			fRec35[l36] = 0.0f;
		}
		for (int l37 = 0; l37 < 3; l37 = l37 + 1) {
			fRec36[l37] = 0.0f;
		}
		for (int l38 = 0; l38 < 3; l38 = l38 + 1) {
			fRec37[l38] = 0.0f;
		}
		for (int l39 = 0; l39 < 3; l39 = l39 + 1) {
			fRec38[l39] = 0.0f;
		}
		for (int l40 = 0; l40 < 3; l40 = l40 + 1) {
			fRec39[l40] = 0.0f;
		}
		for (int l41 = 0; l41 < 3; l41 = l41 + 1) {
			fRec40[l41] = 0.0f;
		}
		for (int l42 = 0; l42 < 3; l42 = l42 + 1) {
			fRec41[l42] = 0.0f;
		}
		for (int l43 = 0; l43 < 3; l43 = l43 + 1) {
			fRec42[l43] = 0.0f;
		}
		for (int l44 = 0; l44 < 3; l44 = l44 + 1) {
			fRec43[l44] = 0.0f;
		}
		for (int l45 = 0; l45 < 3; l45 = l45 + 1) {
			fRec44[l45] = 0.0f;
		}
		for (int l46 = 0; l46 < 3; l46 = l46 + 1) {
			fRec45[l46] = 0.0f;
		}
		for (int l47 = 0; l47 < 3; l47 = l47 + 1) {
			fRec46[l47] = 0.0f;
		}
		for (int l48 = 0; l48 < 3; l48 = l48 + 1) {
			fRec47[l48] = 0.0f;
		}
		for (int l49 = 0; l49 < 3; l49 = l49 + 1) {
			fRec48[l49] = 0.0f;
		}
		for (int l50 = 0; l50 < 3; l50 = l50 + 1) {
			fRec49[l50] = 0.0f;
		}
		for (int l51 = 0; l51 < 3; l51 = l51 + 1) {
			fRec50[l51] = 0.0f;
		}
		for (int l52 = 0; l52 < 3; l52 = l52 + 1) {
			fRec51[l52] = 0.0f;
		}
		for (int l53 = 0; l53 < 3; l53 = l53 + 1) {
			fRec52[l53] = 0.0f;
		}
		for (int l54 = 0; l54 < 3; l54 = l54 + 1) {
			fRec53[l54] = 0.0f;
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
	
	virtual FrenchBellDsp* clone() {
		return new FrenchBellDsp();
	}
	
	virtual int getSampleRate() {
		return fSampleRate;
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("frenchBell");
		ui_interface->declare(&fEntry0, "0", "");
		ui_interface->addNumEntry("strikePosition", &fEntry0, FAUSTFLOAT(0.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(6.0f), FAUSTFLOAT(1.0f));
		ui_interface->declare(&fHslider1, "1", "");
		ui_interface->addHorizontalSlider("strikeCutOff", &fHslider1, FAUSTFLOAT(6.5e+03f), FAUSTFLOAT(2e+01f), FAUSTFLOAT(2e+04f), FAUSTFLOAT(1.0f));
		ui_interface->declare(&fHslider0, "2", "");
		ui_interface->addHorizontalSlider("strikeSharpness", &fHslider0, FAUSTFLOAT(0.5f), FAUSTFLOAT(0.01f), FAUSTFLOAT(5.0f), FAUSTFLOAT(0.01f));
		ui_interface->declare(&fHslider2, "3", "");
		ui_interface->addHorizontalSlider("gain", &fHslider2, FAUSTFLOAT(1.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.01f));
		ui_interface->declare(&fButton0, "4", "");
		ui_interface->addButton("gate", &fButton0);
		ui_interface->closeBox();
	}
	
	void control() {
		iSlow0 = 50 * static_cast<int>(static_cast<float>(fEntry0));
		fSlow1 = ftbl0FrenchBellDspSIG0[iSlow0 + 49];
		fSlow2 = static_cast<float>(fButton0);
		fSlow3 = 1.0f / std::max<float>(1.0f, fConst4 * static_cast<float>(fHslider0));
		fSlow4 = std::tan(fConst5 * static_cast<float>(fHslider1));
		fSlow5 = 2.0f * (1.0f - 1.0f / FrenchBellDsp_faustpower2_f(fSlow4));
		fSlow6 = 1.0f / fSlow4;
		fSlow7 = (fSlow6 + -1.4142135f) / fSlow4 + 1.0f;
		fSlow8 = (fSlow6 + 1.4142135f) / fSlow4 + 1.0f;
		fSlow9 = 1.0f / fSlow8;
		fSlow10 = static_cast<float>(fHslider2) / fSlow8;
		fSlow11 = ftbl0FrenchBellDspSIG0[iSlow0 + 48];
		fSlow12 = ftbl0FrenchBellDspSIG0[iSlow0 + 47];
		fSlow13 = ftbl0FrenchBellDspSIG0[iSlow0 + 46];
		fSlow14 = ftbl0FrenchBellDspSIG0[iSlow0 + 45];
		fSlow15 = ftbl0FrenchBellDspSIG0[iSlow0 + 44];
		fSlow16 = ftbl0FrenchBellDspSIG0[iSlow0 + 43];
		fSlow17 = ftbl0FrenchBellDspSIG0[iSlow0 + 42];
		fSlow18 = ftbl0FrenchBellDspSIG0[iSlow0 + 41];
		fSlow19 = ftbl0FrenchBellDspSIG0[iSlow0 + 40];
		fSlow20 = ftbl0FrenchBellDspSIG0[iSlow0 + 39];
		fSlow21 = ftbl0FrenchBellDspSIG0[iSlow0 + 38];
		fSlow22 = ftbl0FrenchBellDspSIG0[iSlow0 + 37];
		fSlow23 = ftbl0FrenchBellDspSIG0[iSlow0 + 36];
		fSlow24 = ftbl0FrenchBellDspSIG0[iSlow0 + 35];
		fSlow25 = ftbl0FrenchBellDspSIG0[iSlow0 + 34];
		fSlow26 = ftbl0FrenchBellDspSIG0[iSlow0 + 33];
		fSlow27 = ftbl0FrenchBellDspSIG0[iSlow0 + 32];
		fSlow28 = ftbl0FrenchBellDspSIG0[iSlow0 + 31];
		fSlow29 = ftbl0FrenchBellDspSIG0[iSlow0 + 30];
		fSlow30 = ftbl0FrenchBellDspSIG0[iSlow0 + 29];
		fSlow31 = ftbl0FrenchBellDspSIG0[iSlow0 + 28];
		fSlow32 = ftbl0FrenchBellDspSIG0[iSlow0 + 27];
		fSlow33 = ftbl0FrenchBellDspSIG0[iSlow0 + 26];
		fSlow34 = ftbl0FrenchBellDspSIG0[iSlow0 + 25];
		fSlow35 = ftbl0FrenchBellDspSIG0[iSlow0 + 24];
		fSlow36 = ftbl0FrenchBellDspSIG0[iSlow0 + 23];
		fSlow37 = ftbl0FrenchBellDspSIG0[iSlow0 + 22];
		fSlow38 = ftbl0FrenchBellDspSIG0[iSlow0 + 21];
		fSlow39 = ftbl0FrenchBellDspSIG0[iSlow0 + 20];
		fSlow40 = ftbl0FrenchBellDspSIG0[iSlow0 + 19];
		fSlow41 = ftbl0FrenchBellDspSIG0[iSlow0 + 18];
		fSlow42 = ftbl0FrenchBellDspSIG0[iSlow0 + 17];
		fSlow43 = ftbl0FrenchBellDspSIG0[iSlow0 + 16];
		fSlow44 = ftbl0FrenchBellDspSIG0[iSlow0 + 15];
		fSlow45 = ftbl0FrenchBellDspSIG0[iSlow0 + 14];
		fSlow46 = ftbl0FrenchBellDspSIG0[iSlow0 + 13];
		fSlow47 = ftbl0FrenchBellDspSIG0[iSlow0 + 12];
		fSlow48 = ftbl0FrenchBellDspSIG0[iSlow0 + 11];
		fSlow49 = ftbl0FrenchBellDspSIG0[iSlow0 + 10];
		fSlow50 = ftbl0FrenchBellDspSIG0[iSlow0 + 9];
		fSlow51 = ftbl0FrenchBellDspSIG0[iSlow0 + 8];
		fSlow52 = ftbl0FrenchBellDspSIG0[iSlow0 + 7];
		fSlow53 = ftbl0FrenchBellDspSIG0[iSlow0 + 6];
		fSlow54 = ftbl0FrenchBellDspSIG0[iSlow0 + 5];
		fSlow55 = ftbl0FrenchBellDspSIG0[iSlow0 + 4];
		fSlow56 = ftbl0FrenchBellDspSIG0[iSlow0 + 3];
		fSlow57 = ftbl0FrenchBellDspSIG0[iSlow0 + 2];
		fSlow58 = ftbl0FrenchBellDspSIG0[iSlow0 + 1];
		fSlow59 = ftbl0FrenchBellDspSIG0[iSlow0];
	}
	
	virtual void compute(int count, FAUSTFLOAT** RESTRICT inputs, FAUSTFLOAT** RESTRICT outputs) {
		FAUSTFLOAT* output0 = outputs[0];
		FAUSTFLOAT* output1 = outputs[1];
		for (int i0 = 0; i0 < count; i0 = i0 + 1) {
			fVec0[0] = fSlow2;
			iRec1[0] = (iRec1[1] + (iRec1[1] > 0)) * (fSlow2 <= fVec0[1]) + (fSlow2 > fVec0[1]);
			float fTemp0 = fSlow3 * static_cast<float>(iRec1[0]);
			iRec4[0] = 1103515245 * iRec4[1] + 12345;
			fRec3[0] = 4.656613e-10f * static_cast<float>(iRec4[0]) - fConst12 * (fConst10 * fRec3[2] + fConst8 * fRec3[1]);
			fRec2[0] = fConst13 * (fRec3[2] + (fRec3[0] - 2.0f * fRec3[1])) - fSlow9 * (fSlow7 * fRec2[2] + fSlow5 * fRec2[1]);
			float fTemp1 = fSlow10 * (fRec2[2] + fRec2[0] + 2.0f * fRec2[1]) * std::max<float>(0.0f, std::min<float>(fTemp0, 2.0f - fTemp0));
			fRec0[0] = fTemp1 + fConst3 * fRec0[1] - fConst2 * fRec0[2];
			fRec5[0] = fTemp1 + fConst16 * fRec5[1] - fConst15 * fRec5[2];
			fRec6[0] = fTemp1 + fConst19 * fRec6[1] - fConst18 * fRec6[2];
			fRec7[0] = fTemp1 + fConst22 * fRec7[1] - fConst21 * fRec7[2];
			fRec8[0] = fTemp1 + fConst25 * fRec8[1] - fConst24 * fRec8[2];
			fRec9[0] = fTemp1 + fConst28 * fRec9[1] - fConst27 * fRec9[2];
			fRec10[0] = fTemp1 + fConst31 * fRec10[1] - fConst30 * fRec10[2];
			fRec11[0] = fTemp1 + fConst34 * fRec11[1] - fConst33 * fRec11[2];
			fRec12[0] = fTemp1 + fConst37 * fRec12[1] - fConst36 * fRec12[2];
			fRec13[0] = fTemp1 + fConst40 * fRec13[1] - fConst39 * fRec13[2];
			fRec14[0] = fTemp1 + fConst43 * fRec14[1] - fConst42 * fRec14[2];
			fRec15[0] = fTemp1 + fConst46 * fRec15[1] - fConst45 * fRec15[2];
			fRec16[0] = fTemp1 + fConst49 * fRec16[1] - fConst48 * fRec16[2];
			fRec17[0] = fTemp1 + fConst52 * fRec17[1] - fConst51 * fRec17[2];
			fRec18[0] = fTemp1 + fConst55 * fRec18[1] - fConst54 * fRec18[2];
			fRec19[0] = fTemp1 + fConst58 * fRec19[1] - fConst57 * fRec19[2];
			fRec20[0] = fTemp1 + fConst61 * fRec20[1] - fConst60 * fRec20[2];
			fRec21[0] = fTemp1 + fConst64 * fRec21[1] - fConst63 * fRec21[2];
			fRec22[0] = fTemp1 + fConst67 * fRec22[1] - fConst66 * fRec22[2];
			fRec23[0] = fTemp1 + fConst70 * fRec23[1] - fConst69 * fRec23[2];
			fRec24[0] = fTemp1 + fConst73 * fRec24[1] - fConst72 * fRec24[2];
			fRec25[0] = fTemp1 + fConst76 * fRec25[1] - fConst75 * fRec25[2];
			fRec26[0] = fTemp1 + fConst79 * fRec26[1] - fConst78 * fRec26[2];
			fRec27[0] = fTemp1 + fConst82 * fRec27[1] - fConst81 * fRec27[2];
			fRec28[0] = fTemp1 + fConst85 * fRec28[1] - fConst84 * fRec28[2];
			fRec29[0] = fTemp1 + fConst88 * fRec29[1] - fConst87 * fRec29[2];
			fRec30[0] = fTemp1 + fConst91 * fRec30[1] - fConst90 * fRec30[2];
			fRec31[0] = fTemp1 + fConst94 * fRec31[1] - fConst93 * fRec31[2];
			fRec32[0] = fTemp1 + fConst97 * fRec32[1] - fConst96 * fRec32[2];
			fRec33[0] = fTemp1 + fConst100 * fRec33[1] - fConst99 * fRec33[2];
			fRec34[0] = fTemp1 + fConst103 * fRec34[1] - fConst102 * fRec34[2];
			fRec35[0] = fTemp1 + fConst106 * fRec35[1] - fConst105 * fRec35[2];
			fRec36[0] = fTemp1 + fConst109 * fRec36[1] - fConst108 * fRec36[2];
			fRec37[0] = fTemp1 + fConst112 * fRec37[1] - fConst111 * fRec37[2];
			fRec38[0] = fTemp1 + fConst115 * fRec38[1] - fConst114 * fRec38[2];
			fRec39[0] = fTemp1 + fConst118 * fRec39[1] - fConst117 * fRec39[2];
			fRec40[0] = fTemp1 + fConst121 * fRec40[1] - fConst120 * fRec40[2];
			fRec41[0] = fTemp1 + fConst124 * fRec41[1] - fConst123 * fRec41[2];
			fRec42[0] = fTemp1 + fConst127 * fRec42[1] - fConst126 * fRec42[2];
			fRec43[0] = fTemp1 + fConst130 * fRec43[1] - fConst129 * fRec43[2];
			fRec44[0] = fTemp1 + fConst133 * fRec44[1] - fConst132 * fRec44[2];
			fRec45[0] = fTemp1 + fConst136 * fRec45[1] - fConst135 * fRec45[2];
			fRec46[0] = fTemp1 + fConst139 * fRec46[1] - fConst138 * fRec46[2];
			fRec47[0] = fTemp1 + fConst142 * fRec47[1] - fConst141 * fRec47[2];
			fRec48[0] = fTemp1 + fConst145 * fRec48[1] - fConst144 * fRec48[2];
			fRec49[0] = fTemp1 + fConst148 * fRec49[1] - fConst147 * fRec49[2];
			fRec50[0] = fTemp1 + fConst151 * fRec50[1] - fConst150 * fRec50[2];
			fRec51[0] = fTemp1 + fConst154 * fRec51[1] - fConst153 * fRec51[2];
			fRec52[0] = fTemp1 + fConst157 * fRec52[1] - fConst156 * fRec52[2];
			fRec53[0] = fTemp1 + fConst160 * fRec53[1] - fConst159 * fRec53[2];
			float fTemp2 = 0.02f * ((fRec53[0] - fRec53[2]) * fSlow59 + (fRec52[0] - fRec52[2]) * fSlow58 + (fRec51[0] - fRec51[2]) * fSlow57 + (fRec50[0] - fRec50[2]) * fSlow56 + (fRec49[0] - fRec49[2]) * fSlow55 + (fRec48[0] - fRec48[2]) * fSlow54 + (fRec47[0] - fRec47[2]) * fSlow53 + (fRec46[0] - fRec46[2]) * fSlow52 + (fRec45[0] - fRec45[2]) * fSlow51 + (fRec44[0] - fRec44[2]) * fSlow50 + (fRec43[0] - fRec43[2]) * fSlow49 + (fRec42[0] - fRec42[2]) * fSlow48 + (fRec41[0] - fRec41[2]) * fSlow47 + (fRec40[0] - fRec40[2]) * fSlow46 + (fRec39[0] - fRec39[2]) * fSlow45 + (fRec38[0] - fRec38[2]) * fSlow44 + (fRec37[0] - fRec37[2]) * fSlow43 + (fRec36[0] - fRec36[2]) * fSlow42 + (fRec35[0] - fRec35[2]) * fSlow41 + (fRec34[0] - fRec34[2]) * fSlow40 + (fRec33[0] - fRec33[2]) * fSlow39 + (fRec32[0] - fRec32[2]) * fSlow38 + (fRec31[0] - fRec31[2]) * fSlow37 + (fRec30[0] - fRec30[2]) * fSlow36 + (fRec29[0] - fRec29[2]) * fSlow35 + (fRec28[0] - fRec28[2]) * fSlow34 + (fRec27[0] - fRec27[2]) * fSlow33 + (fRec26[0] - fRec26[2]) * fSlow32 + (fRec25[0] - fRec25[2]) * fSlow31 + (fRec24[0] - fRec24[2]) * fSlow30 + (fRec23[0] - fRec23[2]) * fSlow29 + (fRec22[0] - fRec22[2]) * fSlow28 + (fRec21[0] - fRec21[2]) * fSlow27 + (fRec20[0] - fRec20[2]) * fSlow26 + (fRec19[0] - fRec19[2]) * fSlow25 + (fRec18[0] - fRec18[2]) * fSlow24 + (fRec17[0] - fRec17[2]) * fSlow23 + (fRec16[0] - fRec16[2]) * fSlow22 + (fRec15[0] - fRec15[2]) * fSlow21 + (fRec14[0] - fRec14[2]) * fSlow20 + (fRec13[0] - fRec13[2]) * fSlow19 + (fRec12[0] - fRec12[2]) * fSlow18 + (fRec11[0] - fRec11[2]) * fSlow17 + (fRec10[0] - fRec10[2]) * fSlow16 + (fRec9[0] - fRec9[2]) * fSlow15 + (fRec8[0] - fRec8[2]) * fSlow14 + (fRec7[0] - fRec7[2]) * fSlow13 + (fRec6[0] - fRec6[2]) * fSlow12 + (fRec5[0] - fRec5[2]) * fSlow11 + (fRec0[0] - fRec0[2]) * fSlow1);
			output0[i0] = static_cast<FAUSTFLOAT>(fTemp2);
			output1[i0] = static_cast<FAUSTFLOAT>(fTemp2);
			fVec0[1] = fVec0[0];
			iRec1[1] = iRec1[0];
			iRec4[1] = iRec4[0];
			fRec3[2] = fRec3[1];
			fRec3[1] = fRec3[0];
			fRec2[2] = fRec2[1];
			fRec2[1] = fRec2[0];
			fRec0[2] = fRec0[1];
			fRec0[1] = fRec0[0];
			fRec5[2] = fRec5[1];
			fRec5[1] = fRec5[0];
			fRec6[2] = fRec6[1];
			fRec6[1] = fRec6[0];
			fRec7[2] = fRec7[1];
			fRec7[1] = fRec7[0];
			fRec8[2] = fRec8[1];
			fRec8[1] = fRec8[0];
			fRec9[2] = fRec9[1];
			fRec9[1] = fRec9[0];
			fRec10[2] = fRec10[1];
			fRec10[1] = fRec10[0];
			fRec11[2] = fRec11[1];
			fRec11[1] = fRec11[0];
			fRec12[2] = fRec12[1];
			fRec12[1] = fRec12[0];
			fRec13[2] = fRec13[1];
			fRec13[1] = fRec13[0];
			fRec14[2] = fRec14[1];
			fRec14[1] = fRec14[0];
			fRec15[2] = fRec15[1];
			fRec15[1] = fRec15[0];
			fRec16[2] = fRec16[1];
			fRec16[1] = fRec16[0];
			fRec17[2] = fRec17[1];
			fRec17[1] = fRec17[0];
			fRec18[2] = fRec18[1];
			fRec18[1] = fRec18[0];
			fRec19[2] = fRec19[1];
			fRec19[1] = fRec19[0];
			fRec20[2] = fRec20[1];
			fRec20[1] = fRec20[0];
			fRec21[2] = fRec21[1];
			fRec21[1] = fRec21[0];
			fRec22[2] = fRec22[1];
			fRec22[1] = fRec22[0];
			fRec23[2] = fRec23[1];
			fRec23[1] = fRec23[0];
			fRec24[2] = fRec24[1];
			fRec24[1] = fRec24[0];
			fRec25[2] = fRec25[1];
			fRec25[1] = fRec25[0];
			fRec26[2] = fRec26[1];
			fRec26[1] = fRec26[0];
			fRec27[2] = fRec27[1];
			fRec27[1] = fRec27[0];
			fRec28[2] = fRec28[1];
			fRec28[1] = fRec28[0];
			fRec29[2] = fRec29[1];
			fRec29[1] = fRec29[0];
			fRec30[2] = fRec30[1];
			fRec30[1] = fRec30[0];
			fRec31[2] = fRec31[1];
			fRec31[1] = fRec31[0];
			fRec32[2] = fRec32[1];
			fRec32[1] = fRec32[0];
			fRec33[2] = fRec33[1];
			fRec33[1] = fRec33[0];
			fRec34[2] = fRec34[1];
			fRec34[1] = fRec34[0];
			fRec35[2] = fRec35[1];
			fRec35[1] = fRec35[0];
			fRec36[2] = fRec36[1];
			fRec36[1] = fRec36[0];
			fRec37[2] = fRec37[1];
			fRec37[1] = fRec37[0];
			fRec38[2] = fRec38[1];
			fRec38[1] = fRec38[0];
			fRec39[2] = fRec39[1];
			fRec39[1] = fRec39[0];
			fRec40[2] = fRec40[1];
			fRec40[1] = fRec40[0];
			fRec41[2] = fRec41[1];
			fRec41[1] = fRec41[0];
			fRec42[2] = fRec42[1];
			fRec42[1] = fRec42[0];
			fRec43[2] = fRec43[1];
			fRec43[1] = fRec43[0];
			fRec44[2] = fRec44[1];
			fRec44[1] = fRec44[0];
			fRec45[2] = fRec45[1];
			fRec45[1] = fRec45[0];
			fRec46[2] = fRec46[1];
			fRec46[1] = fRec46[0];
			fRec47[2] = fRec47[1];
			fRec47[1] = fRec47[0];
			fRec48[2] = fRec48[1];
			fRec48[1] = fRec48[0];
			fRec49[2] = fRec49[1];
			fRec49[1] = fRec49[0];
			fRec50[2] = fRec50[1];
			fRec50[1] = fRec50[0];
			fRec51[2] = fRec51[1];
			fRec51[1] = fRec51[0];
			fRec52[2] = fRec52[1];
			fRec52[1] = fRec52[0];
			fRec53[2] = fRec53[1];
			fRec53[1] = fRec53[0];
		}
	}

};

#endif
