### 一、部署Ollama（多平台选择安装）

Ollama 支持 Linux/macOS/Windows，需根据系统选择安装方式。

 Linux 系统部署
适用系统：Ubuntu/Debian/CentOS 等

步骤：

一键安装：

curl -fsSL https://ollama.com/install.sh | sh
1
权限配置（避免 sudo 运行）：

sudo usermod -aG ollama $USER  # 将当前用户加入ollama组  
newgrp ollama                  # 刷新用户组
1
2
启动服务：

systemctl start ollama         # 启动服务  
systemctl enable ollama        # 开机自启
1
2
验证安装：

ollama --version               # 输出版本号即成功
1
2. macOS 系统部署
步骤：

一键安装：

curl -fsSL https://ollama.com/install.sh | sh
1
启动服务：

ollama serve                   # 前台运行（调试用）
1
或通过 Launchd 后台服务：

brew services start ollama  # 通过Homebrew管理
1
3. Windows 系统部署
前置要求：

Windows 10/11 64位

已安装 WSL2（推荐Ubuntu发行版）

步骤：

下载安装包：Ollama Windows Installer

下载后进行双击安装

验证安装：

ollama list                    # 查看已安装模型
1


💬

如何配置远程 Ollama 服务？
默认情况下，Ollama 服务仅在本地运行，不对外提供服务。要使 Ollama 服务能够对外提供服务，你需要设置以下两个环境变量：

如果 Ollama 作为 systemd 服务运行，应使用 systemctl 设置环境变量：

调用 systemctl edit ollama.service 编辑 systemd 服务配置。这将打开一个编辑器。

在 [Service] 部分下为每个环境变量添加一行 Environment：

[Service]  
Environment="OLLAMA_HOST=0.0.0.0"  
Environment="OLLAMA_ORIGINS=*"
1
2
3
重新加载 systemd 并重启 Ollama：
systemctl daemon-reload  
systemctl restart ollama
1
2
二、拉取并运行DeepSeek模型
官方模型库参考：Ollama DeepSeek-R1 页面



1. 拉取模型
ollama pull deepseek-r1          # 官方推荐模型名称
1
模型规格选项（按需选择）：

ollama pull deepseek-r1:7b     # 7B参数基础版  
ollama pull deepseek-r1:33b    # 33B参数进阶版（需更高显存）
1
2


2. 运行模型
ollama run deepseek-r1
1
验证交互：

>>> 你好，请用中文回答  
你好！我是DeepSeek-R1，很高兴为您提供帮助！
1
2
三、不同硬件场景配置说明
根据硬件资源选择运行模式：

场景1：纯CPU运行
适用情况：无独立显卡或显存不足

配置优化：

限制线程数（避免资源耗尽）：

OLLAMA_NUM_THREADS=4 ollama run deepseek-r1  # 限制4线程
1
使用量化模型（减少内存占用）：

ollama pull deepseek-r1:7b-q4_0             # 4-bit量化版
1
内存要求：

7B模型：至少8GB空闲内存

33B模型：至少32GB空闲内存

场景2：CPU+GPU混合运行
适用情况：有NVIDIA显卡（需CUDA支持）

配置步骤：

安装驱动：
安装 NVIDIA驱动 和 CUDA Toolkit 12.x
启用GPU加速：

ollama run deepseek-r1 --gpu              # 强制使用GPU
1
显存要求：

7B模型：至少6GB显存

33B模型：至少20GB显存

性能监控：

nvidia-smi               # 查看GPU利用率
1
四、AI问答应用接入本地大模型
方式1：部署MaxKB并接入Ollama


1. 通过Docker部署MaxKB
docker run -d \  
  --name=maxkb \  
  -p 8080:8080 \  
  -v ~/maxkb_data:/var/lib/postgresql/data \  
  1panel/maxkb:latest
1
2
3
4
5
访问 http://localhost:8080，默认账号密码 admin/MaxKB@123…
2. 配置Ollama模型接入
进入MaxKB控制台：模型管理 > 添加模型

填写参数：

模型类型：Ollama

模型名称：DeepSeek-r1

Base URL：http://ollama主机ip:11434（Docker内访问宿主机）

模型名称：deepseek-r1（与Ollama拉取的模型名一致）



3. 创建应用并测试问答功能




方式2：本地安装chatBox AI并接入本地大模型
1、下载安装包
访问 GitHub Releases 页面：ChatBox Releases!ChatBox Releases页面

选择 Windows 版本：

2、安装 ChatBox
运行安装程序：
双击下载的 .exe 文件，按提示完成安装。
3、基础配置
3.1. 连接 本地模型
确保 Ollama 服务已运行。

在 ChatBox 设置中选择 **ollama api**，填写：

API Endpoint: http://localhost:11434

API Key: 留空

Model Name: deepseek-r1（与 Ollama 模型名称一致）





3.2 可以自定义界面


3.3 验证使用


五、常用的故障排查方法
1. Ollama服务无法启动
日志查看：

journalctl -u ollama -f   # Linux
1
端口冲突：修改Ollama端口：

OLLAMA_HOST=0.0.0.0:11435 ollama serve  # 指定新端口
1
2. 模型加载失败
重新初始化：

ollama rm deepseek-r1 && ollama pull deepseek-r1
1
3. MaxKB连接超时
检查网络：

curl http://ollama-ip:11434  # 确认Ollama API可达
1
跨域问题：在Ollama启动时添加：

OLLAMA_ORIGINS="*" ollama serve
1
六、性能优化建议
场景	推荐配置
CPU模式	使用
GPU模式	启用
七、AI服务器性能监控
我使用zabbix+grafana对服务器做了资源使用情况的监控，因为我的服务器没有GPU，全部使用CPU去跑，当我运行大模型的时候，可以看到服务器资源站哟比较高（目前我测试用的的Deepseek-r1:7b）