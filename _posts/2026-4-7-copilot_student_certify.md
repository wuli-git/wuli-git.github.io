# GitHub Copilot

<div style="text-align: center;">

![](https://cloud.tsinghua.edu.cn/f/9f1f76c21f1a4b99b11c/?dl=1)

</div>

想要体验各种最新模型？

<div style="text-align: center;">

![](https://cloud.tsinghua.edu.cn/f/5ebd11963c4a432db045/?dl=1)

</div>

想要更集成的 AI coding 体验？

<div style="text-align: center;">

![更方便的对话窗口&代码补全](https://cloud.tsinghua.edu.cn/f/962377f286c848f7aab6/?dl=1)

</div>

那就来看看~~并白嫖~~**GitHub Copilot** 吧！

:::note

在通过 github 教育认证后，可以免费使用 10 刀每月的 copilot pro 订阅+4 刀每月的 github pro 订阅（为期两年，两年学生认证到期后可重新认证续期）

:::

## 1. 准备工作

首先用**学生邮箱**注册一个 GitHub 账号（已经有 GitHub 号的同学可以添加学生邮箱，并设置成首选）

然后将个人主页按下图模板设置好：

<div style="text-align: center;">

![公开档案](https://cloud.tsinghua.edu.cn/f/00c752a6701241b3a6f1/?dl=1)

</div>

<div style="text-align: center;">

![支付信息](https://cloud.tsinghua.edu.cn/f/fecb151fb46b40848469/?dl=1)

</div>

然后需要设置双重验证，在手机上下载 Microsoft Authenticator（应用商店搜索即可）

在 GitHub 的 password and authentication 栏目中（就在 payment information 下面），在 Two-factor methods 下选择**Authenticator app**，按照网页要求进行设置即可

<div style="text-align: center;">

![双重验证](https://cloud.tsinghua.edu.cn/f/3d4a25df739b4a66a4fc/?dl=1)

</div>

:::warning

这一步会生成账户的恢复代码，注意留存好

:::

## 2. 学生认证

:::note

新注册的 GitHub 账号可能会认证失败，稍等几天就好

:::

:::warning

认证过程中记得关掉代理，若在校外需挂学校 VPN，GitHub 会通过网络定位你是不是在校内

:::

来到 Billing and licensing 下的 Education benefits

<div style="text-align: center;">

![入口](https://cloud.tsinghua.edu.cn/f/4d1e24f3f312493ebc54/?dl=1)

</div>

进入后按指示操作即可

:::note

GitHub 会根据学生邮箱自动选择学校
之后会要求选择用什么做证明，选第一项即可，学生证是最好的证明
如果学生证的中文下没有对应的英文，建议自行补充（用便利贴粘一下）

:::

:::warning

拍照不允许使用虚拟摄像机

:::

如果顺利，大概一天就会通过申请

## 3. 领取福利

认证通过后回到 Billing and licensing 下的 Education benefits 即可领取学生权益

## 4. Copilot 配置

### 1. GitHub 部分

在 Copilot 设置中把能勾的全勾上😏

<div style="text-align: center;">

![Copilot 设置](https://cloud.tsinghua.edu.cn/f/d16d169140c64ae093e4/?dl=1)

</div>

在 GitHub 上可以使用网页版 Copilot，和其他 AI 工具类似：[github.com/copilot](https://github.com/copilot)

### 2. VS Code

:::note

只介绍 C/C++，Python 照猫画虎就行

:::

[官网链接](https://code.visualstudio.com/)

<div style="text-align: center;">

![安装时此页建议都勾选](https://cloud.tsinghua.edu.cn/f/c1cdbced69f04efe8899/?dl=1)

</div>

下载安装打开后，安装 **C/C++** 和 **Code Runner** 插件

<div style="text-align: center;">

![](https://cloud.tsinghua.edu.cn/f/f24218f47ccf4f458ef0/?dl=1)

</div>

<div style="text-align: center;">

![](https://cloud.tsinghua.edu.cn/f/b1accf61e57d4245b017/?dl=1)

</div>

### 编译环境：g++

各位同学应该都有 Dev-C++，其中已经包含了 g++ 编译器

**配置步骤：**

1. 打开 Dev-C++ 安装目录下的 **MinGW64\bin** 文件夹，将路径复制下来
2. 在开始菜单搜索**环境变量**，选择**编辑账户的环境变量**
3. 找到 **Path** 这个变量，在其中添加刚刚复制的路径
4. 在终端输入以下命令验证安装：

```bash
g++ -v
```

5. 配置好之后打开 VS Code（需重启），再找一个 cpp 文件点右上角的运行键即可在下方终端运行程序了

### 进阶：Clang 编译器与 clangd 扩展

:::note

参考资料：
[[万字长文] Visual Studio Code 配置 C/C++ 开发环境的最佳实践(VSCode + Clangd + XMake)](https://zhuanlan.zhihu.com/p/398790625)

:::

微软 IntelliSense 的替代品，具有更好的代码补全，比 g++ 更详细的编译报错，调用函数时能显示其参数的原有定义等等功能，能改善开发体验，但只是锦上添花。

<div style="text-align: center;">

![比如我先定义一个 deleteNeuron 函数](https://cloud.tsinghua.edu.cn/f/062a3e810ca141b594f7/?dl=1)

</div>

<div style="text-align: center;">

![之后调用时便会显示出其意义](https://cloud.tsinghua.edu.cn/f/6ac80dad6a014c8598b8/?dl=1)

</div>

**安装步骤：**

1. **安装 Clang 编译器**
   - [官网链接](https://clang.llvm.org/)
   - 根据系统选择相应的安装方式

2. **安装 clangd 扩展**
   - 在 VS Code 中搜索并安装 **clangd** 扩展

<div style="text-align: center;">

![](https://cloud.tsinghua.edu.cn/f/ef8b994dd74e42dc8570/?dl=1)

</div>

### 3. GitHub Copilot 扩展安装

在扩展中下载 **GitHub Copilot**

安装成功后右下角会弹出登录提醒，登录刚刚开通 Copilot 的账号即可

随后即可使用 Copilot 的所有功能啦！

包括：
- **智能代码补全**
- **编辑器内联聊天**  
- **右侧聊天栏**
- **Agent Mode**