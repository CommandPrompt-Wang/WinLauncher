# WinLauncher
一个基于JSON的轻量启动器 / A JSON-based lightweight launcher
中文（当前） | [English](README_en.md)
## 截图
<img width="400" alt="软件截图1" src="https://github.com/user-attachments/assets/a08baa5f-2980-4767-b7ee-57216fcaa908" />
<img width="500" alt="软件截图2" src="https://github.com/user-attachments/assets/279df42e-92bc-4c56-8252-b13e8656767b" />
<img width="905" alt="软件截图3" src="https://github.com/user-attachments/assets/36479f43-4aeb-4189-b4b1-0e6612313585" />

## 特点
- 基于JSON快速配置
- 支持图标加载（Windows支持图标索引语法）
- 动态布局调整
- Qt6的原生夜间模式
- 跨平台

## 使用方法
1. [下载软件压缩包](https://github.com/CommandPrompt-Wang/WinLauncher/releases/latest)。并放在合适的位置（要卸载直接删掉就好）。
   > 按道理dll是齐的，Win下测了下几个环境是没问题的。不过如果你是《精简》汐统的话可能真会缺，要缺少的话就自行上网补吧。记得程序是64位的。
   > 未来的AppImage本身就是隔离环境。依赖也不必担心。
2. 启动程序
3. 在任务栏找到程序图标，右键，点击“打开配置目录(&F)”  
   <img width="200" alt="image" src="https://github.com/user-attachments/assets/e65da3c5-0df8-453e-88d9-e9c3f0952c9a" />  
   > 目录一般是`%USERPROFILE%\AppData\Local\WinLauncher`（Windows）或者`~/.config/WinLauncher`（Linux）、`~/Library/Preferences/WinLauncher`（MacOS，Qt文档这么说的，没测过。~~因为没苹果 逃.jpg~~）
4. 编写配置文件`config.json`（见下）
5. 再次右键任务栏的图标，点击“重载配置(R)”
6. 使用`META+SHIFT+E`打开窗口，可以压缩窗口高度以调整布局
   > `META`在Windows下对应`Win`键
   > 左键双击托盘图标也可以打开窗口
7. 拖动窗口底部可以调整窗口按钮布局，如视频所示：


https://github.com/user-attachments/assets/eeda2f93-cc39-4ed6-83b2-0aa882f45687


## JSON格式
### 配置示例
```json
{
    "buttons": [
        {
            "cmd": "cmd /c \"taskkill -im explorer.exe -f && explorer\"",
            "icon": "C:/Windows/explorer.exe,0",
            "name": "Restart &explorer.exe",
            "type": "command"
        },
        {
            "icon": "C:/windows/system32/shell32.dll,319",
            "items": [
                {
                    "cmd": "explorer \"E:\\Tools\\something.ahk\"",
                    "icon": "example.ico",
                    "name": "&1. An Example AHK",
                    "type": "command"
                },
                {
                    "cmd": "\"E:/Program Files/JiJiDown/JiJiDownForWPF.exe\"",
                    "icon": "E:/Program Files/JiJiDown/JiJiDownForWPF.exe,0",
                    "name": "&2. JijiDown",
                    "type": "command"
                }
            ],
            "name": "&List",
            "type": "menu"
        }
    ],
    "window": [
        174,
        102
    ]
}
```
一个典型的WinLauncher配置文件包含`buttons`和`window`两个主字段：
- `buttons`: 是一个JSON数组。子成员是JSON对象描述按钮的类型、图标、名称和操作
  - `type`: 描述按钮类型。可以是命令按钮（`command`）或者菜单按钮（`menu`）。
  - `name`: 描述按钮的名称。可以使用&设定Alt快捷键。
  - `icon`: 描述按钮的图标。可以是任何受`QIcon`支持的图片/图标文件
    > 在Windows下，还支持图标索引语法，比如`C:/Windows/system32/shell32.dll,114`。
    > 图标索引语法的路径如果含有空格，不需要**也不能**使用引号包裹。
  - `cmd`: `commamd`按钮独有的属性。描述按钮执行的命令行。如果是cli程序将会静默执行。
    > 这里的路径如果含有空格，**需要**使用引号包裹，比如: `"cmd": "\"E:/Program Files/JiJiDown/JiJiDownForWPF.exe\""`。
    > 有些程序，即使不是cli，也可能会静默执行。要避免这种静默执行的情况，可以尝试以下方式:
    > 1. **使用`start`命令**: Windows可以使用`cmd /c "start xxx"`可以强制弹出窗口，但要注意嵌套引号的转义问题。Linux可以使用`xdg-open`；
    > 2. **使用`explorer`执行**: Windows也可以考虑使用`explorer "xxx"`，这也可以打开部分程序；但是它不能携带参数启动。路径分隔符也只能使用`\`（当然由于是JSON，应该打`\\`），而不能使用`/`。
  - `items`: `list`按钮特有的属性。描述这个按钮的弹出菜单中的内容。子成员还是上述的JSON对象。
    > 允许一层层嵌套`menu`菜单。
- `window`: 也是一个JSON数组。第0、1个子成员是窗口的宽和高
  > `window`字段第2以及后面的元素会被忽略~~也就是"window":[234, 267, 114514, 1919810]是有效的~~
编写完配置后，记得右键单击托盘图标重新加载配置。然后打开窗口检查布局、图标、按钮行为是否正确。
  > 在配置文件有错误的情况下，WinLauncher可能会清空配置文件 ~~绝对不是我懒得写回滚（傲娇.jpg）~~。建议拷贝一份作副本。
然后就可以食用了。可以设置开机启动。WinLauncher会保留上次使用的布局。
  > 开机自启的相关内容不再赘述。Windows可以在`Shell:Startup`下创建快捷方式、添加任务计划或者修改注册表；Linux用户可以使用`systemd`；MacOS用户则可以从Dock管理启动项。具体方法自行查阅。

## 注意事项
1. 程序是静默启动，除非`META+SHIFT+E`或者双击任务栏图标窗口是不会打开的。只要在任务栏见到红黄蓝的立方而且没有报错通知，那就启动成功了
2. 快捷键是`META+SHIFT+E`（Windows下，`META`相当于`WIN`），可能有冲突，自行解决（可以先暂时不使用本软件；或者修改冲突软件的快捷键；或者改用双击任务栏图标。不过总之这是我的锅）。
   > 未来会增加修改快捷键的配置字段`hotkey`。（这个好说）
3. JSON写起来还是有点麻烦，可以考虑拿着模板到JSON编辑器里面复制粘贴（比如`https://www.json.cn/jsonedit/`）
   > 正在考虑写一个图形化配置界面。~~绝对不是挖坑（傲娇 - 副本.jpg）~~
4. Windows图标的路径识别逻辑和命令行的路径识别逻辑不甚一样，特别是图标索引语法。所以如果图标成功加载但是点击按钮没反应，请再检查下命令行是否正确（比如斜杠、引号啥的）。
5. 一定、一定记得备份好配置文件。

## TODO
1. `hotkey`字段
2. 图形化配置器
