# MusicPlayer

```Bash
 git clone --recursive https://github.com/SHM-white/MusicPlayer.git
```

使用Qt C++编写的音乐播放器，支持播放本地音频，兼容`.lrc`格式歌词文件，作为播放器的基本功能基本完善。

[![Linux x64](https://github.com/SHM-white/MusicPlayer/actions/workflows/linux.yml/badge.svg)](https://github.com/SHM-white/MusicPlayer/actions/workflows/linux.yml)
[![Windows x64](https://github.com/SHM-white/MusicPlayer/actions/workflows/Windows.yml/badge.svg)](https://github.com/SHM-white/MusicPlayer/actions/workflows/Windows.yml)

## 使用指南：

1. 添加音频到播放列表：
    - 将文件直接拖进播放器窗口即可在添加到播放列表中。
    - 将文件拖放到可执行文件上（即传入文件所在路径的启动参数）即可在添加到播放列表中并自动播放传入的第一个音频文件。
    - 可在程序菜单中批量添加音频文件，支持多选。
    - 支持多个文件拖入，自动筛选音频文件放入播放列表，可过滤非音频文件。
    - 支持通过右键菜单删除播放列表内的音频文件，支持通过键盘上的`Delete`键删除。
    - 支持通过右键菜单清空播放列表。
2. 播放音频：
    - 双击播放列表中的音频文件名即可开始播放。
    - 点击前一首或后一首按钮可切换到上一首或下一首音频。
        - 若当前播放模式为随机播放两个按钮行为一致，均为随机选择非当前音频进行播放。
        - 若当前模式为单曲循环或列表循环则播放当前音频的下一首或上一首。
    - 播放速度调节：
        - 提供多种常见倍速选项供用户选择。
    - 播放进度和状态调节：
        - 播放进度条可拖动，拖动后会自动更新当前播放时间。
        - 支持按空格键暂停或继续播放。
        - 支持按左右箭头按键调整播放进度。
        - 支持双击歌词跳转至歌词所在时间。
    - 支持音频文件元数据的解析和显示。
3. 歌词显示：
    - 支持解析`.lrc`格式歌词文件，歌词文件名需与音频文件名一致。
    - 歌词支持自动滚动，当前歌词加粗显示，字号加大。
4. 部分设置记忆：
    - 支持调整和记忆歌词字体大小。
    - 支持记忆上次音量。

## 特色实现：
- 加载音频、配置文件、解析歌词文件等耗时操作均使用异步操作，避免阻塞GUI线程。
- 使用`QWindowKit`自定义无边框窗口样式，支持Windows 11原生分屏功能。
- 创建了多个派生类，用于封装倍速调节、音量调节、歌词显示等功能。
- 代码编写时未使用平台专有库，可跨平台编译和运行（已在ubuntu上编译成功，需要安装[Segoe Fluent Icons](https://learn.microsoft.com/zh-cn/windows/apps/design/downloads/#fonts)字体才可正常显示控件图标），wsl2需要[安装Nautilus](https://learn.microsoft.com/zh-cn/windows/wsl/tutorials/gui-apps)才可正常将文件拖入播放列表。(MacOS Android未测试)
- 使用`QSS`样式表自定义了两种颜色主题，和代码解耦，方便后续维护。
- 控件之间大部分使用Qt的信号槽机制进行通信，歌词显示部分使用了模型视图模式。

## 后期可扩展性：
- 预留了配置文件的管理接口，方便后续添加更多配置项。
- UI仍有改进空间，受限于`QListWidget`的功能，添加收藏功能需要大改。
- 目前对`.lrc`格式歌词的解析仅支持逐行歌词，未支持逐字歌词（虽然常见）。
- 程序目前支持了多语言翻译，但我自己只做了中文和英文两种语言。

## 使用的开源库：
- [Qt](https://www.qt.io/) - Qt是一个跨平台的C++图形用户界面应用程序开发框架。
- [QWindowKit](https://github.com/stdware/qwindowkit) - Cross-platform window customization framework for Qt Widgets and Qt Quick.

## [使用Github Actions自动构建](https://github.com/SHM-white/MusicPlayer/actions)
