#ifndef UI_TIME_SETTING_DELEGATE_H
#define UI_TIME_SETTING_DELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>

typedef struct{
    QString icon_path;
    QString text;
    QString text1;
    QWidget *widget;
    bool is_display;
} data_time_setting_delegate;

Q_DECLARE_METATYPE(data_time_setting_delegate)

class ui_time_setting_delegate : public QStyledItemDelegate
{
public:
    ui_time_setting_delegate();
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // UI_TIME_SETTING_DELEGATE_H
