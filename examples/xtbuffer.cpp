#include "RtAudio.h"
#include <iostream>
#include <cstdlib>

#include <vector>
//#include <xtensor/xarray.hpp>
#include <xtensor/containers/xadapt.hpp>

typedef double MY_TYPE;
#define FORMAT RTAUDIO_FLOAT64
#define SCALE  1.0

// Platform-dependent sleep routines.
#if defined( WIN32 )
  #include <windows.h>
  #define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
  #include <unistd.h>
  #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

#define BASE_RATE 0.005
#define TIME   1.0

void usage( void ) {
  // Error function in case of incorrect command-line
  // argument specifications
  std::cout << "\nuseage: playsaw N fs <device> <channelOffset> <time>\n";
  std::cout << "    where N = number of channels,\n";
  std::cout << "    fs = the sample rate,\n";
  std::cout << "    device = optional device to use (default = 0),\n";
  std::cout << "    channelOffset = an optional channel offset on the device (default = 0),\n";
  std::cout << "    and time = an optional time duration in seconds (default = no limit).\n\n";
  exit( 0 );
}

void errorCallback( RtAudioErrorType type, const std::string &errorText )
{
  // This example error handling function does exactly the same thing
  // as the embedded RtAudio::error() function.
  std::cout << "in errorCallback" << std::endl;
  std::cerr << '\n' << errorText << "\n\n";
}

unsigned int channels;
RtAudio::StreamOptions options;
unsigned int frameCounter = 0;
bool checkCount = false;
unsigned int nFrames = 0;
const unsigned int callbackReturnValue = 1;


int saw( void *outputBuffer, void * /*inputBuffer*/, unsigned int nBufferFrames,
         double /*streamTime*/, RtAudioStreamStatus status, void *data )
{
  unsigned int i, j;
  extern unsigned int channels;
  MY_TYPE *_buffer = (MY_TYPE *) outputBuffer;

  size_t size = nBufferFrames * channels;
  std::vector<size_t> shape = { channels, nBufferFrames };
  auto buffer = xt::adapt(_buffer, size, xt::no_ownership(), shape);


  double *lastValues = (double *) data;

  if ( status )
    std::cout << "Stream underflow detected!" << std::endl;

  double increment;
  for ( j=0; j<channels; j++ ) {
    increment = BASE_RATE * (j+1+(j*0.1));
    for ( i=0; i<nBufferFrames; i++ ) {
      buffer(j, i) = lastValues[j] * SCALE * 0.5;
      lastValues[j] += increment;
      if ( lastValues[j] >= 1.0 ) lastValues[j] -= 2.0;
    }
  }

  frameCounter += nBufferFrames;
  if ( checkCount && ( frameCounter >= nFrames ) ) return callbackReturnValue;
  return 0;
}

int main( int argc, char *argv[] )
{
  unsigned int bufferFrames, fs, device = 0, offset = 0;

  // minimal command-line checking
  if (argc < 3 || argc > 6 ) usage();

  RtAudio dac;
  if ( dac.getDeviceCount() < 1 ) {
    std::cout << "\nNo audio devices found!\n";
    exit( 1 );
  }

  channels = (unsigned int) atoi( argv[1] );
  fs = (unsigned int) atoi( argv[2] );
  if ( argc > 3 )
    device = (unsigned int) atoi( argv[3] );
  if ( argc > 4 )
    offset = (unsigned int) atoi( argv[4] );
  if ( argc > 5 )
    nFrames = (unsigned int) (fs * atof( argv[5] ));
  if ( nFrames > 0 ) checkCount = true;

  double *data = (double *) calloc( channels, sizeof( double ) );

  // Let RtAudio print messages to stderr.
  dac.showWarnings( true );

  // Set our stream parameters for output only.
  bufferFrames = 512;
  RtAudio::StreamParameters oParams;
  oParams.deviceId = device;
  oParams.nChannels = channels;
  oParams.firstChannel = offset;

  if ( device == 0 )
    oParams.deviceId = dac.getDefaultOutputDevice();

  options.flags = RTAUDIO_HOG_DEVICE;
  options.flags |= RTAUDIO_SCHEDULE_REALTIME;
#if !defined( USE_INTERLEAVED )
  options.flags |= RTAUDIO_NONINTERLEAVED;
#endif
  RtAudioErrorType e = dac.openStream( &oParams, NULL, FORMAT, fs, &bufferFrames, &saw, (void *)data, &options);
  if (e != RtAudioErrorType::RTAUDIO_NO_ERROR)
    goto cleanup;
  e = dac.startStream();
  if (e != RtAudioErrorType::RTAUDIO_NO_ERROR)
    goto cleanup;

  if ( checkCount ) {
    while ( dac.isStreamRunning() == true ) SLEEP( 100 );
  }
  else {
    char input;
    //std::cout << "Stream latency = " << dac.getStreamLatency() << "\n" << std::endl;
    std::cout << "\nPlaying ... press <enter> to quit (buffer size = " << bufferFrames << ").\n";
    std::cin.get( input );

    e = dac.stopStream();
  }

 cleanup:
  if ( dac.isStreamOpen() ) dac.closeStream();
  free( data );

  return 0;
}
