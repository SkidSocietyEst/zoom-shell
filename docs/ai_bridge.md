# AI HTTP 桥接（`ai` 命令）

## 1. 它是什么？

在开启 `ZOOM_USING_AI_BRIDGE` 后，Shell 会多出一组命令：`ai url`、`ai ask`、`ai status`。

- **库内不包含** HTTP、TLS、DNS 等实现，避免强依赖网络栈与证书管理。
- 实际发请求由你在 **端口层** 实现的 **`zoom_ai_http_post_fn_t` 回调** 完成（例如 ESP-IDF `esp_http_client`、lwIP `httpd` 客户端、或转发到协处理器）。

典型用途：设备通过串口输入一句话 → 固件 POST 到**自建网关** → 网关再调 LLM 或与 OpenClaw 等系统协作。**不要把第三方 API 密钥硬编码在固件里**，应放在网关环境变量或安全存储中。

## 2. 编译开关与容量

在 `zoom_shell_cfg_user.h` 中：

```c
#define ZOOM_USING_AI_BRIDGE    1
```

可选容量（有默认值，可在同一头文件中覆盖）：

| 宏 | 含义 | 默认 |
|----|------|------|
| `ZOOM_AI_URL_MAX` | 保存的 POST 地址最大长度 | 192 |
| `ZOOM_AI_RESP_MAX` | 响应缓冲区大小（字节） | 512 |

确保工程编译并链接 `extensions/ai_bridge/zoom_shell_ai_bridge.c`（官方 Makefile 已包含）。

## 3. C API

### 3.1 回调类型

```c
typedef int (*zoom_ai_http_post_fn_t)(
    zoom_shell_t *shell,
    const char *url,           /* 用户在 Shell 里用 ai url 设置的地址 */
    const char *body,          /* 纯文本请求体 */
    uint16_t body_len,
    char *resp_buf,            /* 将响应写入此缓冲区 */
    uint16_t resp_buf_size     /* 缓冲区大小，含结尾空间 */
);
```

**返回值**：成功时返回写入 `resp_buf` 的字节数（建议不超过 `resp_buf_size - 1`，以便调用方补 `'\0'`）；失败返回负数。

### 3.2 注册回调

在 `zoom_shell_init()` **成功返回之后**调用一次：

```c
void zoom_ai_bridge_set_post(zoom_shell_t *shell, zoom_ai_http_post_fn_t fn);
```

未注册时，`ai ask` 会提示需在 port 中注册。

Shell 实例内会保存：

- `shell->ai_bridge.url[]` — 由 `ai url` 命令写入；
- `shell->ai_bridge.http_post` — 由 `zoom_ai_bridge_set_post` 设置。

## 4. Shell 命令用法

| 命令 | 说明 |
|------|------|
| `ai url <http(s)://主机/路径>` | 设置后续 `ai ask` 使用的 POST 地址。 |
| `ai ask <文本...>` | 将 `<文本...>` 按空格拼成一行，作为 **body**（`Content-Type` 由你的回调决定，库侧仅传 `body`/`body_len`）。 |
| `ai status` | 显示当前 URL 是否已设置、HTTP 回调是否已注册。 |

示例：

```text
zoom> ai url https://192.168.1.10:8080/v1/prompt
zoom> ai ask 读取传感器 A 的当前值
```

## 5. x86 Demo 行为

`demo/x86-gcc` 在打开 `ZOOM_USING_AI_BRIDGE` 时，会注册一个**占位回调**，**不会发起真实网络请求**，仅在响应中回显类似 `[demo] ` + body，用于本地验证命令与链路。

若要在 PC 上测真实 HTTP，应在你自己的 `port` 中用 `libcurl` 等实现 `zoom_ai_http_post_fn_t`（注意许可证与依赖），或单独运行网关进程，设备仍走串口/网络到你的网关。

## 6. 网关侧思路（推荐）

1. 设备 POST **纯文本**或你自定义的 JSON（需在回调里组装 body，当前扩展默认是**纯文本**）。
2. 网关（Node/Python/Go 等）从环境变量读取 `OPENAI_API_KEY` 等，转发到云 API。
3. 网关将模型回复以**纯文本**返回（或你的回调解析 JSON 后把摘要写入 `resp_buf`）。

这样密钥只存在于服务器或开发机，固件只保存内网 URL 或域名。

## 7. 与 OpenClaw / Claw 生态的关系

本扩展是**薄协议层**：不嵌入 OpenClaw/NullClaw 等运行时。主机侧可运行 OpenClaw，设备侧用 Zoom Shell + 本桥接访问你的 HTTP 服务，二者通过**自建网关**协作，而非源码级合并。详见主 README「与 OpenClaw / Claw 生态协作」小节。

## 8. 故障排查

| 现象 | 可能原因 |
|------|----------|
| 无 `ai` 命令 | 未定义 `ZOOM_USING_AI_BRIDGE 1` 或未链接 `zoom_shell_ai_bridge.c` |
| `Set URL first` | 未执行 `ai url ...` |
| `Register HTTP POST in port` | 未调用 `zoom_ai_bridge_set_post` |
| `HTTP POST failed (负数)` | 回调返回错误，检查网络、TLS、URL、超时 |
| 修改配置后链接奇怪错误 | 执行 `make clean` 后全量重编 |

## 9. 参考源码

- 扩展实现：[extensions/ai_bridge/zoom_shell_ai_bridge.c](../extensions/ai_bridge/zoom_shell_ai_bridge.c)
- 类型与结构体：[include/zoom_shell.h](../include/zoom_shell.h)（搜索 `ZOOM_USING_AI_BRIDGE`）
- x86 占位回调：[demo/x86-gcc/zoom_shell_port.c](../demo/x86-gcc/zoom_shell_port.c)
