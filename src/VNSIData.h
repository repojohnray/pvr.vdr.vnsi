/*
 *  Copyright (C) 2010-2020 Team Kodi
 *  Copyright (C) 2010 Alwin Esch (Team Kodi)
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "VNSISession.h"
#include "client.h"

#include <string>
#include <map>

class cResponsePacket;
class cRequestPacket;

class cVNSIData : public cVNSISession, public P8PLATFORM::CThread
{
public:

  cVNSIData();
  virtual ~cVNSIData();

  bool Start(const std::string& hostname, int port, const char* name = NULL, const std::string& mac = "");
  bool        SupportChannelScan();
  bool        SupportRecordingsUndelete();
  bool        EnableStatusInterface(bool onOff, bool wait = true);
  bool        GetDriveSpace(long long *total, long long *used);

  int         GetChannelsCount();
  bool        GetChannelsList(ADDON_HANDLE handle, bool radio = false);
  bool        GetEPGForChannel(ADDON_HANDLE handle, int iChannelUid, time_t start, time_t end);

  int         GetChannelGroupCount(bool automatic);
  bool        GetChannelGroupList(ADDON_HANDLE handle, bool bRadio);
  bool        GetChannelGroupMembers(ADDON_HANDLE handle, const PVR_CHANNEL_GROUP &group);

  bool        GetTimersList(ADDON_HANDLE handle);
  int         GetTimersCount();
  PVR_ERROR   AddTimer(const PVR_TIMER &timerinfo);
  PVR_ERROR   GetTimerInfo(unsigned int timernumber, PVR_TIMER &tag);
  PVR_ERROR   DeleteTimer(const PVR_TIMER &timerinfo, bool force = false);
  PVR_ERROR   RenameTimer(const PVR_TIMER &timerinfo, const char *newname);
  PVR_ERROR   UpdateTimer(const PVR_TIMER &timerinfo);
  PVR_ERROR GetTimerTypes(PVR_TIMER_TYPE types[], int *size);
  bool GenTimerChildren(const PVR_TIMER &timer, ADDON_HANDLE handle);
  std::string GenTimerFolder(std::string directory, std::string title);

  int         GetRecordingsCount();
  PVR_ERROR   GetRecordingsList(ADDON_HANDLE handle);
  PVR_ERROR   RenameRecording(const PVR_RECORDING& recinfo, const char* newname);
  PVR_ERROR   DeleteRecording(const PVR_RECORDING& recinfo);
  PVR_ERROR   GetRecordingEdl(const PVR_RECORDING& recinfo, PVR_EDL_ENTRY edl[], int *size);
  int         GetDeletedRecordingsCount();
  PVR_ERROR   GetDeletedRecordingsList(ADDON_HANDLE handle);
  PVR_ERROR   UndeleteRecording(const PVR_RECORDING& recording);
  PVR_ERROR   DeleteAllRecordingsFromTrash();
  PVR_ERROR   UndeleteAllRecordingsFromTrash();

  std::unique_ptr<cResponsePacket> ReadResult(cRequestPacket* vrp);

protected:

  virtual void *Process(void) override;
  virtual bool OnResponsePacket(cResponsePacket *pkt);

  void OnDisconnect() override;
  void OnReconnect() override;

private:

  struct SMessage
  {
    P8PLATFORM::CEvent event;
    std::unique_ptr<cResponsePacket> pkt;
  };

  class Queue {
    typedef std::map<int, SMessage> SMessages;
    SMessages m_queue;
    P8PLATFORM::CMutex m_mutex;

  public:
    SMessage &Enqueue(uint32_t serial);
    std::unique_ptr<cResponsePacket> Dequeue(uint32_t serial,
                                             SMessage &message);
    void Set(std::unique_ptr<cResponsePacket> &&vresp);
  };

  Queue m_queue;

  std::string m_videodir;
  std::string m_wolMac;
};
