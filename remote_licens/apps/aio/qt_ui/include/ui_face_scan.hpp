/*
* ui_face_scan.hpp - define face preview type of project
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

#ifndef UI_FACE_SCAN_H
#define UI_FACE_SCAN_H

extern "C" {
#include "ui_control.h"
}
#include "signal_for_ui.hpp"
#include "signal_table.hpp"
#include "QtMainWindow.hpp"
#include "ui_rotate_management.hpp"

class ui_rotate_management;

typedef struct tag_main_param main_param_t;
namespace Ui {
class ui_face_scan;
class ui_face_scan_v;
}

enum CursorStatus {DISABLE, HIDE, SHOW};

typedef enum
{
    EN_ITEM_DEV,  /*0*/
    EN_ITEM_MAC, /*1*/
    EN_ITEM_CNT, /*2*/
    EN_ITEM_IP, /*3*/
    EN_ITEM_COUNT
} EN_ITEM_ID;

typedef struct {
  QString icon;
  QString title;
  QString value;
  EN_ITEM_ID type;
}sys_item_t;

class ui_face_scan : public QtMainWindow {
  Q_OBJECT

 public:
  Q_INVOKABLE explicit ui_face_scan(QWidget* parent = 0);
  ~ui_face_scan();
#ifndef PORTRAIT_SCREEN
  void mousePressEvent(QMouseEvent* event);
#endif
  void draw_face(int left, int top, int right,
                 int bottom, int type, QString name);
  void draw_face_sim(int left, int top,
    int right, int bottom);
  void update_icon_status();
  void ui_init();
  void clear_layout(QLayout *layout);

 public slots:
  void deinit();

 protected:
  void paintEvent(QPaintEvent*);
  void paintMoreInfo();
  void update_temp_state(face_t *face);
  QRegion cal_min_region(faces_t* face);
  void draw_debug_info();

 private slots:
  void set_starting_service();
  void slot_show_password_login();
  void slot_for_eth_state(int state);
  void slot_for_wifi_state(int state);
  void slot_for_draw();
  void slot_for_updatetime();
  void slot_for_web_socket_status(int event);
  void slot_for_storage(int event, int type);
  void slot_for_record();
  void slot_for_test_log();
  void slot_for_capture();
  void slot_for_dev_state();
  void set_recg_result(int is_pass, QString name, QString statu);
  void set_qrcode_result(QString data);

 private:
#ifndef PORTRAIT_SCREEN
  Ui::ui_face_scan* ui;
#else
  Ui::ui_face_scan_v* ui;
#endif
  QFont font;
  QPainter* painter;
  QColor border_color;
  faces_t* face;
  SignalTable* signal_t;
  QTimer* timer;
  QRegion last_draw_region;
  main_param_t *m_params;
  struct viss_isp_exif exif[2];
  QString isp_nir_str;
  QString isp_rgb_str;
  bool is_record;
  bool is_show_menu;
  int counter_for_menu;
  int rec_time_len_;
  int qrcode_disp_remain_time;
  int is_display_qrcode;
  QList<sys_item_t> state_menu;
  ui_rotate_management *rotate;
  QMap<int, int> state_map;
  static bool isFaceServiceWorking;
};

#endif  //  UI_FACE_SCAN_H
