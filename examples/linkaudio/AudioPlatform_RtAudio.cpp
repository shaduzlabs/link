/* Copyright 2016, Ableton AG, Berlin. All rights reserved.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  If you would like to incorporate Link into a proprietary software application,
 *  please contact <link-devs@ableton.com>.
 */

#include "AudioPlatform_RtAudio.hpp"
#include <chrono>
#include <iostream>

namespace ableton
{
namespace linkaudio
{

AudioPlatform::AudioPlatform(Link& link)
  : mEngine(link)
  , mSampleTime(0.)
{
  mEngine.setSampleRate(44100.);
  mEngine.setBufferSize(512);
  initialize();
  start();
}

AudioPlatform::~AudioPlatform()
{
  stop();
  uninitialize();
}

int AudioPlatform::audioCallback(void* outputBuffer,
  void* inputBuffer,
  unsigned int nBufferFrames,
  double streamTime,
  RtAudioStreamStatus status,
  void* userData)
{

  using namespace std::chrono;
  float* buffer = static_cast<float*>(outputBuffer);
  AudioPlatform& platform = *static_cast<AudioPlatform*>(userData);
  AudioEngine& engine = platform.mEngine;

  const auto hostTime =
    platform.mHostTimeFilter.sampleTimeToHostTime(platform.mSampleTime);

  platform.mSampleTime += nBufferFrames;

  const auto bufferBeginAtOutput = hostTime + engine.mOutputLatency;

  engine.audioCallback(bufferBeginAtOutput, nBufferFrames);

  for (unsigned long i = 0; i < nBufferFrames; ++i)
  {
    buffer[i * 2] = engine.mBuffer[i];
    buffer[i * 2 + 1] = engine.mBuffer[i];
  }

  return 0;
}

void AudioPlatform::initialize()
{
  if (m_audioDevice.getDeviceCount() < 1)
  {
    std::cerr << "No audio interfaces found" << std::endl;
    std::terminate();
  }

  try
  {
    m_audioStreamParameters.deviceId = m_audioDevice.getDefaultOutputDevice();
    m_audioStreamParameters.nChannels = 2;
    m_audioStreamParameters.firstChannel = 0;
    m_audioBufferSize = mEngine.mBuffer.size();

    RtAudio::DeviceInfo deviceInfo =
      m_audioDevice.getDeviceInfo(m_audioDevice.getDefaultOutputDevice());
    m_audioDevice.openStream(&m_audioStreamParameters, NULL, RTAUDIO_FLOAT32,
      static_cast<unsigned>(mEngine.mSampleRate), &m_audioBufferSize,
      &AudioPlatform::audioCallback, this);
  }
  catch (RtAudioError e)
  {
    std::cerr << "Could not initialize Audio Engine. " << e.getMessage() << std::endl;
    std::terminate();
  }
}

void AudioPlatform::uninitialize()
{
  try
  {
    m_audioDevice.closeStream();
  }
  catch (RtAudioError e)
  {
    std::cerr << "Could not close Audio Stream. " << e.getMessage() << std::endl;
    std::terminate();
  }
}

void AudioPlatform::start()
{
  try
  {
    m_audioDevice.startStream();
  }
  catch (RtAudioError e)
  {
    std::cerr << "Could not start Audio Stream. " << e.getMessage() << std::endl;
    std::terminate();
  }
}

void AudioPlatform::stop()
{
  try
  {
    m_audioDevice.stopStream();
  }
  catch (RtAudioError e)
  {
    std::cerr << "Could not start Audio Stream. " << e.getMessage() << std::endl;
    std::terminate();
  }
}

} // namespace linkaudio
} // namespace ableton
