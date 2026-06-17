# 06 application 模块逐行讲解

这一篇专门对照下面两个文件看：

```text
code/cpp-ai-copilot/include/copilot/application.hpp
code/cpp-ai-copilot/src/application.cpp
```

你可以把 `application` 模块理解成：

```text
公司的"业务登记表"。
公司开张前，把有哪些窗口（接口）、每个窗口谁负责（Handler）
全部登记好，然后交给前台（Router）。
```

---

## 1. 这个模块为什么这么小

整个模块只有一个函数：

```text
application.hpp → 7 行
application.cpp → 20 行
```

**这是故意的。** 它的职责极单纯：创建 Router，往里注册 Handler，返回 Router。以后项目变复杂了，所有新接口都只在这里加——别的模块不用动。

---

## 2. application.hpp 全貌

源码：

```cpp
#pragma once

#include "copilot/router.hpp"

namespace copilot {

Router create_app_router();

}  // namespace copilot
```

### 2.1 逐行解释

```cpp
#include "copilot/router.hpp"
```

需要 `Router` 这个类型——函数返回值是 `Router`。

```cpp
Router create_app_router();
// ↑           ↑
// 返回类型   函数名（创建应用的路由表）
```

**声明**一个函数，告诉编译器：

> 存在一个叫 `create_app_router` 的函数，它不接受参数，返回一个配置好的 `Router` 对象。

实现放在 `.cpp` 里。

---

## 3. application.cpp 全貌

源码：

```cpp
#include "copilot/application.hpp"
#include "copilot/http.hpp"

namespace copilot {

Router create_app_router() {
    Router router;

    router.get("/health", [](const HttpRequest&) {
        return HttpResponse::json(
            200,
            R"({"status":"ok","service":"cpp-ai-copilot","version":"0.1.0"})");
    });

    return router;
}

}  // namespace copilot
```

### 3.1 为什么还包含了 `http.hpp`

虽然 `application.hpp` 只包含 `router.hpp`，但 `.cpp` 里多包含了 `http.hpp`，因为 Handler（那个 lambda）里用到了 `HttpRequest` 和 `HttpResponse`。

```cpp
[](const HttpRequest&) {                    // ← HttpRequest 定义在 http.hpp
    return HttpResponse::json(200, "...");  // ← HttpResponse 定义在 http.hpp
}
```

---

## 4. 逐行翻译

### 4.1 函数签名

```cpp
Router create_app_router() {
```

`Router` 是自建类型，跟 `int`、`string` 没本质区别——函数返回一个 `Router` 对象。

### 4.2 建空表

```cpp
    Router router;
```

此时 `router` 内部 `routes_` 是空的——还没有注册任何接口。

### 4.3 注册路由

```cpp
    router.get("/health", [](const HttpRequest&) {
        return HttpResponse::json(
            200,
            R"({"status":"ok","service":"cpp-ai-copilot","version":"0.1.0"})");
    });
```

拆开看：

| 部分                               | 含义                                |
| -------------------------------- | --------------------------------- |
| `router.get(...)`                | 调 Router 的 `get()` 方法，注册一条 GET 路由 |
| `"/health"`                      | URL 路径，以后 `GET /health` 的请求归它处理   |
| `[](const HttpRequest&) { ... }` | 处理函数（Handler），一个 lambda           |
| `HttpResponse::json(200, ...)`   | 工厂方法，创建一个 200 响应，body 是 JSON      |
| `R"(...)"`                       | 原始字符串，避免 JSON 里的 `"` 需要转义         |

**效果**：在路由表里记了一条：

```text
"GET /health"  →  返回 {"status":"ok","service":"cpp-ai-copilot","version":"0.1.0"}
```

### 4.4 返回路由表

```cpp
    return router;
}
```

把填好的 Router 交出去。谁调 `create_app_router()` 谁拿到配置好的路由表。

---

## 5. 它在整个项目里的位置

```text
main.cpp
    ↓
create_app_router()          ← application.cpp（就是这里）
    ↓
返回填好的 Router 对象
    ↓
传给 SimpleHttpServer 构造函数
    ↓
server 存进 router_ 成员
    ↓
server.run() 里每次收到请求 → router_.route(request)
```

它是 `main.cpp` 和 `Router` 之间的**唯一桥梁**。

---

## 6. 以后怎么加新接口

现在只有一个 `/health`。以后你要加 RAG、聊天、文档上传，全在这里改。

比如加一个获取用户列表的接口：

```cpp
Router create_app_router() {
    Router router;

    // 原来的 /health
    router.get("/health", [](const HttpRequest&) {
        return HttpResponse::json(200, R"({"status":"ok"})");
    });

    // 新加的 /users
    router.get("/users", [](const HttpRequest&) {
        std::string users_json = get_all_users_from_db();  // 你的业务逻辑
        return HttpResponse::json(200, users_json);
    });

    // 新加的 /chat（POST）
    router.post("/chat", [](const HttpRequest& request) {
        // request.body 里有用户发来的 JSON
        // 调用 AI 模型，返回结果
        return HttpResponse::json(200, ai_response);
    });

    return router;
}
```

**只改这一个文件**，Router 不用动，server 不用动，http 不用动。

---

## 7. 这段代码你最该掌握什么

```text
1. application 是项目的"业务入口"——所有接口在这里注册
2. create_app_router() 返回填好路由的 Router 对象
3. router.get("/health", lambda) — 注册一条路由的完整写法
4. lambda 作为 Handler 传入 Router，体现了 std::function 的万能包装能力
5. 以后加新接口只改这一个文件——这就是"开闭原则"的体现
```

## 8. 后面新加的内容
```cpp
static std::string extract_message(const std::string& body) { 
	const std::string key = "\"message\""; 
	auto pos = body.find(key); 
	if (pos == std::string::npos) { 
	return ""; 
	} 
	
	pos = body.find(':', pos); 
	if (pos == std::string::npos) { 
	return ""; 
	} 
	
	pos = body.find('"', pos); 
	if (pos == std::string::npos) { 
	return ""; 
	} 
	
	auto end = body.find('"', pos + 1);
	if (end == std::string::npos) { 
	return ""; 
	} 
	
	return body.substr(pos + 1, end - pos - 1); 
	}
```

这段代码的作用是：

```
从请求体 body 里，找到 JSON 字符串中的 "message" 字段，然后把 message 对应的内容提取出来。
```

比如请求体是：

```JSON
{"message":"你好"}
```

调用：

```cpp
std::string message = extract_message(request.body);
```

最后得到：

```cpp
message = "你好";
```

---

### 1. 函数整体含义

```cpp
static std::string extract_message(const std::string& body)
```

拆开看：

```cpp
static
```

表示这个函数只在当前 `.cpp` 文件里可见。  
比如你把它写在 `application.cpp` 里，那么别的 `.cpp` 文件不能直接调用它。

这里的 `static` 不是类里面的静态成员函数，而是 **限制函数作用域**。

---

```cpp
std::string
```

表示这个函数返回一个字符串。

比如返回：

```cpp
"你好"
```

或者返回：

```cpp
""
```

---

```cpp
extract_message
```

函数名，意思是“提取 message”。

---

```cpp
const std::string& body
```

表示传进来的 HTTP 请求体。

比如：

```cpp
body = "{\"message\":\"你好\"}"
```

这里用引用 `&` 是为了避免复制一份字符串，提高效率。  
加 `const` 是为了保证函数内部不会修改 `body`。

---

### 2. 找 `"message"` 这个字段

```cpp
const std::string key = "\"message\"";
```

这里定义要查找的关键字。

为什么写成：

```cpp
"\"message\""
```

而不是：

```cpp
""message""
```

因为 C++ 字符串本身用双引号包住，所以内部的双引号要转义。

```cpp
\" 
```

表示字符串里的一个真正的双引号。

所以：

```cpp
"\"message\""
```

实际内容是：

```cpp
"message"
```

---

然后：

```cpp
auto pos = body.find(key);
```

在 `body` 里面找 `"message"` 出现的位置。

比如：

```cpp
body = "{\"message\":\"你好\"}"
```

可以看成：

```
{ " m e s s a g e " : " 你 好 " }
  ^  pos 找到这里
```

如果找到了，`pos` 就是 `"message"` 开始的位置。

如果没找到，`find()` 会返回：

```cpp
std::string::npos
```

所以这里判断：

```cpp
if (pos == std::string::npos) {
    return "";
    }
```
意思是：
```
如果body里没有"message"字段，那就返回空字符串。
```

### 3. 找冒号 `:`

```
pos = body.find(':', pos);
```

这句的意思是：

```
从 "message" 出现的位置开始，继续往后找冒号 :
```

因为 JSON 是这样的：

```
{"message":"你好"}
```

`message` 后面应该有一个冒号：

```
"message" : "你好"          ^
```

如果没找到冒号：

```
if (pos == std::string::npos) {    return "";}
```

说明格式不对，直接返回空字符串。

---

### 4. 找 message 值前面的双引号

```
pos = body.find('"', pos);
```

这句是从冒号位置继续往后找第一个双引号。

比如：

```
{"message":"你好"}
```

冒号后面是：

```
: "你好"  ^  找到这个双引号
```

找到后，`pos` 指向值开始前的双引号。

如果找不到：

```
if (pos == std::string::npos) {    return "";}
```

说明 `message` 后面不是字符串格式，返回空。

---

### 5. 找 message 值后面的双引号

```
auto end = body.find('"', pos + 1);
```

这句是从 `pos + 1` 位置继续往后找下一个双引号。

也就是找字符串结束位置。

比如：

```
{"message":"你好"}
```

现在：

```
"你好"^    ^pos  end
```

`pos` 是前面的双引号，`end` 是后面的双引号。

如果找不到结束双引号：

```
if (end == std::string::npos) {    return "";}
```

说明 JSON 字符串不完整，返回空。

---

### 6. 提取中间内容

```
return body.substr(pos + 1, end - pos - 1);
```

这句最关键。

`substr` 的格式是：

```
字符串.substr(开始位置, 长度)
```

现在：

```
"你好"^    ^pos  end
```

真正想要的是双引号中间的内容：

```
你好
```

所以开始位置是：

```
pos + 1
```

长度是：

```
end - pos - 1
```

举个例子：

```
body = {"message":"你好"}前双引号位置 pos = 11后双引号位置 end = 18
```

那么：

```
body.substr(pos + 1, end - pos - 1)
```

就是：

```
body.substr(12, 6)
```

因为中文 UTF-8 下一个汉字可能占多个字节，`std::string` 按字节算，但整体截取不会有问题，最终得到：

```
"你好"
```

---

### 7. 整体流程图

```
body = {"message":"你好"}
        ↓
找 "message"
        ↓
找 message 后面的 :
        ↓
找 : 后面的第一个 " 
       ↓
找下一个 " 
       ↓
截取两个 " 中间的内容
        ↓
    返回 "你好"
```

---

### 8. 放到接口里就是这样

```cpp
router.post("/api/v1/chat", [](const HttpRequest& request) {
    std::string message = extract_message(request.body);
    std::string response =        
	    R"({"code":"OK","data":{"reply":"我收到了：)" + message + R"("}})";
	    
    return HttpResponse::json(200, response);
});
```

如果请求是：

```JSON
{"message":"你好"}
```

那么：

```cpp
request.body
```

就是：

```cpp
"{\"message\":\"你好\"}"
```

然后：

```cpp
extract_message(request.body)
```

返回：

```cpp
"你好"
```

最后拼成：

```JSON
{"code":"OK","data":{"reply":"我收到了：你好"}}
```

---

### 9. 这段代码的局限性

这不是完整 JSON 解析器，只是学习阶段的简化写法。

它能处理：

```JSON
{"message":"你好"}
```

也大概率能处理：

```JSON
{  "message": "你好"}
```

但是遇到复杂情况就不稳，比如：

```JSON
{"message":"他说：\"你好\""}
```

或者：

```JSON
{"data":{"message":"你好"}}
```

或者：

```JSON
{"message":123}
```

所以它适合理解：

```
request.body → 提取参数 → 生成响应
```

后面真正做项目时，应该用 JSON 库，比如 `nlohmann/json`。

这样你发：

```PowerShell
curl.exe -X POST http://127.0.0.1:18080/api/v1/chat -H "Content-Type: application/json" -d "{\"message\":\"你好\"}"
```

#### 更推荐加 `-i` 看完整 HTTP 响应

```PowerShell
curl.exe -i -X POST "http://127.0.0.1:18080/api/v1/chat" -H "Content-Type: application/json" -d '{"message":"你好"}'
```

后端就能根据 body 返回：

```JSON
## 9.A extract_message 改进版——和旧版的两处差异

对比旧版（第 8 节）和新版（当前 `application.cpp` 里的实际代码），有两处改动：

### 改动点速览

| 行 | 旧版 | 新版 | 改了什么 |
|---|---|---|---|
| key 定义 | `"\"message\""` | `R"("message")"` | 原始字符串，不用转义，更清晰 |
| 找冒号 | `body.find(':', pos)` | `body.find(':', pos + key.size())` | 从 key **后面**开始找 |
| 找前双引号 | `body.find('"', pos)` | `body.find('"', pos + 1)` | 跳过当前字符再找 |

### 改动 1：`R"("message")"` 替代 `"\"message\""`

```cpp
// 旧版——反斜杠转义，看起来累
const std::string key = "\"message\"";

// 新版——原始字符串，所见即所得
const std::string key = R"("message")";
```

`R"(...)"` 是 C++ 的**原始字符串字面量**——括号里写什么，字符串内容就是什么，不需要 `\"` 转义。在 [[04_http模块逐行讲解]] 第 7.4 节有详细讲解。

### 改动 2：`pos + key.size()` —— 从 key 后面开始找冒号

这是**语义修正**。旧版虽然碰巧能跑，但逻辑不严谨。

**旧版**：

```cpp
pos = body.find(':', pos);
// pos 指向 "message" 的开头（第一个双引号）
// 从 key 开头开始找 ':'  →  语义上很奇怪："在 message 里面找冒号？"
```

**新版**：

```cpp
pos = body.find(':', pos + key.size());
// pos + key.size() = 跳过整个 "message"
// 从 key 结尾往后找 ':'  →  语义正确："在 message 后面找冒号"
```

画图对比：

```text
body = {"message":"你好"}
       0123456789012
       
旧版 find(':', pos)：   从位置 1 开始找 → 找到位置 11
新版 find(':', pos+10)： 从位置 11 开始找 → 找到位置 11

结果一样，但新版的意图清楚："跳过 key，找 key 后面的冒号。"
```

### 改动 3：`pos + 1` —— 跳过冒号本身再找双引号

**旧版**：

```cpp
pos = body.find('"', pos);   // pos 当前指向 ':'
// 从 ':' 位置开始找 '"'
```

如果冒号后面紧跟着 `"`（`{"message":"你好"}`），没问题，因为 `find` 会从 `:` 开始，下一个字符就是 `"`。

但如果冒号后面有空格呢？

```text
body = {"message": "你好"}
                 ^^
                 冒号+空格
```

```cpp
旧版 find('"', pos)：  从 ':' 开始找 → 跳过一个空格 → 找到 "
                      → 依然能工作（因为 find 不是从 pos+1 开始，是从 pos 开始）
```

旧版实际上也能处理这种情况。**真正的区别仍然是语义**：

```cpp
// 新版
pos = body.find('"', pos + 1);
// pos 指向 ':'，pos + 1 跳过冒号
// 语义："从冒号往后，找第一个双引号"
```

### 一句话总结两个改动

```text
旧版：从当前位置开始找 → "碰巧能跑，但如果你是代码审查者，会皱眉头"
新版：明确跳过已处理的部分再找 → "每一步都告诉你'从哪往后找'"
```

这就是**防御性编程**的体现——代码不仅要做对，还要让读的人一眼看出它为什么对。

---

## 10. 面试常问：POST body / Content-Type / 参数校验 / 400 / 统一响应结构

这 5 个问题是由 `POST /api/v1/chat` 这个接口自然引出的，面试中常被追问。

### 10.1 POST body 是什么

**POST body = HTTP 请求的消息体（Message Body）**，是请求头和空行之后的那一段数据。

> 不只有 POST 才有 body——PUT、PATCH 也有。GET 理论上可以有 body，但实际很少用。

复习一下 HTTP 请求结构：

```text
POST /api/v1/chat HTTP/1.1          ← 请求行
Content-Type: application/json       ← 请求头（元信息）
Content-Length: 21                   ← 告诉服务器 body 字节数
                                     ← 空行（头结束标志）
{"message":"你好"}                   ← 这就是 POST body
```

**body 不是浏览器"自动"生成的**，是前端代码或 `curl` 命令主动写入的。在你的服务端代码中，`request.body` 就是这个字符串。

### 10.2 Content-Type 是什么

**Content-Type 告诉服务端："body 里的数据是什么格式，你应该用什么方式解析。"**

它不是浏览器自动生成的，是请求发送方（前端/curl/Postman）主动设置的。

| Content-Type | 含义 | body 示例 |
|---|---|---|
| `application/json` | JSON 格式 | `{"message":"你好"}` |
| `application/x-www-form-urlencoded` | 表单格式 | `message=%E4%BD%A0%E5%A5%BD` |
| `multipart/form-data` | 文件上传 | 分多段，每段一个文件 |
| `text/plain` | 纯文本 | `你好` |

**如果 Content-Type 和 body 实际格式不匹配**，服务端按错误格式解析就会失败——这直接引出下一个问题。

### 10.3 参数为什么会缺失（四种情况）

参数缺失 ≠ 客户端没发。更多时候是**两边对参数的名字、格式、位置没有对齐**。

**情况一：客户端确实没发这个字段**

```json
{"content":"你好"}   // 发了 content，但没发 message
```

`extract_message()` 找 `"message"` → `find()` 返回 `npos` → 返回空串。

**情况二：Content-Type 不匹配（最隐蔽）**

客户端用 `application/x-www-form-urlencoded` 发：

```text
message=%E4%BD%A0%E5%A5%BD
```

但服务端按 JSON 的格式去找 `"message"` —— 根本找不到，因为 body 不是 JSON。

**情况三：参数名字写错了**

```json
{"msg":"你好"}        // 客户端发了 msg
// extract_message 找的是 "message" → 找不到
```

**情况四：JSON 嵌套层级问题**

```json
{"data":{"message":"你好"}}   // message 在 data 子对象里
```

你的简易 `extract_message` 只做平层查找，嵌套结构处理不了。

> **一句话总结**：真实项目要引入 JSON schema 校验 + JSON 库（如 `nlohmann/json`），而不是手写字符串查找。

### 10.4 为什么缺参数要返回 400

> **400 Bad Request 是 HTTP 规范定义的标准状态码，语义是"服务器无法处理，因为这个请求本身有客户端错误"。**

HTTP 状态码是分大类的：

| 范围 | 含义 | 例子 |
|---|---|---|
| 2xx | 成功 | 200 OK |
| 3xx | 重定向 | 301 Moved Permanently |
| **4xx** | **客户端错误** | **400 Bad Request**, 401, 404 |
| 5xx | 服务端错误 | 500 Internal Server Error |

**返回 400 而不是 500 的关键区别**：

- `400` → "**你**的请求有问题，改了再试"
- `500` → "**服务器**自己崩了，跟你没关系"

如果缺参数返回 500，对方排查时第一反应是"服务器是不是挂了"，而不是"我是不是少传了参数"。**用对状态码，就是在帮排查者省时间。**

### 10.5 为什么返回结构要统一

HTTP 协议只规定了 **状态行 + 头 + body** 的格式，body 里面长什么样 HTTP 不管。这两种都是合法的 HTTP 响应：

```json
{"status":"ok"}
{"code":200,"data":{}}
```

**统一结构不是为了满足 HTTP 协议，而是为了让前端用一个函数处理所有接口返回值。**

不统一的痛苦（前端要写多套判断逻辑）：

```javascript
const res1 = await fetch("/health");   // {"status":"ok",...}
const res2 = await fetch("/chat");     // {"code":"OK","data":{...}}

if (res1.status === "ok") { ... }      // 每套接口一种判断方式
if (res2.code === "OK") { ... }        // 累死
```

统一后的清爽：

```javascript
const res = await fetch("/api/v1/chat", {body: ...});
if (res.error) { showError(res.error); }    // 一套逻辑覆盖全部接口
else           { useData(res.data); }
```

> **统一结构 = 前后端之间的 API 契约（API Contract）**。所有接口返回同一种形状，前端只写一套"判成功/失败 → 取数据/取错误信息"的逻辑。

### 10.6 面试时一句话回答这 5 个问题

> POST body 是 HTTP 请求体的数据，Content-Type 告诉服务端 body 是什么格式以便正确解析。参数缺失不只是客户端没传——Content-Type 不匹配、参数名不一致、JSON 嵌套层级问题都会导致服务端解析不到。缺参数返回 400 是因为 HTTP 规范里 4xx 代表客户端错误，告诉调用方"是你的请求有问题"。统一返回结构是为了建立前后端 API 契约，前端只写一套处理逻辑覆盖所有接口。