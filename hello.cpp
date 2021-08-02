#include <iostream>

#include <QLocale>
#include <QObject>
#include <QPushButton>
#include <QString>
#include <QTranslator>
#include <QtWidgets/QApplication>


using std::cout;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    const QLocale qlocale{QLocale::Spanish};
    QTranslator translator;
    bool loadSuccess = translator.load("hello_es");
    assert(loadSuccess);
    app.installTranslator(&translator);

    QString qhw = QPushButton::tr("Hello world!");  // QObject::tr doesn't work here.
    QPushButton button{qhw};
    button.resize(150, 50);
    button.show();
    return app.exec();
}
