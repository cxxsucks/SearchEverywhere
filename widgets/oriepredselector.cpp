#include "oriepredselector.hpp"
#include "ui_oriepredselector.h"

namespace seev {

OriePredSelector::OriePredSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OriePredSelector)
{
    ui->setupUi(this);
    ui->accessBox->setHidden(true);
}

OriePredSelector::~OriePredSelector()
{
    delete ui;
}

} // namespace seev
