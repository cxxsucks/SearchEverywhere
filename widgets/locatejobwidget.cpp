#include "locatejobwidget.h"
#include "ui_locatejobwidget.h"

namespace seev {

LocateJobWidget::LocateJobWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LocateJobWidget)
{
    ui->setupUi(this);
}

LocateJobWidget::~LocateJobWidget()
{
    delete ui;
}

} // namespace seev
