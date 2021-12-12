#include <Wt/WLineEdit.h>
#include <Wt/WTemplate.h>
#include <Wt/WText.h>
#include <Wt/Auth/PasswordService.h>
#include <Wt/Auth/RegistrationWidget.h>
#include "BlogLoginWidget.h"
#include "..//BlogSession.h"
#include "Token.h"
#include "../entity/User.h"


using namespace Wt;
using Auth::AuthModel;

BlogLoginWidget::BlogLoginWidget(BlogSession& session,
				 const std::string& basePath)
  : AuthWidget(session.login())
{
  setInline(true);

  auto model
	  = std::make_unique<Wt::Auth::AuthModel>(session.passwordAuth()->baseAuth(),
		  session.users());
  model->addPasswordAuth(session.passwordAuth());
  model->addOAuth(session.oAuth());

  setModel(std::move(model));

  setInternalBasePath(basePath + "login");
}

void BlogLoginWidget::createLoginView()
{
  AuthWidget::createLoginView();

  setTemplateText(tr("blog-login"));

  WLineEdit *userName = resolve<WLineEdit *>("user-name");
  userName->setPlaceholderText("login");
  userName->setToolTip("login");

  WLineEdit *password = resolve<WLineEdit *>("password");
  password->setPlaceholderText("password");
  password->setToolTip("password");
  password->enterPressed().connect(this, &BlogLoginWidget::attemptPasswordLogin);
}

void BlogLoginWidget::createLoggedInView()
{
  AuthWidget::createLoggedInView();

  auto logout = std::make_unique<Wt::WText>(tr("logout"));
  logout->setStyleClass("link");
  logout->clicked().connect(&login(), &Auth::Login::logout);
  bindWidget("logout", std::move(logout));
}
