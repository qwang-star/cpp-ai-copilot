# 07 logger 日志模块逐行讲解

这一篇专门对照下面两个文件看：

```text
code/cpp-ai-copilot/include/copilot/logger.hpp
code/cpp-ai-copilot/src/logger.cpp
```

你可以把 `logger` 模块理解成：

```text
公司的记录员。
谁来了（请求）、处理多久、有没有出问题——
全部记下来，按级别过滤（debug < info < warn < error）。
```

---

## 1. logger.hpp 全貌

源码：

```cpp
#pragma once

#include <string>

namespace copilot {

enum class LogLevel {
    Debug,
    Info,
    Warn,
    Error,
};

class Logger {
public:
    explicit Logger(LogLevel level = LogLevel::Info);

    void debug(const std::string& message) const;
    void info(const std::string& message) const;
    void warn(const std::string& message) const;
    void error(const std::string& message) const;

private:
    LogLevel level_;
    void write(LogLevel level, const std::string& label, const std::string& message) const;
};

LogLevel parse_log_level(const std::string& value);

}  // namespace copilot
```

---

## 2. `enum class LogLevel` — 日志级别

```cpp
enum class LogLevel {
    Debug,   // 调试信息（最详细）
    Info,    // 普通信息
    Warn,    // 警告
    Error,   // 错误
};
```

### 2.1 它是什么

`enum class` 定义一个**取值有限的名字集合**——只能在这四个里选一个。

### 2.2 为什么用 `enum class` 而不是普通 `enum`

```cpp
// 普通 enum — 值会"漏"到外面，容易冲突
enum LogLevel { Debug, Info, Warn, Error };
int Debug = 5;   // ❌ 编译错误！Debug 已经被占了

// enum class — 值关在 LogLevel 这个"房间"里
enum class LogLevel { Debug, Info, Warn, Error };
int Debug = 5;   // ✅ 没问题，两个 Debug 互不影响
```

使用时必须带前缀：

```cpp
LogLevel level = LogLevel::Info;   // ✅
LogLevel level = Info;             // ❌ 编译错误
```

### 2.3 级别的大小关系

`Debug` 最小（序号 0），`Error` 最大（序号 3）：

```text
Debug  <  Info  <  Warn  <  Error
(0)       (1)      (2)      (3)
```

设置级别为 `Info` 时，`Debug` 消息不会输出（被过滤掉），`Info`、`Warn`、`Error` 会输出。

---

## 3. `class Logger` — 日志器

```cpp
class Logger {
public:
    explicit Logger(LogLevel level = LogLevel::Info);
    //   ↑                ↑                  ↑
    //   防止隐式转换  参数是日志级别            默认值是 Info

    void debug(const std::string& message) const;
    void info(const std::string& message) const;
    void warn(const std::string& message) const;
    void error(const std::string& message) const;

private:
    LogLevel level_;    // 当前日志级别
    void write(LogLevel level, const std::string& label, const std::string& message) const;
    //          ↑                ↑                        ↑
    //     这条消息的级别      标签（"DEBUG"/"INFO"...）   消息内容
};
```

### 3.1 `explicit Logger(LogLevel level = LogLevel::Info)` — 拆解

```cpp
explicit Logger(LogLevel level = LogLevel::Info);
//  ↑           ↑          ↑         ↑
//  ①           ②          ②         ③
```

**① `explicit` — 禁止隐式转换**

防止编译器在你不经意间把 `LogLevel` 悄悄转成 `Logger`：

```cpp
void foo(Logger logger) { ... }

// 没加 explicit — 这行能编译，但你可能不是故意的
foo(LogLevel::Info);     // 编译器悄悄创建了一个临时 Logger

// 加了 explicit — 编译直接报错，必须显式写
foo(Logger(LogLevel::Info));   // ✅ 我明确要创建一个 Logger
```

**② `LogLevel level` — 参数类型 + 参数名**

构造函数收一个 `LogLevel` 枚举值，决定这个 Logger 的最低输出级别。低于这个级别的日志会被过滤掉。

**③ `= LogLevel::Info` — 默认参数**

如果不传参数，默认就是 `Info` 级别：

```cpp
Logger logger;                       // 等价于 Logger(LogLevel::Info)
Logger logger(LogLevel::Debug);      // Debug 级别——显示所有日志（包括 Debug）
Logger logger(LogLevel::Error);      // Error 级别——只显示错误
```

**默认参数的含义：** 调用者可以省略这个参数，省略时自动用 `LogLevel::Info`。所以 `main.cpp` 里可以写 `Logger logger(level)`，也可以写 `Logger logger`（默认 Info）。

### 3.2 四个公开方法 — 四条日志通道

| 方法           | 级别    | 用途                 |
| ------------ | ----- | ------------------ |
| `debug(msg)` | Debug | 开发调试，如"收到请求"       |
| `info(msg)`  | Info  | 正常运行信息，如"服务器已启动"   |
| `warn(msg)`  | Warn  | 警告，如"accept 失败，跳过" |
| `error(msg)` | Error | 错误，如"Handler 抛异常"  |


四个方法都标记 `const`——记日志不改 Logger 本身。

### 3.3 私有成员

```cpp
LogLevel level_;   // 当前级别，低于这个级别的消息不输出

void write(...);   // 实际写日志的内部函数，四个公开方法都调它
```

---

## 4. 全局函数 `parse_log_level`

```cpp
LogLevel parse_log_level(const std::string& value);
```

不属于任何类，放在 `copilot` 命名空间下。作用是把配置文件里的字符串转成枚举：

```text
"debug" → LogLevel::Debug
"info"  → LogLevel::Info
"warn"  → LogLevel::Warn
"error" → LogLevel::Error
```

---

## 5. logger.cpp 全貌

分三块：

```
匿名 namespace — 小工具（rank、now_string、lower）
Logger 成员函数  — 构造函数 + 四个公开方法 + write
parse_log_level  — 字符串 → 枚举
```

---

## 6. 匿名 namespace 里的三个工具函数

### 6.1 `rank()` — 级别转数字

```cpp
int rank(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:  return 0;
        case LogLevel::Info:   return 1;
        case LogLevel::Warn:   return 2;
        case LogLevel::Error:  return 3;
    }
    return 1;  // 兜底
}
```

给每个级别一个数字，用于比较大小。后面 `write()` 里用它过滤：

```cpp
if (rank(level) < rank(level_)) return;
//       ↑              ↑
//     这条消息的级别    Logger 设置的级别
//     如果消息级别太低，直接 return，不输出
```

### 6.2 `now_string()` — 当前时间转字符串

```cpp
std::string now_string() {
    const auto now = std::chrono::system_clock::now();     // ① 取当前时间
    const auto time = std::chrono::system_clock::to_time_t(now);  // ② 转成 time_t
    std::tm tm{};

#ifdef _WIN32
    localtime_s(&tm, &time);                               // ③ Windows 取本地时间
#else
    localtime_r(&time, &tm);                               // ③ Linux 取本地时间
#endif

    std::ostringstream output;
    output << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");     // ④ 格式化成 "2026-06-10 14:30:05"
    return output.str();
}
```

输出效果：

```text
[2026-06-10 14:30:05] [INFO] cpp-ai-copilot listening on http://127.0.0.1:8080
```

你不需要深究 `chrono` 的每个细节，知道它在**取当前时间并格式化成可读字符串**就行。

### 6.3 `lower()` — 字符串转小写

```cpp
std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
        [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
    return value;
}
```

`std::transform` 对字符串每个字符执行 lambda，转成小写：

```text
"DEBUG" → "debug"
"Warn"  → "warn"
"Info"  → "info"
```

`parse_log_level` 用它做大小写无关匹配——用户写 "DEBUG"、"Debug"、"debug" 都能识别。

---

## 7. Logger 成员函数

### 7.1 构造函数

```cpp
Logger::Logger(LogLevel level) : level_(level) {}
```

初始化列表把 `level` 存入 `level_` 成员。一行搞定。

### 7.2 四个公开方法 — 全部委托给 `write`

```cpp
void Logger::debug(const std::string& message) const {
    write(LogLevel::Debug, "DEBUG", message);
}
void Logger::info(const std::string& message) const {
    write(LogLevel::Info, "INFO", message);
}
void Logger::warn(const std::string& message) const {
    write(LogLevel::Warn, "WARN", message);
}
void Logger::error(const std::string& message) const {
    write(LogLevel::Error, "ERROR", message);
}
```

四个方法全做同一件事——调 `write()`，只是传入的级别和标签不同。

这种模式叫**委托**——公共方法只负责传参数，实际逻辑集中在一处。以后想改日志格式，只改 `write()` 就行。

### 7.3 `write()` — 真正写日志的内部函数

```cpp
void Logger::write(LogLevel level, const std::string& label, const std::string& message) const {
    // ① 级别过滤
    if (rank(level) < rank(level_)) {
        return;   // 消息级别太低，不输出
    }

    // ② 选输出目标
    std::ostream& stream = level == LogLevel::Error ? std::cerr : std::cout;
    //                                    ↑ 三元运算符     ↑         ↑
    //                              是 Error？       → stderr    不是 → stdout

    // ③ 拼格式输出
    stream << '[' << now_string() << "] [" << label << "] " << message << '\n';
}
```

**逐行解释：**

**① 级别过滤：**

```cpp
if (rank(level) < rank(level_)) return;
```

Logger 设置了 `Info` 级别 → `Debug` 的消息 `rank 0 < rank 1` → 被过滤掉。

**② 选输出目标 — 三元运算符：**

```cpp
std::ostream& stream = level == LogLevel::Error ? std::cerr : std::cout;
//                    └ 条件              ┘  └ true用这个┘ └ false用这个┘
```

```text
level == Error  →  用 std::cerr（标准错误流，通常红色）
level != Error  →  用 std::cout（标准输出流）
```

把错误和普通日志分开输出——方便排查问题。

**③ 拼格式输出：**

```text
[2026-06-10 14:30:05] [INFO] GET /health -> 200 in 2ms
```

---

## 8. `parse_log_level()` — 字符串 → 枚举

```cpp
LogLevel parse_log_level(const std::string& value) {
    const std::string normalized = lower(value);    // 先转小写

    if (normalized == "debug") return LogLevel::Debug;
    if (normalized == "warn")  return LogLevel::Warn;
    if (normalized == "error") return LogLevel::Error;

    return LogLevel::Info;   // 默认：没匹配到就返回 Info
}
```

在 `main.cpp` 里这样用：

```cpp
copilot::Logger logger(copilot::parse_log_level(config.log_level));
//                     config.log_level 是 "info" 字符串
//                     parse_log_level("info") → LogLevel::Info
//                     Logger(LogLevel::Info) → 创建 Info 级别的日志器
```

---

## 9. 整体流程图

```text
main.cpp
    ↓
config.log_level = "info"（字符串）
    ↓
parse_log_level("info") → LogLevel::Info
    ↓
Logger logger(LogLevel::Info)
    ↓
构造函数：level_ = Info
    ↓
各处调用 logger.info("xxx")
    ↓
write(LogLevel::Info, "INFO", "xxx")
    ↓
rank(Info) >= rank(level_) → 通过过滤
    ↓
选 std::cout（不是 Error）
    ↓
输出：[2026-06-10 14:30:05] [INFO] xxx
```

---

## 10. 这段代码你最该掌握什么

```text
1. enum class — 限定了取值范围的类型，值关在"房间"里，用 LogLevel::Info 访问
2. explicit — 防止构造函数的隐式转换
3. 委托模式 — debug/info/warn/error 都调 write()，逻辑集中在一处
4. 级别过滤 — rank() 比较数字，低级别消息不输出
5. 三元运算符 — level == Error ? cerr : cout，一个表达式选输出目标
6. std::transform + lambda — 对字符串逐字转小写
7. #ifdef _WIN32 — 平台兼容，Windows 用 localtime_s，Linux 用 localtime_r
8. <iomanip> 的 put_time — 格式化时间输出
```
