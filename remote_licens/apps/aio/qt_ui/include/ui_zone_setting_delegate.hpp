#ifndef UI_ZONE_SETTING_DELEGATE_H
#define UI_ZONE_SETTING_DELEGATE_H

#include <QStyledItemDelegate>
typedef struct{
    QString text;
    QWidget *widget;
} data_zone_setting_delegate;

Q_DECLARE_METATYPE(data_zone_setting_delegate)
class ui_zone_setting_delegate : public QStyledItemDelegate
{
public:
    ui_zone_setting_delegate();
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // UI_ZONE_SETTING_DELEGATE_H
