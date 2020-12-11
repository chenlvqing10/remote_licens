#ifndef UI_TIME_SETTING_H
#define UI_TIME_SETTING_H

#include "ui_base_widget.hpp"
#include <QPushButton>
#include <QLineEdit>
#include <QListView>
#include <QStandardItemModel>
#include "ui_time_setting_delegate.hpp"

class ui_time_setting : public ui_base_widget
{
    Q_OBJECT
public:
    ui_time_setting();
private  slots:
    void slot_for_save_btn_click();
    void slot_for_back_click();
    void slot_for_auto_btn_click();

    void slot_for_year_add();
    void slot_for_mon_add();
    void slot_for_day_add();
    void slot_for_hour_add();
    void slot_for_min_add();
    void slot_for_sec_add();

    void slot_for_year_sub();
    void slot_for_mon_sub();
    void slot_for_day_sub();
    void slot_for_hour_sub();
    void slot_for_min_sub();
    void slot_for_sec_sub();
    void slot_for_list_view_clicked(const QModelIndex &index);
private:
    QLabel *label_for_auto_text;
    clicked_Label *label_for_auto_btn;
    QLabel *label_for_manual_text;
    QLabel *label_for_time_view;

    clicked_Label *label_for_year_sub;
    clicked_Label *label_for_mon_sub;
    clicked_Label *label_for_day_sub;
    clicked_Label *label_for_hour_sub;
    clicked_Label *label_for_min_sub;
    clicked_Label *label_for_sec_sub;

    clicked_Label *label_for_year_add;
    clicked_Label *label_for_mon_add;
    clicked_Label *label_for_day_add;
    clicked_Label *label_for_hour_add;
    clicked_Label *label_for_min_add;
    clicked_Label *label_for_sec_add;

    QLineEdit *edit_for_year;
    QLineEdit *edit_for_mon;
    QLineEdit *edit_for_day;
    QLineEdit *edit_for_hour;
    QLineEdit *edit_for_min;
    QLineEdit *edit_for_sec;

    QLabel *label_for_year_text;
    QLabel *label_for_mon_text;
    QLabel *label_for_day_text;
    QLabel *label_for_hour_text;
    QLabel *label_for_min_text;

    QPushButton *save_btn;

    QListView *list_view;
    QStandardItemModel *model;
    ui_time_setting_delegate *delegate;
    int auto_enable;

    void hide_manual_time(bool enable);
};

#endif // UI_TIME_SETTING_H
