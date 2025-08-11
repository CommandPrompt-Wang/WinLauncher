#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLocalServer>
#include <QLocalSocket>
#include <QMessageBox>
#include <QTimer>

bool checkInstance(const QString& serverName)
{
    QLocalSocket socket;
    socket.connectToServer(serverName);
    if (socket.waitForConnected(500)) {
        // 已有一个实例在运行，发送关闭命令
        socket.write("terminate");
        socket.waitForBytesWritten();
        socket.disconnectFromServer();

        QLocalServer::removeServer(serverName);

        return true;
    }

    // 创建新的服务器实例
    QLocalServer::removeServer(serverName);
    QLocalServer* server = new QLocalServer();
    if (!server->listen(serverName)) {
        qWarning() << "Failed to create single instance server";
        return true; // 虽然创建失败，但允许继续运行
    }

    QObject::connect(server, &QLocalServer::newConnection, [server]() {
        QLocalSocket* socket = server->nextPendingConnection();
        if (socket && socket->waitForReadyRead(1000)) {
            QByteArray data = socket->readAll();
            if (data == "terminate") {
                // 收到终止命令，退出应用
                QCoreApplication::quit();
            }
        }
        delete socket;
    });

    return true;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    const QString serverName = "WinLauncher_Instance";

    // 尝试关闭已有实例
    checkInstance(serverName);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        QLocale loc(locale);
        // 如果是中文相关语言，跳过不加载任何翻译
        if (loc.language() == QLocale::Chinese) {
            break;
        }

        const QString baseName = "WinLauncher_" + loc.name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    MainWindow w;
    // w.show();
    return a.exec();
}
