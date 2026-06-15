# 04 http 模块逐行讲解

这一篇专门对照下面两个文件看：

```text
code/cpp-ai-copilot/include/copilot/http.hpp
code/cpp-ai-copilot/src/http.cpp
```

你可以把 `http` 模块理解成：

```text
一个翻译官。
浏览器说的话（HTTP 文本字符串）→ 翻译成 C++ 对象（HttpRequest）
C++ 对象（HttpResponse）→ 翻译成浏览器能听懂的（HTTP 文本字符串）
```

---

## 1. http.hpp 全貌

源码：

```cpp
#pragma once

#include <map>
#include <string>

namespace copilot {

struct HttpRequest {
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;
};

struct HttpResponse {
    int status_code = 200;
    std::string reason = "OK";
    std::map<std::string, std::string> headers;
    std::string body;

    static HttpResponse json(int status_code, std::string body);
    std::string serialize() const;
};

HttpRequest parse_http_request(const std::string& raw);
std::string reason_phrase(int status_code);

}  // namespace copilot
```

这个头文件定义了两样东西：
1. **两个数据结构**（`HttpRequest` 和 `HttpResponse`）
2. **四个函数声明**（`json`、`serialize`、`parse_http_request`、`reason_phrase`）

---

## 2. `struct HttpRequest` — 解析后的请求对象

```cpp
struct HttpRequest {
    std::string method;    // "GET"、"POST"、"PUT"、"DELETE"
    std::string path;      // "/health"、"/api/v1/chat"
    std::string version;   // "HTTP/1.1"
    std::map<std::string, std::string> headers;  // {"Host": "localhost:8080", ...}
    std::string body;      // POST 请求的正文（JSON 等）
};
```

### 2.1 它长什么样

当浏览器发来这样的 HTTP 文本：

```http
GET /health HTTP/1.1
Host: localhost:8080
Accept: application/json

```

解析后变成：

```text
HttpRequest {
    method:   "GET"
    path:     "/health"
    version:  "HTTP/1.1"
    headers:  {"Host": "localhost:8080", "Accept": "application/json"}
    body:     ""      ← GET 请求没有正文
}
```

### 2.2 为什么用 `map` 存 headers

HTTP Header 是键值对——一个名字对应一个值：

```
Host: localhost:8080           →  headers["Host"] = "localhost:8080"
Accept: application/json       →  headers["Accept"] = "application/json"
Content-Type: text/html        →  headers["Content-Type"] = "text/html"
```

`map<string, string>` 就是专干这事的——给一个名字，拿到对应值。

### 2.3 `method` 和 `path` 是 router 分发的依据

回顾 `router.cpp`：

```cpp
routes_.find(key(request.method, request.path));
//                  ↑                ↑
//            就是 HttpRequest 的这两个字段
```

Router 只看 method 和 path，其他字段是 Handler 内部自己用的。

---

## 3. `struct HttpResponse` — 还没发出去的响应对象

```cpp
struct HttpResponse {
    int status_code = 200;        // 状态码，默认 200
    std::string reason = "OK";    // 状态文本，默认 "OK"
    std::map<std::string, std::string> headers;  // 响应头部
    std::string body;             // 响应正文

    static HttpResponse json(int status_code, std::string body);
    std::string serialize() const;
};
```

### 3.1 默认值

```cpp
int status_code = 200;
std::string reason = "OK";
```

如果你不显式设置，`HttpResponse` 出生就是"200 OK 空正文"。

### 3.2 `static HttpResponse json(...)` — 静态工厂方法

```cpp
static HttpResponse json(int status_code, std::string body);
```

`static` 意味着这个函数**属于 HttpResponse 类型本身**，不需要先创建对象：

```cpp
// 直接用类名调，返回一个填好的 HttpResponse
HttpResponse resp = HttpResponse::json(200, R"({"status":"ok"})");
```

这叫"工厂方法"——不是你自己一点点填字段，而是调一个函数，它帮你把该填的都填好（具体怎么填见后面 `http.cpp` 的讲解）。

### 3.3 `serialize() const` — 序列化

```cpp
std::string serialize() const;
```

把 HttpResponse 对象**转回 HTTP 文本字符串**。末尾的 `const` 表示这个操作是只读的，不修改对象自身。

---

## 4. 两个全局函数

```cpp
HttpRequest parse_http_request(const std::string& raw);
std::string reason_phrase(int status_code);
```

这两个是**不属于任何 struct/class 的普通函数**，放在 `copilot` 命名空间下。

| 函数                   | 输入         | 输出               | 作用           |
| -------------------- | ---------- | ---------------- | ------------ |
| `parse_http_request` | HTTP 文本字符串 | `HttpRequest` 对象 | 解析（字符串 → 对象） |
| `reason_phrase`      | 状态码（如 200） | 状态文本（如 "OK"）     | 数字 → 英文短语    |

---

## 5. http.cpp 全貌

源码：

```cpp
#include "copilot/http.hpp"
#include <sstream>

namespace copilot {

std::string reason_phrase(int status_code) { ... }           // ①
HttpResponse HttpResponse::json(int, std::string) { ... }    // ②
std::string HttpResponse::serialize() const { ... }          // ③
HttpRequest parse_http_request(const std::string& raw) { ... } // ④

}  // namespace copilot
```

四个函数，按调用频率从低到高讲。

### 5.1 `#include <sstream>` — `istringstream` 和 `ifstream` 的区别

`http.cpp` 只包含了 `<sstream>`，`config.cpp` 包含的是 `<fstream>`。它们分别提供了两套流：

| 头文件         | 提供的类型                                   | 数据来源      | 你的项目用在哪                         |
| ----------- | --------------------------------------- | --------- | ------------------------------- |
| `<sstream>` | `istringstream`（输入）、`ostringstream`（输出） | **内存字符串** | `http.cpp` 解析 HTTP 请求文本         |
| `<fstream>` | `ifstream`（输入）、`ofstream`（输出）           | **磁盘文件**  | `config.cpp` 读 `config/app.env` |

**它们是一家人，继承自同一个爹：**

```text
              istream（输入流基类——定义了 >> 和 getline）
              /        \
         ifstream    istringstream
        （文件流）     （字符串流）

两个都支持同样的操作：
  >>       逐词读
  getline  逐行读
```

**所以用法一模一样，只是"数据来源"不同：**

```cpp
// ifstream — 水管接在磁盘文件上
std::ifstream input("config/app.env");
std::string line;
while (std::getline(input, line)) {   // 从文件逐行读
    ...
}

// istringstream — 水管接在一根内存字符串上
std::string raw = "GET /health HTTP/1.1\r\nHost: ...\r\n\r\n";
std::istringstream input(raw);
std::string line;
while (std::getline(input, line)) {   // 从字符串逐行读
    ...
}
```

**为什么 `http.cpp` 用 `istringstream` 而不是 `ifstream`？**

因为 HTTP 请求文本已经由 socket 收到内存里了（`simple_server.cpp` 里 `recv()` 读到 `buffer`），解析时数据已经在内存里，不需要再碰磁盘。



---

## 6. `reason_phrase()` — 数字 → 英文短语

### 6.1 源码

```cpp
std::string reason_phrase(int status_code) {
    switch (status_code) {
        case 200:  return "OK";
        case 400:  return "Bad Request";
        case 404:  return "Not Found";
        case 405:  return "Method Not Allowed";
        case 500:  return "Internal Server Error";
        default:   return "OK";
    }
}
```

### 6.2 `switch` 是什么

```cpp
switch (status_code)     // 看 status_code 是哪个值
    case 200:            // 如果是 200 → 执行 return "OK"
    case 404:            // 如果是 404 → 执行 return "Not Found"
    default:             // 上面都不是 → 执行 return "OK"
```

`switch` 适合**一个变量有多个固定取值**的场景。这里比写一长串 `if-else` 清晰得多。

### 6.3 为什么用 `switch` 而不是 `if-else`

对比：

```cpp
// switch 写法 — 清晰
switch (code) {
    case 200: return "OK";
    case 404: return "Not Found";
    ...
}

// if-else 写法 — 啰嗦
if (code == 200) return "OK";
else if (code == 404) return "Not Found";
else if ...
```

效果一样，`switch` 更适合这种"一个值对比多个常量"的场景。

### 6.4 `default` 分支

```cpp
default: return "OK";
```

如果状态码不在列表里（比如 302），兜底返回 "OK"。这是一种简单处理——不报错，给个默认值。

---

## 7. `HttpResponse::json()` — 工厂方法，一步填好响应

### 7.1 源码

```cpp
HttpResponse HttpResponse::json(int status_code_value, std::string response_body) {
    HttpResponse response;                                    // ①
    response.status_code = status_code_value;                 // ②
    response.reason = reason_phrase(status_code_value);       // ③
    response.body = std::move(response_body);                 // ④
    response.headers["Content-Type"] = "application/json; charset=utf-8";  // ⑤
    response.headers["Connection"] = "close";                 // ⑥
    return response;                                          // ⑦
}
```

### 7.2 逐行翻译

**① `HttpResponse response;`**

创建一个空的响应对象。此时它的字段都是默认值：

```text
status_code = 200
reason = "OK"
headers = {}  （空 map）
body = ""     （空字符串）
```

**② `response.status_code = status_code_value;`**

把传进来的状态码（比如 200）写入。

**③ `response.reason = reason_phrase(status_code_value);`**

根据状态码自动查出对应的英文短语：

```text
200 → "OK"
404 → "Not Found"
500 → "Internal Server Error"
```

**④ `response.body = std::move(response_body);`**

把传进来的 JSON 字符串移入 body。`std::move` 是"移交所有权"，避免复制。

**⑤ `response.headers["Content-Type"] = "application/json; charset=utf-8";`**

设置这个 Header，告诉浏览器："我给你的是 JSON 格式，编码是 UTF-8"。

**⑥ `response.headers["Connection"] = "close";`**

设置这个 Header，告诉浏览器："我回复完就关闭连接"。

**⑦ `return response;`**

返回填好的响应对象。

### 7.3 调用效果

```cpp
// application.cpp 里这样调：
return HttpResponse::json(200, R"({"status":"ok","service":"cpp-ai-copilot","version":"0.1.0"})");

// 返回的对象等价于手写了这么多：
HttpResponse {
    status_code: 200,
    reason: "OK",
    headers: {
        "Content-Type": "application/json; charset=utf-8",
        "Connection": "close"
    },
    body: "{\"status\":\"ok\",\"service\":\"cpp-ai-copilot\",\"version\":\"0.1.0\"}"
}
```

工厂方法省掉了手写每一行的麻烦——调一个函数，全部搞定。

### 7.4 `R"(...)"` — 原始字符串字面量（Raw String Literal）

注意上面调用里写的不是普通引号字符串：

```cpp
R"({"status":"ok","service":"cpp-ai-copilot","version":"0.1.0"})"
```

这是 **C++11 的原始字符串字面量**。

**为什么不用普通字符串？**

JSON 里大量使用双引号 `"`，如果写普通 C++ 字符串，每个 `"` 都得转义：

```cpp
// 普通字符串 —— 丑且容易出错
"{\"status\":\"ok\",\"service\":\"cpp-ai-copilot\",\"version\":\"0.1.0\"}"

// 原始字符串 —— 一目了然
R"({"status":"ok","service":"cpp-ai-copilot","version":"0.1.0"})"
```

**语法拆解：**

```text
R  "  (   {"status":"ok"}   )  "
│  │  │   │                 │  │
│  │  定界符开始              定界符结束
│  │
│  前缀（raw 标识）
│
raw 标记
```

| 部分 | 含义 |
|------|------|
| `R` | Raw，告诉编译器这是原始字符串 |
| `"(` | 开始定界符 |
| `中间所有字符` | 原样保留，不解析转义 |
| `)"` | 结束定界符 |

**关键：`R"(` 和 `)"` 是编译器识别的东西，运行时不存在。**

```cpp
R"({"status":"ok"})"
```

编译后，`std::string` 的内容就是纯 JSON：

```json
{"status":"ok"}
```

客户端收到的 HTTP body 也是纯 JSON，没有任何 `R"(` 前缀。

**对比总结：**

| | 普通字符串 | 原始字符串 |
|--|----------|----------|
| 写法 | `"{\"key\":\"val\"}"` | `R"({"key":"val"})"` |
| 需要转义 | 是 | 否 |
| 运行时内容 | `{"key":"val"}` | `{"key":"val"}` |

它只是 C++ 的**语法糖**——让写 JSON、正则表达式、文件路径这些含大量反斜杠和引号的字符串更干净，不影响最终 HTTP 响应的内容。

### 7.5 答疑：`serialize()` 没有参数，拿什么来序列化？

你可能注意到了，很多成员函数声明里 `()` 是空的：

```cpp
std::string serialize() const;     // 没有参数！
int run();                          // 也没有参数！
```

没有参数 ≠ 没有输入。秘密在于 **成员函数有一个你看不见的隐藏参数——`this` 指针。**

**`this` 是什么：**

C++ 里，每个非静态成员函数都悄悄接收一个隐藏参数——指向**调用它的那个对象**的指针。

```cpp
// 你看到的：
std::string serialize() const;

// 编译器实际看到的（概念上）：
std::string serialize(const HttpResponse* this) const;
//                                    ↑
//                          this 指向调用它的那个对象
```

调用时，对象自己就是输入：

```cpp
HttpResponse resp = HttpResponse::json(200, "{...}");
resp.serialize();
//  ↑
//  隐式把 &resp 传了进去，this 指向 resp
//  所以函数内部读到的 status_code、headers、body 都是 resp 的字段
```

**具体到 `serialize()` 的实现：**

```cpp
std::string HttpResponse::serialize() const {
    output << "HTTP/1.1 " << status_code << ' ' << reason << "\r\n";
    //                      ↑               ↑
    //                  this->status_code  this->reason
    //     （this 省略了没写，实际上是 this->status_code）

    for (const auto& [name, value] : headers) {
    //                                 ↑
    //                            this->headers
        output << name << ": " << value << "\r\n";
    }
    output << body;
    //       ↑
    //   this->body
    ...
}
```

函数里写的 `status_code`、`headers`、`body`，全都是 `this->xxx` 的简写。`this` 指向调用它的那个对象，所以读到的是那个对象的字段。

**你项目里的分类：**

| 函数 | `()` 里有参数？ | 输入从哪来 | 为什么 |
|------|:---:|-----------|--------|
| `serialize()` | 空 | `this` 自己的字段 | 序列化自己，不需要外部数据 |
| `route(request)` | `request` | 外部传入 + `this` 的路由表 | 需要知道"请求是什么"才能查表 |
| `key(method, path)` | `method`, `path` | 全部外部传入 | `static` 函数，根本没有 `this` |
| `run()` | 空 | `this` 的 config、router、logger | 服务器用自己的配置启动 |
| `reason_phrase(code)` | `code` | 全部外部传入 | 普通函数，不属于任何对象 |

**一句话：**

> 成员函数天生能访问**调用它的那个对象**的所有字段——这就是 `()` 为空也不缺输入的原因。

---

## 8. `HttpResponse::serialize()` — 对象 → HTTP 文本（序列化）

### 8.1 源码

```cpp
std::string HttpResponse::serialize() const {
    std::ostringstream output;                                          // ①
    output << "HTTP/1.1 " << status_code << ' ' << reason << "\r\n";    // ②
    for (const auto& [name, value] : headers) {                         // ③
        output << name << ": " << value << "\r\n";
    }
    output << "Content-Length: " << body.size() << "\r\n";              // ④
    output << "\r\n";                                                   // ⑤
    output << body;                                                     // ⑥
    return output.str();                                                // ⑦
}
```

### 8.2 ① `ostringstream` — 字符串拼接器

```cpp
std::ostringstream output;
```

`ostringstream` = output string stream（输出字符串流）。是一个c++内置的一个类。

把它想象成一根**空水管**——你往里面塞什么，它就攒什么，最后一股脑倒出来变成一根字符串。

### 8.3 ② 写状态行

```cpp
output << "HTTP/1.1 " << status_code << ' ' << reason << "\r\n";
```

`<<` 是流插入运算符，往水管里塞东西。不同类型自动转换：

```text
"HTTP/1.1 "  →  字符串，原样塞入
status_code  →  int 200，自动转成 "200"
' '          →  空格字符
reason       →  "OK"
"\r\n"       →  换行符（HTTP 协议规定用 \r\n）
```

拼出来的结果：

```text
HTTP/1.1 200 OK\r\n
```

### 8.4 ③ 遍历 headers，逐行写 Header

```cpp
for (const auto& [name, value] : headers) {
    output << name << ": " << value << "\r\n";
}
```

这是 **range-based for + 结构化绑定**：

| 语法                    | 含义                        |
| --------------------- | ------------------------- |
| `for (... : headers)` | 遍历 headers 这个 map 里的每个键值对 |
| `const auto&`         | 用引用的方式拿，不复制，不改            |
| `[name, value]`       | 把键值对拆成两个变量 name 和 value   |


循环效果：

```text
Content-Type: application/json; charset=utf-8\r\n
Connection: close\r\n
```

### 8.5 ④ 加 Content-Length

```cpp
output << "Content-Length: " << body.size() << "\r\n";
```

`body.size()` 返回 body 字符串的字节长度。浏览器需要知道正文多长才能正确接收。

```text
Content-Length: 55\r\n
```

### 8.6 ⑤ 空行 — Header 和 Body 的分隔

```cpp
output << "\r\n";
```

HTTP 协议规定：**Header 和 Body 之间必须有一个空行**。这一个 `\r\n` 就是那个空行。

### 8.7 ⑥ 写 Body

```cpp
output << body;
```

把 JSON 正文原样塞进去。

### 8.8 ⑦ 导出为字符串

```cpp
return output.str();
```

把水管里攒的所有内容倒出来，变成一根 `std::string`。

### 8.9 完整效果

输入 HttpResponse 对象 → 输出 HTTP 文本：

```http
HTTP/1.1 200 OK
Content-Type: application/json; charset=utf-8
Connection: close
Content-Length: 55

{"status":"ok","service":"cpp-ai-copilot","version":"0.1.0"}
```

---

## 9. `parse_http_request()` — 字符串 → 对象（解析）

这是整个 http 模块最核心也最长的函数。按步骤逐块讲解。

### 9.1 全貌

```cpp
HttpRequest parse_http_request(const std::string& raw) {
    HttpRequest request;                                // ①
    std::istringstream input(raw);                      // ②

    input >> request.method >> request.path >> request.version;  // ③
    std::string line;
    std::getline(input, line);                          // ④

    while (std::getline(input, line)) {                 // ⑤
        if (line == "\r" || line.empty()) {             // ⑥
            break;
        }
        if (!line.empty() && line.back() == '\r') {     // ⑦
            line.pop_back();
        }
        const auto separator = line.find(':');          // ⑧
        if (separator == std::string::npos) {
            continue;
        }
        std::string name = line.substr(0, separator);           // ⑨
        std::string value = line.substr(separator + 1);         // ⑩
        if (!value.empty() && value.front() == ' ') {           // ⑪
            value.erase(value.begin());
        }
        request.headers[name] = value;                          // ⑫
    }

    std::ostringstream body;                                    // ⑬
    body << input.rdbuf();
    request.body = body.str();

    return request;                                             // ⑭
}
```

### 9.2 输入：一段 HTTP 原始文本

```http
GET /health HTTP/1.1\r\n
Host: localhost:8080\r\n
Accept: application/json\r\n
\r\n
```

`\r\n` 是换行符（Windows 风格的 HTTP 协议行尾）。

### 9.3 ① 建空盒子

```cpp
HttpRequest request;
```

创建一个空 `HttpRequest`，待会儿往里填。

### 9.4 ② `istringstream` — 把字符串变成"水龙头"

```cpp
std::istringstream input(raw);
```

`istringstream` = input string stream（输入字符串流）。

把一根大字符串包成"水龙头"，可以逐词逐行地读。

`raw` 只是个**参数名**，指向调用者（`simple_server.cpp`）从 socket 收到的原始 HTTP 文本，`istringstream input(raw)` 把它包成流，方便后面逐词逐行地解析。

### 9.5 ③ 读第一行的三个词

```cpp
input >> request.method >> request.path >> request.version;
```

`>>` 是流提取运算符，遇到空格/换行停，每次读一个词：

```text
原始第一行： "GET /health HTTP/1.1\r\n"

第 1 个 >> ：读 "GET"          → request.method = "GET"
第 2 个 >> ：读 "/health"      → request.path = "/health"
第 3 个 >> ：读 "HTTP/1.1"     → request.version = "HTTP/1.1"
```

读完后，流的读头停在了第一行的行尾（`\r\n` 还没读）。

### 9.6 ④ 吞掉第一行剩余内容

```cpp
std::string line;
std::getline(input, line);
```

`getline` 从流的当前位置读到换行符为止（换行符本身读掉但不存）。

这行把第一行末尾的 `\r\n` 消耗掉，让读头跳到第二行开头。

### 9.7 ⑤ 循环读 Header

```cpp
while (std::getline(input, line)) {
```

`getline` 会返回流的状态——读到了内容返回 `true`，读到文件末尾返回 `false`，循环结束。

每次循环 `line` 里存的是当前行（`\r\n` 已被 strip）。

循环开始前：

```text
读头位置：
  GET /health HTTP/1.1\r\n          ← 已读
📍 Host: localhost:8080\r\n          ← 读头在这里
  Accept: application/json\r\n
  \r\n
```

### 9.8 ⑥ 遇到空行 → 停止读 Header

```cpp
if (line == "\r" || line.empty()) {
    break;
}
```

HTTP 协议中，Header 和 Body 之间用**一个空行**分隔。

```text
Host: localhost:8080\r\n
Accept: application/json\r\n
\r\n                        ← 这就是空行，Header 结束了
Body 从这里开始...
```

`getline` 读到空行时，`line` 可能是 `"\r"` 或 `""`。遇到了就 `break` 跳出循环。

### 9.9 ⑦ 去掉行尾的 `\r`

```cpp
if (!line.empty() && line.back() == '\r') {
    line.pop_back();    // `pop_back()` 就是**删掉字符串最后一个字符**。
}
```

`std::getline(input, line)` 默认行为：
**读到 `\n` 为止 → 把 `\n` 之前的字符存进 `line` → 丢掉 `\n` 本身。**

`getline` 去掉了 `\n`，但可能留下 `\r`。这一行检查最后一个字符是不是 `\r`，是就删掉。

```
处理前："Host: localhost:8080\r"
处理后："Host: localhost:8080"
```

### 9.10 ⑧ 找冒号位置

```cpp
const auto separator = line.find(':');
if (separator == std::string::npos) {
    continue;
}
```

`find(':')` 返回冒号在第几个字符（从 0 开始）。

```
"Host: localhost:8080"
      ↑
 separator = 4（冒号在第 4 个位置）
```

如果没找到冒号，`find` 返回 `std::string::npos`（一个特殊值，表示"没找到"），跳过这行。

### 9.11 ⑨⑩ 按冒号切成 name 和 value

```cpp
std::string name = line.substr(0, separator);        // 冒号左边
std::string value = line.substr(separator + 1);       // 冒号右边（+1 跳过冒号本身）
```

```
"Host: localhost:8080"
 0123456789...
    ↑
 separator = 4

substr(0, 4)           → "Host"              ← name
substr(5)              → " localhost:8080"   ← value（注意开头有个空格）
```

### 9.12 ⑪ 去掉 value 开头的空格

```cpp
if (!value.empty() && value.front() == ' ') {
    value.erase(value.begin());
}
```

```
处理前：" localhost:8080"
处理后："localhost:8080"
```

### 9.13 ⑫ 存入 headers map

```cpp
request.headers[name] = value;
```

```text
headers["Host"] = "localhost:8080"
```

循环继续，用同样方式处理下一行 `Accept: application/json`。

### 9.14 ⑬ 读 Body

```cpp
std::ostringstream body;
body << input.rdbuf();
request.body = body.str();   // 把内容变成字符串
```

循环退出后，流的读头停在空行之后。`input.rdbuf()` 把流里剩余的所有内容一次性读出来。用 `ostringstream` 接到 `request.body`。

对于 GET 请求，Body 通常为空。POST 请求才会有 JSON 正文。

### 9.15 ⑭ 返回填好的 HttpRequest

```cpp
return request;
```

解析完成，`HttpRequest` 对象交给 Router 做路由分发。

---

## 10. 解析全流程走一遍

输入：

```http
GET /health HTTP/1.1\r\n
Host: localhost:8080\r\n
Accept: application/json\r\n
\r\n
```

过程：

```text
① HttpRequest request;                            → request = {}
② istringstream input(raw);                       → 水龙头就位
③ input >> method >> path >> version;             → method="GET", path="/health", version="HTTP/1.1"
④ getline(input, line);                           → 吞掉第一行剩余 \r\n

⑤ while (getline(input, line))
  ⑥ line = "Host: localhost:8080"                  → 不是空行，继续
  ⑦ 末尾没有 \r
  ⑧ separator = 4                                  → 冒号在第 4 个位置
  ⑨ name = "Host"
  ⑩ value = " localhost:8080"
  ⑪ 去掉开头空格 → value = "localhost:8080"
  ⑫ headers["Host"] = "localhost:8080"

  ⑥ line = "Accept: application/json"              → 不是空行，继续
  ⑧ separator = 6
  ⑨ name = "Accept"
  ⑩ value = " application/json"
  ⑪ 去掉开头空格 → value = "application/json"
  ⑫ headers["Accept"] = "application/json"

  ⑥ line = ""                                      → 空行！break 跳出

⑬ 读剩余内容 → body = ""                           → GET 请求无 Body
⑭ return request;
```

输出：

```text
HttpRequest {
    method:   "GET",
    path:     "/health",
    version:  "HTTP/1.1",
    headers:  {"Host": "localhost:8080", "Accept": "application/json"},
    body:     ""
}
```

---

## 11. 序列化和解析的对称关系

```text
请求方向 — 字符串 → 对象：
     HTTP 文本  ──parse_http_request()──→  HttpRequest 对象
     （收到的一根大字符串）                  （有 method、path、headers、body 的结构体）

响应方向 — 对象 → 字符串：
     HttpResponse 对象  ──serialize()──→  HTTP 文本
     （有 status_code、headers、body 的结构体）  （发回去的一根大字符串）
```

这两个是**互逆操作**——解析是"拆"（一 → 多），序列化是"拼"（多 → 一）。

它们操纵的对象不同（`HttpRequest` vs `HttpResponse`），但做的事对称——都在 "HTTP 文本字符串 ↔ 结构化 C++ 对象" 之间翻译。

---

## 12. 这段代码你最该掌握什么

```text
1. struct 定义数据结构（HttpRequest、HttpResponse），打包相关字段
2. istringstream 把字符串变成可逐词逐行读的"水龙头"
3. ostringstream 是"字符串拼接头"，往里塞东西，最后 str() 导出
4. >> 从流里读一个词（遇空格/换行停）
5. getline(input, line) 从流里读一整行
6. find(':') + substr() 按分隔符切字符串
7. switch 处理多个固定取值
8. for (const auto& [name, value] : map) 遍历键值对，结构化绑定拆开
9. static 函数 = 类级别的函数，用 HttpResponse::json(...) 调用
10. 末尾 const = 承诺不修改对象自身
11. \r\n 是 HTTP 协议的行尾约定，空行分隔 Header 和 Body
```

http 模块是整个项目的**翻译层**——所有进出的数据都在这里完成"字符串 ↔ 对象"的转换。理解它，整个数据流就通了。
