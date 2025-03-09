#ifndef COLUMNNAMES_H_
#define COLUMNNAMES_H_

#include <QSqlRecord>
#include <QString>

struct ColumnNames
{
  static QSqlRecord d_recipient_record;
  static QSqlRecord d_sms_record;
  static QSqlRecord d_message_record;
  static QSqlRecord d_attachment_record;

  static QString d_part_table;
  static QString d_mms_table;

  static QString d_recipient_aci ;
  static QString d_recipient_e164;
  static QString d_recipient_avatar_color;
  static QString d_recipient_system_joined_name;
  static QString d_recipient_profile_given_name;
  static QString d_recipient_storage_service;
  static QString d_recipient_type;
  static QString d_recipient_profile_avatar;
  static QString d_recipient_sealed_sender;

  static QString d_thread_recipient_id;
  static QString d_thread_message_count;
  static QString d_thread_delivery_receipts;
  static QString d_thread_read_receipts;

  static QString d_sms_date_received;
  static QString d_sms_recipient_id;
  static QString d_sms_recipient_device_id;

  static QString d_mms_delivery_receipts;
  static QString d_mms_read_receipts;
  static QString d_mms_viewed_receipts;
  static QString d_mms_date_sent;
  static QString d_mms_ranges;
  static QString d_mms_recipient_id;
  static QString d_mms_recipient_device_id;
  static QString d_mms_type;
  static QString d_mms_previews;

  static QString d_groups_v1_members;

  static QString d_part_mid;
  static QString d_part_ct;
  static QString d_part_pending;
  static QString d_part_cd;
  static QString d_part_cl;

  static void init();
};

#endif
