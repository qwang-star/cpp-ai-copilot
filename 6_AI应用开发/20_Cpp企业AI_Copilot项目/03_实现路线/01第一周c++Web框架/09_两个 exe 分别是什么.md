
|文件|是什么|由哪些 .cpp 编译|怎么运行|
|---|---|---|---|
|`cpp-ai-copilot.exe`|**服务器程序**|`main.cpp` + `simple_server.cpp` + `application.cpp` + `config.cpp` + `http.cpp` + `router.cpp` + `logger.cpp`|`make run` 或 `./build/cpp-ai-copilot.exe config/app.env`|
|`test_core.exe`|**测试程序**|`tests/test_core.cpp` + `application.cpp` + `config.cpp` + `http.cpp` + `router.cpp` + `logger.cpp`|`make test` 或 `./build/test_core.exe`|

---

## 在哪生成的

`Makefile` 第 24-28 行：

```makefile
# 服务器程序
build/cpp-ai-copilot$(EXE): build $(SERVER_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SERVER_SRC) -o $@ $(SERVER_LIBS)
	#                                                                 ↑
	#                                          Windows 下额外链接 ws2_32.lib（socket 需要）

# 测试程序
build/test_core$(EXE): build $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(TEST_SRC) -o $@
```

两者的区别：

```text
服务器程序 = main.cpp + simple_server.cpp + 所有模块
测试程序   = test_core.cpp            + 所有模块（不含 main 和 server）
             ↑
          用自己的 main() 替换了 main.cpp 里的 main()
          test_core.cpp 第 83 行也有一个 int main()
```

---

## 怎么运行

```bash
# 编译并启动服务器（在浏览器访问 http://localhost:8080/health）
make run

# 编译并运行测试（5 个测试，全部通过 = 代码没坏）
make test

# 只编译不运行
make

# 清理编译产物
make clean
```

---

## test_core.cpp 测了什么

`test_core.cpp` 第 83-89 行，依次运行了 5 个测试：

```
test_json_response_has_status_content_type_and_body()   → 测 HttpResponse::json()
test_router_returns_health_response()                   → 测 Router 路由到正确 Handler
test_router_returns_404_for_unknown_route()             → 测找不到路由返回 404
test_config_loads_key_value_file()                      → 测 AppConfig::load() 读文件
test_application_router_registers_health_route()        → 测 create_app_router() 注册的 /health
```

这些测试不需要启动服务器——直接在内存里构造假的 `HttpRequest`，调用 Router 和 HttpResponse，验证返回值。这就是为什么它不含 `simple_server.cpp` 和 `main.cpp`。