准确说，第五阶段现在做的是：

**保留 toy HTTP Server 作为教学版，同时新增 Drogon 产品版骨架。**  
还不是“完全替换成功并验收完”，因为 Drogon 的 CMake 构建在你当前环境里卡过 `CMakeScratch` 临时文件权限问题；但代码结构已经按迁移方向搭起来了。

**第四阶段做了什么**  
第四阶段主要是把 [application.cpp](e:/note/面试八股 1/6_AI应用开发/20_Cpp企业AI_Copilot项目/code/cpp-ai-copilot/src/application.cpp) 里混在一起的东西拆开：

```
application.cpp      只负责注册路由
chat_service.cpp     处理聊天业务
api_response.cpp     生成统一 JSON 响应
error_code.cpp       管理错误码
```

也就是说，第四阶段解决的是：

```
代码职责混乱
```

但它仍然运行在你自己写的 toy HTTP Server 上：

```
socket accept
recv 请求
手动 parse HTTP
router 分发
手动 serialize response
```

## **第五阶段做了什么**  
第五阶段开始解决的是：

```
底层 Web 框架太原始，不适合继续做产品功能
```

所以我们没有继续增强 `simple_server.cpp`，而是新增 Drogon 产品版。

目前第五阶段分成两层：

```
5A：产品 API 基础层，不依赖 Drogon
5B：Drogon 产品版骨架，依赖 Drogon
```

###  **第 1 步：把 Drogon 构建关到开关后面**  
改了 [CMakeLists.txt](e:/note/面试八股 1/6_AI应用开发/20_Cpp企业AI_Copilot项目/code/cpp-ai-copilot/CMakeLists.txt)：

```
option(COPILOT_BUILD_DROGON "Build Drogon-based product server" OFF)

if(COPILOT_BUILD_DROGON)
    add_subdirectory(third_party/drogon-src/drogon-1.9.13)
endif()
```

为什么这么做：

默认不构建 Drogon，避免你学习 toy 后端时被复杂依赖卡住。只有明确打开：

```
COPILOT_BUILD_DROGON=ON
```

才进入产品版构建。

好处：

```
教学版稳定
产品版可逐步接入
Drogon 问题不会污染 toy server
```

### **第 2 步：抽出产品版 API 库**  
在 CMake 里新增：

```
add_library(copilot_product_api
    src/product/api_response.cpp
    src/product/error_code.cpp
)
```

为什么这么做：

产品 API 逻辑不能直接写在 Drogon Controller 里。否则以后所有业务规则都和 Drogon 绑死。

现在结构变成：

```
copilot_product_api
  ↓
Drogon Controller 调用它
  ↓
返回 HTTP 响应
```

好处：

```
业务逻辑可测试
框架可替换
Controller 更干净
```

### **第 3 步：补产品版健康检查数据**  
改了：

- [include/copilot/product/api_response.hpp](e:/note/面试八股 1/6_AI应用开发/20_Cpp企业AI_Copilot项目/code/cpp-ai-copilot/include/copilot/product/api_response.hpp)
- [src/product/api_response.cpp](e:/note/面试八股 1/6_AI应用开发/20_Cpp企业AI_Copilot项目/code/cpp-ai-copilot/src/product/api_response.cpp)
- [tests/test_api_response.cpp](e:/note/面试八股 1/6_AI应用开发/20_Cpp企业AI_Copilot项目/code/cpp-ai-copilot/tests/test_api_response.cpp)

新增：

```
std::string health_data(
    const std::string& service_name,
    const std::string& version);
```

它生成：

```
{"status":"ok","service":"cpp-ai-copilot","version":"0.2.0"}
```

为什么这么做：

`/health` 不是 Drogon 独有的业务，它是产品服务自己的健康信息。所以先做成框架无关函数。

### **第 4 步：新增产品 API 测试入口**  
改了 [Makefile](e:/note/面试八股 1/6_AI应用开发/20_Cpp企业AI_Copilot项目/code/cpp-ai-copilot/Makefile)：

```
product-api-test
```

现在可以运行：

```
mingw32-make product-api-test
```

为什么这么做：

不用 CMake，不用 Drogon，也能验证产品 API 层。

已经验证通过：

```
mingw32-make product-api-test
mingw32-make test
```

这说明：

```
产品 API 层通过
toy 教学版没有坏
```

### **第 5 步：新增 Drogon 产品配置**  
新增 [config/product.json](e:/note/面试八股 1/6_AI应用开发/20_Cpp企业AI_Copilot项目/code/cpp-ai-copilot/config/product.json)：

```
{
  "listeners": [
    {
      "address": "127.0.0.1",
      "port": 18081
    }
  ],
  "custom_config": {
    "service_name": "cpp-ai-copilot",
    "version": "0.2.0"
  }
}
```

为什么端口用 `18081`：

toy 教学版可以继续用原来的端口，Drogon 产品版用新端口，避免冲突。

### **第 6 步：新增 Drogon 响应适配层**  
新增 [drogon_response.hpp](e:/note/面试八股 1/6_AI应用开发/20_Cpp企业AI_Copilot项目/code/cpp-ai-copilot/src/product/drogon_response.hpp)。

它做的事是：

```
产品 API JSON 字符串
  ↓
Drogon HttpResponse
```

例如：

```
success_drogon_response(...)
error_drogon_response(...)
```

为什么这么做：

`api_response.cpp` 不依赖 Drogon。  
`drogon_response.hpp` 才依赖 Drogon。

这就是典型的适配层：

```
业务响应格式  ← 保持纯净
Drogon 响应对象 ← 单独适配
```

### **第 7 步：新增 Drogon HealthController**  
新增：

- [health_controller.hpp](e:/note/面试八股 1/6_AI应用开发/20_Cpp企业AI_Copilot项目/code/cpp-ai-copilot/src/product/health_controller.hpp)
- [health_controller.cpp](e:/note/面试八股 1/6_AI应用开发/20_Cpp企业AI_Copilot项目/code/cpp-ai-copilot/src/product/health_controller.cpp)

它注册：

```
GET /health
```

内部流程是：

```
读取 product.json 里的 service_name/version
调用 health_data()
调用 success_drogon_response()
返回统一 JSON
```

这说明 Drogon 只负责 HTTP 框架层，业务数据仍然来自 `copilot_product_api`。

### **第 8 步：新增 Drogon 产品版入口**  
新增 [src/product/main.cpp](e:/note/面试八股 1/6_AI应用开发/20_Cpp企业AI_Copilot项目/code/cpp-ai-copilot/src/product/main.cpp)。

里面做了：

```
加载 config/product.json
注册 404 默认处理
注册统一错误处理
注册异常处理
注册请求日志
启动 app.run()
```

目标响应：

```
GET /health   -> 200 + OK
GET /missing  -> 404 + ROUTE_NOT_FOUND
POST /health  -> 405 + METHOD_NOT_ALLOWED
异常           -> 500 + INTERNAL_ERROR
```

**为什么要从 toy HTTP Server 迁移到 Drogon**  
toy server 的价值是学习底层原理：

```
socket
HTTP 请求解析
路由分发
响应序列化
```

但它不适合继续承载复杂浏览器应用，因为后面你要做：

```
并发请求
大文件上传
SSE 流式输出
错误处理
日志
配置
中间件
数据库连接
Redis
用户鉴权
RAG / AI 调用
```

如果继续手写，精力会被底层网络细节吃掉。

Drogon 的优势是：

```
成熟路由系统
异步 HTTP 服务
配置文件支持
统一错误处理入口
Controller 模型
更好的并发能力
后续容易接 MySQL / Redis / SSE
```

所以第五阶段的本质是：

```
toy server 用来证明你理解原理
Drogon server 用来继续做产品功能
```

这也是面试里很好讲的项目演进：

```
我先手写 toy HTTP Server 理解 Web 后端底层链路，
然后把业务层抽出来，
最后迁移到 Drogon，让项目具备产品化扩展能力。
```

当前状态一句话总结：

**5A 已经基本完成并验证；5B 代码骨架已加，但 Drogon 构建和 HTTP 联调还没最终通过，因为 CMake 在当前环境里遇到了临时文件权限问题。**