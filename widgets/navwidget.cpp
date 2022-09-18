#include "navwidget.hpp"
#include "orieconfwidget.hpp"
#include "locatejobwidget.hpp"
#include "oriepredselector.hpp"
#include "./ui_navwidget.h"

#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtWidgets/QDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>

namespace seev {

QString NavWidget::seevDefaultConfPath = 
    QString(::getenv("HOME")) + "/.config/orie/seev_default.txt";
QString NavWidget::orieDefaultConfPath = 
    QString(::getenv("HOME")) + "/.config/orie/default.txt";

NavWidget::NavWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::NavWidget)
    , m_orieApp(m_pool), m_lastUpdateTime(::time(nullptr))
{
    ui->setupUi(this);
    ui->orieCmdEdit->setHidden(true);
    ui->advOptBox->setHidden(true);

    connect(ui->updateDbBut, &QPushButton::clicked, this, 
            &NavWidget::updateDbButClicked);
    connect(ui->typeOrieCmdBut, &QPushButton::clicked, this, 
            &NavWidget::typeOrieCmdButClicked);
    connect(ui->seevConfPathBut, &QPushButton::clicked, this,
            &NavWidget::selSeevConfButClicked);
    connect(ui->searchEditDlgBut, &QPushButton::clicked, this,
            &NavWidget::showSearchDlg);

    connect(ui->savedSearchLst, &QListWidget::doubleClicked, this,
        [this] (const QModelIndex& idx) { emit seevWidgetCreated(new LocateJobWidget(
                m_savedSearches.at(idx.column()).toObject(), m_orieApp
        )); });
    connect(ui->initSearchBut, &QPushButton::clicked, this,
        [this] () { emit seevWidgetCreated(new LocateJobWidget(
            ui->predWidg->genCommand(), m_orieApp
        )); });

    connect(ui->seevConfPathEdit, &QLineEdit::returnPressed, this,
        [this] () {setSeevConfPath(ui->seevConfPathEdit->text());});
    connect(ui->openStartPathDlgBut, &QPushButton::clicked, this,
        [this] () { StartPathConfDialog(m_orieApp, this).exec(); });
    connect(ui->openConfWidgBut, &QPushButton::clicked, this,
        [this] () { emit seevWidgetCreated(new OrieConfWidget(
            QString::fromStdString(m_orieApp.conf_path())));
        });

    // Always launch with default seev configuration path
    setSeevConfPath(seevDefaultConfPath);
    m_orieApp.read_db()
        .add_start_path(NATIVE_PATH("/")) // TODO: edit start path
        .start_auto_update(std::chrono::seconds(ui->updIntSpin->value()));
}

void NavWidget::fromJsonDoc(const QJsonDocument& doc) {
    QString orieConfPath = doc["orieConfPath"].toString(orieDefaultConfPath);
    // Write default configuration if the specified orie conf file does not exist
    if (!m_orieApp.read_conf(orieConfPath.toStdString())) {
        m_orieApp = orie::app::os_default(m_pool);
        // TODO: Auto update stops here!
        m_orieApp.write_conf(orieConfPath.toStdString());
    }

    m_savedSearches = doc["savedSearches"].toArray();
    ui->savedSearchLst->clear();
    for (const QJsonValueRef& val : m_savedSearches) {
        ui->savedSearchLst->addItem(new QListWidgetItem(
            QIcon(val[QStringLiteral("iconPath")].toString()),
            val[QStringLiteral("description")].toString()
        ));
    }
    ui->updIntSpin->setValue(doc[QStringLiteral("updateDbInterval")].toInt(3600));
}

void NavWidget::setSeevConfPath(const QString &path) {
    ui->seevConfPathEdit->setText(path);

    // Read the seev configuration
    QFile confStream(path);
    confStream.open(QIODeviceBase::ReadOnly);
    // Set the internals 
    fromJsonDoc(QJsonDocument::fromJson(confStream.readAll()));
}

QJsonObject NavWidget::toJsonObj() const {
    QJsonObject res;
    res[QStringLiteral("savedSearches")] = m_savedSearches;
    res[QStringLiteral("orieConfPath")] =
        QString::fromStdString(m_orieApp.conf_path());
    res[QStringLiteral("updateDbInterval")] = ui->updIntSpin->value();
    return res;
}

NavWidget::~NavWidget()
{
    // Write seev configuration
    QJsonDocument seevConf;
    seevConf.setObject(toJsonObj());
    QFile confStream(ui->seevConfPathEdit->text());
    confStream.open(QIODeviceBase::WriteOnly);
    confStream.write(seevConf.toJson());
    delete ui;
}

void NavWidget::updateDbButClicked() {
    time_t nowClk = ::time(nullptr);
    if (nowClk < m_lastUpdateTime + 10) {
        QMessageBox::warning(this, tr("Scan Too Frequent"),
            tr("It is useless to frequently scan filesystem.\n"
               "Too much scanning can only burden your disks more."));
        return;
    }

    m_lastUpdateTime = nowClk;
    m_orieApp.stop_auto_update();
    m_orieApp.start_auto_update(std::chrono::seconds(ui->updIntSpin->value()));
}

void NavWidget::typeOrieCmdButClicked() {
    if (ui->orieCmdEdit->isHidden()) {
        ui->orieCmdEdit->setHidden(false);
        ui->typeOrieCmdBut->setText(tr("Search with this command"));
        return;
    }
    // If the command editor is not hidden,
    // the user would have already edited the command
    // therefore start a search
    emit seevWidgetCreated(new LocateJobWidget(
        ui->orieCmdEdit->toPlainText(), m_orieApp
    ));
    ui->typeOrieCmdBut->setText(tr("Type `orient` Command (Expert)"));
    ui->orieCmdEdit->setHidden(true);
}

void NavWidget::selSeevConfButClicked() {
    QString confPath = QFileDialog::getOpenFileName(
        this, tr("Select seev Conf File"), QDir::currentPath());
    if (confPath.isEmpty())
        return;
    ui->seevConfPathEdit->setText(confPath);
    setSeevConfPath(confPath);
}

void NavWidget::showSearchDlg() {
    QDialog exprEditDlg(this);
    auto editing = ui->predWidg;
    ui->newSearchLayout->removeWidget(editing);
    (new QHBoxLayout(&exprEditDlg))->addWidget(editing);
    exprEditDlg.exec();

    // Edit finished
    ui->newSearchLayout->addWidget(editing, 0, 0, 1, 3);
}

} // namespace seev
