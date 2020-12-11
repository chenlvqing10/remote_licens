#ifndef UI_BASE_WIDGET_H
#define UI_BASE_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "clicked_label.hpp"

class ui_base_widget : public QWidget
{
    Q_OBJECT
public:
    explicit ui_base_widget(QWidget *parent = nullptr);
    ~ui_base_widget();
    void set_title_text(QString &text);
    QWidget *center_widget;
signals:
    void signal_back_click();
private slots:
    void slot_for_back_click();
private:
    QVBoxLayout *layout_v;
    QHBoxLayout *layout_h;

    QWidget *menu_widget;
    clicked_Label *click_label_back;
    QLabel *label_title;
};

#endif // UI_BASE_WIDGET_H
