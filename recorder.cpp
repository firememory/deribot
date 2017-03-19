#include <iostream>

#include "quickfix/FileStore.h"
#include "quickfix/FileLog.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/Session.h"
#include "quickfix/SessionSettings.h"
#include "quickfix/Application.h"
#include "quickfix/fix44/Logon.h"

#include "recorder.h"
#include "tools.h"

Recorder::Recorder() :
  isRunning_(false)
{
}

void Recorder::onCreate(const FIX::SessionID& id)
{
  /// Notification of a session begin created
  std::cout << "Recorder::onCreate" << std::endl;
}

void Recorder::onLogon(const FIX::SessionID& id)
{
  /// Notification of a session successfully logging on
  std::cout << "Recorder::onLogon" << std::endl;
  isRunning_ = true;
}

void Recorder::onLogout(const FIX::SessionID& id)
{
  /// Notification of a session logging off or disconnecting
  std::cout << "Recorder::onLogout" << std::endl;
}

void Recorder::toAdmin(FIX::Message& msg, const FIX::SessionID& id)
{
  /// Notification of admin message being sent to target
  std::cout << "Recorder::toAdmin" << std::endl;

  if (msg.getHeader().getField(FIX::FIELD::MsgType) == FIX::MsgType_Logon)
    {
      std::string key = "2fUfCxWz3xWE";
      std::string secret = "7HSJWQQ6XHAM6UTNIIZ4PVWDIA4K3OXB";
      std::string nonce = "3pM14pgJOYBATBDaCmE+0YvdPExppgd5";

      nonce = Tools().b64_encode(nonce);

      std::string signature = Tools().sha256(nonce + secret);
      signature = Tools().b64_encode(signature);

      msg.setField(FIX::HeartBtInt(10));
      msg.setField(FIX::RawData(nonce));
      msg.setField(FIX::Username(key));
      msg.setField(FIX::Password(signature));
    }
}

void Recorder::toApp(FIX::Message& msg, const FIX::SessionID& id)
  throw (FIX::DoNotSend)
{
  /// Notification of app message being sent to target
  std::cout << "Recorder::toApp" << std::endl;
  std::cout << msg;
}

void Recorder::fromAdmin(const FIX::Message& msg, const FIX::SessionID& id)
  throw (FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon)
{
  /// Notification of admin message being received from target
  std::cout << "Recorder::fromAdmin" << std::endl;
  // std::cout << msg;
}

void Recorder::fromApp(const FIX::Message& msg, const FIX::SessionID& id)
  throw (FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType)
{
  /// Notification of app message being received from target
  std::cout << "Recorder::fromApp" << std::endl;
  std::cout << msg;
}

int main(int argc, char **argv)
{
  try
    {
      if(argc < 2)
	{
	  std::cout << "Usage\n" << argv[0] << " fix.conf" << std::endl;
	  return 1;
	}
      std::string fileName = argv[1];

      FIX::SessionSettings settings(fileName);

      Recorder application = Recorder();
      FIX::FileStoreFactory storeFactory(settings);
      FIX::FileLogFactory logFactory(settings);
      FIX::SocketInitiator initiator(application, storeFactory, settings, logFactory);

      initiator.start();
      while (!application.isRunning())
	sleep(1);
      std::cout << "##########\nWe are connected !!!\n##########" << std::endl;
      initiator.stop();

      return 0;
    }
  catch(FIX::ConfigError& e)
    {
      std::cerr << e.what();
      return 1;
    }
}
