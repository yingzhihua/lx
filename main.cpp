//#include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QFont>

#include "module/sqlitemgr.h"
#include "module/exglobal.h"
#include "module/sequence.h"
#include "module/log.h"
#include "module/dao/usermgr.h"

int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    QFont font;
    font.setPixelSize(30);
    app.setFont(font);

    sqlitemgrinstance = new SqliteMgr();
    sqlitemgrinstance->conn(Log::getDir()+"/data.db","sa","123456");
    //init dao manager
    //sqlitemgrinstance->init();
    ExGlobal::exInit();

    sequence = new Sequence();
    usermgrinstance = new UserMgr();
    if (argc > 1 && strcmp(argv[1],"debug")==0)
        ExGlobal::setDebug(true);

    qmlRegisterSingletonType<ExGlobal>("Dx.Global",1,0,"ExGlobal",exglobal_provider);
    qmlRegisterSingletonType<Sequence>("Dx.Sequence",1,0,"Sequence",sequence_provider);
    qmlRegisterSingletonType<UserMgr>("Dx.UserMgr",1,0,"UserMgr",usermgr_provider);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    engine.addImageProvider("CodeImg",sequence->imageProvider);
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
