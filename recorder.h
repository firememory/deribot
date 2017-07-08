#ifndef _RECORDER_H_
#define _RECORDER_H_

#include "quickfix/FileStore.h"
#include "quickfix/FileLog.h"
#include "quickfix/SocketAcceptor.h"
#include "quickfix/Session.h"
#include "quickfix/SessionSettings.h"
#include "quickfix/Application.h"

class Recorder : public FIX::Application, public FIX::MessageCracker
{
public:
  Recorder();
  void show(const FIX::Message& msg);
  void onCreate(const FIX::SessionID& id);
  void onLogon(const FIX::SessionID& id);
  void onLogout(const FIX::SessionID& id);
  void toAdmin(FIX::Message& msg, const FIX::SessionID& id);
  void toApp(FIX::Message& msg, const FIX::SessionID& id) throw (FIX::DoNotSend);
  void fromAdmin(const FIX::Message& msg, const FIX::SessionID& id)
    throw (FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon);
  void fromApp(const FIX::Message& msg, const FIX::SessionID& id)
    throw (FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType);

  bool isRunning() { return isRunning_; };
  FIX::SessionID& sessionId() { return sessionId_; };

private:
  bool isRunning_;
  FIX::SessionID sessionId_;
};

#endif // _RECORDER_H_
