#ifndef MY_PROGRESSBAR_H
#define MY_PROGRESSBAR_H

#include <QProgressBar>

class my_progressbar : public QProgressBar
{
public:
    explicit my_progressbar(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *);
private:
    void draw_bg(QPainter *painter);
    void draw_value(QPainter *painter);
};

#endif // MY_PROGRESSBAR_H
