#ifndef SEEV_ORIECONFWIDGET_HPP
#define SEEV_ORIECONFWIDGET_HPP

#include <QWidget>

namespace seev {

namespace Ui {
class OrieConfWidget;
}

class OrieConfWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OrieConfWidget(QWidget *parent = nullptr);
    ~OrieConfWidget();

private:
    Ui::OrieConfWidget *ui;
};


} // namespace seev
#endif // SEEV_ORIECONFWIDGET_HPP
