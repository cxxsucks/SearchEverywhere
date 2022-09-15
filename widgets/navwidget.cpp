#include "navwidget.hpp"
#include "./ui_navwidget.h"

#include <QtCore/QFile>
#include <QtCore/QJsonDocument>

namespace seev {

QString NavWidget::seevDefaultConfPath = 
    QString(::getenv("HOME")) + "/.config/orie/seev_default.txt";

NavWidget::NavWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::NavWidget)
    , _orie_app(_pool)
{
    ui->setupUi(this);
    ui->orieCmdEdit->setHidden(true);

    // Read default seev configuration
    QFile confStream(seevDefaultConfPath);
    confStream.open(QIODeviceBase::ReadOnly);
    fromJsonDoc(QJsonDocument::fromJson(confStream.readAll()));
}

NavWidget::~NavWidget()
{
    // Write seev configuration
    QJsonDocument seevConf = toJsonDoc();
    QFile confStream(ui->confPathEdit->text());
    confStream.open(QIODeviceBase::WriteOnly);
    confStream.write(seevConf.toJson());
    delete ui;
}

} // namespace seev
