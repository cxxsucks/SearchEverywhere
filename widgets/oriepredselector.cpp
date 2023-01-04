#include "oriepredselector.hpp"
#include "ui_oriepredselector.h"

#ifdef _WIN32
#define fromStdString fromStdWString
#define toStdString toStdWString
#endif

namespace seev {

OriePredSelector::OriePredSelector(QWidget *parent)
    : QWidget(parent), ui(new Ui::OriePredSelector)
{
    ui->setupUi(this);
    connect(ui->predKind, &QComboBox::currentIndexChanged,
            this, &OriePredSelector::onPredKindChange);

    // By default match path
    ui->predKind->setCurrentIndex(2);
}

void OriePredSelector::onPredKindChange(int idx) {
    ui->accessBox->setHidden(true);
    ui->fileStatBox->setHidden(true);
    ui->fileTypeBox->setHidden(true);
    ui->contMatchBox->setHidden(true);
    ui->pathMatchBox->setHidden(true);
    ui->condModifBox->setHidden(true);

    switch (idx) {
    case 0: // -true
    case 1: // -false
        return;
    case 2: // -name
        ui->pathMatchBox->setHidden(false); return;
    case 3: // -type
        ui->fileTypeBox->setHidden(false); return;
    case 4: // -amctime -ugid -size
        ui->fileStatBox->setHidden(false); return;
    case 5: // -content-*
        ui->contMatchBox->setHidden(false); return;
    case 6: // -access
        ui->accessBox->setHidden(false); return;

    case 7: // -and
    case 8: // -or
        ui->condModifBox->setHidden(false); 
        if (qobject_cast<OriePredSelector*>(ui->lhsPredWidg) == nullptr) {
            delete ui->lhsPredWidg;
            ui->lhsPredWidg = new OriePredSelector;
            ui->condModifBoxLayout->insertWidget(0, ui->lhsPredWidg);
        }
        if (qobject_cast<OriePredSelector*>(ui->rhsPredWidg) == nullptr) {
            delete ui->rhsPredWidg;
            ui->rhsPredWidg = new OriePredSelector;
            ui->condModifBoxLayout->insertWidget(1, ui->rhsPredWidg);
        }
        return;

    case 9: // -not
    case 10: // -updir
    case 11: // -downdir
    case 12: // -prunemod
        ui->condModifBox->setHidden(false); 
        if (qobject_cast<OriePredSelector*>(ui->lhsPredWidg) == nullptr) {
            delete ui->lhsPredWidg;
            ui->lhsPredWidg = new OriePredSelector;
            ui->condModifBoxLayout->insertWidget(1, ui->lhsPredWidg);
        }
        delete ui->rhsPredWidg;
        ui->rhsPredWidg = nullptr;
        return;
    }
    // Unreachable
    std::terminate();
}

QString OriePredSelector::genCommand() const {
    switch (ui->predKind->currentIndex()) {
    case 0: // -true
        return QStringLiteral("-true");
    case 1: // -false
        return QStringLiteral("-false");
    case 2: // -name
        return __genPathCommand();
    case 3: // -type
        return __genFileTypeCommand();
    case 4: // -amctime -ugid -size
        return __genFileStatCommand();
    case 5: // -content-*
        return __genContCommand();
    case 6: // -access
        return __genAccessCommand();
    case 7: // -and
        return __genBridgeCommand(QStringLiteral("-and"));
    case 8: // -or
        return __genBridgeCommand(QStringLiteral("-or"));
    case 9: // -not
        return __genModifCommand(QStringLiteral("!"));
    case 10: // -updir
        return __genModifCommand(QStringLiteral("-updir"));
    case 11: // -downdir
        return __genModifCommand(QStringLiteral("-downdir"));
    case 12: // -prunemod
        return __genModifCommand(QStringLiteral("-prunemod"));
    }
    // Unreachable
    std::terminate();
}

OriePredSelector::~OriePredSelector() {
    delete ui;
}

QString OriePredSelector::__genPathCommand() const {
    QString res;
    if (ui->strstrPathRadio->isChecked())
        res = QStringLiteral("-strstr");
    else if (ui->rgxPathRadio->isChecked())
        res = QStringLiteral("-bregex");
    else if (ui->fnmatchRadio->isChecked())
        res = QStringLiteral("-name");
    else if (ui->fuzzPathRadio->isChecked())
        res = QStringLiteral("-fuzz --cutoff %1")
              .arg(ui->fuzzCutPathSpin->value());

    if (ui->icaseBox->isChecked() && !ui->fuzzPathRadio->isChecked())
        res += QStringLiteral("--ignore-case");
    if (ui->fullPathBox->isChecked())
        res += QStringLiteral("--full");
    if (ui->lnameBox->isChecked())
        res += QStringLiteral("--readlink");

    if (ui->pathPredEdit->text().contains('`'))
    // TODO: Prevent expression injection by banning '`' altogether is rude?
        res += QStringLiteral(" `` -a -false ");
    else 
        res += QStringLiteral(" `") += ui->pathPredEdit->text() 
            += QStringLiteral("` ");
    return res;
}

QString OriePredSelector::__genContCommand() const {
    if (ui->contPredEdit->text().isEmpty())
        return QStringLiteral("-true ");
    QString res;
    if (ui->strstrContRadio->isChecked())
        res = QStringLiteral("-content-strstr ");
    else if (ui->rgxContRadio->isChecked())
        res = QStringLiteral("-content-regex ");
    else if (ui->fuzzContRadio->isChecked())
        res = QStringLiteral("-content-fuzz --cutoff %1 ")
              .arg(ui->fuzzCutContSpin->value());

    if (ui->icaseContBox->isChecked() && !ui->fuzzContRadio->isChecked())
        res += QStringLiteral("--ignore-case ");
    if (!ui->coucurContBox->isChecked())
        res += QStringLiteral("--blocked ");
    if (ui->allowBinContBox->isChecked())
        res += QStringLiteral("--binary ");

    if (ui->contPredEdit->text().contains('`'))
    // TODO: Prevent expression injection by banning '`' altogether is rude?
        res += QStringLiteral(" `` -a -false ");
    else 
        res += QStringLiteral(" `") += ui->contPredEdit->text() 
            += QStringLiteral("` ");
    return res;
}

QString OriePredSelector::__genFileStatCommand() const {
    static QString formatAtIdx[] = {
        QStringLiteral("-size "), QStringLiteral("-size "),
        QStringLiteral("-size "), QStringLiteral("-atime "),
        QStringLiteral("-mtime "), QStringLiteral("-ctime "),
        QStringLiteral("-amin "), QStringLiteral("-mmin "),
        QStringLiteral("-cmin "), QStringLiteral("-ctime "),
    };

    QString res = formatAtIdx[ui->statWhichCbo->currentIndex()];
    if (ui->gtRadio->isChecked())
        res.push_back('+');
    else if (ui->ltRadio->isChecked())
        res.push_back('-');

    if (ui->absStatRadio->isChecked())
        res += QString::number(ui->absStatSpn->value());
    else 
        res += ui->targEdit->text().contains('`') ? QStringLiteral("``") :
               ui->targEdit->text();

    // MiB KiB and B suffix
    switch (ui->statWhichCbo->currentIndex()) {
    case 0: res.push_back('M'); break;
    case 1: res.push_back('k'); break;
    case 2: res.push_back('c'); break;
    }
    res.push_back(' ');
    return res;
}

QString OriePredSelector::__genAccessCommand() const {
    QString res = QStringLiteral("-access");
    if (ui->readableChk->isChecked())
        res += QStringLiteral(" --readable");
    if (ui->writableChk->isChecked())
        res += QStringLiteral(" --writable");
    if (ui->executableChk->isChecked())
        res += QStringLiteral(" --executable");
    if (res.back() != 'e')
        // None selected?
        return QStringLiteral("-false ");

    res.push_back(' ');
    return res;
}

QString OriePredSelector::__genFileTypeCommand() const {
    QString res = QStringLiteral("-type ");
    if (ui->regFileChk->isChecked())
        res += QStringLiteral("f,");
    if (ui->dirChk->isChecked())
        res += QStringLiteral("d,");
    if (ui->symlinkChk->isChecked())
        res += QStringLiteral("l,");
    if (ui->fifoChk->isChecked())
        res += QStringLiteral("p,");
    if (ui->blockDevChk->isChecked())
        res += QStringLiteral("b,");
    if (ui->charDevChk->isChecked())
        res += QStringLiteral("c,");

    if (res.back() != ',')
        // None selected?
        return QStringLiteral("-false ");
    // Redundant ',' do not affect searches in orient,
    // but does affect `find`
    res.push_back(' ');
    return res;
}

QString OriePredSelector::__genModifCommand(QString res) const {
    res += QStringLiteral(" ( ");
    OriePredSelector* sub = qobject_cast<OriePredSelector*>(ui->lhsPredWidg);
    if (nullptr == sub)
        res += QStringLiteral("-false ");
    else res += sub->genCommand();
    res += QStringLiteral(") ");
    return res;
}

QString OriePredSelector::__genBridgeCommand(QString res) const {
    OriePredSelector* lhs = qobject_cast<OriePredSelector*>(ui->lhsPredWidg);
    OriePredSelector* rhs = qobject_cast<OriePredSelector*>(ui->rhsPredWidg);
    QString lhsStr = QStringLiteral("( ") +
        (lhs ? lhs->genCommand() : QStringLiteral("-false ")) + QStringLiteral(") ");
    QString rhsStr = QStringLiteral(" ( ") +
        (rhs ? rhs->genCommand() : QStringLiteral("-false ")) + QStringLiteral(") ");
    return lhsStr + res + rhsStr;
}

} // namespace seev
