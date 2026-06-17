这两个文件都是**构建配置文件**，不是你真正写业务逻辑的 `.cpp/.h` 文件。

你可以把它们理解成：

```text
.cpp / .h         = 项目源码
CMakeLists.txt   = 告诉 CMake 怎么编译这个项目
Makefile         = 告诉 make 怎么编译、运行、测试这个项目
```

---
**Makefile 和 CMakeLists.txt 是干什么的**

`Makefile` 是现在这个 toy 项目的主要启动脚本。你输入：

```PowerShell
mingw32-make run
```

它会按照 `Makefile` 里的规则去编译、链接、运行。你可以把它理解成：

```
给 mingw32-make 看的施工说明书
```

里面写了：

```make
test: 编译并运行测试
all: 编译后端服务
run: 启动后端服务
clean: 删除 build 目录
```

`CMakeLists.txt` 是更成熟 C++ 项目常用的构建说明。它不是直接给 `mingw32-make` 用的，而是给 CMake 用的。你可以把它理解成：

```
更通用、更工程化的施工图纸
```

后面接 Drogon、MySQL、Redis、第三方库时，主要会靠 `CMakeLists.txt`。所以现在两者的关系是：

```
Makefile：当前 toy 版方便启动
CMakeLists.txt：后续产品版工程化构建
```

## 1. `CMakeLists.txt` 是干嘛的？

它是 **CMake 的项目配置文件**。

CMake 是跨平台构建工具，Windows / Linux / macOS 都能用。它负责告诉编译器：

```text
项目叫什么
用 C++ 几
有哪些 .cpp 文件
生成哪个 exe
要不要链接库
测试程序怎么编译
```

比如它里面通常会有类似内容：

```cmake
cmake_minimum_required(VERSION 3.16)
project(cpp-ai-copilot)

set(CMAKE_CXX_STANDARD 17)

add_executable(cpp-ai-copilot
    src/main.cpp
    src/server.cpp
)
```

意思就是：把这些 `.cpp` 编译成 `cpp-ai-copilot.exe`。

你在 Windows 上更推荐用这个。

常用命令：

```powershell
cmake -S . -B build
cmake --build build
```

然后运行：

```powershell
.\build\cpp-ai-copilot.exe .\config\app.env
```

---

## 2. `Makefile` 是干嘛的？

`Makefile` 是给 `make` 工具用的。

它一般把一堆复杂命令封装成简单命令，比如：

```powershell
make run
make test
make clean
```

本质上它可能帮你执行：

```powershell
cmake -S . -B build
cmake --build build
.\build\cpp-ai-copilot.exe .\config\app.env
```

所以你文档里说：

```powershell
make run
```

就是因为作者在 `Makefile` 里面提前写好了 `run` 这个任务。

但是你刚才报错：

```text
make : 无法将“make”项识别为 cmdlet
```

说明你的 Windows 当前没有安装 `make`，所以这个方式暂时用不了。

---

## 3. 为什么项目里同时有这两个？

因为作者给了两种启动方式：

```text
方式一：Makefile
适合 Linux / macOS / 安装了 make 的 Windows 环境
命令简单：make run

方式二：CMake
跨平台标准方式
Windows 更推荐
命令稍微长一点，但更通用
```

也就是说：

```text
Makefile = 快捷按钮
CMakeLists.txt = 真正标准的构建说明书
```

---

## 4. 你现在应该用哪个？

你现在在 Windows PowerShell 里，先用 **CMakeLists.txt 对应的 CMake 方式**，不要用 `make run`。

进入项目目录后执行：

```powershell
cd "E:\note\code\cpp-ai-copilot"
```

如果已经有 `build` 目录和 exe，直接运行：

```powershell
.\build\cpp-ai-copilot.exe .\config\app.env
```

如果你改了代码，需要重新编译：

```powershell
cmake -S . -B build
cmake --build build
```

再启动：

```powershell
.\build\cpp-ai-copilot.exe .\config\app.env
```

---

另外你截图里文件名前面的 **M**，一般表示 Git 认为这个文件被修改过了，`M = Modified`，不是错误。

## `CMake` 和 `make` 都是用来**编译 C/C++ 项目**的工具，但它们不是一回事。

可以这样理解：

```text
.cpp / .h 源码
   ↓
CMakeLists.txt：告诉 CMake 项目怎么组织
   ↓
CMake 生成构建文件
   ↓
Makefile / VS 工程 / Ninja 文件
   ↓
make 或其他构建工具真正调用编译器
   ↓
生成 .exe
```

---

## 1. 编译器是干嘛的？

真正把 C++ 代码变成 `.exe` 的是编译器，比如：

```text
g++        Linux/MinGW 常用
clang++    macOS/Linux 常用
MSVC       Windows Visual Studio 常用
```

比如最原始可以这样编译：

```powershell
g++ main.cpp server.cpp -o app.exe
```

但是项目一大，`.cpp` 很多，依赖很多，每次手敲命令就很麻烦，所以才需要 `CMake` / `make`。

---

## 2. make 是干嘛的？

`make` 是一个**自动化构建工具**。

它读取项目里的：

```text
Makefile
```

然后根据里面写好的规则执行命令。

比如 `Makefile` 里可能写了：

```makefile
run:
	cmake --build build
	./build/cpp-ai-copilot config/app.env

test:
	cmake --build build
	ctest --test-dir build
```

所以你只需要输入：

```powershell
make run
```

它就会自动帮你执行一堆命令。

但是你刚才报错：

```text
make : 无法将“make”项识别为 cmdlet
```

说明你的 Windows 里没有安装 `make`，所以现在不能直接用 `make run`。

一句话：

```text
make = 按 Makefile 里的规则自动执行编译、运行、测试命令
```

---

## 3. CMake 是干嘛的？

`CMake` 是一个**跨平台项目构建生成工具**。

它读取：

```text
CMakeLists.txt
```

然后根据你的系统生成对应的构建文件。

在 Linux 上，它可以生成：

```text
Makefile
```

在 Windows 上，它可以生成：

```text
Visual Studio 工程
Ninja 构建文件
Makefile
```

所以 `CMake` 更像是一个“构建工程生成器”。

一句话：

```text
CMake = 根据 CMakeLists.txt 生成适合当前平台的构建工程
```

---

## 4. 二者核心区别

|工具|读取的文件|主要作用|类比|
|---|---|---|---|
|`make`|`Makefile`|直接执行编译/运行/测试命令|执行员|
|`CMake`|`CMakeLists.txt`|生成 Makefile / VS 工程 / Ninja 文件|项目工程生成器|

更通俗地说：

```text
CMakeLists.txt  = 项目说明书
CMake           = 根据说明书生成施工方案
Makefile        = 具体施工命令
make            = 按施工命令干活
```

---

## 5. 你这个项目应该怎么用？

你现在是 Windows，优先用 `CMake`，不要先折腾 `make`。

进入项目目录：

```powershell
cd "E:\note\code\cpp-ai-copilot"
```

重新生成构建目录：

```powershell
cmake -S . -B build
```

编译：

```powershell
cmake --build build
```

运行：

```powershell
.\build\cpp-ai-copilot.exe .\config\app.env
```

如果你已经看到 `build` 里面有：

```text
cpp-ai-copilot.exe
test_core.exe
```

那可以直接运行：

```powershell
.\build\cpp-ai-copilot.exe .\config\app.env
```

---

## 6. 为什么有了 CMake 还要有 Makefile？

很多项目会这样设计：

```text
CMakeLists.txt：标准构建配置
Makefile：封装快捷命令
```

例如：

```powershell
make run
```

可能本质上就是帮你执行：

```powershell
cmake -S . -B build
cmake --build build
.\build\cpp-ai-copilot.exe .\config\app.env
```

所以 `Makefile` 只是让命令更短、更方便。

---

你现在记住这句话就够了：

```text
CMake 是用来“生成构建工程”的，make 是用来“执行构建命令”的；
Windows 下你没装 make，所以直接用 cmake --build build 最稳。
```

## 01进入项目目录
#### 你这个主要是 **路径问题 + 路径里有空格没加引号**。

你第三条其实已经接近对了，但是你写的是：

```powershell
cd E:\note\面试八股 1\6_AI应用开发\20_Cpp企业AI_Copilot项目\code\cpp-ai-copilot
```

这里面有空格：

```text
面试八股 1
```

PowerShell 会把它拆开，所以报错。

---

你直接复制这一条试试：

```powershell
cd "E:\note\面试八股 1\6_AI应用开发\20_Cpp企业AI_Copilot项目\code\cpp-ai-copilot"
```

或者更稳一点：

```powershell
Set-Location -LiteralPath "E:\note\面试八股 1\6_AI应用开发\20_Cpp企业AI_Copilot项目\code\cpp-ai-copilot"
```

进去之后检查一下：

```powershell
dir
```

如果能看到：

```text
CMakeLists.txt
Makefile
build
config
```

说明你进对目录了。

然后启动项目：

```powershell
.\build\cpp-ai-copilot.exe .\config\app.env
```

---

如果还是提示路径不存在，就用这个命令自动搜索项目真实位置：

```powershell
Get-ChildItem -Path "E:\note" -Recurse -Directory -Filter "cpp-ai-copilot" -ErrorAction SilentlyContinue | Select-Object FullName
```

它会输出类似：

```text
FullName
--------
E:\note\面试八股 1\6_AI应用开发\20_Cpp企业AI_Copilot项目\code\cpp-ai-copilot
```

然后你复制输出的完整路径，加引号进入：

```powershell
cd "这里粘贴搜索出来的完整路径"
```

---

你现在最可能正确的一整套命令是：

```powershell
cd "E:\note\面试八股 1\6_AI应用开发\20_Cpp企业AI_Copilot项目\code\cpp-ai-copilot"
.\build\cpp-ai-copilot.exe .\config\app.env
```

核心记住一句：==**Windows PowerShell 路径里只要有空格，就必须用英文双引号包起来。**


## 02调用该服务
#### 已经启动成功了。关键日志是这一句：

```
[INFO] cpp-ai-copilot listening on http://127.0.0.1:18080
```

意思是：你的 C++ 服务已经在本机 `18080` 端口监听了。

---

#### 下一步 1：不要关闭这个 PowerShell

这个窗口现在就是你的服务进程。  
只要它还开着，服务就还在运行。

你可以直接打开浏览器访问：

```
http://127.0.0.1:18080
```

如果文档里有 `/health` 接口，就访问：

```
http://127.0.0.1:18080/health
```

注意：你现在实际端口是 **18080**，不是之前文档里可能写的 `8080`。

---

#### 下一步 2：新开一个 PowerShell 测试接口

因为当前 PowerShell 被服务占用了，所以你要再开一个 PowerShell，然后执行：

```
curl http://127.0.0.1:18080/health
```

或者：

```
Invoke-WebRequest http://127.0.0.1:18080/health
```

如果返回类似：

```
OK
```

或者 JSON 内容，就说明服务正常。

---

#### 下一步 3：如果访问首页没内容，不一定是错

很多后端项目没有首页 `/`，只提供接口。  
所以访问：

```
http://127.0.0.1:18080
```

可能显示 404、Not Found、空白页，这不一定代表服务失败。

你应该优先测：

```
http://127.0.0.1:18080/health
```

---

#### 下一步 4：停止服务的方法

在当前这个正在运行服务的 PowerShell 里按：

```
Ctrl + C
```

就能停止服务。

---

你现在可以先在浏览器打开：

```
http://127.0.0.1:18080/health
```

看看返回什么。


![[Pasted image 20260616165754.png]]

![[Pasted image 20260616165747.png]]


这就说明 **项目已经启动成功，而且健康检查通过了**。

你现在看到的：

```json
{"status":"ok","service":"cpp-ai-copilot","version":"0.1.0"}
```

含义是：

```text
status: ok              服务正常
service: cpp-ai-copilot 当前服务名
version: 0.1.0          项目版本
```

PowerShell 里也返回了：

```text
StatusCode        : 200
StatusDescription : OK
```

这就表示 HTTP 请求成功。

---

##03 现在你可以做三件事。

### 1. 保持服务运行

右边那个叫：

```text
cpp-ai-copilot
```

的终端不要关，它是服务端。

只要它还在运行，你就可以访问：

```text
http://127.0.0.1:18080/health
```

停止服务的时候，在那个终端按：

```text
Ctrl + C
```

---

### 2. 看项目还有哪些接口

你现在只是测了 `/health` 接口。

接下来要看 README 里有没有接口说明。你可以在项目目录执行：

```powershell
type .\README.md
```

或者用 VS Code/Obsidian 直接打开 `README.md`。

重点找这些关键词：

```text
/health
/chat
/api
/ask
/index
/search
```

也可以直接用命令搜：

```powershell
Select-String -Path .\src\*.cpp,.\include\*.h -Pattern "/health","/chat","/api","POST","GET"
```

这个命令会帮你找源码里注册了哪些 HTTP 路由。

**README.md 中文乱码**，不是项目坏了。  
原因是：你的 `README.md` 大概率是 **UTF-8 编码**，但是 PowerShell/终端按别的编码方式显示了，所以中文变成了乱码。

你的服务已经正常：

```
{"status":"ok","service":"cpp-ai-copilot","version":"0.1.0"}
```

现在只是文档显示乱码。

---

#### 先用这个办法重新看 README

在当前项目目录执行：

```bash
chcp 65001[Console]::OutputEncoding = [System.Text.Encoding]::UTF8Get-Content .\README.md -Encoding UTF8
```

不要再用：

```
type .\README.md
```

`type` 在 PowerShell 里其实是 `Get-Content` 的别名，有时候编码显示不稳定。


---

### 3. 运行测试程序

你目录里有：

```text
build\test_core.exe
```

可以新开一个 PowerShell，进入项目目录后运行：

```powershell
.\build\test_core.exe
```

或者运行 CTest：

```powershell
ctest --test-dir build -V
```

如果测试通过，说明核心模块也正常。

---

你现在的状态可以总结成：

```text
项目路径已进入正确
exe 已成功启动
18080 端口监听正常
/health 接口返回 200 OK
服务没有问题
```

下一步就是看 `README.md` 或源码，找到真正的业务接口，然后用浏览器、curl 或 Postman 去调用。


## 04找目前实现了哪些接口

执行：

```
Select-String -Path .\src\*.cpp,.\include\copilot\*.hpp -Pattern "/health","GET","POST","route","add_route","Router"
```

它会帮你定位 `/health` 是在哪里注册的。

大概率在：

```
src/application.cpp
```

里面会有类似：

```
router.get("/health", ...);
```

或者：

```
router.add_route("GET", "/health", ...);
```


## 05现在最该打开这几个文件

先看这个：

```
Get-Content .\src\application.cpp -Encoding UTF8
```

这里能看到 `/health` 是怎么写的。

然后看：

```
Get-Content .\src\router.cpp -Encoding UTF8
```

这里能看到 `GET /health` 是怎么被存进 `routes_` 表里的。

再看：

```
Get-Content .\src\simple_server.cpp -Encoding UTF8
```

这里能看到服务端怎么接收浏览器请求。

---

## 06你可以做一个小实验

访问一个不存在的接口：

```
curl.exe -i http://127.0.0.1:18080/not_exist
```

你应该会看到类似：

```
HTTP/1.1 404{"error":"route_not_found"}
```

这对应你搜到的这段：

```
src\router.cpp:15: if (found == routes_.end()) {src\router.cpp:16:     return HttpResponse::json(404, R"({"error":"route_not_found"})");}
```

意思是：

```
Router 没找到对应路径  ↓返回 404
```

---

## 07下一步可以加一个自己的接口

比如在 `src/application.cpp` 里面，仿照 `/health` 加一个：

```
router.get("/api/v1/ping", [](const HttpRequest&) {    return HttpResponse::json(200, R"({"message":"pong"})");});
```

加完之后重新编译：

```
mingw32-make all
```

或者：

```
cmake --build build
```

然后重新启动服务：

```
.\build\cpp-ai-copilot.exe .\config\app.env
```

再访问：

```
curl.exe http://127.0.0.1:18080/api/v1/ping
```

预期返回：

```
{"message":"pong"}
```

这样你就从“会启动项目”进入到“能改接口”的阶段了。

