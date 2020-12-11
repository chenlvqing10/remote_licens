/*
* ui_face_input.hpp - define face preview type of project
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

#ifndef UI_FACE_INPUT_H
#define UI_FACE_INPUT_H

extern "C" {
#include "ui_control.h"
}
#include "signal_for_ui.hpp"
#include "signal_table.hpp"
#include "QtMainWindow.hpp"

namespace Ui {
class ui_face_input;
class ui_face_input_v;
}

enum CursorStatus {DISABLE, HIDE, SHOW};

class ui_face_input : public QtMainWindow {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_face_input(QWidget* parent = 0);
  ~ui_face_input();
#ifndef PORTRAIT_SCREEN
  void mousePressEvent(QMouseEvent* event);
#endif
  void draw_face(int left, int top, int right,
                 int bottom, int type, QString name);
  void update_icon_status();

 public slots:
  void deinit();

 protected:
  void paintEvent(QPaintEvent*);

 private slots:
  void slot_show_password_login();
  void slot_for_draw();
  void slot_for_updatetime();
  void slot_for_web_socket_status(int event);
  void slot_for_storage(int event, int type);
  void slot_for_record();
  void slot_for_test_log();
  void slot_for_capture();

 private:
#ifndef PORTRAIT_SCREEN
  Ui::ui_face_input* ui;
#else
  Ui::ui_face_input_v* ui;
#endif
  QFont font;
  QPainter* painter;
  faces_t* face;
  SignalTable* signal_t;
  QTimer* timer;
  QRegion last_draw_region;
  QRegion cal_min_region(faces_t* face);
  void draw_debug_info();
  un_debug_switch_t debug_switch;
  algorithm_system_info_t p_info;
  struct viss_isp_exif exif[2];
  QString isp_nir_str;
  QString isp_rgb_str;
  bool is_record;
  int counter_for_menu;
  bool is_show_menu;
  int rec_time_len_;
  int qrcode_disp_remain_time;
  int is_display_qrcode;
};

#endif  //  UI_FACE_INPUT_H
