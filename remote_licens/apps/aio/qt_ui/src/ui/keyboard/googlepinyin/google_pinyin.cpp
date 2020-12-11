/*
* google_pinyin.cpp - define function of googlepinyin method
*
* Copyright (C) 2019, LomboTech Co.Ltd.
* Author: lomboswer <lomboswer@lombotech.com>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifdef __cplusplus
extern "C" {
#endif
#define LOG_TAG     "ui"
#include <log/log.h>
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <QApplication>
#include <QVector>
#include <string.h>

#include "pinyinime.h"
#include "google_pinyin.hpp"

#define DICT          "dict_pinyin.dat"
#define DICT_USER     "dict_pinyin_user.dat"

using namespace ime_pinyin;

GooglePinyin::GooglePinyin(QObject *parent) :
      QObject(parent), enable(false), max_spelling_length(32),
      max_decoded_length(32), max_hz_length(20) {
}

GooglePinyin::~GooglePinyin() {
}

bool GooglePinyin::init(const QString &dir) {
  bool ret;
  QString app_dir = dir;

  if (app_dir.isEmpty())
    app_dir = qApp->applicationDirPath() + "/dict";
  ret = im_open_decoder(QString("%1/" DICT).arg(app_dir).toLocal8Bit().data(),
            QString("%1/" DICT_USER).arg(app_dir).toLocal8Bit().data());
  enable = ret;
  if (ret == false)
    return ret;

  im_set_max_lens(max_spelling_length, max_hz_length);
  im_reset_search();

  return ret;
}

void GooglePinyin::deinit() {
  if (enable) {
    im_close_decoder();
    enable = false;
  }
}

unsigned GooglePinyin::search(const QString &spell) {
  if (!enable)
    return 0;

  QByteArray bytearray(spell.toUtf8());
  char *pinyin(bytearray.data());
  size_t cand_num = im_search(pinyin, bytearray.size());

  return cand_num;
}

QString GooglePinyin::getCandidate(unsigned index) {
  char16 *cand_buf = new char16[max_hz_length];
  char16 *cand;
  QString cand_str;

  cand = im_get_candidate(index, cand_buf, max_hz_length);
  if (cand) {
      cand_str = QString::fromUtf16(cand);
      if (index == 0) {
          cand_str.remove(0, im_get_fixed_len());
      }
  } else {
      cand_str = "";
  }

  delete cand_buf;
  return cand_str;
}

QVector<int> GooglePinyin::spellingStartPositions() {
  const uint16 *spl_start;
  int len;
  // There will be len + 1 elements in the buffer when len > 0.
  len = static_cast<int>(im_get_spl_start_pos(spl_start));
  QVector<int> arr;

  arr.resize(len + 2);
  arr[0] = len;  // element 0 is used to store the length of buffer.
  for (int i = 0; i <= len; i++)
    arr[i + 1] = spl_start[i];

  return arr;
}

QString GooglePinyin::pinyinString(bool decoded) {
  size_t py_len;
  const char *py = im_get_sps_str(&py_len);

  if (!decoded)
    py_len = strlen(py);
  QString str = QString(QLatin1String(py));

  return str.mid(0, static_cast<int>(py_len));
}

int GooglePinyin::pinyinStringLength(bool decoded) {
  size_t py_len;
  const char *py = im_get_sps_str(&py_len);

  if (!decoded)
    py_len = strlen(py);

  return static_cast<int>(py_len);
}

int GooglePinyin::fixedLength() {
  return static_cast<int>(im_get_fixed_len());
}

int GooglePinyin::deleteSearch(int pos, bool isPosInSpellingId,
      bool clearFixedInThisStep) {
  if (pos <= 0)
    pos = 0;

  return static_cast<int>(im_delsearch(size_t(pos),
    isPosInSpellingId, clearFixedInThisStep));
}

void GooglePinyin::flushCache() {
    im_flush_cache();
}

