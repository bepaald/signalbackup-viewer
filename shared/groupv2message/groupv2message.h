/*
  Copyright (C) 2025  Selwin van Dijk

  This file is part of signalbackup-viewer.

  signalbackup-viewer is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  signalbackup-viewer is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with signalbackup-viewer.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef GROUPV2MESSAGE_H_
#define GROUPV2MESSAGE_H_

#include "../protobufparser/protobufparser.h"

typedef ProtoBufParser<protobuffer::optional::STRING, protobuffer::optional::STRING, protobuffer::optional::STRING,
                       protobuffer::optional::STRING, protobuffer::optional::STRING, protobuffer::optional::STRING,
                       protobuffer::optional::STRING> AvatarUploadAttributes;
typedef ProtoBufParser<protobuffer::optional::BYTES, protobuffer::optional::ENUM, protobuffer::optional::BYTES,
                       protobuffer::optional::BYTES, protobuffer::optional::UINT32> Member;
typedef ProtoBufParser<Member, protobuffer::optional::BYTES, protobuffer::optional::UINT64> PendingMember;
typedef ProtoBufParser<protobuffer::optional::BYTES, protobuffer::optional::BYTES, protobuffer::optional::BYTES,
                       protobuffer::optional::UINT64> RequestingMember;
typedef ProtoBufParser<protobuffer::optional::ENUM, protobuffer::optional::ENUM, protobuffer::optional::ENUM> AccessControl;
typedef ProtoBufParser<protobuffer::optional::BYTES, protobuffer::optional::BYTES, protobuffer::optional::STRING,
                       protobuffer::optional::BYTES, AccessControl, protobuffer::optional::UINT32, std::vector<Member>,
                       std::vector<PendingMember>, std::vector<RequestingMember>, protobuffer::optional::BYTES> Group;
typedef ProtoBufParser<protobuffer::optional::BYTES, protobuffer::optional::BYTES, protobuffer::optional::UINT32> GroupChange;
typedef ProtoBufParser<std::vector<ProtoBufParser<GroupChange, Group>>> GroupChanges;
//typedef ProtoBufParser<> GroupAttributeBlob;
//typedef ProtoBufParser<> GroupInviteLink;
typedef ProtoBufParser<protobuffer::optional::BYTES, protobuffer::optional::BYTES, protobuffer::optional::STRING,
                       protobuffer::optional::UINT32, protobuffer::optional::ENUM, protobuffer::optional::UINT32,
                       protobuffer::optional::BOOL> GroupJoinInfo;
typedef ProtoBufParser<protobuffer::optional::STRING> GroupExternalCredential;


class GroupV2Message
{
 public:
  GroupV2Message();
};

#endif
