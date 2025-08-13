# WinLauncher
一个基于JSON的轻量启动器 / A JSON-based lightweight launcher   
  
[中文](README.md) | English (current)

## Screenshots

<img width="400" alt="Screenshot 1" src="https://github.com/user-attachments/assets/a08baa5f-2980-4767-b7ee-57216fcaa908" />
<img width="500" alt="Screenshot 2" src="https://github.com/user-attachments/assets/279df42e-92bc-4c56-8252-b13e8656767b" />
<img width="905" alt="Screenshot 3" src="https://github.com/user-attachments/assets/36479f43-4aeb-4189-b4b1-0e6612313585" />

## Features

- Rapid configuration via JSON
- Icon loading support (Windows supports icon index syntax)
- Dynamic layout adjustment
- Native Qt6 dark mode
- Cross-platform compatibility

## Usage

1. [Download the latest release](https://github.com/CommandPrompt-Wang/WinLauncher/releases/latest) and extract to your preferred location (uninstall by simply deleting)  
   > DLLs are included and tested on common Windows environments.  
   > Stripped-down Windows versions might require manual dependency installation.  
   > Note: Supported Windows version is 64-bit, check you downloaded dll.  
   > Future AppImage releases will include all dependencies.  
2. Launch the application
3. Right-click the tray icon and select "Open Config &Folder"  
   <img width="200" alt="Tray icon menu" src="https://github.com/user-attachments/assets/76653046-5c62-4c55-90b8-e8c9d654a0a4" />  
   > Default paths:  
   > - Windows: `%USERPROFILE%\AppData\Local\WinLauncher`  
   > - Linux: `~/.config/WinLauncher`  
   > - macOS: `~/Library/Preferences/WinLauncher` (untested, per Qt documentation)  
4. Create/edit `config.json` (see format below)
5. Right-click tray icon → "&Reload Config"
6. Open launcher with `META+SHIFT+E` (adjust layout by compressing window height)
   > `META` = `Win` key on Windows  
   > Double-click tray icon also opens window
7. Drag window bottom edge to adjust button layout (see demo):

https://github.com/user-attachments/assets/eeda2f93-cc39-4ed6-83b2-0aa882f45687

## JSON format
### Configuration Example
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
    "hotkey": "META+Shift+E",
    "window": [
        174,
        102
    ]
}
```
Result:  
<img width="200" alt="Example configuration" src="https://github.com/user-attachments/assets/08f1581a-94ae-4cf1-b096-803fd2dd7c7f" />  

A typical WinLauncher configuration file contains three main fields: `buttons`, `hotkey` and `window`:
- `buttons`: It is a JSON array. The sub-members are JSON objects describing the type, icon, name, and action of the buttons
  - `type`: Describes the type of button. Can be a command button (`command`) or a menu button (`menu`).
  - `name`: Describes the name of the button. You can use & to set the Alt shortcut key.
  - `icon`(Optional): Describes the icon of the button. Can be any image/icon file supported by `QIcon`
    > On Windows, it also supports icon index syntax, such as `C:/Windows/system32/shell32.dll,114`.
    > If the path of the icon index syntax contains spaces, it **NEED NOT** to be enclosed in quotation marks.
  - `cmd`: `command` only. Describes the command line executed by the button. If it is a CLI program, it will be executed silently.
    > If the path here contains spaces, it **NEEDS** to be enclosed in quotation marks, eg: `"cmd": "\"E:/Program Files/JiJiDown/JiJiDownForWPF.exe\""`.
For some programs, even if they are not command-line interfaces (CLI), may execute silently. To avoid such silent execution, you can try the following methods:
    > 1.  **Using the `start` command**: On Windows, you can force a window to pop up using `cmd: "cmd /c \"start ^ \"an app.exe^\""` ~~What the hell is this!!!~~;  
    >   > `\"`is nested quotation mark in JSON, `^` is the escape char in `cmd`. `^\"` does two-level escaping (nested like Russian dolls).     
    > 2.  **Using `explorer` to execute**: Windows user can also consider using `cmd: "explorer \"C:\\app.exe\""`, which can also open some programs; however, This method cannot launch programs with parameters. The path separator can be ONLY `\` (of course, in JSON, it should be `\\`), and cannot use `/`;
    > 3.  **For Linux**: Linux user can use `xdg-open`
  - `items`: `list` only. Describes the content of the pop-up menu for this button. The sub-member is still the aforementioned JSON object.
    > Nested `menu` is allowed
- `hotkey`: A string defining the keyboard shortcut to show/hide the main window.
  > On Windows, `Win` and `Windows` will be converted to `META`  
  > If the hotkey is invalid, it will fall back to `META+SHIFT+E`  

- `window`: It is also a JSON array. Format: `[width, height]` (in pixels)
  > Additional elements are ignored. ~~That is, "window": [234, 567, 114514, 1919810] is valid~~
  > Auto-updated on program/window close. 

After completing the configuration, remember to:
1. right-click the tray icon to reload the configuration.
2. Open the window to check whether the layout, icons, and button behaviors are correct.  
  > In case of errors in the configuration file, WinLauncher may clear the configuration file.   
  > It is recommended to make a backup of the file.  
  > ~~It's definitely NOT because I'm too lazy to write a rollback!(TSUNDERE_FACE.jpg)~~. 

Then you can enjoy it. You can set it to start automatically when the computer boots up. Just WinLauncher will retain the last used layout.
  > About automatic startup, will not be elaborated further.
  > For Windows, you can create a shortcut under `Shell:Startup`, add a task schedule, or modify the registry;
  > for Linux users, you can use `systemd`;
  > for MacOS users, you can manage startup items from the Dock.
  > Please refer to specific methods on your own.

## ~~Warm Tips~~Notes
1. Runs silently - only shows when activated (tray icon or hotkey)
   > Successful start: RGB cube icon appears in tray without error notifications
2. Default hotkey: `META+SHIFT+E` (Windows: `Win+Shift+E`)
   > Conflicts possible - change the `hotkey` field in `config.json` (see above).
3. JSON editing tips:
   - Use online validators like [JSONLint](https://jsonlint.com/)
   - Graphical config editor planned
4. **Critical path handling difference**:
   - Icon paths: Never quote (even with spaces)
   - Command paths: Always quote (with spaces)
     > If icons load but commands fail, check path formatting
5. **ALWAYS BACKUP YOUR CONFIGURATION**

## TODO
~~1. `hotkey` field~~
2. Graphical configurator
