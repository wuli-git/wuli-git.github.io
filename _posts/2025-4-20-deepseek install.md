---
title: deepseek 部署
tags: TeXt
---

# Ollama 完整部署与使用指南

## 目录

- [一、Ollama 多平台安装](#一ollama-多平台安装)
  - [Linux 系统](#linux-系统)
  - [macOS 系统](#macos-系统)
  - [Windows 系统](#windows-系统)
- [二、远程访问配置](#二远程访问配置)
- [三、DeepSeek 模型部署](#三deepseek-模型部署)
  - [模型拉取](#模型拉取)
  - [模型运行](#模型运行)
- [四、硬件适配方案](#四硬件适配方案)
  - [纯CPU运行](#纯cpu运行)
  - [GPU加速](#gpu加速)
- [五、应用接入方案](#五应用接入方案)
  - [MaxKB 部署](#maxkb-部署)
  - [ChatBox 部署](#chatbox-部署)
- [六、故障排查](#六故障排查)
- [七、性能监控](#七性能监控)

## 一、Ollama 多平台安装

### Linux 系统

```bash
# 一键安装
curl -fsSL https://ollama.com/install.sh | sh

# 权限配置
sudo usermod -aG ollama $USER
newgrp ollama

# 服务管理
sudo systemctl start ollama
sudo systemctl enable ollama

# 验证
ollama --version
```

### macOS 系统

```bash
# 安装
curl -fsSL https://ollama.com/install.sh | sh

# 运行方式
ollama serve           # 前台运行
brew services start ollama  # 后台服务
```

### Windows 系统

1. 下载安装包：[Ollama Windows Installer](https://ollama.com/download)
2. 双击安装
3. 验证：

```cmd
ollama list
```

## 二、远程访问配置

```bash
sudo systemctl edit ollama.service
```

添加内容：

```
[Service]
Environment="OLLAMA_HOST=0.0.0.0"
Environment="OLLAMA_ORIGINS=*"
```

重载服务：

```bash
sudo systemctl daemon-reload
sudo systemctl restart ollama
```

## 三、DeepSeek 模型部署

### 模型拉取

```bash
# 基础版
ollama pull deepseek-r1

# 可选规格
ollama pull deepseek-r1:7b     # 7B参数
ollama pull deepseek-r1:33b    # 33B参数
ollama pull deepseek-r1:7b-q4_0 # 量化版
```

### 模型运行

```bash
# 基础运行
ollama run deepseek-r1

# GPU加速
ollama run deepseek-r1 --gpu

# 限制线程
OLLAMA_NUM_THREADS=4 ollama run deepseek-r1
```

## 四、硬件适配方案

### 纯CPU运行

**配置建议**：

- 7B模型：≥8GB内存
- 33B模型：≥32GB内存
- 推荐使用量化模型

### GPU加速

**要求**：

- 7B模型：≥6GB显存
- 33B模型：≥20GB显存
- 需安装NVIDIA驱动和CUDA Toolkit 12.x

## 五、应用接入方案

### MaxKB 部署

```bash
docker run -d \
  --name=maxkb \
  -p 8080:8080 \
  -v ~/maxkb_data:/var/lib/postgresql/data \
  1panel/maxkb:latest
```

访问：`http://localhost:8080` (默认账号: admin/MaxKB@123...)

### ChatBox 部署

1. 下载地址：[ChatBox Releases](https://github.com/Bin-Huang/chatbox/releases)
2. 配置Ollama连接：
   - API Endpoint: `http://localhost:11434`
   - Model Name: `deepseek-r1`

## 六、故障排查

1. **服务无法启动**：

```bash
journalctl -u ollama -f   # 查看日志
```

2. **模型加载失败**：

```bash
ollama rm deepseek-r1 && ollama pull deepseek-r1
```

3. **连接问题**：

```bash
curl http://ollama-ip:11434  # 测试连通性
```

## 七、性能监控

建议部署监控工具：

- **Zabbix** + **Grafana** 监控系统资源
- **nvidia-smi** 监控GPU使用情况

---

> **提示**：本文档持续更新，建议定期查看[官方文档](https://ollama.com)获取最新信息。
