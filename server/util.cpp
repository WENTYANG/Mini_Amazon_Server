#include "util.h"
#include "Poco/Net/SMTPClientSession.h"
#include "Poco/Net/MailMessage.h"
#include "Poco/Exception.h"
#include "Poco/Net/InvalidCertificateHandler.h"
#include "Poco/Net/SecureStreamSocket.h"
#include "Poco/Net/AcceptCertificateHandler.h"
#include "Poco/Net/SSLManager.h"
#include "Poco/Net/SecureStreamSocket.h"
#include "Poco/Net/SecureSMTPClientSession.h"

using namespace std;
using namespace Poco::Net;
using Poco::Net::SecureSMTPClientSession;
using Poco::Exception;
using Poco::SharedPtr;
using Poco::Net::AcceptCertificateHandler;
using Poco::Net::SSLManager;
using Poco::Net::SecureStreamSocket;

bool send_email(string recipient, string subject, string content) {
    try {
      SharedPtr<InvalidCertificateHandler> pCert = new AcceptCertificateHandler(false);
      Context::Ptr pContext = new Poco::Net::Context(Context::CLIENT_USE, "", "", "", Context::VERIFY_NONE, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
      SSLManager::instance().initializeClient(0, pCert, pContext);

      SecureSMTPClientSession secure("smtp-mail.outlook.com", 587);
      secure.login();
      secure.startTLS(pContext);
      secure.login(SMTPClientSession::AUTH_LOGIN, "amazingMiniAmz@outlook.com", "Mini_amazon");

      MailMessage msg;
      msg.addRecipient(MailRecipient(MailRecipient::PRIMARY_RECIPIENT, recipient));
      msg.setSender("amazingMiniAmz@outlook.com");
      msg.setSubject(subject);
      msg.setContent(content);

      secure.sendMessage(msg);
      secure.close();
      std::cout << "Sent mail successfully!" << std::endl;
      return true;
    }
	  catch (Exception& exc) {
      std::cerr << "Failed to send mail: " << std::endl;
		  std::cout << exc.displayText() << std::endl;
    }
    catch (std::exception &e)
    {
      std::cerr << "Failed to send mail: " << e.what() << std::endl;
    }
    return false;
}
