#ifndef UI_ABOAT_DEVICE_MAIN_HPP
#define UI_ABOAT_DEVICE_MAIN_HPP

#include <QObject>
#include <QListView>
#include <QModelIndex>
#include <QStandardItemModel>

#include "ui_base_widget.hpp"
#include "ui_aboat_device_main_delegate.hpp"

#define CLICK_ALG_VERSION_COUNT 5

class ui_aboat_device_main : public ui_base_widget
{
    Q_OBJECT
public:
    ui_aboat_device_main();
    ~ui_aboat_device_main();
private slots:
    void slot_for_list_view_clicked(const QModelIndex &index);
    void slot_for_back_click();
private:
    QListView *list_view;
    QStandardItemModel *pModel;
    ui_aboat_device_main_delegate *pItemDelegate;
    int click_count_;
    int development_status;
    bool clicked;
};

#endif // UI_ABOAT_DEVICE_MAIN_HPP
