#include "columnnames.ih"

#include <QSqlDatabase>
#include <QSqlDriver>

void ColumnNames::init() // static
{
  QStringList tables = QSqlDatabase::database().tables();

  // started at dbv 215
  d_part_table = "attachment";
  if (tables.contains("part") &&
      !tables.contains("attachment"))
    d_part_table = "part";

  // started at dbv 174
  d_mms_table = "message";
  if (tables.contains("mms") &&
      !tables.contains("message"))
    d_mms_table = "mms";

  d_recipient_record = QSqlDatabase::database().driver()->record("recipient");

  d_recipient_aci = "aci";
  if (d_recipient_record.contains("uuid")) // before dbv200
    d_recipient_aci = "uuid";

  d_recipient_e164 = "e164";
  if (d_recipient_record.contains("phone")) // before dbv201
    d_recipient_e164 = "phone";

  d_recipient_avatar_color = "avatar_color";
  if (d_recipient_record.contains("color")) // before dbv201
    d_recipient_avatar_color = "color";

  d_recipient_system_joined_name = "system_joined_name";
  if (d_recipient_record.contains("system_display_name")) // before dbv201
    d_recipient_system_joined_name = "system_display_name";

  d_recipient_profile_given_name = "profile_given_name";
  if (d_recipient_record.contains("signal_profile_name")) // before dbv201
    d_recipient_profile_given_name = "signal_profile_name";

  d_recipient_storage_service = "storage_service_id";
  if (!d_recipient_record.contains("storage_service_id") &&
      d_recipient_record.contains("storage_service_key"))
    d_recipient_storage_service = "storage_service_key";

  d_recipient_type = "type";
  if (!d_recipient_record.contains("type") &&    // before dbv201
      d_recipient_record.contains("group_type"))
    d_recipient_type = "group_type";

  d_recipient_profile_avatar = "profile_avatar";
  if (!d_recipient_record.contains("profile_avatar") &&    // before dbv201
      d_recipient_record.contains("signal_profile_avatar"))
    d_recipient_profile_avatar = "signal_profile_avatar";

  d_recipient_sealed_sender = "sealed_sender_mode";
  if (!d_recipient_record.contains("sealed_sender_mode") &&    // before dbv201
      d_recipient_record.contains("unidentified_access_mode"))
    d_recipient_sealed_sender = "unidentified_access_mode";


  QSqlRecord threadrecord = QSqlDatabase::database().driver()->record("thread");

  // started at dbv166
  d_thread_recipient_id = "recipient_id";
  // from dbv 108
  if (!threadrecord.contains("recipient_id") &&
      threadrecord.contains("thread_recipient_id"))
    d_thread_recipient_id = "thread_recipient_id";
  //earliest
  if (!threadrecord.contains("recipient_id") &&
      !threadrecord.contains("thread_recipient_id") &&
      threadrecord.contains("recipient_ids")) // before dbv108
    d_thread_recipient_id = "recipient_ids";

  // started at dbv166
  d_thread_message_count = "meaningful_messages";
  // before 166
  if (!threadrecord.contains("meaningful_messages") &&
      threadrecord.contains("message_count"))
    d_thread_message_count = "message_count";

  // from dbv211
  d_thread_delivery_receipts = "has_delivery_receipt";
  // before 211
  if (!threadrecord.contains("has_delivery_receipt") &&
      threadrecord.contains("delivery_receipt_count"))
    d_thread_delivery_receipts = "delivery_receipt_count";

  // from dbv211
  d_thread_read_receipts = "has_read_receipt";
  // before 211
  if (!threadrecord.contains("has_read_receipt") &&
      threadrecord.contains("read_receipt_count"))
    d_thread_read_receipts = "read_receipt_count";


  d_sms_record = QSqlDatabase::database().driver()->record("sms");

  // started at dbv166
  d_sms_date_received = "date_received";
  // before 166
  if (!d_sms_record.contains("date_received") &&
      d_sms_record.contains("date"))
    d_sms_date_received = "date";

  // started at dbv166
  d_sms_recipient_id = "recipient_id";
  // before 166
  if (!d_sms_record.contains("recipient_id") &&
      d_sms_record.contains("address"))
    d_sms_recipient_id = "address";

  // started at dbv166
  d_sms_recipient_device_id = "recipient_device_id";
  // before 166
  if (!d_sms_record.contains("recipient_device_id") &&
      d_sms_record.contains("address_device_id"))
    d_sms_recipient_device_id = "address_device_id";



  d_message_record = QSqlDatabase::database().driver()->record(d_mms_table);

  // from dbv211
  d_mms_delivery_receipts = "has_delivery_receipt";
  // before 211
  if (!d_message_record.contains("has_delivery_receipt") &&
      d_message_record.contains("delivery_receipt_count"))
    d_mms_delivery_receipts = "delivery_receipt_count";

  // from dbv211
  d_mms_read_receipts = "has_read_receipt";
  // before 211
  if (!d_message_record.contains("has_read_receipt") &&
      d_message_record.contains("read_receipt_count"))
    d_mms_read_receipts = "read_receipt_count";

  // from dbv211
  d_mms_viewed_receipts = "viewed";
  // before 211
  if (!d_message_record.contains("viewed") &&
      d_message_record.contains("viewed_receipt_count"))
    d_mms_viewed_receipts = "viewed_receipt_count";

  // started at dbv166
  d_mms_date_sent = "date_sent";
  // before 166
  if (!d_message_record.contains("date_sent") &&
      d_message_record.contains("date"))
    d_mms_date_sent = "date";

  // started at dbv166
  d_mms_ranges = "message_ranges";
  // before 166
  if (!d_message_record.contains("message_ranges") &&
      d_message_record.contains("ranges"))
    d_mms_ranges = "ranges";

  // started at dbv185
  d_mms_recipient_id = "from_recipient_id";
  // before 185
  if (!d_message_record.contains("from_recipient_id") &&
      d_message_record.contains("recipient_id"))
    d_mms_recipient_id = "recipient_id";
  // before 166
  if (!d_message_record.contains("recipient_id") &&
      d_message_record.contains("address"))
    d_mms_recipient_id = "address";

  // started at dbv185
  d_mms_recipient_device_id = "from_device_id";
  // before 185
  if (!d_message_record.contains("from_device_id") &&
      d_message_record.contains("recipient_device_id"))
    d_mms_recipient_device_id = "recipient_device_id";
  // before 166
  if (!d_message_record.contains("recipient_device_id") &&
      d_message_record.contains("address_device_id"))
    d_mms_recipient_device_id = "address_device_id";

  // started at dbv166
  d_mms_type = "type";
  // before 166
  if (!d_message_record.contains("type") &&
      d_message_record.contains("msg_box"))
    d_mms_type = "msg_box";

  // started at dbv166
  d_mms_previews = "link_previews";
  // before 166
  if (!d_message_record.contains("link_previews") &&
      d_message_record.contains("previews"))
    d_mms_previews = "previews";


  QSqlRecord grouprecord = QSqlDatabase::database().driver()->record("groups");

  d_groups_v1_members = "unmigrated_v1_members";
  if (!grouprecord.contains("unmigrated_v1_members") &&
      grouprecord.contains("former_v1_members"))
    d_groups_v1_members = "former_v1_members";



  d_attachment_record = QSqlDatabase::database().driver()->record(d_part_table);

  d_part_mid = "message_id";  // dbv 215
  if (!d_attachment_record.contains("message_id") &&
      d_attachment_record.contains("mid"))
    d_part_mid = "mid";

  d_part_ct = "content_type"; // dbv 215
  if (!d_attachment_record.contains("content_type") &&
      d_attachment_record.contains("ct"))
    d_part_ct = "ct";

  d_part_pending = "transfer_state"; // dbv 215
  if (!d_attachment_record.contains("transfer_state") &&
      d_attachment_record.contains("pending_push"))
    d_part_pending = "pending_push";

  d_part_cd = "remote_key"; // dbv 215
  if (!d_attachment_record.contains("remote_key") &&
      d_attachment_record.contains("cd"))
    d_part_cd = "cd";

  d_part_cl = "remote_location"; // dbv 215
  if (!d_attachment_record.contains("remote_location") &&
      d_attachment_record.contains("cl"))
    d_part_cl = "cl";
}
