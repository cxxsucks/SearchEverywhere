#include "orieconfwidget.hpp"
#include "ui_orieconfwidget.h"

namespace seev {

OrieConfWidget::OrieConfWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OrieConfWidget)
{
    ui->setupUi(this);
}

OrieConfWidget::~OrieConfWidget()
{
    delete ui;
}

} // namespace seev
