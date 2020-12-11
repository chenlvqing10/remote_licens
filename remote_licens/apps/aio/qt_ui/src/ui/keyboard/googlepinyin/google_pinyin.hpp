/*
* google_pinyin.hpp - define interface of googlepinyin method
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

#ifndef GOOGLE_PINYIN_H
#define GOOGLE_PINYIN_H

#include <QObject>

class GooglePinyin : public QObject {
  Q_OBJECT

 public:
  explicit GooglePinyin(QObject *parent = 0);

  ~GooglePinyin();

  bool init(const QString &dir);

  void deinit();

  unsigned search(const QString &spell);

  QString getCandidate(unsigned index);

  QVector<int> spellingStartPositions();

  QString pinyinString(bool decoded);

  int pinyinStringLength(bool decoded);

  int fixedLength();

  int deleteSearch(int pos, bool isPosInSpellingId, bool clearFixedInThisStep);

  void flushCache();

 private:
  /* open google decoder */
  bool enable;

  int max_spelling_length;

  int max_decoded_length;

  int max_hz_length;
};

#endif  // GOOGLE_PINYIN_H
