---
title: Uni-app 项目如何用 GitHub CI/CD 自动部署到 GitHub Pages：完整 SOP、底层原理与避坑总结
tag: tips  deploy CI/CD
author: cc00mi
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
- 最终还要处理浏览器跨域、代理、运行时拓扑等问题

所以如果说上一篇是在回答：

> “静态页面如何通过 GitHub 自动化部署？”

那么这一篇回答的就是：

> “当项目从纯静态页，演进到带工程化构建和运行时接口依赖的前端应用后，CI/CD 体系要如何随之升级？”

也可以把这篇理解成：

- 从“静态页面自动部署”
- 进阶到“前端应用自动部署”

它们用的是同一套 GitHub 自动化基础设施，但面对的问题复杂度已经不是一个量级。

这篇文章沉淀的是一次真实的项目改造过程：把一个原本更偏 `HBuilderX / uni-app` 开发流的壁纸项目，改造成：

- 前端通过 GitHub Actions 自动构建
- 构建产物自动发布到 GitHub Pages
- 运行期接口通过独立后端代理解决 CORS

这类项目的难点，不在“把静态文件传到网上”，而在于：

1. `uni-app` 工程能不能在 CI 环境中稳定构建出 H5 产物
2. 构建出来的 H5 是否能正确适配 GitHub Pages 的子路径
3. 页面运行时依赖的接口，是否允许浏览器跨域访问

如果只理解成“把页面部署出去”，最后通常会出现一个典型症状：

- 页面外壳能打开
- 路由能跳
- 但数据是空的
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

如果你的项目只是纯静态 HTML/CSS/JS，没有接口依赖，也没有 `uni-app` 这层构建链，流程会简单很多。本文后面会专门讲差异。

---

## 三、最终架构图

最终落地后的架构是：

```text
开发者 push 到 GitHub
        ↓
GitHub Actions 拉代码
        ↓
构建 uni-app H5 产物
        ↓
发布到 GitHub Pages
        ↓
用户访问 GitHub Pages 站点
        ↓
前端页面请求 Railway 上的 Node 代理
        ↓
Node 代理转发到上游壁纸接口
        ↓
代理补齐 CORS 响应头
        ↓
浏览器拿到数据并渲染页面
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
5. 安装构建依赖，比如 `sass`

一个典型的 H5 构建脚本是：

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

### 4.2 处理 GitHub Pages 的子路径问题

GitHub Pages 的项目页通常不是部署在根路径，而是：

```text
https://用户名.github.io/仓库名/
```

例如：

```text
https://cc00mi.github.io/00wallpaper/
```

这就要求 `vite.config.js` 明确配置 `base`：

```js
import { defineConfig } from "vite";
import uni from "@dcloudio/vite-plugin-uni";

export default defineConfig({
  base: "/00wallpaper/",
  plugins: [uni()],
});
```

**为什么这一步必须做**

如果不写 `base`，构建产物里的资源路径默认会按根路径处理，例如：

```text
/assets/index.js
```

而 GitHub Pages 项目页实际需要的是：

```text
/00wallpaper/assets/index.js
```

不修这个，页面可能会出现：

- CSS 丢失
- JS 404
- 路由资源错位

---

### 4.3 配置 GitHub Actions 自动构建与部署

推荐使用 GitHub 官方 Pages workflow，而不是老式的“把 dist 推到 gh-pages 分支”的做法。

典型 workflow 结构如下：

```yaml
name: Deploy 00wallpaper to GitHub Pages

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
        uses: actions/checkout@v5

      - name: Setup Node
        uses: actions/setup-node@v6
        with:
          node-version: 18
          cache: npm

      - name: Install dependencies
        run: npm install

      - name: Build H5
        env:
          VITE_API_BASE_URL: ${{ vars.VITE_API_BASE_URL }}
        run: npm run build:h5

      - name: Setup Pages
        uses: actions/configure-pages@v5

      - name: Upload Pages artifact
        uses: actions/upload-pages-artifact@v3
        with:
          path: dist/build/h5

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

### GitHub 仓库还要做的配置

进入：

```text
Settings -> Pages
```

把 Source 设置为：

```text
GitHub Actions
```

---

### 4.4 修复原项目中的模板与编码问题

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

- `pages/index/index.vue`
- `pages/user/user.vue`
- `pages/notice/detail.vue`
- `components/theme-item/theme-item.vue`
- `pages/preview/preview.vue`
- `pages/search/search.vue`

### 这类问题的本质

不是 GitHub Pages 有问题，也不是 CI 有问题，而是：

- 本地某些工具链容错更高
- 但标准构建器更严格

CI 的价值之一，就是把这类“隐性坏代码”逼出来。

---

### 4.5 页面能打开但没有壁纸：定位运行时接口问题

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

即：**CORS 不通过**

---

### 4.6 为什么静态站点需要额外后端代理

既然上游接口不支持浏览器跨域，那就必须在中间加一层自己的服务。

最终采用的方案是：

- 前端继续部署到 GitHub Pages
- 代理后端部署到 Railway

代理的职责很简单：

1. 接收来自浏览器的请求
2. 转发给 `https://tea.qingnian8.com/api/bizhi`
3. 把上游返回的数据原样回传
4. 在响应中补上 CORS 头

这层代理不是“可选优化”，而是浏览器环境下的必要条件。

---

### 4.7 代理服务的实现方式

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

这样前端最终请求的是：

```text
https://你的代理域名/api/bizhi/classify?select=true
```

而不是直接请求上游。

---

### 4.8 Railway 部署代理

部署步骤如下：

1. 登录 Railway
2. 选择从 GitHub 仓库部署
3. 选择仓库 `cc00mi/00wallpaper`
4. 把 `Root Directory` 设成：

```text
proxy-server
```

5. 配置环境变量：

```text
ALLOWED_ORIGIN=https://cc00mi.github.io
UPSTREAM_ACCESS_KEY=1328433750wuli@
TARGET_ORIGIN=https://tea.qingnian8.com
TARGET_PREFIX=/api/bizhi
PROXY_PREFIX=/api/bizhi
```

6. 生成公网域名，例如：

```text
https://00wallpaper-production.up.railway.app
```

注意：

- `railway.internal` 这种地址不能给浏览器用
- 必须使用 Railway 生成的公网域名

---

### 4.9 回填 GitHub Actions 构建变量

为了让前端在 H5 构建时自动使用代理地址，需要在 GitHub 仓库里增加变量：

```text
Settings -> Secrets and variables -> Actions -> Variables
```

新增：

```text
VITE_API_BASE_URL=https://00wallpaper-production.up.railway.app/api/bizhi
```

之后重新运行 GitHub Pages workflow。

前端请求逻辑会优先读取：

```text
import.meta.env.VITE_API_BASE_URL
```

从而在 H5 环境中走代理，而不是直连上游。

---

## 五、为什么这和普通静态页面部署完全不同

很多人第一次做这类项目时，最大误区是：

> “GitHub Pages 不就是上传静态文件吗？为什么这么复杂？”

问题在于，**uni-app H5 项目并不等于纯静态页面项目**。

它和纯静态页至少有 4 个本质差异。

### 5.1 差异一：它先是“工程”，然后才是“静态产物”

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

### 5.2 差异二：GitHub Pages 是子路径，不是根域部署

纯静态页很多时候直接部署在根路径，或者资源路径手写容易控制。

但 `uni-app + Vite + GitHub Pages` 里，如果不明确写：

```js
base: "/仓库名/"
```

资源路径就会错。

这在本地开发环境不明显，但线上必暴露。

### 5.3 差异三：页面本身是静态的，但业务数据是动态的

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

### 5.4 差异四：浏览器安全模型会接管运行时

本地工具、服务端脚本、curl 可以请求成功，不代表浏览器也能请求成功。

浏览器还会额外检查：

- CORS
- 预检请求
- 自定义请求头是否合法
- 目标域名是否允许该来源访问

所以一旦页面部署到 GitHub Pages，真正的运行时环境就变成了：

- 用户浏览器
- 公网源站
- 标准 Web 安全策略

这和在 HBuilderX 预览、小程序端运行，是两套不同的约束模型。

---

## 六、技术底层分析

### 6.1 GitHub Pages 的本质

GitHub Pages 本质上是一个静态文件托管平台。

它做的事情只有两件：

1. 接收你构建出来的产物
2. 把这些产物通过 CDN / 静态站点形式提供给浏览器访问

它**不执行 Node 后端逻辑**，也**不提供动态 API**。

所以：

- 适合放前端
- 不适合直接承载需要服务端逻辑的业务

### 6.2 GitHub Actions 的本质

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

### 6.3 CORS 的本质

CORS 是浏览器的安全机制。

当页面从：

```text
https://cc00mi.github.io
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

### 6.4 为什么代理能解决问题

加代理后，浏览器请求目标变成：

```text
https://你的代理域名
```

代理服务自己再去请求：

```text
https://tea.qingnian8.com
```

此时：

- 浏览器只和你的代理打交道
- 代理返回你自己定义的 CORS 头
- 上游接口变成服务器之间的通信

于是浏览器的跨域限制就被正确处理掉了。

这不是“绕过浏览器”，而是符合 Web 架构约束的标准做法。

---

## 七、这次实践里最值得复用的经验

### 7.1 不要把“页面能打开”误认为“部署完成”

真正的验收标准应该至少包括：

1. 页面能打开
2. 静态资源无 404
3. API 请求成功
4. 数据能正常渲染
5. 关键交互可用

只做到第 1 步，不叫业务上线。

### 7.2 先解决“可构建”，再解决“可运行”

迁移这类项目时，不要一开始就纠结线上域名、代理、UI 空白。

正确节奏是：

1. 先让项目能在 CI 里稳定 build
2. 再让 Pages 正确托管构建产物
3. 最后处理运行时接口问题

否则会把“构建错误”和“运行时错误”混在一起，排查非常痛苦。

### 7.3 对依赖接口的静态站，代理通常不是补丁，而是架构组成部分

如果业务依赖第三方接口，而你又不能控制对方 CORS，那么：

- 代理层不是临时权宜之计
- 而是正式架构的一部分

从设计阶段就应该考虑进去。

---

## 八、最终可复用模板

如果以后再做类似项目，可以直接复用这一套模板：

### 前端侧

- `uni-app` H5 构建
- `vite.config.js` 配置 Pages 子路径
- GitHub Actions 构建并发布到 Pages
- 用 `VITE_API_BASE_URL` 注入代理地址

### 后端侧

- 单独一个最小 Node 代理
- 独立目录部署到 Railway / Render / Zeabur
- 代理负责：
  - 转发请求
  - 补 CORS
  - 管理上游密钥

### 配置侧

- GitHub 仓库管理前端构建变量
- Railway 管理代理服务环境变量

这样职责清晰，迁移成本低。

---

## 九、结语

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

> Uni-app 项目部署到 GitHub Pages，真正的分界线不在“能不能发版”，而在“页面上线后是否还能拿到真实业务数据”。

如果只是纯静态站，GitHub Pages 足够。  
如果页面依赖运行时接口，那就必须把代理、CORS 和部署拓扑一起设计进去。

从系列化的视角看

- 上一篇，解决“纯静态网页如何通过 GitHub 自动构建与发布”
- 这一篇，解决“工程化前端项目如何通过 GitHub CI/CD 稳定上线”
- 下一步，则可以继续扩展到更复杂的项目形态，例如：
  - 带后端服务的前后端分离项目
  - 带数据库的全栈应用
  - 带多环境区分的项目
  - 带灰度、回滚、监控的生产级交付流程

如果说静态网页部署是 GitHub CI/CD 的入门层，那么 `uni-app + Pages + 代理` 这类项目就是一个非常典型的中阶过渡层：

- 它已经不再只是“传文件”
- 但又还没有重到必须完整引入 Kubernetes、容器编排、复杂后端集群

---

## 十、从 uni-app 再往后：更复杂项目如何使用 CI/CD 部署

如果把“静态网页自动部署”看成入门，把“`uni-app + GitHub Pages + 代理`”看成进阶，那么再往后，CI/CD 体系通常会继续演进到更复杂的项目类型。

这时重点不再只是“能不能构建并发布”，而会逐渐转向：

- 如何管理多个服务
- 如何管理多个环境
- 如何保证发布稳定性
- 如何实现回滚、监控和灰度

下面可以把更复杂项目大致分成 4 类来看。

### 10.1 前后端分离项目

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

### 10.2 Monorepo / 多服务项目

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

### 10.3 含数据库迁移的全栈项目

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

### 10.4 生产级项目：灰度、回滚、监控

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