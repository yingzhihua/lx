//#include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFont>
#include "globalapplication.h"

#include "module/sqlitemgr.h"
#include "module/exglobal.h"
#include "module/sequence.h"
#include "module/log.h"
#include "module/dao/usermgr.h"

#include "module/dao/testmodel.h"
#include "module/dao/testresultmodel.h"
#include "module/dao/usermodel.h"
#include "module/dao/wifimodel.h"


int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    qputenv("LC_ALL","zh_CN.GB2312");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    GlobalApplication app(argc, argv);

    QFont font;
    font.setPixelSize(30);
    app.setFont(font);

    app.global = ExGlobal::getPtr();

    SqliteMgr::sqlitemgrinstance = new SqliteMgr();
    ExGlobal::pTestModel = new TestModel();
    ExGlobal::pTestResultModel = new TestResultModel();
    ExGlobal::pUserModel = new UserModel();
    ExGlobal::pWifiModel = new WifiModel();

    SqliteMgr::sqlitemgrinstance->conn(Log::getDir()+"/data.db","sa","123456");

    ExGlobal::exInit();

    usermgrinstance = new UserMgr();
    if (argc > 1 && strcmp(argv[1],"debug")==0)
        ExGlobal::setDebug(true);

    qmlRegisterSingletonType<ExGlobal>("Dx.Global",1,0,"ExGlobal",ExGlobal::exglobal_provider);
    qmlRegisterSingletonType<Sequence>("Dx.Sequence",1,0,"Sequence",Sequence::sequence_provider);
    qmlRegisterSingletonType<UserMgr>("Dx.UserMgr",1,0,"UserMgr",usermgr_provider);

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("testModel",ExGlobal::pTestModel);
    engine.rootContext()->setContextProperty("testResultModel",ExGlobal::pTestResultModel);
    engine.rootContext()->setContextProperty("userModel",ExGlobal::pUserModel);
    engine.rootContext()->setContextProperty("wifiModel",ExGlobal::pWifiModel);

    if (ExGlobal::LanguageCode == 0){
        QTranslator tor;
        tor.load(Log::getDir()+"/en_US.qm");
        app.installTranslator(&tor);
    }


    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    engine.addImageProvider("CodeImg",Sequence::getPtr()->imageProvider);

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
