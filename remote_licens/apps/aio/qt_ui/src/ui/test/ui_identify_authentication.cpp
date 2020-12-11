/*
* ui_identify_authentication.cpp -
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

#include "ui_identify_authentication.h"
#include "ui_ui_identify_authentication.h"

identify_authentication::identify_authentication(QWidget *parent) :
  QtMainWindow(parent),
  ui(new Ui::ui_identify_authentication) {
  ui->setupUi(this);
}

ui_identify_authentication::~ui_identify_authentication() {
  delete ui;
}

