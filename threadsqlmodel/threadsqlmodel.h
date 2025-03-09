#ifndef THREADSQLMODEL_H_
#define THREADSQLMODEL_H_

#include <QStandardItemModel>
#include <QMap>
#include <QVariantList>

#include <QDebug>

#include "../conversationsqlmodel/conversationsqlmodel.h"
#include "../shared/framewithattachment/framewithattachment.h"

class ThreadSqlModel : public QStandardItemModel
{
  Q_OBJECT;

  /*
  enum SQLFIELDS : int
  {
   THREAD_ID = 0, THREAD_DATE, THREAD_MESSAGE_COUNT, THREAD_RECIPIENT_IDS, THREAD_SNIPPET, THREAD_SNIPPET_CS, THREAD_READ, THREAD_TYPE, THREAD_ERROR, THREAD_SNIPPET_TYPE, THREAD_SNIPPET_URI, THREAD_ARCHIVED, THREAD_STATUS, THREAD_DELIVERY_RECEIPT_COUNT, THREAD_EXPIRES_IN, THREAD_LAST_SEEN, THREAD_HAS_SENT, THREAD_READ_RECEIPT_COUNT, THREAD_UNREAD_COUNT,
   RECIPIENT_PREFERENCES_ID, RECIPIENT_PREFERENCES_RECIPIENT_IDS, RECIPIENT_PREFERENCES_BLOCK, RECIPIENT_PREFERENCES_NOTIFICATION, RECIPIENT_PREFERENCES_VIBRATE, RECIPIENT_PREFERENCES_MUTE_UNTIL, RECIPIENT_PREFERENCES_COLOR, RECIPIENT_PREFERENCES_SEEN_INVITE_REMINDER, RECIPIENT_PREFERENCES_DEFAULT_SUBSCRIPTION_ID, RECIPIENT_PREFERENCES_EXPIRE_MESSAGES, RECIPIENT_PREFERENCES_REGISTERED, RECIPIENT_PREFERENCES_SYSTEM_DISPLAY_NAME, RECIPIENT_PREFERENCES_SYSTEM_CONTACT_PHOTO, RECIPIENT_PREFERENCES_SYSTEM_PHONE_LABEL, RECIPIENT_PREFERENCES_SYSTEM_CONTACT_URI, RECIPIENT_PREFERENCES_PROFILE_KEY, RECIPIENT_PREFERENCES_SIGNAL_PROFILE_NAME, RECIPIENT_PREFERENCES_SIGNAL_PROFILE_AVATAR, RECIPIENT_PREFERENCES_PROFILE_SHARING_APPROVAL, RECIPIENT_PREFERENCES_CALL_RINGTONE, RECIPIENT_PREFERENCES_CALL_VIBRATE,
   GROUPS_ID, GROUPS_GROUP_ID, GROUPS_TITLE, GROUPS_MEMBERS, GROUPS_AVATAR, GROUPS_AVATAR_ID, GROUPS_AVATAR_KEY, GROUPS_AVATAR_CONTENT_TYPE, GROUPS_AVATAR_RELAY, GROUPS_TIMESTAMP, GROUPS_ACTIVE, GROUPS_AVATAR_DIGEST, GROUPS_MMS
  };
  */

  ConversationSqlModel *d_conversationmodel;
  //std::map<std::string, std::unique_ptr<BackupFrame>> *d_avatars;
  uint32_t d_databaseversion;
 public:
  inline ThreadSqlModel(/*std::map<std::string, std::unique_ptr<BackupFrame>> *avatars, */ConversationSqlModel *conversationmodel,
                        uint32_t dbv = std::numeric_limits<uint32_t>::max(), QObject *parent = nullptr);
  void setQuery(QString const &query);
 public slots:
  inline void setConversation(QModelIndex const &index);
 signals:
  void conversationSet();
};

inline ThreadSqlModel::ThreadSqlModel(ConversationSqlModel *conversationmodel, uint32_t dbv, QObject *parent)
  :
  QStandardItemModel(parent),
  d_conversationmodel(conversationmodel),
  d_databaseversion(dbv)
{}

inline void ThreadSqlModel::setConversation(QModelIndex const &index)
{
  QString tid = index.data(bepaald::ThreadIdRole).toString();
  bool groupthread = index.data(bepaald::IsGroupThreadRole).toBool();

  if (d_conversationmodel->setConversation(tid, groupthread))
  {
    emit conversationSet();
    return;
  }
}

#endif
