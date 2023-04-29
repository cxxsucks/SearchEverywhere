# SearchEverywhere

[English](./README_en.md)

![stolenLogo](assets/search_img/stolenLogo.ico)
**Search***Everywhere*  
^ 图标是从一个[和这个仓库撞名的仓库](https://github.com/Jycjmf/SearchEverywhere)
里面偷来的。

**可以在Linux, macOS与Windows**上运行的图形界面文件检索工具，含有`find`, `locate`
以及`Everything`的各种功能，外加内容查找、上下层目录查找等。另有简洁明了的表达式编辑器，
普通可以简单地使用鼠标构造复杂的搜索表达式。

## 重要注意事项

这个 README 是关于 `orient` 的 GUI 前端的。 对于 CLI 和库，另见
[orient](https://github.com/cxxsucks/orient)。  
与 GUI 无关的功能/优点/缺点的完整列表，如性能和 `find` 兼容性也在那里。

最新版本是 `v0.4.0`，还不稳定。以下的所有演示，屏幕截图都是在`v0.3.1`上拍摄的。

### 不是一个专门的内容搜索器

虽然`SearchEverywhere`确实提供了内容搜索功能，但文件内容**没有**索引，
因此内容搜索无法很好地扩展。  
**始终**将内容搜索与名称匹配结合使用，或者在内容搜索之前设置一个较小的起点。

### Linux 上的内存占用问题

由于某种原因（似乎是内核或 Qt 问题），Linux 上的内存使用量增长得相当快。  
在我有 1.2M 文件的笔记本电脑上，启动占用65MiB。几次搜索和更新后，内存使用量猛增
最终位于 350~400MiB 之间。  
（低于 Chromium/Electron？😅）

只有在 Linux 上才会发生这种情况。 在 Windows 和 macOS 上，空闲时内存使用量远低于
120MiB，在索引更新期间内存使用量出现短暂峰值 \<200MiB，之后内存使用量回落。

`valgrind` 也没有检测到泄漏，这表明它可能是框架或系统问题。欢迎在Issue和Discussion
里面报告更多的细节。  
目前，一个好的解决方案是*经常*重新启动应用程序，就好像它是一个内存泄漏的怪物，
或者更好的是，使用更具有Linux特色的 `orient` CLI。

### 客户端/服务器架构

客户端/服务器架构提供高度的代码分离、连接到远程系统的能力和更高级别的安全性。
对于 `orient` 和 `SearchEverywhere`，这将有助于 `SearchEverywhere` 完全专注于
GUI 而较少关注 `orient` 的内部实现细节，从而提高与旧版 `orient` 的兼容性。  
同时，也可以在其他开启了`orient`服务器的电脑上远程搜索文件。
（类比 SQL 服务器：它们查询表而我们查询文件。）
如果真的是是`Qt`框架引起的内存问题，该问题也可以被C/S架构解决。

不幸的是，添加连接器层需要对 `orient` 与 `SearchEverywhere` 进行大面积更改。
当此仓库达到 256 颗星后，将着手开发连接器层。如果您觉得该项目有用或是有趣，
请点个star吧！  

## 比较

|              | Linux | Windows | macOS | Android |   License   |
|:------------:|:-----:|:-------:|:-----:|:-------:|:-----------:|
| `Everything` | 👎NO  |  👍YES  | 👎NO  |  👎NO   | Proprietary |
|    `find`    | 👍YES |  👎NO   | 👎NO  |  👎NO   |    GPLv3    |
|  `fsearch`   | 👍YES |  👎NO   | 👎NO  |  👎NO   |    GPLv2    |
|   `locate`   | 👍YES |  👎NO   | 👎NO  |  👎NO   |    GPLv3    |
|   `orient`   | 👍YES |  👍YES  | 👍YES |  👎NO   |    GPLv3    |

续表  
|              | `-and -or` | Invert Index | Match Parent |  GUI  |  CLI  |
|:------------:|:----------:|:------------:|:------------:|:-----:|:-----:|
| `Everything` |   👍YES    |     👎NO     |   Partial    | 👍YES |  😕   |
|    `find`    |   👍YES    |     👎NO     |     👎NO     | 👎NO  | 👍YES |
|  `fsearch`   |   👍YES    |     👎NO     |   Partial    | 👍YES | 👎NO  |
|   `locate`   |   👎NO     |     👍YES    |     👎NO     | 👎NO  | 👍YES |
|   `orient`   |   👍YES    |     👍YES    |     👍YES    | 👍YES | 👍YES |

注意：

- *部分*匹配父项和子项意味着虽然它们确实提供了匹配文件的父项或目录的子项的选项，
    但此类搜索仅限于字符串匹配而不是应用程序的所有功能。
- `Everything` CLI 似乎对所有结果都进行了*美化*，使其很难与其他工具结合使用，
    因此那里放了个疑惑的表情。

## 图形界面功能

此处仅列出 GUI 功能。 非 GUI 相关功能在
[CLI](https://github.com/cxxsucks/orient) 文档中。

### 预览内容

搜索完成后，可以在名为“previewer”的窗口右侧预览给定结果的内容：  
![预览器](docs/md_pics/preview.png)

要进行预览，`SearchEverywhere`窗口的宽度必须至少为 600 像素，
可通过鼠标拖动窗口边缘进行拉伸或最大化来实现。 要关闭预览，必须通过单击 *清除预览*
来清除预览内容，然后再将窗口拖回到小于 600 像素宽。  
以下是可以预览的文档类型列表以及预览它们的先决条件：

- 所有类型的图像，如`.png .jpg .svg .webm`
    > 不幸的是，`.gif` 没有动画。
- 纯文本文件
- 呈现静态 HTML 文件
    > 不是一个功能全面的浏览器
- 渲染 Markdown 文件
- 多媒体文件，如 `.mp{3,4} .mov`
    > 需要在编译时设置 `SEEV_PREVIEW_MEDIA` 选项  
    > Qt Multimedia 无法运行我的古董 MacBook🥹，因此除非在 macOS
    > 上从源代码手动编译，否则无法进行媒体预览。
- PDF文件的第一页
- 如果 LibreOffice 安装在 PATH 中，可以预览Office 文件的第一页
    > ~~这很愚蠢，因为 `orient` 能够通过 `-name soffice -a -updir bin -a -executable`~~
    > ~~很好地定位 office 可执行文件，而 PATH 要求只是表明开发人员是多么无能。~~

### Linux、macOS 和 **Windows**

`SearchEverywhere`提供了 3 种语言 `法语，西班牙语` 和 `简体中文` 的本地化。
当系统语言为其中之一时（或设置 `LC_ALL` 环境变量进行测试），将显示相应语言的 GUI。

法语和西班牙语翻译是通过谷歌翻译完成的，因此没有质量保证。中文也是谷歌翻译，
但是翻译成依托答辩的地方会手动改正。（应该可以看出README也是一样的，比如这里😜）

分别在 macOS 和 Windows 上截图：
![seev_mac](docs/md_pics/seev_mac.png)  
![seev_win](docs/md_pics/seev_win.png)  

### 表达式编辑器

新用户可能会被抛给他们的过多功能弄得不知所措。值得庆幸的是，
`SearchEverywhere`里面有一个丑陋但直观的表达式编辑器：  
![expr_edit](docs/md_pics/expr_edit.png)  
此图中的表达式搜索所有包含类似“helloworld”的`.c`文件。

编辑器位于主页面的左上角，若左上角太小时，可以点击下方按钮将其变成单独的对话框。  
编辑器支持的匹配器：

- 始终为真/假 `-true -false`
- 条件`-and -or`
- 反 `-not !`
- 路径匹配`-strstr -bregex -name -fuzz`
- 文件类型`-type`
- 文件统计`-size -{u,g}id -{a,m,c}{time,min}`
- 文件内容`-content-{strstr,regex,fuzz}`
- 文件可访问性`-readable -writable -executable`
- 上层目录与下层文件`-updir -downdir`
- 按条件忽略目录(永真，必须和`-and`一起使用) `-prunemod`

## 用户指南

### 索引配置

首次启动时，系统会要求用户提供根路径和索引时忽略的路径。
进入主界面后仍然可以通过单击“高级选项”-“编辑方向设置”来配置它们。  
默认配置适用于大多数设备，但大致过一下这部分以查看您的设备是否属于*大多数*。

#### 根路径

根路径是文件系统索引开始的地方。 对于 Windows 用户，这些通常是像
`C:\ D:\` 这样的驱动器路径。  
库存设置可能开箱即用，但请注意“HDD”和“SSD”根路径。所有根路径必须与所在磁盘类型相对应，
否则索引速度会大大下降。  
如果有任何未列出的 HDD 根路径，请务必添加它们。  
如果不确定，请将所有根路径放入 SSD 中，因为大多数现代计算机都在随机访问友好的 SSD 上运行。

#### 忽略的路径

在索引时，所有忽略的路径都会被忽略。  
通常不经常使用或不稳定的路径会被忽略，例如外部驱动器、Linux 自动文件系统和临时文件。
默认设置包括所有这些（Linux 上的 `/dev /sys` 除外）。

**Windows 用户注意**：默认情况下，`C:\Windows` 被忽略，因为它很少被普通用户访问，
并且有*很多很多*文件，大约 100 万个左右。  
如果您坚持要为它编制索引，请将其从忽略的路径中删除。根据上一节，如果`C:\Windows`
被索引，它应该成为根路径。  
**另请注意**：某些 Windows 系统的 `C:\Windows` 称为 `C:\WINDOWS`。
`SearchEverywhere` 区分大小写，因此如果索引时间过长，请仔细检查您的
`C:\Windows` 的名称。在 Windows 上，每个路径可能有一个尾随反斜杠 `\`，
这是为了符合 Unix 标准，可以忽略。

### 开始索引

索引建立已经在最初配置时就已经开始了。之后会出现一个消息框，其`确定`按钮在索引
建立完成后变为可点击。 单击它会打开主页。  
`Cancel` 按钮也会打开主页，但会丢弃创建了一半的索引。 除非对默认配置进行实质性更改，
例如将 SSD 根路径移动到 HDD 根路径或添加忽略的路径，否则请勿单击它。

单击主页上的“扫描文件系统”按钮会启动索引更新。
索引更新也会每小时自动开始一次，但前提是“SearchEverywhere”保持打开状态。
配置编辑需要索引更新才能生效。

### 设置起点

与根路径和忽略路径不同，起点是每次搜索设置的。默认情况下，起点是根目录，
这意味着搜索所有被建立索引的文件。  
设置不同的起点可能有助于在较小的区域中查找文件，但请确保它们不重叠，
否则重叠部分将被多次搜索。

### 准备好了，开始吧

现在是搜索的时候了！ 主页下有 4 个“已保存”搜索，用于演示和测试`SearchEverywhere`
的功能。 给他们一个机会来测试`SearchEverywhere`是否真的有效。  
![保存的搜索](docs/md_pics/saved_searches.png)  
搜索可以保存以供将来使用，并且也会出现在这里。

## 安装

从 Release 中获取预编译的二进制文件，它们应该可以工作。
Windows 版本还需
[MSVC 运行时](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist)

很可惜， macOS dmg 不支持预览多媒体😭  
macOS ARM 版本缺失，因为苹果机太贵了🫥。请在Issue或Discussion
中报告在该机器上是否可用。
> 目前测试太少，不是很适合发布到发行版。  
> 可能首先发布到 Arch AUR btw, 顺便说一下。

### 从源代码构建

现在仍在发布的早期阶段，建议从源代码构建。(`v0.4.0`暂时必须源码编译)  
构建所需依赖：

- Qt 小部件
- Qt 多媒体（仅限多媒体预览）
- CMake
- PCRE2
- rapidfuzz
- GoogleTest（仅限`orient` 测试）

除了 `CMake Qt` 之外，所有依赖项都可以由 CMake 自动下载。
如果您已经在系统上安装了其中一部分，通过切换下面的这些配置选项，也可以使用已安装的。

配置选项：

- `SEEV_PREVIEW_MEDIA`：启用预览多媒体
- `ORIE_TEST`：构建 GoogleTest 测试套件
- `ORIE_SYSTEM_PCRE2`：使用系统 PCRE2 库而不是编译新库。
- `ORIE_LINK_STATIC`：静态链接 orient 可执行文件
- `ORIE_SYSTEM_RAPIDFUZZ`：使用系统 rapidfuzz 库（仅标头）

> `rapidfuzz` 在作为子项目构建时存在一些问题，因此（目前） `SEEV_SYSTEM_ORIENT` 不稳定。

将下面的 `OPTION` 替换为您启用的选项，然后运行以下命令：

```sh
git clone https://github.com/cxxsucks/SearchEverywhere.git
cd orient; mkdir build; cd build
cmake -DOPTION1=ON -DOPTION2=ON -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
sudo make install
```

## 致谢

### 名称和图标

在漫无目的地浏览 DuckDuckGo 和 GitHub 寻找*这个*`SearchEverywhere` 的图标时，
我遇到了另一个 [SearchEverywhere](https://github.com/Jycjmf/SearchEverywhere)。  
它似乎仅适用于 Windows，并且依赖于非*free-as-in-freedom*的 `Everything`，
这有点破坏了它使用 *free-as-in-freedom* GPLv3 许可证的目的。 尽管缺少核心功能，
但它的用户界面远远领先于这个`SearchEverywhere`，最重要的是，它包含一个为
`SearchEverywhere`量身打造的图标。

因此，按照 Stallman 的意愿，我宣布该图标独立于其原始的非自由依赖项`Everything`，
也就是偷走了它。

虽然[这个SearchEverywhere](https://github.com/Jycjmf/SearchEverywhere)
**仅能在 MSWindows 上运行😭**，但也不妨看看。

### 其他

- 用于 GUI 框架的 Qt
- LibreOffice 用于 Office 文档预览
