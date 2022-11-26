#include "widgets/homepagewidget.hpp"

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
    qRegisterMetaType<QFileInfo>("QFileInfo");
    seev::AppWidget w;
    w.setWindowIcon(QIcon(QStringLiteral(":/search_img/stolenLogo.png")));
    w.show();
    return a.exec();
}
