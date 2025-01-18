## TagUI

### 配置

1. 下载地址：https://github.com/aisingapore/TagUI/releases

2. 添加 D:\tagui\src 到环境变量 PATH

3. 更改 D:\tagui\src\tagui.cmd 中浏览器路径如下：

```cmd
# Before
set chrome_command=C:\Program Files (x86)\Google\Chrome\Application\chrome.exe
# After
set chrome_command=C:\Program Files\Google\Chrome\Application\chrome.exe
```

4. 配置 cmd 为 UTF-8 编码： win10 : https://blog.csdn.net/lyyybz/article/details/120782669 win11 : https://blog.csdn.net/jymd2508/article/details/135334716

```cmd
# 查看当前编码，应该是 936，即 GBK，输出为 65001时，即 UTF-8
chcp
```

5. cd D:\tagui\flows\samples

6. 运行示例、其中有些定位不出元素、所以会报错，跑了一遍其中貌似只有 3，5 是正常的、而且有些网站还会跳人机验证。

```cmd
tagui .\1_google.tag
tagui .\2_github.tag
tagui .\3_conditions.tag
tagui .\4_loops.tag
tagui .\5_repositories.tag
tagui .\6_datatables.tag
tagui .\7_newtab.tag
tagui .\8_chineseflow.tag
tagui .\9_lazada.tag
```

7. tagui live 模式，类似 idle、可以实时运行输入的代码。

### 注意

1. 利用 tagui 进行 cmd 执行命令的默认路径在 tagui.cmd 所在的目录（比如直接创建文件夹会创建到这个路径下面，而不是 tag 脚本在的工作目录）

2. tagui 用反引号 `xxx` 来进行变量引用 xxx 变量

3. 运行 tagui for python 时，他生成图片文件的默认路径貌似又是 cmd 中当前路径了。

4. 懂了、如果要用图片作为操作目标。要先把需要操作的图片放在脚本所在的目录下、然后用相对路径引用。

5. 有一个问题、如果在自动化流程中把浏览器关了、目前好像这个脚本不会终止。

6. 感觉应该统一使用命令行启动，这样可以统一路径

## TagUI for Python

- https://github.com/tebelorg/RPA-Python?tab=readme-ov-file

1. 安装

```cmd
pip install rpa
```

2. 第一次使用会自动重新安装 tagui，需要手动在 C:\Users\Horiz\AppData\Roaming 目录下找到、并像上面那样更改浏览器路径。

## TagUI RPA Web Recorder

1. https://chromewebstore.google.com/detail/tagui-rpa-web-recorder/gjdajlcpkkjgjjokkekiniengajiamnk

2. 感觉用不了 不如自己写

## Microsoft Word & Excel Add-in

1. https://github.com/aisingapore/TagUI/blob/master/src/office/README.md

2. 下载完之后会在 Word 和 Excel 中多出一个 TagUI 的选项卡
