/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#pragma once
#include <list>
#include <utility>

#include "DVDClock.h"
#include "IAudioSink.h"
#include "DVDMessageQueue.h"
#include "DVDStreamInfo.h"
#include "IDVDPlayer.h"
#include "threads/Thread.h"
#include "utils/BitstreamStats.h"

class CDVDPlayer;
class CDVDAudioCodec;
class CDVDAudioCodec;

class CDVDPlayerAudio : public CThread, public IDVDStreamPlayerAudio
{
public:
  CDVDPlayerAudio(CDVDClock* pClock, CDVDMessageQueue& parent);
  virtual ~CDVDPlayerAudio();

  bool OpenStream(CDVDStreamInfo &hints);
  void CloseStream(bool bWaitForBuffers);

  void SetSpeed(int speed);
  void Flush(bool sync);

  // waits until all available data has been rendered
  void WaitForBuffers();
  bool AcceptsData() const;
  bool HasData() const                                  { return m_messageQueue.GetDataSize() > 0; }
  int  GetLevel() const                                 { return m_messageQueue.GetLevel(); }
  bool IsInited() const                                 { return m_messageQueue.IsInited(); }
  void SendMessage(CDVDMsg* pMsg, int priority = 0)     { m_messageQueue.Put(pMsg, priority); }
  void FlushMessages()                                  { m_messageQueue.Flush(); }

  void SetVolume(float fVolume)                         { m_dvdAudio->SetVolume(fVolume); }
  void SetMute(bool bOnOff)                             { }
  void SetDynamicRangeCompression(long drc)             { m_dvdAudio->SetDynamicRangeCompression(drc); }
  float GetDynamicRangeAmplification() const            { return 0.0f; }


  std::string GetPlayerInfo();
  int GetAudioBitrate();
  int GetAudioChannels();

  // holds stream information for current playing stream
  CDVDStreamInfo m_streaminfo;

  double GetCurrentPts()                            { return m_info.pts; }

  bool IsStalled() const                            { return m_stalled;  }
  bool IsEOS()                                      { return false; }
  bool IsPassthrough() const;

protected:

  virtual void OnStartup();
  virtual void OnExit();
  virtual void Process();

  void OpenStream(CDVDStreamInfo &hints, CDVDAudioCodec* codec);
  //! Switch codec if needed. Called when the sample rate gotten from the
  //! codec changes, in which case we may want to switch passthrough on/off.
  bool SwitchCodecIfNeeded();
  float GetCurrentAttenuation()                         { return m_dvdAudio->GetCurrentAttenuation(); }
  bool AllowDTSHDDecode();

  CDVDMessageQueue m_messageQueue;
  CDVDMessageQueue& m_messageParent;

  double m_audioClock;

  // data for audio decoding
  struct PacketStatus
  {
    PacketStatus()
    {
        msg = NULL;
        Release();
    }

   ~PacketStatus()
    {
        Release();
    }

    CDVDMsgDemuxerPacket* msg;
    uint8_t* data;
    int size;
    double dts;
    double pts;

    void Attach(CDVDMsgDemuxerPacket* msg2)
    {
      if(msg) msg->Release();
      msg = msg2;
      msg->Acquire();
      DemuxPacket* p = msg->GetPacket();
      data = p->pData;
      size = p->iSize;
      dts = p->dts;
      pts = p->pts;
    }
    void Release()
    {
      if(msg) msg->Release();
      msg  = NULL;
      data = NULL;
      size = 0;
      dts  = DVD_NOPTS_VALUE;
      pts  = DVD_NOPTS_VALUE;
    }
  } m_decode;

  IAudioSink* m_dvdAudio;         // audio output device
  CDVDClock* m_pClock;            // dvd master clock
  CDVDAudioCodec* m_pAudioCodec;  // audio codec
  BitstreamStats m_audioStats;

  int m_speed;
  bool m_stalled;
  bool m_paused;
  IDVDStreamPlayer::ESyncState m_syncState;
  XbmcThreads::EndTime m_syncTimer;

  bool OutputPacket(DVDAudioFrame &audioframe);

  //SYNC_DISCON, SYNC_SKIPDUP, SYNC_RESAMPLE
  int    m_synctype;
  int    m_setsynctype;
  int    m_prevsynctype; //so we can print to the log

  void   SetSyncType(bool passthrough);

  bool   m_prevskipped;
  double m_maxspeedadjust;

  struct SInfo
  {
    SInfo()
    : pts(DVD_NOPTS_VALUE)
    , bitrate(0.0)
    , resamplerate(1.0)
    , attenuation(1.0)
    , passthrough(false)
    {}
    double pts;
    double bitrate;
    double resamplerate;
    double attenuation;
    bool   passthrough;
  };

  SInfo m_info;
};

