#ifndef UI_VOLUM_SETTING_H
#define UI_VOLUM_SETTING_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QEventLoop>

#include "clicked_label.hpp"

class ui_volum_setting : public QWidget
{
    Q_OBJECT
public:
    explicit ui_volum_setting(QWidget *parent = nullptr,int type = 0);

signals:
protected:
    void paintEvent(QPaintEvent *);
    void closeEvent(QCloseEvent *event);
public:
    int exec();
public slots:
    void slot_for_add_clicked();
    void slot_for_sub_clicked();
    void slot_for_btn_clicked();
private:
    QWidget *small_dlg;
    QLabel *label_for_title;
    clicked_Label *label_for_sub;
    clicked_Label *label_for_add;
    QPushButton *btn;
    QLineEdit *edit;
    QFrame *ll;
    int _type;
    int value;
    QEventLoop *m_Loop;
};

#endif // UI_VOLUM_SETTING_H
