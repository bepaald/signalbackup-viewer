/*
  Copyright (C) 2019-2025  Selwin van Dijk

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

#include "cryptbase.ih"

bool CryptBase::getCipherAndMac(unsigned int hashoutputsize, size_t outputsize)
{
  std::unique_ptr<EVP_PKEY_CTX, decltype(&::EVP_PKEY_CTX_free)> pctx(EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, nullptr), &::EVP_PKEY_CTX_free);

  if (EVP_PKEY_derive_init(pctx.get()) != 1 ||
      EVP_PKEY_CTX_set_hkdf_md(pctx.get(), EVP_sha256()) != 1)
  {
    Logger::error("Failed to init HKDF");
    return false;
  }

  unsigned int const info_size = 13;
  unsigned char info[info_size] = {'B','a','c','k','u','p',' ','E','x','p','o','r','t'};
  //std::unique_ptr<unsigned char[]> localsalt(new unsigned char[hashoutputsize]);
  if (EVP_PKEY_CTX_set1_hkdf_key(pctx.get(), d_backupkey, d_backupkey_size) != 1 ||
      // EVP_PKEY_CTX_set1_hkdf_salt(pctx.get(), localsalt, hashoutputsize) != 1 ||
      EVP_PKEY_CTX_add1_hkdf_info(pctx.get(), info, info_size) != 1)
  {
    Logger::error("Failed to set data for HKDF");
    return false;
  }

  std::unique_ptr<unsigned char[]> derived(new unsigned char[outputsize]);
  if (EVP_PKEY_derive(pctx.get(), derived.get(), &outputsize) != 1)
  {
    Logger::error("Error deriving HKDF");
    return false;
  }

  d_cipherkey_size = hashoutputsize;
  d_cipherkey = new unsigned char[d_cipherkey_size];
  std::memcpy(d_cipherkey, derived.get(), hashoutputsize);

  d_mackey_size = hashoutputsize;
  d_mackey = new unsigned char[d_mackey_size];
  std::memcpy(d_mackey, derived.get() + hashoutputsize, hashoutputsize);

  return true;
}
