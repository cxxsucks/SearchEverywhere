#include "oriepredselector.hpp"
#include "ui_oriepredselector.h"

namespace seev {

QString OriePredSelector::genCommand() const {
    return QStringLiteral("Not Implemented");
}

OriePredSelector::OriePredSelector(QWidget *parent)
    : QWidget(parent), ui(new Ui::OriePredSelector)
{
    ui->setupUi(this);
    ui->accessBox->setHidden(true);
    ui->fileStatBox->setHidden(true);
    ui->fileTypeBox->setHidden(true);
    ui->contMatchBox->setHidden(true);
    ui->pathMatchBox->setHidden(true);
    ui->condModifBox->setHidden(true);
}

OriePredSelector::~OriePredSelector()
{
    delete ui;
}

QString OriePredSelector::__genPathCommand() const {
    return QStringLiteral("Not Implemented");
}

QString OriePredSelector::__genContCommand() const {
    return QStringLiteral("Not Implemented");
}

QString OriePredSelector::__genFileStatCommand() const {
    return QStringLiteral("Not Implemented");
}

QString OriePredSelector::__genModifCommand(const QString &modifName) const {
    return QStringLiteral("Not Implemented");
}

QString OriePredSelector::__genBridgeCommand(const QString &bridName) const {
    return QStringLiteral("Not Implemented");
}

} // namespace seev
