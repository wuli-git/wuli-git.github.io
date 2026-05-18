---
title: GitHub上利用CI/CD自动化部署上线静态网页
tag: tips	project
author: cc00mi
---

## 一、什么是 CI/CD 流水线？

**CI（Continuous Integration 持续集成）**

每次提交代码到 GitHub 时，自动执行：拉取代码 → 构建 → 测试 → 检查，保证代码质量。

**CD（Continuous Deployment 持续部署）**

构建测试通过后，**自动将项目部署到线上环境**，无需手动上传、手动发布。

**GitHub CI/CD 流水线 = 提交代码 → 自动构建 → 自动上线**

全程自动化，一次配置，永久生效。

## 二、 CI/CD 实现效果

- 向 `master` 分支推送代码
- **GitHub Actions 自动触发流水线**
- 自动将项目部署到 GitHub Pages
- 全球可访问在线网站
- **无需服务器、无需花钱、0 成本上线**

## 三、流水线配置

1. 配置文件路径（必须固定）：

```
.github/workflows/deploy.yml
```

2. 配置文件

```yaml
# 流水线名称（可自定义，随便写）
name: GitHub CI/CD 自动部署

# 触发条件：当代码推送到 master 分支时自动运行
on:
  push:
    branches: [ master ]  # 你的分支名：master / main 按需修改

# 执行任务
jobs:
  build-and-deploy:
    runs-on: ubuntu-latest  # 运行环境：固定写 ubuntu-latest 即可
    steps:
      # 步骤1：从 GitHub 拉取最新代码
      - name: 拉取代码
        uses: actions/checkout@v4

      # 步骤2：自动部署到 GitHub Pages（上线）
      - name: 部署到 GitHub Pages
        uses: peaceiris/actions-gh-pages@v4
        with:
          github_token: ${{ secrets.GITHUB_TOKEN }}  # GitHub 自动提供的密钥，不用改
          publish_dir: ./  # 要上线的文件夹：
                           # ./ = 普通静态网页项目
                           # ./dist = Vue/React 打包目录
                           # ./html = 网页目录（按需改）
```

3. 提交推送：

```bash
git add .
git commit -m "add ci/cd"
git push
```

4. 修改github gh-pages源分支

   1. 进入仓库 → `Settings` → 左侧找到 `Pages`。

   2. 在 `Build and deployment`部分，检查 `Source`选项：必须选择 `Deploy from a branch`。下面的 `Branch` 必须选 `gh-pages`，文件夹选 `/ (root)`。

      ![gh-pages源分支](/assets/images/src-img/20260517CICD.png)

5. 查看 Actions → 显示绿色 ✅ 即为部署成功

5. 访问：`https://你的用户名.github.io/仓库名/`

ps: 在仓库右侧设置 Website

1. 进入仓库首页
2. 右侧找到 About → 点击设置齿轮 ⚙️
3. 在 Website 输入你的上线地址
4. 保存

仓库首页右侧会显示可点击的网站链接，读者一键体验。

## 四、常见问题

1. 部署成功但访问 404？

- GitHub Pages 生效延迟 1～3 分钟
- 检查是否有 `index.html`
- GitHub Pages 的内容是从 `gh-pages` 分支拉取的。如果流水线执行失败，或者根本没运行，`gh-pages` 分支里还是旧的（甚至是空的），访问自然是 404。去你的仓库页面，点 `Actions`，看最近一次流水线是否成功（绿色 ✅）。去仓库分支列表，看有没有 `gh-pages` 这个分支。如果没有，说明流水线从来没成功部署过。

2. 部署失败（红色 ×）？

- 进入仓库 Settings → Actions → General
- 找到 Workflow permissions → 选择 Read and write permissions
- 重新运行流水线

3. 如何自动更新？

```bash
git push
```

自动构建、自动上线，无需任何操作。

