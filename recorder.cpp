#include <iostream>

#include "quickfix/FileStore.h"
#include "quickfix/FileLog.h"
#include "quickfix/SocketAcceptor.h"
#include "quickfix/Session.h"
#include "quickfix/SessionSettings.h"
#include "quickfix/Application.h"

#include "recorder.hpp"

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
  std::cout << msg;
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
      if(argc < 2) return 1;
      std::string fileName = argv[1];

      FIX::SessionSettings settings(fileName);

      Recorder application = Recorder();
      FIX::FileStoreFactory storeFactory(settings);
      FIX::FileLogFactory logFactory(settings);
      FIX::SocketAcceptor acceptor
      	(application, storeFactory, settings, logFactory /*optional*/);
      acceptor.start();
      // while( condition == true ) { do something; }
      acceptor.stop();
      return 0;
    }
  catch(FIX::ConfigError& e)
    {
      std::cout << e.what();
      return 1;
    }
}
