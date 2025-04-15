---
title: gitpage 建站
tags: TeXt
---
### 1.1github presentation

github code仓库，int上传，equal net 云盘，进行线上code管理

### 1.2github pages

你可能有一些混淆，通常所说的是GitHub Pages，下面为你介绍并讲解如何用它来搭建个人博客网站。

### GitHub Pages介绍

GitHub Pages是一项由GitHub提供的服务，旨在为托管在GitHub上的项目提供介绍页面。其设计初衷是让开发者能够为自己的项目创建静态网站，用于展示项目的相关信息、文档等。不过，由于它提供了免费且稳定的空间，因此也非常适合用来搭建个人博客网站。

### 使用GitHub Pages搭建个人博客网站步骤

1. **准备工作**：
    - 注册[GitHub账号](https://github.com/)。
    - 安装[Git](https://git-scm.com/downloads)，并配置好用户名和邮箱。
2. **创建仓库**：
    - 登录GitHub账号后，点击“New”创建新仓库。
    - 仓库名为`username.github.io`，其中`username`是你的GitHub用户名，如`abc.github.io`。
3. **选择博客框架**：
    - **Jekyll**：是一个简单的免费Blog生成工具，能将纯文本转化为静态网站和博客。
    - **Hexo**：也是常用的博客框架，支持Markdown语法，部署方便。
4. **配置本地环境**：以Hexo为例：
    - 安装Node.js，可从[官网](https://nodejs.org/en/download/)下载。
    - 打开命令行，执行`npm install -g hexo`安装Hexo。
    - 在电脑中新建一个目录作为博客项目文件夹，如`D:\Blog`，在该目录右键打开Git Bash，执行`hexo init`和`npm install`，自动创建网站所需文件。
5. **配置SSH Key**：
    - 执行`ssh-keygen -t rsa`生成SSH Key，在`C:\Users\用户\.ssh`目录下会生成相关文件，如`id_rsa.pub`。
    - 登录GitHub，在“Settings”的“SSH and GPG keys”中添加SSH Key，Title任意写，Key为`id_rsa.pub`中的内容。
6. **部署到GitHub**：
    - 安装部署插件，在命令行执行`npm install hexo-deployer-git --save`。
    - 修改博客项目中的`_config.yml`文件，找到“Deployment”部分，配置为：

```yaml
deploy:
  type: git
  repo: git@github.com:XXX/XXX.github.io.git # 将XXX替换为你的GitHub用户名
  branch: master
```

- 每次部署执行`hexo generate`和`hexo deploy`命令，将博客内容部署到GitHub Pages上。


部署成功后，通过`https://username.github.io`即可访问个人博客网站。

### 7.部署到 GitHub Pages

- 将项目文件添加到 Git 暂存区，执行git add .。
- 提交更改，执行git commit -m "Initial commit"。
- 将代码推送到 GitHub 仓库，执行git push -u origin main。
- 进入 GitHub 仓库的Settings页面，找到Pages选项，在Source中选择main分支，点击Save。等待片刻后，通过（）[https://username.github.io即可访问个人博客网站]。
- 如果想要绑定自定义域名，可在设置域名解析后，在 GitHub Pages 的Custom domain输入框中输入域名并保存，同时更新_config.yml文件中的url配置

### 1.3how to

- 1.在要上传的_post目录右键打开open git bash here,

```git
git add .//上传所有已改文件，相当于装车
git commit -m "the file name"//上传
git push//发送
```

need downnode git bush

### 1.4litle tips

-  _posts文件夹不能搞错了，必须要在myblog的文件下的posts文件修改，否则将出现

```git
$ git commit -m "shangchuan"
On branch master
Your branch is up to date with 'origin/master'.

nothing to commit, working tree clean
```

- 你的文件并没有被修改