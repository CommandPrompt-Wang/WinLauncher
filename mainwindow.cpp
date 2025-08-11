#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAction>
#include <QMessageBox>
#include <QKeySequence>
#include <QMenu>
#include <Qtimer>
#include <QDesktopServices>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , trayIcon(nullptr)
    , trayIconMenu(nullptr)
    , hotkey(nullptr)
{
    ui->setupUi(this);

    createTrayIcon();

    registerHotkey();

    setWindowFlags(Qt::Window | Qt::WindowTitleHint |
                   Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint |
                   Qt::WindowStaysOnTopHint);

    setWindowState(Qt::WindowMinimized);

    m_configLoader = new ConfigLoader(this);
    connect(m_configLoader, &ConfigLoader::configLoaded,
            this, &MainWindow::handleConfigLoaded);
    connect(m_configLoader, &ConfigLoader::errorOccurred,
            this, &MainWindow::handleConfigError);
    reloadConfig();

    resizeTimer = new QTimer(this);
    resizeTimer->setSingleShot(true);
    resizeTimer->setInterval(50);
    connect(resizeTimer, &QTimer::timeout, this, &MainWindow::rearrangeButtons);
}

MainWindow::~MainWindow()
{
    delete hotkey;
    delete trayIconMenu;
    delete trayIcon;
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveWindowSize();
    hide(); // 隐藏窗口而不是退出
    event->ignore();
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange) {
        if (isMinimized()) {
            hide(); // 最小化时隐藏窗口
        }
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    resizeTimer->start();
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Escape){
        close();
    }
}

void MainWindow::rearrangeButtons()
{
    // 获取所有按钮
    QList<QPushButton*> buttons;
    for (int i = 0; i < ui->gridLayout->count(); ++i) {
        if (auto btn = qobject_cast<QPushButton*>(ui->gridLayout->itemAt(i)->widget())) {
            buttons.append(btn);
        }
    }
    if (buttons.isEmpty()) return;

    // 基础参数（逻辑像素）
    const int minHeight = 23;
    const int maxHeight = 40;
    const int spacing = ui->gridLayout->verticalSpacing();
    const int margin = ui->gridLayout->contentsMargins().top() +
                       ui->gridLayout->contentsMargins().bottom();
    const int availableHeight = height() - margin;
    const int buttonCount = buttons.size();

    // 列数计算 - 使用当前列数作为基准
    int currentColumns = qMax(1, ui->gridLayout->columnCount());

    // 计算当前列数下所需高度
    int itemsInColumn = qCeil(buttonCount / static_cast<qreal>(currentColumns));
    int neededHeightForCurrent = itemsInColumn * maxHeight + (itemsInColumn - 1) * spacing;

    // 尝试增加列数（当空间不足时）
    if (availableHeight < neededHeightForCurrent) {
        while (currentColumns < buttonCount) {
            int newItemsInColumn = qCeil(buttonCount / static_cast<qreal>(currentColumns + 1));
            int neededHeight = newItemsInColumn * minHeight + (newItemsInColumn - 1) * spacing;
            if (availableHeight >= neededHeight) {
                currentColumns++;
                break;
            }
            currentColumns++;
        }
    }
    // 尝试减少列数（当空间充足时）
    else {
        while (currentColumns > 1) {
            int newItemsInColumn = qCeil(buttonCount / static_cast<qreal>(currentColumns - 1));
            int neededHeight = newItemsInColumn * maxHeight + (newItemsInColumn - 1) * spacing;
            if (availableHeight < neededHeight) {
                break;
            }
            currentColumns--;
        }
    }

    // 清除旧布局
    while (QLayoutItem *item = ui->gridLayout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->setParent(nullptr);
        }
        delete item;
    }

    // 重新排列
    int rowsPerColumn = qCeil(buttonCount / static_cast<qreal>(currentColumns));
    int index = 0;

    for (int col = 0; col < currentColumns; ++col) {
        int itemsInThisColumn = (col < buttonCount % currentColumns || buttonCount % currentColumns == 0)
        ? rowsPerColumn : rowsPerColumn - 1;

        for (int row = 0; row < itemsInThisColumn; ++row) {
            if (index >= buttons.size()) break;
            ui->gridLayout->addWidget(buttons[index], row, col);
            index++;
        }
    }
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    QAction *showAction = new QAction(tr("显示主窗口(&W)"), this);
    QAction *reloadAction = new QAction(tr("重载配置(&R)"), this);
    QAction *opencfgAction = new QAction(tr("打开配置目录(&F)"), this);
    QAction *quitAction = new QAction(tr("退出(&X)"), this);

    connect(showAction,   &QAction::triggered, this, &MainWindow::showMainWindow);
    connect(opencfgAction,&QAction::triggered, this, &MainWindow::openConfig);
    connect(reloadAction, &QAction::triggered, this, &MainWindow::reloadConfig);
    connect(quitAction,   &QAction::triggered, this, &MainWindow::quitApplication);

    trayIconMenu->addAction(showAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(reloadAction);
    trayIconMenu->addAction(opencfgAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/icons/app_icon.ico"));
    trayIcon->setToolTip("Winlauncher");
    trayIcon->show();

    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayIconActivated);
}

void MainWindow::registerHotkey()
{
    hotkey = new QHotkey(Qt::META | Qt::SHIFT | Qt::Key_E, true, this);
    connect(hotkey, &QHotkey::activated, this, &MainWindow::toggleWindowVisibility);
}

void MainWindow::toggleWindowVisibility()
{
    if (isHidden()) {
        showNormal();
        activateWindow();
    } else {
        hide();
    }
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick) {
        toggleWindowVisibility();
    }
}

void MainWindow::showMainWindow()
{
    showNormal();
    activateWindow();
}

void MainWindow::reloadConfig()
{
    m_configLoader->loadFromFile(getConfigPath());
}

// 直接打开配置文件所在目录
void MainWindow::openConfig()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(getConfigPath()).path()));
}

void MainWindow::quitApplication()
{
    saveWindowSize();
    qApp->quit();
}

void MainWindow::handleConfigError(const QString& error)
{
    if (trayIcon && trayIcon->isVisible()) {
        trayIcon->showMessage(
            "配置错误",
            error,
            QSystemTrayIcon::Warning,
            3000 // 3秒后自动消失
            );
    } else {
        qCritical().noquote() << error; // 无托盘时至少记录日志
    }
}

#ifdef QT_DEBUG
void MainWindow::debugPrintItems(const QList<CommandItem>& items, int indent)
{
    const QString indentStr(indent * 2, ' ');

    for (const CommandItem& item : items) {
        // 打印基础信息
        qDebug().noquote() << indentStr
                           << (item.type == CommandItem::Menu ? "[菜单]" : "[命令]");

        // 打印图标信息（如果有）
        if (!item.icon.isNull() && !item.icon.name().isEmpty()) {
            qDebug().noquote() << indentStr << " 图标:" << item.icon.name();
        }

        // 命令类型打印执行命令
        if (item.type == CommandItem::Command) {
            qDebug().noquote() << indentStr << " 命令:" << item.cmd;
        }
        // 菜单类型打印子项
        else if (!item.items.isEmpty()) {
            qDebug().noquote() << indentStr << " 子项:";
            debugPrintItems(item.items, indent + 1);
        }
    }
}
#else
void MainWindow::debugPrintItems(const QList<CommandItem>&, int)
{}
#endif

void MainWindow::handleConfigLoaded(const QList<CommandItem>& items, QSize wndSize) {
    // 清空现有布局
    QLayoutItem* child;
    while ((child = ui->gridLayout->takeAt(0))) {
        delete child->widget();
        delete child;
    }

    qInfo() << "成功加载" << items.size() << "个配置项";
    debugPrintItems(items);

    // 构建UI
    buildMenuUI(items);

    resize(wndSize);
}

void MainWindow::buildMenuUI(const QList<CommandItem>& items)
{
    // 清除现有布局
    QLayoutItem* child;
    while ((child = ui->gridLayout->takeAt(0))) {
        delete child->widget();
        delete child;
    }

    const int minHeight = 20;  // 最小高度
    const int maxHeight = 30;  // 最大高度

    // 添加按钮到网格布局
    int row = 0, col = 0;
    for (const CommandItem& item : items) {
        QPushButton* btn = (item.type == CommandItem::Command) ?
                               createCommandButton(item) : createMenuButton(item);

        // 设置大小策略
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
        btn->setMinimumHeight(minHeight);
        btn->setMaximumHeight(maxHeight);

        ui->gridLayout->addWidget(btn, row, col);

        // 自动换列逻辑
        row++;
        if (row * maxHeight > this->height() - 50) { // 50为窗口边框和边距
            col++;
            row = 0;
        }
    }
}

QPushButton* MainWindow::createCommandButton(const CommandItem& item) {
    QPushButton* btn = new QPushButton(this);
    btn->setText(QString(" %1 ").arg(item.name));

    if (!item.icon.isNull()) {
        btn->setIcon(item.icon);
        btn->setIconSize(QSize(24, 24));
    }

    // 连接点击事件
    connect(btn, &QPushButton::clicked, [item, this]() {
        QStringList para = QProcess::splitCommand(item.cmd);
        QString cmd = para.takeFirst();
        QProcess::startDetached(cmd, para);
        qDebug() <<"exectue: " << item.cmd;
        this->hide();
    });

    return btn;
}

QPushButton* MainWindow::createMenuButton(const CommandItem& item) {
    QPushButton* btn = new QPushButton(this);
    btn->setText(QString(" %1 ").arg(item.name));

    if (!item.icon.isNull()) {
        btn->setIcon(item.icon);
        btn->setIconSize(QSize(24, 24));
    }

    // 创建下拉菜单
    QMenu* menu = new QMenu(btn);
    buildSubMenu(item.items, menu);
    btn->setMenu(menu);

    return btn;
}

void MainWindow::buildSubMenu(const QList<CommandItem>& items, QMenu* parentMenu) {
    for (const CommandItem& item : items) {
        if (item.type == CommandItem::Command) {
            QAction* action = parentMenu->addAction(item.icon, QString(" %1 ").arg(item.name));
            connect(action, &QAction::triggered, [item, this]() {
                QStringList para = QProcess::splitCommand(item.cmd);
                QString cmd = para.takeFirst();
                QProcess::startDetached(cmd, para);
                qDebug() <<"exectue: " << item.cmd;
                this->hide();
            });
        } else {
            // 嵌套子菜单
            QMenu* subMenu = parentMenu->addMenu(item.icon, QString(" %1 ").arg(item.name));
            buildSubMenu(item.items, subMenu);
        }
    }
}

void MainWindow::saveWindowSize()
{
    const QString configPath = getConfigPath();
    QFile configFile(configPath);

    if (!configFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qWarning() << "无法打开配置文件:" << configPath;
        return;
    }

    // 读取现有配置
    QJsonDocument doc = QJsonDocument::fromJson(configFile.readAll());
    QJsonObject config = doc.object();

    // 更新窗口尺寸
    QJsonArray windowSize;
    windowSize.append(width());
    windowSize.append(height());
    config["window"] = windowSize;

    // 写回文件
    configFile.resize(0);
    configFile.write(QJsonDocument(config).toJson());
    configFile.close();
}
