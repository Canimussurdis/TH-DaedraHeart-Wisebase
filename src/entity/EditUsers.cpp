#include "EditUsers.h"

#include <Wt/Dbo/Dbo.h>
#include <Wt/WApplication.h>
#include <Wt/WBreak.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WSignal.h>
//#include <Wt/WSignalMapper>
#include <Wt/WTemplate.h>
#include <Wt/WText.h>
#include <Wt/WAny.h>
#include <boost/lexical_cast.hpp>

using namespace Wt;
namespace dbo = Wt::Dbo;

EditUsers::EditUsers(dbo::Session& aSession, const std::string& basePath)
: session_(aSession), basePath_(basePath)
{
  setStyleClass("user-editor");
  setTemplateText(tr("edit-users-list"));
  auto limitEdit = std::make_unique<Wt::WLineEdit>();
  auto goLimit = std::make_unique<Wt::WPushButton>(tr("go-limit"));
  goLimit->clicked().connect(this, &EditUsers::limitList);

  limitEdit_ = bindWidget("limit-edit", std::move(limitEdit));
  bindWidget("limit-button", std::move(goLimit));
  limitList();
}

void EditUsers::limitList()
{
  auto listPtr = std::make_unique<Wt::WContainerWidget>();
  auto list = listPtr.get();
  bindWidget("user-list", std::move(listPtr));

  typedef dbo::collection<dbo::ptr<User> > UserList;
  dbo::Transaction t(session_);
  UserList users = session_.find<User>().where("name like ?").bind("%"+limitEdit_->text()+"%").orderBy("name");

  /*WSignalMapper<dbo::dbo_traits<User>::IdType >* userLinkMap = new WSignalMapper<dbo::dbo_traits<User>::IdType >(this);
  userLinkMap->mapped().connect(this,&EditUsers::onUserClicked);
  for (UserList::const_iterator i = users.begin(); i != users.end(); ++i) {
    WText* t = new WText((*i)->name, list);
    t->setStyleClass("link");
    new WBreak(list);
    userLinkMap->mapConnect(t->clicked(), (*i).id());
  }*/
  for (auto user : users) {
      Wt::WText* t = list->addWidget(std::make_unique<Wt::WText>(user->name));
      t->setStyleClass("link");
      list->addWidget(std::make_unique<Wt::WBreak>());
      t->clicked().connect(std::bind(&EditUsers::onUserClicked, this, user.id()));
  }
  if (!users.size())
      list->addWidget(std::make_unique<Wt::WText>(tr("no-users-found")));
}

void EditUsers::onUserClicked(Wt::Dbo::dbo_traits<User>::IdType id)
{
  wApp->setInternalPath(basePath_+"edituser/"+boost::lexical_cast<std::string>(id), true);
}


EditUser::EditUser(Wt::Dbo::Session& aSession)
: WTemplate(tr("edit-user")),
  session_(aSession),
  roleButton_(new WPushButton)
{
  auto roleButton = std::make_unique<Wt::WPushButton>();
  roleButton_ = bindWidget("role-button", std::move(roleButton));
  roleButton_->clicked().connect(this, &EditUser::switchRole);
}

void EditUser::switchUser(Wt::Dbo::ptr<User> target)
{
  target_ = target;
  bindTemplate();
}

void EditUser::bindTemplate()
{
  bindString("username", target_->name);
  if (target_->role == User::Admin)
    roleButton_->setText(tr("demote-admin"));
  else
    roleButton_->setText(tr("promote-user"));
}

void EditUser::switchRole()
{
  dbo::Transaction t(session_);
  target_.reread();
  if (target_->role == User::Admin)
    target_.modify()->role = User::Visitor;
  else
    target_.modify()->role = User::Admin;
  t.commit();
  bindTemplate();
}
