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

OriePredSelector::~OriePredSelector() {
    delete ui;
}

// All string literals end with a space but do not start with one
QString OriePredSelector::__genPathCommand() const {
    QString res;
    if (ui->strstrPathRadio->isChecked())
        res = QStringLiteral("-strstr ");
    else if (ui->rgxPathRadio->isChecked())
        res = QStringLiteral("-regex ");
    else if (ui->fnmatchRadio->isChecked())
        res = QStringLiteral("-name ");

    if (ui->icaseBox->isChecked())
        res += QStringLiteral("--ignore-case ");
    if (ui->fullPathBox->isChecked())
        res += QStringLiteral("--full ");
    if (ui->lnameBox->isChecked())
        res += QStringLiteral("--readlink ");

    (res += ui->pathPredEdit->text()) += ' ';
    return res;
}

QString OriePredSelector::__genContCommand() const {
    QString res;
    if (ui->strstrContRadio->isChecked())
        res = QStringLiteral("-content-strstr ");
    else if (ui->rgxContRadio->isChecked())
        res = QStringLiteral("-content-regex ");

    if (ui->icaseContBox->isChecked())
        res += QStringLiteral("--ignore-case ");
    if (ui->noCoucurContBox->isChecked())
        res += QStringLiteral("--blocked ");
    if (ui->allowBinContBox->isChecked())
        res += QStringLiteral("--binary ");
    (res += ui->pathPredEdit->text()) += ' ';
    return res;
}

QString OriePredSelector::__genFileStatCommand() const {
    static QString formatAtIdx[] = {
        QStringLiteral("-size"), QStringLiteral("-size"),
        QStringLiteral("-size"), QStringLiteral("-atime"),
        QStringLiteral("-mtime"), QStringLiteral("-ctime"),
        QStringLiteral("-amin"), QStringLiteral("-mmin"),
        QStringLiteral("-cmin"), QStringLiteral("-ctime"),
    };

    QString res = formatAtIdx[ui->statWhichCbo->currentIndex()];
    if (ui->gtRadio->isChecked())
        res += QStringLiteral(" + ");

    return QStringLiteral("Not Implemented");
}

QString OriePredSelector::__genModifCommand(const QString &modifName) const {
    return QStringLiteral("Not Implemented");
}

QString OriePredSelector::__genBridgeCommand(const QString &bridName) const {
    return QStringLiteral("Not Implemented");
}

} // namespace seev
