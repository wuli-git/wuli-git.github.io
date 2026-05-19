---
title: Uni-app 项目如何用 GitHub CI/CD 自动部署到 GitHub Pages：完整 SOP、底层原理与避坑总结
tag: tips  deploy CI/CD
author: cc00mi fox
---

## 一、写在前面

如果你已经看过上一篇“静态网页如何通过 GitHub 自动构建并发布”的文章，那么这一篇可以看作它的续篇。

上一篇解决的是更基础的一层问题：

- 如何把纯静态网页接入 GitHub
- 如何使用 GitHub Actions 自动构建
- 如何把构建产物发布到 GitHub Pages

那一层的核心关注点是：

- 构建有没有跑通
- 静态资源路径对不对
- 页面能不能正常打开

而这一篇开始进入更复杂但也更真实的项目场景：

- 项目不再只是简单静态页面
- 而是 `uni-app` 这类需要工程化构建的前端项目
- 页面运行后还依赖外部 API
- 最终还要处理浏览器跨域、代理、图片 CDN 可达性、运行时拓扑等问题

所以如果说上一篇是在回答：

> “静态页面如何通过 GitHub 自动化部署？”

那么这一篇回答的就是：

> “当项目从纯静态页，演进到带工程化构建和运行时接口依赖的前端应用后，CI/CD 体系要如何随之升级？”

这篇文章沉淀的是一次真实的项目改造过程：把一个原本更偏 `HBuilderX / uni-app` 开发流的壁纸项目，改造成：

- 前端通过 GitHub Actions 自动构建
- 构建产物自动发布到 GitHub Pages
- 运行期接口通过独立后端代理解决 CORS
- 图片资源通过代理兜底解决客户端 CDN 连接失败

本次最终上线结果是：

- GitHub 仓库：`wuli-git/wallpaper`
- GitHub Pages 前端：`https://wuli-git.github.io/wallpaper/`
- Railway 代理：`https://wallpaper-production-9537.up.railway.app`
- 前端 API Base：`https://wallpaper-production-9537.up.railway.app/api/bizhi`

这类项目的难点，不在“把静态文件传到网上”，而在于：

1. `uni-app` 工程能不能在 CI 环境中稳定构建出 H5 产物
2. 构建出来的 H5 是否能正确适配 GitHub Pages 的子路径
3. 页面运行时依赖的接口，是否允许浏览器跨域访问
4. 页面拿到数据后，图片资源是否也能被用户浏览器正常加载

如果只理解成“把页面部署出去”，最后通常会出现一个典型症状：

- 页面外壳能打开
- 路由能跳
- 但数据是空的
- 或者数据出来了，图片一片空白
- 看起来像“部署成功了”，实际上业务并没有跑通

本文会把这个问题拆透。

---

## 二、适用场景

这套 SOP 适合下面这类项目：

- 使用 `uni-app` 开发
- 需要发布 H5 版本
- 代码托管在 GitHub
- 想通过 GitHub Actions 做自动构建与自动部署
- 最终站点托管在 GitHub Pages
- 项目运行时依赖外部 API
- 上游 API 不完全受自己控制

如果你的项目只是纯静态 HTML/CSS/JS，没有接口依赖，也没有 `uni-app` 这层构建链，流程会简单很多。本文后面会专门讲差异。

---

## 三、最终架构图

最终落地后的架构是：

```text
开发者 push 到 GitHub main 分支
        ↓
GitHub Actions 拉取代码
        ↓
安装依赖并构建 uni-app H5 产物
        ↓
上传 Pages artifact
        ↓
GitHub Pages 发布静态前端
        ↓
用户访问 https://wuli-git.github.io/wallpaper/
        ↓
前端页面请求 Railway 上的 Node 代理
        ↓
Node 代理转发到上游壁纸接口
        ↓
代理补齐 CORS 响应头并自动附加 access-key
        ↓
代理把 CDN 图片地址改写成自己的图片代理地址
        ↓
浏览器拿到数据与图片并渲染页面
```

也就是说，最终不是“一个服务”，而是两个部分协同：

- 静态前端：GitHub Pages
- 动态代理：Railway 上的 Node 服务

---

## 四、完整 SOP

### 4.1 整理 uni-app 工程为可 CI 构建结构

首先要解决的是：原项目必须能在命令行里构建，而不是只能在 HBuilderX 图形界面里点按钮。

核心处理包括：

1. 补齐 `package.json`
2. 补齐 `vite.config.js`
3. 把源码整理到 `src/` 目录
4. 确保 `manifest.json`、`pages.json`、`main.js`、`App.vue` 等位于 `src/`
5. 把 `api/`、`components/`、`utils/`、`commom/`、`static/`、`uni.scss` 等运行所需资源同步到 `src/`
6. 安装构建依赖，比如 `sass`

本项目最终采用的构建脚本是：

```json
{
  "scripts": {
    "dev:h5": "uni",
    "build:h5": "uni build"
  }
}
```

**为什么这一步必须做**

因为 GitHub Actions 本质上只会执行脚本，它不会打开 HBuilderX 帮你点“发行到 H5”。

如果项目不能命令行构建，那么就谈不上真正的 CI/CD。

---

### 4.2 锁定一组兼容的 DCloud / Vue / Vite 依赖

这次迁移里最容易误判的一类错误，是把缺失依赖当成“少装一个包”来补。

例如依次遇到：

- 缺 `@dcloudio/uni-cli-shared`
- 缺 `@dcloudio/uni-cli-i18n`
- 缺 `webpack`
- 缺 `semver`

如果每报一个就手动 `npm install` 一个，最后很容易把依赖树补歪。

正确做法是：**把 DCloud 相关包按同一代版本锁成一组**。

本项目最终使用的核心依赖类似：

```json
{
  "dependencies": {
    "@dcloudio/uni-app": "3.0.0-5000720260410001",
    "@dcloudio/uni-h5": "3.0.0-5000720260410001",
    "@dcloudio/uni-components": "3.0.0-5000720260410001",
    "@dcloudio/uni-i18n": "3.0.0-5000720260410001",
    "vue": "3.4.21"
  },
  "devDependencies": {
    "@dcloudio/vite-plugin-uni": "3.0.0-5000720260410001",
    "@vitejs/plugin-vue": "5.2.4",
    "@vue/compiler-sfc": "3.4.21",
    "sass": "^1.77.8",
    "vite": "5.2.8"
  }
}
```

注意：不要使用不存在的包或不存在的版本，例如：

- `@dcloudio/cli`
- `@dcloudio/uni-app@^2.0.2`
- `@dcloudio/vite-plugin-uni@^4.61.2026051901`

这些会直接导致 `E404` 或 `ETARGET`。

---

### 4.3 处理 GitHub Pages 的子路径问题

GitHub Pages 的项目页通常不是部署在根路径，而是：

```text
https://用户名.github.io/仓库名/
```

本项目实际地址是：

```text
https://wuli-git.github.io/wallpaper/
```

所以 `vite.config.js` 里必须配置：

```js
import { defineConfig } from "vite";
import uniPlugin from "@dcloudio/vite-plugin-uni";

const uni = uniPlugin.default || uniPlugin;

export default defineConfig({
  base: "/wallpaper/",
  plugins: [uni()],
});
```

这里有两个关键点：

- `base: "/wallpaper/"` 用来适配 GitHub Pages 子路径
- `const uni = uniPlugin.default || uniPlugin` 用来兼容插件导出形态，避免 `uni is not a function`

**为什么这一步必须做**

如果不写 `base`，构建产物里的资源路径默认会按根路径处理，例如：

```text
/assets/index.js
```

而 GitHub Pages 项目页实际需要的是：

```text
/wallpaper/assets/index.js
```

不修这个，页面可能会出现：

- CSS 丢失
- JS 404
- 路由资源错位
- 页面空白

---

### 4.4 配置 GitHub Actions 自动构建与部署

推荐使用 GitHub 官方 Pages workflow，而不是老式的“把 dist 推到 gh-pages 分支”的做法。

本项目最终的 workflow 重点如下：

```yaml
name: Deploy to GitHub Pages

on:
  push:
    branches:
      - main
  workflow_dispatch:

permissions:
  contents: read
  pages: write
  id-token: write

concurrency:
  group: pages
  cancel-in-progress: true

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - name: Checkout
        uses: actions/checkout@v4

      - name: Setup Node.js
        uses: actions/setup-node@v4
        with:
          node-version: 18
          cache: npm

      - name: Install dependencies
        run: npm ci

      - name: Build H5
        env:
          VITE_API_BASE_URL: ${{ vars.VITE_API_BASE_URL || 'https://wallpaper-production-9537.up.railway.app/api/bizhi' }}
        run: npm run build:h5

      - name: Setup Pages
        uses: actions/configure-pages@v5

      - name: Detect build output path
        id: detect
        run: |
          if [ -d "dist/build/h5" ]; then
            echo "path=dist/build/h5" >> "$GITHUB_OUTPUT"
          elif [ -d "unpackage/dist/build/h5" ]; then
            echo "path=unpackage/dist/build/h5" >> "$GITHUB_OUTPUT"
          elif [ -d "dist" ]; then
            echo "path=dist" >> "$GITHUB_OUTPUT"
          else
            echo "Build output not found"
            ls -la
            ls -la dist || true
            ls -la unpackage || true
            exit 1
          fi

      - name: Create Pages fallback
        run: cp "${{ steps.detect.outputs.path }}/index.html" "${{ steps.detect.outputs.path }}/404.html"

      - name: Upload artifact
        uses: actions/upload-pages-artifact@v3
        with:
          path: ${{ steps.detect.outputs.path }}

  deploy:
    needs: build
    runs-on: ubuntu-latest
    environment:
      name: github-pages
      url: ${{ steps.deployment.outputs.page_url }}
    steps:
      - name: Deploy to GitHub Pages
        id: deployment
        uses: actions/deploy-pages@v4
```

这里比最小示例多做了两件事：

1. **自动检测构建产物目录**
   - 不同 uni-app / Vite 组合可能输出到 `dist/build/h5` 或 `unpackage/dist/build/h5`
   - workflow 里自动判断，避免产物路径写死后上传失败

2. **生成 `404.html`**
   - GitHub Pages 刷新 hash / history 路由时可能需要 fallback
   - 复制一份 `index.html` 为 `404.html` 可以提升路由容错

### GitHub 仓库还要做的配置

进入：

```text
Settings -> Pages
```

把 Source 设置为：

```text
GitHub Actions
```

如果出现环境保护规则导致部署被拒绝，还要进入：

```text
Settings -> Environments -> github-pages
```

把 Deployment branches and tags 调整为允许 `main` 分支部署，或者取消不必要的分支限制。

---

### 4.5 修复原项目中的模板与编码问题

真实项目迁移到 CI 时，常见并且最费时间的问题不是“部署”，而是“代码本身不能在严格构建环境里通过”。

这次实践里主要踩到的坑有：

- Vue 模板存在坏掉的闭合标签
- 属性之间缺少空格
- 字符串被错误编码后破坏模板结构
- 中文乱码导致引号和标签异常
- 某些页面在原运行环境里能“凑合跑”，但在 Vite 严格解析时直接报错

典型报错长这样：

```text
[plugin:vite:vue] Whitespace was expected.
```

所以实际过程里，需要逐个修：

- `src/pages/index/index.vue`
- `src/pages/user/user.vue`
- `src/pages/notice/detail.vue`
- `src/components/theme-item/theme-item.vue`
- `src/pages/preview/preview.vue`
- `src/pages/search/search.vue`

### 这类问题的本质

不是 GitHub Pages 有问题，也不是 CI 有问题，而是：

- 本地某些工具链容错更高
- 但标准构建器更严格

CI 的价值之一，就是把这类“隐性坏代码”逼出来。

---

### 4.6 页面能打开但没有壁纸：定位运行时接口问题

这一步是最关键的认知分水岭。

部署成功后，页面出现了，但壁纸数据没有出来。  
这时很多人会误判为：

- GitHub Pages 不支持 uni-app
- GitHub Actions 没构建完整
- 资源路径还有问题

但真实原因可能完全不同。

本项目里，前端请求逻辑原本写死为：

```js
const BASE_URL = "https://tea.qingnian8.com/api/bizhi";
```

也就是说，页面加载后会在浏览器里直接请求这个上游接口。

经排查发现：

- 这个接口本身有数据
- 用 `curl`、服务端脚本请求是成功的
- 但浏览器跨域请求会被拦截

根因是接口响应头缺少：

- `Access-Control-Allow-Origin`
- `Access-Control-Allow-Headers`
- `Access-Control-Allow-Methods`

即：**CORS 不通过**。

---

### 4.7 为什么静态站点需要额外后端代理

既然上游接口不支持浏览器跨域，那就必须在中间加一层自己的服务。

最终采用的方案是：

- 前端继续部署到 GitHub Pages
- 代理后端部署到 Railway

代理的职责很简单：

1. 接收来自浏览器的请求
2. 转发给 `https://tea.qingnian8.com/api/bizhi`
3. 把上游返回的数据回传
4. 在响应中补上 CORS 头
5. 自动附加上游接口需要的 `access-key`
6. 必要时改写图片 URL，让图片也走代理

这层代理不是“可选优化”，而是浏览器环境下的必要组成部分。

---

### 4.8 代理服务的实现方式

最终代理使用一个最小 Node 服务实现，目录单独放在：

```text
proxy-server/
```

原因是：

- 前端仓库根目录是 `uni-app`
- 代理是纯 Node 服务
- 把两者拆开，方便 Railway 只部署代理子目录

### 代理服务的核心能力

- 支持 `OPTIONS` 预检
- 按请求头动态回写允许的 `Origin`
- 转发查询参数
- 转发请求体
- 自动补 `access-key`
- 过滤 hop-by-hop headers
- 提供 `/` 和 `/health` 健康检查
- 提供 `/proxy-image?url=...` 图片代理
- 把上游 JSON 里的 `https://cdn.qingnian8.com/...` 图片地址改写为代理图片地址

这样前端最终请求的是：

```text
https://wallpaper-production-9537.up.railway.app/api/bizhi/classify?select=true
```

而不是直接请求上游：

```text
https://tea.qingnian8.com/api/bizhi/classify?select=true
```

图片最终请求的是：

```text
https://wallpaper-production-9537.up.railway.app/proxy-image?url=https%3A%2F%2Fcdn.qingnian8.com%2F...
```

而不是浏览器直接请求：

```text
https://cdn.qingnian8.com/...
```

---

### 4.9 Railway 部署代理

部署步骤如下：

1. 登录 Railway
2. 选择从 GitHub 仓库部署
3. 选择仓库 `wuli-git/wallpaper`
4. 把 `Root Directory` 设成：

```text
proxy-server
```

5. 配置环境变量：

```text
ALLOWED_ORIGIN=https://wuli-git.github.io
UPSTREAM_ACCESS_KEY=<你的上游 access-key>
TARGET_ORIGIN=https://tea.qingnian8.com
TARGET_PREFIX=/api/bizhi
PROXY_PREFIX=/api/bizhi
IMAGE_PROXY_PREFIX=/proxy-image
```

6. 生成公网域名，本项目为：

```text
https://wallpaper-production-9537.up.railway.app
```

注意：

- `railway.internal` 这种地址不能给浏览器用
- 必须使用 Railway 生成的公网域名
- `UPSTREAM_ACCESS_KEY` 不建议写进公开文章或前端构建产物，应放在 Railway 环境变量里

---

### 4.10 回填 GitHub Actions 构建变量

为了让前端在 H5 构建时自动使用代理地址，需要在 GitHub 仓库里增加变量：

```text
Settings -> Secrets and variables -> Actions -> Variables
```

新增：

```text
VITE_API_BASE_URL=https://wallpaper-production-9537.up.railway.app/api/bizhi
```

之后重新运行 GitHub Pages workflow。

前端请求逻辑会优先读取：

```js
import.meta.env.VITE_API_BASE_URL
```

从而在 H5 环境中走代理，而不是直连上游。

本项目里请求层还做了一个关键修复：H5 下手动把 GET 参数拼到 URL 上，避免 `uni.request` 在 H5 构建后出现列表接口参数没有正确带上的情况。

核心思想类似：

```js
const BASE_URL = import.meta.env.VITE_API_BASE_URL || "https://tea.qingnian8.com/api/bizhi";

function appendQuery(url, data = {}) {
  const params = new URLSearchParams();
  Object.keys(data).forEach((key) => {
    const value = data[key];
    if (value !== undefined && value !== null && value !== "") {
      params.append(key, value);
    }
  });

  const query = params.toString();
  if (!query) return url;
  return `${url}${url.includes("?") ? "&" : "?"}${query}`;
}
```

---

## 五、排错分支：从报错现象定位根因

这一节是本次部署最值得沉淀的部分。

不要把所有错误混成一句“部署失败”。更好的方式是像走树一样排查：先判断错误发生在哪一层，再进入对应分支。

```text
部署 / 上线异常
├─ A. npm install / npm ci 阶段失败
│  ├─ A1. E404：包不存在
│  ├─ A2. ETARGET：版本不存在
│  └─ A3. 缺模块：依赖代际混乱
├─ B. npm run build:h5 阶段失败
│  ├─ B1. uni is not a function
│  ├─ B2. Could not resolve ./main.js
│  ├─ B3. @ 路径找不到
│  ├─ B4. Sass 变量不存在
│  └─ B5. Vue 模板解析失败
├─ C. GitHub Pages 部署阶段失败
│  ├─ C1. Pages source 配错
│  ├─ C2. artifact 路径不对
│  └─ C3. 环境保护规则拒绝 main
├─ D. 页面能打开，但没有接口数据
│  ├─ D1. 前端仍直连上游
│  ├─ D2. 上游 CORS 不通过
│  └─ D3. 构建变量没有注入
├─ E. 首页有数据，但分类列表没有数据
│  └─ E1. H5 GET 参数没有稳定序列化
├─ F. 数据有了，但图片不显示
│  └─ F1. CDN 图片连接被关闭
└─ G. Railway 代理访问异常
   ├─ G1. 根路径返回 404
   ├─ G2. Root Directory 配错
   └─ G3. 环境变量缺失
```

---

### 分支 A：依赖安装失败

#### A1：`@dcloudio/cli` 报 `E404`

**现象**

```text
npm error code E404
npm error 404 Not Found - GET https://registry.npmjs.org/@dcloudio%2fcli
npm error 404 '@dcloudio/cli@...' is not in this registry.
```

**判断方式**

如果错误里出现 `@dcloudio/cli`，基本可以判断是包名用错。

**根因**

`@dcloudio/cli` 并不是这个项目需要安装的 npm 包。uni-app Vite 项目应该通过 `@dcloudio/vite-plugin-uni` 和相关 `@dcloudio/uni-*` 包构建。

**修复方案**

- 不要执行 `npx @dcloudio/cli`
- 不要在 `package.json` 里添加 `@dcloudio/cli`
- 使用 `npm run build:h5` 调用本项目脚本

---

#### A2：DCloud 包报 `ETARGET`

**现象**

```text
npm error code ETARGET
npm error notarget No matching version found for @dcloudio/vite-plugin-uni@^4.61.2026051901.
```

或者：

```text
npm error notarget No matching version found for @dcloudio/uni-app@^2.0.2.
```

**判断方式**

如果错误里出现 `No matching version found`，说明你请求的版本在 npm registry 里不存在。

**根因**

版本号是猜出来的，或者把不同代际的包混在了一起。

**修复方案**

统一锁定一组真实存在且互相兼容的版本，例如：

```text
@dcloudio/uni-app               3.0.0-5000720260410001
@dcloudio/uni-h5                3.0.0-5000720260410001
@dcloudio/uni-components        3.0.0-5000720260410001
@dcloudio/uni-i18n              3.0.0-5000720260410001
@dcloudio/vite-plugin-uni       3.0.0-5000720260410001
vue                             3.4.21
vite                            5.2.8
```

---

#### A3：不断缺 `@dcloudio/uni-cli-shared`、`@dcloudio/uni-cli-i18n`、`webpack`、`semver`

**现象**

```text
Error: Cannot find module '@dcloudio/uni-cli-shared'
Error: Cannot find module '@dcloudio/uni-cli-i18n'
Error: Cannot find module 'webpack'
Error: Cannot find module 'semver'
```

**判断方式**

如果每装一个包又缺下一个包，说明不是“少一个依赖”，而是依赖树整体不匹配。

**根因**

DCloud 相关包版本混用，导致插件内部引用的配套包没有被正确安装。

**修复方案**

- 停止一个个补包
- 清理错误依赖组合
- 重新统一 DCloud 版本
- 重新生成 `package-lock.json`
- 本地用 `npm install` 验证，CI 用 `npm ci` 固化安装

---

### 分支 B：构建失败

#### B1：`uni is not a function`

**现象**

```text
TypeError: uni is not a function
```

**判断方式**

错误出现在 `vite.config.js` 里的 `plugins: [uni()]` 附近。

**根因**

`@dcloudio/vite-plugin-uni` 的导出形态在不同环境下可能表现为默认导出或对象导出。

**修复方案**

使用兼容写法：

```js
import uniPlugin from "@dcloudio/vite-plugin-uni";

const uni = uniPlugin.default || uniPlugin;
```

---

#### B2：`Could not resolve "./main.js" from "index.html"`

**现象**

```text
Could not resolve "./main.js" from "index.html"
```

**判断方式**

项目已经整理成 `src/` 结构，但 `index.html` 还在引用旧路径。

**根因**

入口文件从根目录移动到了 `src/main.js`，但 HTML 没同步。

**修复方案**

把 `index.html` 入口改成：

```html
<script type="module" src="/src/main.js"></script>
```

---

#### B3：`@/api/apis.js`、组件或工具文件找不到

**现象**

```text
Failed to resolve import "@/api/apis.js"
```

或某些组件、工具、静态资源路径找不到。

**判断方式**

错误里出现 `@/xxx`，说明构建器正在从 `src/` 里解析路径。

**根因**

源码迁移到 `src/` 后，原本根目录下的 `api/`、`components/`、`utils/`、`commom/` 等目录没有一起进入 `src/`。

**修复方案**

把运行时需要的目录同步到 `src/`：

```text
src/api
src/components
src/utils
src/commom
src/static
src/uni.scss
```

---

#### B4：Sass 报 `$brand-theme-color` 未定义

**现象**

```text
Undefined variable.
$brand-theme-color
```

**判断方式**

样式编译阶段失败，变量来自项目全局 `uni.scss`。

**根因**

`uni.scss` 没有放到 uni-app 构建器预期的位置，导致全局变量没有注入。

**修复方案**

确保 `uni.scss` 存在于 `src/uni.scss`，并且里面包含项目所需的全局主题变量。

---

#### B5：Vue 模板报 `Whitespace was expected`

**现象**

```text
[plugin:vite:vue] Whitespace was expected.
```

**判断方式**

错误通常会指向某个 `.vue` 文件的 template 区域。

**根因**

模板里存在格式损坏，例如：

- 标签没闭合
- 属性之间缺少空格
- 中文乱码破坏了引号
- 拷贝过程中混入异常字符

**修复方案**

逐个打开报错文件，按标准 Vue 模板语法修复。不要只盯 CI，因为这类问题本质是源码模板损坏。

---

### 分支 C：GitHub Pages 部署失败

#### C1：Pages Source 没有设置为 GitHub Actions

**现象**

workflow 成功或部分成功，但 Pages 站点没有更新。

**判断方式**

进入仓库：

```text
Settings -> Pages
```

检查 Source。

**根因**

Pages 还在使用旧的分支发布模式，而不是 GitHub Actions artifact 发布。

**修复方案**

把 Source 设置为：

```text
GitHub Actions
```

---

#### C2：上传 artifact 路径不对

**现象**

```text
Build output not found
```

或 Pages 部署后是空站点。

**判断方式**

检查构建产物实际在哪个目录：

```text
dist/build/h5
unpackage/dist/build/h5
dist
```

**根因**

不同构建链的 H5 输出目录不完全一致，workflow 写死了错误目录。

**修复方案**

在 workflow 里做输出目录检测，找到真实存在的目录后再上传。

---

#### C3：`main` 不允许部署到 `github-pages`

**现象**

```text
Branch "main" is not allowed to deploy to github-pages due to environment protection rules.
The deployment was rejected or didn't satisfy other protection rules.
```

**判断方式**

错误出现在 `Deploy to GitHub Pages` 步骤。

**根因**

GitHub Environment 对 `github-pages` 设置了部署分支保护，但 `main` 没被允许。

**修复方案**

进入：

```text
Settings -> Environments -> github-pages
```

在 Deployment branches and tags 里允许 `main`，或者取消不必要的限制。

---

### 分支 D：页面能打开，但没有接口数据

#### D1：前端仍然直连上游接口

**现象**

页面打开，但 Network 里请求的是：

```text
https://tea.qingnian8.com/api/bizhi/...
```

而不是：

```text
https://wallpaper-production-9537.up.railway.app/api/bizhi/...
```

**判断方式**

打开浏览器 DevTools 的 Network 面板，看请求域名。

**根因**

`VITE_API_BASE_URL` 没有在构建时注入，或者前端请求层没有读取它。

**修复方案**

GitHub Actions 构建步骤中增加：

```yaml
env:
  VITE_API_BASE_URL: ${{ vars.VITE_API_BASE_URL || 'https://wallpaper-production-9537.up.railway.app/api/bizhi' }}
```

前端请求层读取：

```js
const BASE_URL = import.meta.env.VITE_API_BASE_URL || "https://tea.qingnian8.com/api/bizhi";
```

---

#### D2：上游接口 CORS 不通过

**现象**

浏览器控制台出现 CORS 错误，或者接口请求状态看似失败。

**判断方式**

同一个接口：

- `curl` 请求成功
- 服务端脚本请求成功
- 浏览器请求失败

**根因**

上游接口没有返回浏览器需要的 CORS 响应头。

**修复方案**

加自己的 Node 代理：

- 浏览器请求 Railway 代理
- Railway 代理请求上游
- Railway 代理给浏览器补 CORS 头

---

#### D3：构建变量改了，但线上仍然没生效

**现象**

GitHub Variables 已经配置了 `VITE_API_BASE_URL`，但线上 JS 仍然请求旧地址。

**判断方式**

检查 GitHub Actions 是否在变量配置后重新运行过。

**根因**

Vite 的环境变量是在构建时注入的，不是页面运行时动态读取 GitHub 设置。

**修复方案**

重新运行 GitHub Pages workflow，或者重新 push 一次触发构建。

---

### 分支 E：首页有数据，但分类列表没有数据

#### E1：H5 GET 参数没有稳定序列化

**现象**

首页分类能出来，但进入：

```text
https://wuli-git.github.io/wallpaper/#/pages/classlist/classlist?id=...&name=...
```

页面列表为空。

**判断方式**

直接访问代理接口时有数据，但前端页面没有：

```text
https://wallpaper-production-9537.up.railway.app/api/bizhi/wallList?classid=...&pageNum=1&pageSize=...
```

**根因**

H5 环境下 `uni.request` 对 GET `data` 的处理不够稳定，参数没有按预期出现在 URL 查询串里。

**修复方案**

在请求封装层里手动序列化 GET 参数：

```js
if (method.toUpperCase() === "GET") {
  url = appendQuery(url, data);
  data = {};
}
```

---

### 分支 F：数据有了，但图片不显示

#### F1：`cdn.qingnian8.com` 图片请求 `ERR_CONNECTION_CLOSED`

**现象**

接口数据已经返回，页面结构也渲染了，但图片不显示。控制台出现：

```text
GET https://cdn.qingnian8.com/public/xxmBizhi/... net::ERR_CONNECTION_CLOSED
```

**判断方式**

Network 里 API 请求成功，但图片请求失败，失败域名集中在 `cdn.qingnian8.com`。

**根因**

这已经不是 API CORS 问题，而是用户浏览器直接访问图片 CDN 时连接被关闭。页面拿到了图片 URL，但浏览器加载不到图片。

**修复方案**

继续复用 Railway 代理，增加图片代理能力：

```text
/proxy-image?url=https%3A%2F%2Fcdn.qingnian8.com%2F...
```

同时在代理返回 API JSON 时，把里面的 CDN 图片地址改写成代理图片地址：

```text
https://cdn.qingnian8.com/...
```

改为：

```text
https://wallpaper-production-9537.up.railway.app/proxy-image?url=...
```

这样浏览器不再直接请求 `cdn.qingnian8.com`，而是请求自己的 Railway 代理。

---

### 分支 G：Railway 代理访问异常

#### G1：打开 Railway 根路径返回 404

**现象**

访问：

```text
https://wallpaper-production-9537.up.railway.app/
```

返回 404 或 `Proxy route not found`。

**判断方式**

访问 API 路径：

```text
https://wallpaper-production-9537.up.railway.app/api/bizhi/classify?select=true
```

如果 API 路径能返回数据，根路径 404 不一定是问题。

**根因**

代理服务原本只设计给 `/api/bizhi/...` 使用，根路径没有业务意义。

**修复方案**

为了方便排查，可以给代理增加：

```text
/
/health
```

返回：

```json
{
  "ok": true
}
```

这样后续判断 Railway 是否活着会更直观。

---

#### G2：Railway 部署后请求不到代理接口

**现象**

Railway 服务启动了，但 `/api/bizhi/...` 不通。

**判断方式**

检查 Railway 服务设置里的 Root Directory。

**根因**

仓库根目录是前端项目，代理实际在 `proxy-server/`。如果 Railway 从仓库根目录启动，会找不到正确的 `package.json` 或启动脚本。

**修复方案**

Railway 的 Root Directory 设置为：

```text
proxy-server
```

---

#### G3：代理接口 502 或上游鉴权失败

**现象**

代理地址能访问，但返回 502，或者上游返回鉴权错误。

**判断方式**

检查 Railway Variables。

**根因**

缺少上游所需的 `access-key`，或者目标地址环境变量写错。

**修复方案**

Railway 至少配置：

```text
UPSTREAM_ACCESS_KEY=<你的上游 access-key>
TARGET_ORIGIN=https://tea.qingnian8.com
TARGET_PREFIX=/api/bizhi
PROXY_PREFIX=/api/bizhi
```

---

## 六、为什么这和普通静态页面部署完全不同

很多人第一次做这类项目时，最大误区是：

> “GitHub Pages 不就是上传静态文件吗？为什么这么复杂？”

问题在于，**uni-app H5 项目并不等于纯静态页面项目**。

它和纯静态页至少有 4 个本质差异。

### 6.1 差异一：它先是“工程”，然后才是“静态产物”

纯静态页通常是：

- 直接写好的 HTML/CSS/JS
- 上传即可

而 `uni-app` 项目是：

- 源码
- 需要构建器转译
- 需要生成 H5 产物

也就是说，部署前必须先完成：

- Vue 编译
- 路由与资源处理
- SCSS 编译
- 平台适配

所以它天然需要 CI 构建链。

### 6.2 差异二：GitHub Pages 是子路径，不是根域部署

纯静态页很多时候直接部署在根路径，或者资源路径手写容易控制。

但 `uni-app + Vite + GitHub Pages` 里，如果不明确写：

```js
base: "/wallpaper/"
```

资源路径就会错。

这在本地开发环境不明显，但线上必暴露。

### 6.3 差异三：页面本身是静态的，但业务数据是动态的

这是最关键的一点。

GitHub Pages 只能托管静态文件：

- HTML
- CSS
- JS
- 图片

它不会替你提供：

- 运行时接口
- 服务端渲染
- 鉴权中转
- 跨域豁免

如果前端代码运行时还要调用第三方接口，那么：

- 页面能部署成功
- 不代表业务能跑通

这也是为什么“页面打开了但没有数据”会成为最常见的假成功状态。

### 6.4 差异四：浏览器安全模型会接管运行时

本地工具、服务端脚本、curl 可以请求成功，不代表浏览器也能请求成功。

浏览器还会额外检查：

- CORS
- 预检请求
- 自定义请求头是否合法
- 目标域名是否允许该来源访问
- 图片 CDN 是否可从用户浏览器直接访问

所以一旦页面部署到 GitHub Pages，真正的运行时环境就变成了：

- 用户浏览器
- 公网源站
- 标准 Web 安全策略

这和在 HBuilderX 预览、小程序端运行，是两套不同的约束模型。

---

## 七、技术底层分析

### 7.1 GitHub Pages 的本质

GitHub Pages 本质上是一个静态文件托管平台。

它做的事情只有两件：

1. 接收你构建出来的产物
2. 把这些产物通过 CDN / 静态站点形式提供给浏览器访问

它**不执行 Node 后端逻辑**，也**不提供动态 API**。

所以：

- 适合放前端
- 不适合直接承载需要服务端逻辑的业务

### 7.2 GitHub Actions 的本质

GitHub Actions 是构建流水线，不是线上运行环境。

它负责：

- checkout 代码
- 安装依赖
- 运行构建脚本
- 上传构建产物
- 触发 Pages 发布

但它不会在页面上线后继续给你提供运行时 API。

也就是说：

- CI 解决的是“怎么产出文件”
- 不是“怎么处理运行中的接口调用”

### 7.3 CORS 的本质

CORS 是浏览器的安全机制。

当页面从：

```text
https://wuli-git.github.io
```

去请求：

```text
https://tea.qingnian8.com
```

浏览器会认为这是跨源请求。  
即便目标接口真实有数据，只要响应头不声明允许这个来源访问，浏览器就会拦截。

换句话说：

- 服务端“能响应”
- 不等于浏览器“允许前端读取”

这也是为什么 curl 测试成功，但页面仍然空白。

### 7.4 为什么代理能解决问题

加代理后，浏览器请求目标变成：

```text
https://wallpaper-production-9537.up.railway.app
```

代理服务自己再去请求：

```text
https://tea.qingnian8.com
```

此时：

- 浏览器只和你的代理打交道
- 代理返回你自己定义的 CORS 头
- 上游接口变成服务器之间的通信
- 上游 access-key 不再需要暴露给浏览器

于是浏览器的跨域限制就被正确处理掉了。

这不是“绕过浏览器”，而是符合 Web 架构约束的标准做法。

### 7.5 为什么图片也可能需要代理

很多人以为接口能返回数据就结束了。

但壁纸项目还有第二层运行时依赖：图片 CDN。

接口返回的数据里包含图片地址，例如：

```text
https://cdn.qingnian8.com/public/xxmBizhi/...
```

如果用户浏览器访问这个 CDN 时出现：

```text
net::ERR_CONNECTION_CLOSED
```

那么页面依旧会空白或只有占位结构。

所以这次最终代理不只代理 API，还代理图片：

- API 代理解决数据和 CORS
- 图片代理解决 CDN 客户端不可达

这也是这次实践里最容易漏掉的一层。

---

## 八、这次实践里最值得复用的经验

### 8.1 不要把“页面能打开”误认为“部署完成”

真正的验收标准应该至少包括：

1. 页面能打开
2. 静态资源无 404
3. API 请求成功
4. 数据能正常渲染
5. 图片能正常加载
6. 关键交互可用

只做到第 1 步，不叫业务上线。

### 8.2 先解决“可构建”，再解决“可运行”

迁移这类项目时，不要一开始就纠结线上域名、代理、UI 空白。

正确节奏是：

1. 先让项目能在 CI 里稳定 build
2. 再让 Pages 正确托管构建产物
3. 再让 API 通过代理拿到数据
4. 最后验证图片、路由、交互这些运行时细节

否则会把“构建错误”和“运行时错误”混在一起，排查非常痛苦。

### 8.3 对依赖接口的静态站，代理通常不是补丁，而是架构组成部分

如果业务依赖第三方接口，而你又不能控制对方 CORS，那么：

- 代理层不是临时权宜之计
- 而是正式架构的一部分

从设计阶段就应该考虑进去。

### 8.4 不要把密钥塞进公开前端

静态前端的所有 JS 最终都会被浏览器下载。

所以：

- GitHub Pages 上的前端代码不能真正保密
- access-key 这类配置应该尽量放在代理服务端
- Railway Variables 比写死在前端源码里更合适

---

## 九、最终可复用模板

如果以后再做类似项目，可以直接复用这一套模板。

### 前端侧

- `uni-app` H5 构建
- `vite.config.js` 配置 Pages 子路径
- GitHub Actions 构建并发布到 Pages
- 用 `VITE_API_BASE_URL` 注入代理地址
- GET 请求参数在 H5 下手动序列化

### 后端侧

- 单独一个最小 Node 代理
- 独立目录部署到 Railway / Render / Zeabur
- 代理负责：
  - 转发请求
  - 补 CORS
  - 管理上游密钥
  - 改写图片 CDN 地址
  - 提供健康检查

### 配置侧

- GitHub 仓库管理前端构建变量
- Railway 管理代理服务环境变量
- GitHub Pages Source 使用 GitHub Actions
- `github-pages` 环境允许 `main` 分支部署

---

## 十、最终验收清单

上线后不要只看页面有没有打开，建议按下面顺序验收：

```text
1. GitHub Actions 是否成功
2. Pages 地址是否能打开
3. 控制台是否没有 JS / CSS 资源 404
4. Network 里的 API 域名是否为 Railway 代理
5. /api/bizhi/classify?select=true 是否返回数据
6. 分类页 /wallList 是否带上 classid 等查询参数
7. 图片 URL 是否已经变成 /proxy-image?url=...
8. 页面是否能看到真实壁纸图片
9. 刷新页面和复制链接打开是否正常
```

本项目最终验收地址：

```text
https://wuli-git.github.io/wallpaper/
```

分类页示例：

```text
https://wuli-git.github.io/wallpaper/#/pages/classlist/classlist?id=6524a48f6523417a8a8b825d&name=可爱萌宠
```

代理健康检查：

```text
https://wallpaper-production-9537.up.railway.app/health
```

---

## 十一、结语

把 `uni-app` 项目部署到 GitHub Pages，看似只是一个“前端上线”问题，实际上它横跨了三层：

- 工程构建层
- 静态托管层
- 运行时接口层

真正难的不是把文件传上去，而是理解：

- 哪些问题属于构建阶段
- 哪些问题属于托管阶段
- 哪些问题属于浏览器运行时安全模型

一旦把这三层分开，整个问题就会清晰很多。

这次实践最终得出的结论可以浓缩成一句话：

> Uni-app 项目部署到 GitHub Pages，真正的分界线不在“能不能发版”，而在“页面上线后是否还能拿到真实业务数据与图片资源”。

如果只是纯静态站，GitHub Pages 足够。  
如果页面依赖运行时接口，那就必须把代理、CORS、图片资源可达性和部署拓扑一起设计进去。

从系列化的视角看：

- 上一篇，解决“纯静态网页如何通过 GitHub 自动构建与发布”
- 这一篇，解决“工程化前端项目如何通过 GitHub CI/CD 稳定上线”
- 下一步，则可以继续扩展到更复杂的项目形态，例如：
  - 带后端服务的前后端分离项目
  - 带数据库的全栈应用
  - 带多环境区分的项目
  - 带灰度、回滚、监控的生产级交付流程

如果说静态网页部署是 GitHub CI/CD 的入门层，那么 `uni-app + Pages + Railway 代理` 这类项目就是一个非常典型的中阶过渡层：

- 它已经不再只是“传文件”
- 但又还没有重到必须完整引入 Kubernetes、容器编排、复杂后端集群

---

## 十二、从 uni-app 再往后：更复杂项目如何使用 CI/CD 部署

如果把“静态网页自动部署”看成入门，把“`uni-app + GitHub Pages + 代理`”看成进阶，那么再往后，CI/CD 体系通常会继续演进到更复杂的项目类型。

这时重点不再只是“能不能构建并发布”，而会逐渐转向：

- 如何管理多个服务
- 如何管理多个环境
- 如何保证发布稳定性
- 如何实现回滚、监控和灰度

### 12.1 前后端分离项目

典型形态：

- 前端：React、Vue、Next.js 前端站点
- 后端：Node.js、Java、Go、Python API 服务
- 数据库：MySQL、PostgreSQL、Redis

这类项目的 CI/CD 一般会拆成两条流水线：

1. 前端流水线  
   负责安装依赖、跑测试、打包前端产物、发布到静态托管平台或前端运行平台。

2. 后端流水线  
   负责安装依赖、跑单元测试、构建服务、发布到云主机、容器平台或 Serverless 平台。

相比 uni-app 这篇文章里的架构，升级点在于：

- 不再只有“前端 + 轻代理”
- 而是“前端 + 正式后端 + 数据库”

CI/CD 的关键关注点也会变成：

- 接口环境变量管理
- 数据库连接配置
- 前后端版本兼容
- 后端发布成功后再切换前端

### 12.2 Monorepo / 多服务项目

当项目进一步复杂化，经常会进入 monorepo 形态，例如：

- `apps/web`
- `apps/admin`
- `apps/api`
- `packages/ui`
- `packages/shared`

这类项目的 CI/CD 难点在于：

- 哪个服务变了，就只构建哪个服务
- 公共包变了，哪些下游要一起重建
- 如何避免每次提交都全量部署所有应用

这时常见做法是：

- 按目录拆分 workflow
- 做受影响范围检测
- 为不同应用配置独立部署目标

也就是说，流水线从“单项目直线型”开始走向“多项目选择性执行”。

### 12.3 含数据库迁移的全栈项目

一旦项目带数据库，并且数据结构会变化，CI/CD 就不再只是代码上线问题，还会变成数据演进问题。

此时典型链路会变成：

1. 代码测试通过
2. 构建镜像或构建服务产物
3. 执行数据库迁移
4. 发布后端服务
5. 发布前端
6. 做健康检查

这类项目最怕的不是构建失败，而是：

- 新代码已上线，但数据库未迁移
- 数据库已迁移，但服务未成功启动
- 某一步失败后，没有回滚策略

所以这里的 CI/CD 会更强调：

- 迁移顺序控制
- 发布前后健康检查
- 失败回滚
- 数据兼容窗口设计

### 12.4 生产级项目：灰度、回滚、监控

再往上走，真正生产级的 CI/CD 通常不会停留在“push 即上线”。

它会继续增加：

- 多环境  
  例如 `dev / test / staging / production`

- 灰度发布  
  例如先放 5% 流量，再逐步扩大

- 自动回滚  
  例如健康检查失败后自动回退到上一版本

- 监控告警  
  例如发布后自动观察错误率、延迟、接口可用性

- 审批流  
  例如生产环境发布前必须人工确认

这时，CI/CD 的本质就从“自动部署工具”进一步升级成“交付治理系统”。

---

## 参考资料

- GitHub Pages 自定义工作流：  
  https://docs.github.com/pages/getting-started-with-github-pages/using-custom-workflows-with-github-pages
- GitHub Actions 变量参考：  
  https://docs.github.com/en/actions/reference/variables-reference
- GitHub Actions 仓库变量 API：  
  https://docs.github.com/en/rest/actions/variables
- Railway Variables：  
  https://docs.railway.com/variables
- Railway CLI / Deploying：  
  https://docs.railway.com/cli/deploying
