#include "orieconfwidget.hpp"
#include "ui_orieconfwidget.h"
#include "ui_startpathconfwidget.h"
#include <orient/app.hpp>

namespace seev {

void OrieConfWidget::writeConfFile() const
{

}

OrieConfWidget::OrieConfWidget(const QString &orieConfPath, QWidget *parent) :
    QWidget(parent),  ui(new Ui::OrieConfWidget)
{
    ui->setupUi(this);
    Q_UNUSED(orieConfPath);
}

OrieConfWidget::~OrieConfWidget() {
    delete ui;
}

StartPathConfDialog::StartPathConfDialog(orie::app &orieApp, QWidget *parent)
    : QDialog(parent), m_orieApp(orieApp), ui(new Ui::StartPathConfWidget)
{
    ui->setupUi(this);
    // Load starting paths to list widget
    for (const orie::str_t& sp : m_orieApp.start_paths()) {
        ui->startPathLst->addItem(QString::fromStdString(sp));
    }
}

StartPathConfDialog::~StartPathConfDialog() {
    delete ui;
}

} // namespace seev
