#include <iostream>

#include "quickfix/FileStore.h"
#include "quickfix/FileLog.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/Session.h"
#include "quickfix/SessionSettings.h"
#include "quickfix/Application.h"
#include "quickfix/fix44/Logon.h"

#include "recorder.h"
#include "sha256.h"
#include "base64.h"

Recorder::Recorder()
{
}

void Recorder::onCreate(const FIX::SessionID& id)
{
  /// Notification of a session begin created
  std::cout << "Recorder::onCreate id " << id << std::endl;
}

void Recorder::onLogon(const FIX::SessionID& id)
{
  /// Notification of a session successfully logging on
  std::cout << "Recorder::onLogon id " << id << std::endl;
  // isRunning_ = true;
}

void Recorder::onLogout(const FIX::SessionID& id)
{
  /// Notification of a session logging off or disconnecting
  std::cout << "Recorder::onLogout id " << id << std::endl;
}

void Recorder::toAdmin(FIX::Message& msg, const FIX::SessionID& id)
{
  /// Notification of admin message being sent to target
  std::cout << "Recorder::toAdmin id " << id << std::endl;

  if (msg.getHeader().getField(FIX::FIELD::MsgType) == FIX::MsgType_Logon)
  // if (FIX::MsgType_Logon == FIELD_GET_REF(msg.getHeader(), MsgType))
    {
      std::cout << "Login in" << std::endl;
      std::string nonce = "3pM14pgJOYBATBDaCmE+0YvdPExppg5bJXAZtxakwDQ=";
      std::string key = "2fUfCxWz3xWE";
      std::string secret = "DF3P2HDRWERDNWN6ONYMW4KNPGHQAY2H";

      std::string signature = sha256(nonce + secret);
      signature = base64_encode(reinterpret_cast<const unsigned char*>(signature.c_str()), signature.length());
      std::cout << "signature=" << signature << std::endl;

      // FIX44::Logon& logon_message = dynamic_cast<FIX44::Logon&>(msg);
      // FIX::Logon logon_message = msg
      msg.setField(FIX::HeartBtInt(1));
      msg.setField(FIX::RawData(nonce));
      msg.setField(FIX::Username(key));
      msg.setField(FIX::Password(signature));
    }
  std::cout << "Message: " << msg.toString() << std::endl;
}

void Recorder::toApp(FIX::Message& msg, const FIX::SessionID& id)
  throw (FIX::DoNotSend)
{
  /// Notification of app message being sent to target
  std::cout << "Recorder::toApp " << id << std::endl;
  std::cout << msg;
}

void Recorder::fromAdmin(const FIX::Message& msg, const FIX::SessionID& id)
  throw (FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon)
{
  /// Notification of admin message being received from target
  std::cout << "Recorder::fromAdmin id " << id << std::endl;
  std::cout << msg;
}

void Recorder::fromApp(const FIX::Message& msg, const FIX::SessionID& id)
  throw (FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType)
{
  /// Notification of app message being received from target
  std::cout << "Recorder::fromApp " << id << std::endl;
  std::cout << msg;
}

int main(int argc, char **argv)
{
  try
    {
      if(argc < 2)
	{
	  std::cout << "Usage\n " << argv[0] << " fix.conf" << std::endl;
	  return 1;
	}
      std::string fileName = argv[1];

      FIX::SessionSettings settings(fileName);

      Recorder application = Recorder();
      FIX::FileStoreFactory storeFactory(settings);
      FIX::FileLogFactory logFactory(settings);
      FIX::SocketInitiator initiator(application, storeFactory, settings, logFactory);
      initiator.start();

      // while (!application.isRunning())
      // 	;
      // std::cout << "We are connected" << std::endl;

      // FIX::Message message;
      // FIX::Header header = message.getHeader();

      // header.setField(FIX::BeginString("FIX.4.2"));
      // header.setField(FIX::SenderCompID("DERIBITCLIENT"));
      // header.setField(FIX::TargetCompID("DERIBITSERVER"));
      // header.setField(FIX::MsgType(FIX::MsgType_OrderCancelRequest));
      // message.setField(FIX::OrigClOrdID("123"));
      // message.setField(FIX::ClOrdID("321"));
      // message.setField(FIX::Symbol("LNUX"));
      // message.setField(FIX::Side(FIX::Side_BUY));
      // message.setField(FIX::Text("Cancel My Order!"));

      // FIX::Session::sendToTarget(message);
      while (1)
	;

      // initiator.stop();
      return 0;
    }
  catch(FIX::ConfigError& e)
    {
      std::cout << e.what();
      return 1;
    }
}
