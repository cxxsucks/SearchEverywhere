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
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    qRegisterMetaType<QFileInfo>("QFileInfo");

    QTabWidget tabw;
    seev::NavWidget navw;
    tabw.addTab(&navw, QObject::tr("Home Page"));
    tabw.setTabsClosable(true);
    QObject::connect(&tabw, &QTabWidget::tabCloseRequested, 
        [&tabw] (int i) { if (i >= 1) delete tabw.widget(i); });

    tabw.show();
    QObject::connect(&navw, &seev::NavWidget::seevWidgetCreated,
        [&tabw] (QWidget* w) { tabw.addTab(w, w->windowIcon(), w->windowTitle()); });
    QObject::connect(&navw, &seev::NavWidget::seevWidgetCreated,
                     &tabw, &QTabWidget::setCurrentWidget);
    int ret = a.exec();
    while (tabw.count() > 1)
        delete tabw.widget(1);
    return ret;
}
