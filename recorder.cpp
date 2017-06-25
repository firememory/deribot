#include <iostream>

#include "quickfix/FileStore.h"
#include "quickfix/FileLog.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/Session.h"
#include "quickfix/SessionSettings.h"
#include "quickfix/Application.h"
#include "quickfix/fix44/Logon.h"
#include "quickfix/fix44/MarketDataRequest.h"
#include "quickfix/MessageCracker.h"

#include "recorder.h"
#include "tools.h"

Recorder::Recorder() :
  isRunning_(false)
{
  srand(time(NULL));
}

void Recorder::show(const FIX::Message& msg)
{
  const char *s = msg.toString().c_str();
  for (unsigned i = 0; i < strlen(s) - 1; ++i)
    std::cout << ((s[i] == 1) ? '|' : s[i]);
  std::cout << std::endl;
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
  sessionId_ = id;
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
      std::string secret = "DZKYEWZHUXEXIRBEN6O34PFSDPEAFQTK";
      std::string nonce = "3pM14pgJOYBATBDaCmE+0YvdPExppge0";

      for (unsigned int i = 0; i < nonce.length(); ++i)
	nonce[i] = rand() % 256;
      nonce = Tools().b64_encode(nonce);

      std::string signature = Tools().sha256(nonce + secret);
      signature = Tools().b64_encode(signature.c_str());

      msg.setField(FIX::HeartBtInt(10));
      msg.setField(FIX::RawData(nonce));
      msg.setField(FIX::Username(key));
      msg.setField(FIX::Password(signature));
    }
  show(msg.toString());
}


void Recorder::toApp(FIX::Message& msg, const FIX::SessionID& id)
  throw (FIX::DoNotSend)
{
  /// Notification of app message being sent to target
  std::cout << "Recorder::toApp" << std::endl;
  show(msg);
}

void Recorder::fromAdmin(const FIX::Message& msg, const FIX::SessionID& id)
  throw (FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon)
{
  /// Notification of admin message being received from target
  std::cout << "Recorder::fromAdmin" << std::endl;
  show(msg);
}

void Recorder::fromApp(const FIX::Message& msg, const FIX::SessionID& id)
  throw (FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType)
{
  /// Notification of app message being received from target
  std::cout << "Recorder::fromApp" << std::endl;
  show(msg);
  //  crack(msg, id);
}


void Recorder::onMessage(const FIX::Message& msg, const FIX::SessionID& id)
{
  std::cout << "Recorder::onMessage" << std::endl;
  show(msg);
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
	usleep(1000);
      std::cout << "##########\nWe are connected !!!\n##########" << std::endl;

      FIX44::MarketDataRequest msg;

      // Fill message fields
      msg.setField(FIX::MDReqID("Test123"));
      msg.setField(FIX::SubscriptionRequestType('1'));
      msg.setField(FIX::MarketDepth(0));
      msg.setField(FIX::MDUpdateType(0));

      // Add the MDEntryTypes group
      FIX44::MarketDataRequest::NoMDEntryTypes noMDEntryTypes;
      noMDEntryTypes.setField(FIX::MDEntryType('0'));
      msg.addGroup(noMDEntryTypes);

      // Add the NoRelatedSym group
      FIX44::MarketDataRequest::NoRelatedSym noRelatedSym;
      noRelatedSym.set(FIX::Symbol("BTC-29SEP17"));
      msg.addGroup(noRelatedSym);

      FIX::Session::sendToTarget(msg, application.sessionId());

      while (1)
	sleep(1);
      initiator.stop();

      return 0;
    }
  catch(FIX::ConfigError& e)
    {
      std::cerr << e.what();
      return 1;
    }
}
