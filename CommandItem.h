#ifndef COMMANDITEM_H
#define COMMANDITEM_H

#pragma once
#include <QString>
#include <QList>
#include <QIcon>

struct CommandItem {
    enum Type { Command, Menu };

    Type type;
    QString name;
    QIcon icon;
    QString cmd;  // 仅command使用
    QList<CommandItem> items; // 仅menu使用

    // 正确定义的构造函数
    CommandItem(Type t = Command,
                const QString& n = "",
                const QIcon& i = QIcon(),
                const QString& c = "",
                const QList<CommandItem>& subItems = {})
        : type(t), name(n), icon(i), cmd(c), items(subItems) {}
};

#endif // COMMANDITEM_H
