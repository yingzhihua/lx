#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QFont>

#include "module/sqlitemgr.h"
#include "module/exglobal.h"
#include "module/sequence.h"
#include "module/log.h"

int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QFont font;
    font.setPixelSize(30);
    app.setFont(font);

    sqlitemgrinstance = new SqliteMgr();
    sqlitemgrinstance->conn(Log::getDir()+"/data.db","sa","123456");
    //init dao manager
    //sqlitemgrinstance->init();
    ExGlobal::exInit();

    sequence = new Sequence();

    if (argc > 1 && strcmp(argv[1],"debug")==0)
        ExGlobal::setDebug(true);

    qmlRegisterSingletonType<ExGlobal>("Dx.Global",1,0,"ExGlobal",exglobal_provider);
    qmlRegisterSingletonType<Sequence>("Dx.Sequence",1,0,"Sequence",sequence_provider);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
