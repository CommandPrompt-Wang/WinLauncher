#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ConfigLoader.h"
#include "qdialog.h"
#include <QMainWindow>
#include <QHotkey>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QPushButton>
#include <QLayoutItem>
#include <QProcess>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void handleConfigError(const QString& error);
    void handleConfigLoaded(const QList<CommandItem>& items, QSize wndSize);

protected:
    void closeEvent(QCloseEvent *event) override;
    void changeEvent(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void toggleWindowVisibility();
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void showMainWindow();
    void quitApplication();
    void reloadConfig();
    void openConfig();

private:
    void createTrayIcon();
    void registerHotkey();
    void debugPrintItems(const QList<CommandItem>& items, int indent = 0);
    void buildMenuUI(const QList<CommandItem>& items);
    void buildSubMenu(const QList<CommandItem>& items, QMenu* parentMenu);
    void rearrangeButtons();
    void saveWindowSize();
    QPushButton* createCommandButton(const CommandItem& item);
    QPushButton* createMenuButton(const CommandItem& item);


    Ui::MainWindow *ui;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QHotkey *hotkey;
    QTimer *resizeTimer;

    ConfigLoader *m_configLoader;

    QString getConfigPath() {
        // 用户配置文件路径
        QString userConfig = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)
                             + "/config.json";

        return userConfig;
    }

};

#endif // MAINWINDOW_H
