#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

#include "CommandItem.h"
#include <QObject>
#include <QString>
#include <QList>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDir>

class ConfigLoader : public QObject
{
    Q_OBJECT
public:
    explicit ConfigLoader(QObject *parent = nullptr);

    void loadFromFile(const QString &filePath);
    void loadFromJson(const QByteArray &jsonData);

signals:
    void configLoaded(const QList<CommandItem> &items, QSize wndSize);
    void errorOccurred(const QString &reason);

private:
    CommandItem parseCommandItem(const QJsonObject &obj);
    CommandItem parseMenuItem(const QJsonObject &obj);
    QIcon loadIcon(QString iconPath);
};

#endif // CONFIGLOADER_H
