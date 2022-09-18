#ifndef SEEV_ORIECONFWIDGET_HPP
#define SEEV_ORIECONFWIDGET_HPP

#include <QDialog>
#include <QWidget>

namespace orie {
class app;
}

namespace seev {

namespace Ui {
class OrieConfWidget;
class StartPathConfWidget;
}

class OrieConfWidget : public QWidget
{
    Q_OBJECT

public slots:
    void writeConfFile() const;

public:
    explicit OrieConfWidget(const QString& orieConfPath,
                            QWidget *parent = nullptr);
    ~OrieConfWidget();

private:
    Ui::OrieConfWidget *ui;
};

class StartPathConfDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartPathConfDialog(orie::app& orieApp,
                                 QWidget *parent = nullptr);
    ~StartPathConfDialog();

private:
    Ui::StartPathConfWidget *ui;
    orie::app& m_orieApp;
};

} // namespace seev
#endif // SEEV_ORIECONFWIDGET_HPP
