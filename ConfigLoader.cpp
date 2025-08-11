#include "ConfigLoader.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

ConfigLoader::ConfigLoader(QObject *parent) : QObject(parent) {}

void ConfigLoader::loadFromFile(const QString &filePath)
{
    QFile file(filePath);

    // 处理文件不存在的情况
    if (!file.exists()) {
        QFile defaultFile(":/default/default_config.json");
        if (!defaultFile.open(QIODevice::ReadOnly)) {
            emit errorOccurred(tr("默认配置模板加载失败"));
            return;
        }

        // 直接创建目标文件
        QDir().mkpath(QFileInfo(filePath).path());
        if (!file.open(QIODevice::WriteOnly)) {
            emit errorOccurred(tr("无法创建配置文件: %1").arg(filePath));
            return;
        }
        if (file.write(defaultFile.readAll()) <= 0) {
            emit errorOccurred(tr("写入默认配置失败"));
            return;
        }
        file.close();
        qInfo() << "已创建默认配置文件:" << filePath;
    }

    // 继续正常加载流程
    if (!file.open(QIODevice::ReadOnly)) {
        emit errorOccurred(tr("无法打开文件: %1").arg(filePath));
        return;
    }

    loadFromJson(file.readAll());
}

void ConfigLoader::loadFromJson(const QByteArray &jsonData)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        emit errorOccurred(tr("JSON解析错误: %1").arg(parseError.errorString()));
        return;
    }

    if (!doc.isObject()) {
        emit errorOccurred(tr("配置格式错误：需要顶层对象"));
        return;
    }

    QList<CommandItem> items;
    const QJsonObject root = doc.object();
    QSize windowSize;

    // 新版格式解析 - 只处理buttons数组
    if (!root.contains("buttons") || !root["buttons"].isArray()) {
        emit errorOccurred(tr("配置格式错误：缺少buttons数组"));
        return;
    }

    const QJsonArray buttons = root["buttons"].toArray();
    for (const QJsonValue &btnValue : buttons) {
        if (!btnValue.isObject()) {
            qWarning() << "跳过非对象按钮配置";
            continue;
        }

        const QJsonObject btnObj = btnValue.toObject();
        const QString type = btnObj["type"].toString().toLower();

        CommandItem item;
        if (type == "command") {
            item = parseCommandItem(btnObj);
        } else if (type == "menu") {
            item = parseMenuItem(btnObj);
        } else {
            qWarning() << "未知按钮类型:" << type;
            continue;
        }

        if (!item.name.isEmpty()) {
            items.append(item);
        }
    }

    if (root.contains("window") && root["window"].isArray()) {
        const QJsonArray sizeArray = root["window"].toArray();
        if (sizeArray.size() >= 2) {
            windowSize.setWidth(sizeArray[0].toInt());
            windowSize.setHeight(sizeArray[1].toInt());
        }
    }

    emit configLoaded(items, windowSize);
}

CommandItem ConfigLoader::parseCommandItem(const QJsonObject &obj)
{
    CommandItem item;
    item.type = CommandItem::Command;
    item.name = obj["name"].toString();
    item.cmd = obj["cmd"].toString();


    QString iconPath = obj["icon"].toString();

    if (!iconPath.isEmpty()) {
        item.icon = loadIcon(QFileInfo(iconPath).absoluteFilePath());
    }

    // 验证必要字段
    if (item.name.isEmpty() || item.cmd.isEmpty()) {
        qWarning() << "命令项缺少必要字段:" << obj;
        return CommandItem(); // 返回空对象将被上层过滤
    }

    return item;
}

CommandItem ConfigLoader::parseMenuItem(const QJsonObject &obj)
{
    CommandItem item;
    item.type = CommandItem::Menu;
    item.name = obj["name"].toString();

    // 处理图标
    QString iconPath = obj["icon"].toString();
    if (!iconPath.isEmpty()) {
        item.icon = loadIcon(QFileInfo(iconPath).absoluteFilePath());
    }

    // 解析子项（必须为数组）
    if (!obj.contains("items") || !obj["items"].isArray()) {
        qWarning() << "菜单项缺少items数组:" << obj;
        return CommandItem();
    }

    const QJsonArray itemsArray = obj["items"].toArray();
    for (const QJsonValue &itemValue : itemsArray) {
        if (!itemValue.isObject()) {
            qWarning() << "跳过非对象菜单子项";
            continue;
        }

        const QJsonObject itemObj = itemValue.toObject();
        const QString itemType = itemObj["type"].toString().toLower();

        CommandItem subItem;
        if (itemType == "command") {
            subItem = parseCommandItem(itemObj);
        } else if (itemType == "menu") {
            subItem = parseMenuItem(itemObj); // 递归解析嵌套菜单
        } else {
            qWarning() << "未知子项类型:" << itemType;
            continue;
        }

        if (!subItem.name.isEmpty()) {
            item.items.append(subItem);
        }
    }

    // 验证菜单有效性
    if (item.name.isEmpty() || item.items.isEmpty()) {
        qWarning() << "无效菜单项:" << obj;
        return CommandItem();
    }

    return item;
}

QIcon ConfigLoader::loadIcon(QString iconPath){
    if (iconPath.isEmpty()) {
        return QIcon();
    }

    QIcon icon;

#ifdef Q_OS_WIN
    if (iconPath.contains(',')) {
        QStringList parts = iconPath.split(',');
        if (parts.size() == 2) {
            QString dllPath = parts[0].trimmed();
            int iconIndex = parts[1].toInt();

            // 使用 Windows API 加载图标
            HICON hIcon = ExtractIconW(GetModuleHandleW(NULL),
                                       dllPath.toStdWString().c_str(),
                                       iconIndex);
            if (hIcon) {
                // 将 HICON 转换为 QPixmap
                QPixmap pixmap = QPixmap::fromImage(QImage::fromHICON(hIcon));
                DestroyIcon(hIcon);
                if (!pixmap.isNull()) {
                    icon.addPixmap(pixmap);
                    return icon;
                }
            }
        }
    }
#endif

    icon = QIcon(iconPath);

    return icon;
}
