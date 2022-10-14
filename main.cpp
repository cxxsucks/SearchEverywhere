#include "widgets/navwidget.hpp"

#include <QTabWidget>
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QFileInfo>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "seev_" + QLocale(locale).name();
        if (translator.load(baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    Q_INIT_RESOURCE(seev_rsc);
    Q_INIT_RESOURCE(previewFailTxt);
    QFile qssFile(":/qss/seev.qss");
    qssFile.open(QIODevice::ReadOnly);
    qApp->setStyleSheet(QString::fromLatin1(qssFile.readAll()));
    qRegisterMetaType<QFileInfo>("QFileInfo");

    QTabWidget tabw;
    seev::NavWidget navw;
    tabw.addTab(&navw, QObject::tr("Home Page"));
    tabw.setTabsClosable(true);
    tabw.resize(600 * 1.618, 600); // :D
    tabw.show();

    QObject::connect(&tabw, &QTabWidget::tabCloseRequested, 
        [&tabw] (int i) { if (i >= 1) delete tabw.widget(i); });
    QObject::connect(&navw, &seev::NavWidget::seevWidgetCreated,
        [&tabw] (QWidget* w) { tabw.addTab(w, w->windowIcon(), w->windowTitle()); });
    QObject::connect(&navw, &seev::NavWidget::seevWidgetCreated,
                     &tabw, &QTabWidget::setCurrentWidget);
    int ret = a.exec();
    while (tabw.count() > 1)
        delete tabw.widget(1);
    return ret;
}
