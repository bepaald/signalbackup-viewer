#include "columnnames.ih"

QSqlRecord ColumnNames::d_recipient_record;
QSqlRecord ColumnNames::d_sms_record;
QSqlRecord ColumnNames::d_message_record;
QSqlRecord ColumnNames::d_attachment_record;

QString ColumnNames::d_part_table;
QString ColumnNames::d_mms_table;

QString ColumnNames::d_recipient_aci ;
QString ColumnNames::d_recipient_e164;
QString ColumnNames::d_recipient_avatar_color;
QString ColumnNames::d_recipient_system_joined_name;
QString ColumnNames::d_recipient_profile_given_name;
QString ColumnNames::d_recipient_storage_service;
QString ColumnNames::d_recipient_type;
QString ColumnNames::d_recipient_profile_avatar;
QString ColumnNames::d_recipient_sealed_sender;

QString ColumnNames::d_thread_recipient_id;
QString ColumnNames::d_thread_message_count;
QString ColumnNames::d_thread_delivery_receipts;
QString ColumnNames::d_thread_read_receipts;

QString ColumnNames::d_sms_date_received;
QString ColumnNames::d_sms_recipient_id;
QString ColumnNames::d_sms_recipient_device_id;

QString ColumnNames::d_mms_delivery_receipts;
QString ColumnNames::d_mms_read_receipts;
QString ColumnNames::d_mms_viewed_receipts;
QString ColumnNames::d_mms_date_sent;
QString ColumnNames::d_mms_ranges;
QString ColumnNames::d_mms_recipient_id;
QString ColumnNames::d_mms_recipient_device_id;
QString ColumnNames::d_mms_type;
QString ColumnNames::d_mms_previews;

QString ColumnNames::d_groups_v1_members;

QString ColumnNames::d_part_mid;
QString ColumnNames::d_part_ct;
QString ColumnNames::d_part_pending;
QString ColumnNames::d_part_cd;
QString ColumnNames::d_part_cl;
