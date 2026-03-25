#include <augr/rack/rack.h>

class RtAudio;

namespace augr {

class AudioInputDevice;
class AudioOutputDevice;

class ExeRack : public Rack {
public:
  virtual bool Create() override;
  bool CreateAudioInputDevice();
  bool CreateAudioOutputDevice();
  int ProcessAudio(double streamTime, void* inbuf, void* outbuf, unsigned long frames);
  //
  bool Start();
  void Stop();
  //Accessors
  int NumInputs() { return devNumInChans_; }
  int NumOutputs() { return devNumOutChans_; }
  RtAudio& audio_dac() { return *audio_dac_; }
  //Data members
  RtAudio* audio_dac_;
  AudioInputDevice* audio_input_device_ = nullptr;
  AudioOutputDevice* audio_output_device_ = nullptr;
  //----------------------------------------------------------------------------
  // 	number of physical input and output channels of the PA device
  //----------------------------------------------------------------------------
  int	devNumInChans_;
  int	devNumOutChans_;

  REFLECT_ENABLE(Rack)
};

} // namespace augr