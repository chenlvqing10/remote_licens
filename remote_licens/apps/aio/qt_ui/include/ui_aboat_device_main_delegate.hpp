#ifndef UI_ABOAT_DEVICE_MAIN_DELEGATE_HPP
#define UI_ABOAT_DEVICE_MAIN_DELEGATE_HPP

#include <QStyledItemDelegate>

typedef struct{
    QString icon_path;
    QString name;
    QString text;
	bool is_display;
} data_aboat_setting_delegate;

Q_DECLARE_METATYPE(data_aboat_setting_delegate)
class ui_aboat_device_main_delegate : public QStyledItemDelegate
{
public:
    ui_aboat_device_main_delegate();
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // UI_ABOAT_DEVICE_MAIN_DELEGATE_HPP
