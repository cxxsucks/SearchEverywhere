#ifndef SEEV_LOCATEJOBWIDGET_H
#define SEEV_LOCATEJOBWIDGET_H

#include <QWidget>

namespace seev {

namespace Ui {
class LocateJobWidget;
}

class LocateJobWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LocateJobWidget(QWidget *parent = nullptr);
    ~LocateJobWidget();

private:
    Ui::LocateJobWidget *ui;
};


} // namespace seev
#endif // SEEV_LOCATEJOBWIDGET_H
