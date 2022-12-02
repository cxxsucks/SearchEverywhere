#include "widgets/homepagewidget.hpp"
#include "widgets/orieconfwidget.hpp"

#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QAbstractButton>
#include <QtCore/QLocale>
#include <QtCore/QTranslator>
#include <QtCore/QFileInfo>
#include <QtCore/QProcess>

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

    if (!QFileInfo::exists(seev::HomePageWidget::orieDefaultConfPath)) {
        // First time running because default config missing.
        orie::fifo_thpool defaultPool;
        orie::app defaultApp(orie::app::os_default(defaultPool));
        // Show a welcome dialog that helps user configure `orient` root paths,
        // while also updatedb in the background.
        QDialog welcomeDlg;
        QVBoxLayout *layout = new QVBoxLayout(&welcomeDlg);
        QLabel* label = new QLabel(QObject::tr(
            "First Time Configuration!\n To start searches, an index must be "
            "created. Creating such index scans the entire filesystem from root "
            "paths, pruning ignored paths, which can be set here. \nIt is not "
            "recommended to remove C:\\Windows from ignored path now -- you can "
            "do it later.\nIs this good for you?"
        ));
        label->setWordWrap(true);
        QFont font = label->font(); font.setPointSize(13); label->setFont(font);
        layout->addWidget(label);
        seev::OrieConfWidget* confWidg = 
            new seev::OrieConfWidget(seev::HomePageWidget::orieDefaultConfPath);
        layout->addWidget(confWidg);
        QObject::connect(confWidg, &seev::OrieConfWidget::confDone,
                         &welcomeDlg, &QDialog::close);

        QMessageBox confDoneBox(
            QMessageBox::Information, QObject::tr("Done Configuration"),
            QObject::tr("Configuration finished. Wait for first index to finish, "
                        "which makes the OK button clickable, click it and enjoy!"
                        "\nThis may take several seconds or minutes depending on "
                        "your disk speed."),
            QMessageBox::Ok | QMessageBox::Cancel
        );
        confDoneBox.button(QMessageBox::Ok)->setDisabled(true);

        // The connection is just for bringing to GUI thread
        QObject::connect(&welcomeDlg, &QDialog::rejected, 
            [&] () {confDoneBox.button(QMessageBox::Ok)->setEnabled(true);});
        // Updatedb in the background while user is configuring
        std::thread updateThread([&] () {
            defaultApp.update_db(); 
            emit welcomeDlg.rejected(); 
        });

        welcomeDlg.exec();
        if (confDoneBox.exec() != QMessageBox::Ok) {
            // Indexing cancellation requested, but we cannot cancel thread
            // execution, therefore simply restart the program.
            QProcess::startDetached(argv[0]);
            _exit(1);
        }
        updateThread.join();
    }

    // Show main page
    seev::AppWidget w;
    w.resize(445, 720);
    w.show();
    return a.exec();
}
