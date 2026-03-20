# ESP-IDF 移植指南

在 [ESP-IDF](https://esp-idf.espressif.com/) 工程中使用 Zoom Shell 的说明，风格与 [getting_started.md](getting_started.md) 一致。

模板文件：`demo/esp-idf/`（[zoom_shell_port.c](../demo/esp-idf/zoom_shell_port.c)、[zoom_shell_cfg_user.h](../demo/esp-idf/zoom_shell_cfg_user.h)、[README](../demo/esp-idf/README.md)）。

---

## 1. 集成步骤清单

| # | 事项 |
|---|------|
| 1 | 将 `zoom-shell/include/`、`zoom-shell/src/` 作为组件源码或子模块引入。 |
| 2 | 按需加入 `zoom-shell/extensions/` 下的 `.c` 文件（与 [Makefile](../Makefile) 中 `EXT_SRCS` 对齐或按需裁剪）。 |
| 3 | 复制并修改 `demo/esp-idf/zoom_shell_port.c`、`zoom_shell_cfg_user.h` 到组件目录。 |
| 4 | 在组件 `CMakeLists.txt` 中设置 `INCLUDE_DIRS`、源文件列表及 `ZOOM_SHELL_CFG_USER`。 |
| 5 | 配置 UART（`uart_read_bytes` / `uart_write_bytes` 等），在 `app_main` 中创建任务调用 `zoom_shell_run`。 |
| 6 | 为 `ZOOM_USING_CMD_EXPORT` 配置链接段（见下文）。 |

---

## 2. CMakeLists.txt 示例

最小可用示例（路径按你仓库调整）：

```cmake
idf_component_register(
    SRCS
        "zoom_shell_port.c"
        "../path/to/zoom-shell/src/zoom_shell_core.c"
        "../path/to/zoom-shell/src/zoom_shell_cmds.c"
        "../path/to/zoom-shell/src/zoom_shell_var.c"
    INCLUDE_DIRS
        "."
        "../path/to/zoom-shell/include"
)

target_compile_definitions(${COMPONENT_LIB} PUBLIC
    ZOOM_SHELL_CFG_USER="zoom_shell_cfg_user.h"
)
```

加入扩展时，在 `SRCS` 中追加对应 `extensions/.../*.c`，并为需要额外头文件的扩展增加 `INCLUDE_DIRS`（如 `extensions/log`）。

---

## 3. Tick 与配置

在 `zoom_shell_cfg_user.h` 中：

```c
#include <stdint.h>
#include "esp_log.h" /* 或 esp_timer / FreeRTOS tick，按项目统一 */

#define ZOOM_GET_TICK()  (uint32_t)(esp_log_timestamp())
```

也可使用 `xTaskGetTickCount()` × `portTICK_PERIOD_MS` 等，需与命令超时、repeat 等逻辑一致。

---

## 4. 链接脚本与段导出

ESP-IDF 使用 `.lf` 与 **链接器片段生成**（linker fragment）。需为 `zoomCommand`（及 `zoomVar`、`zoomUser` 等）配置与 `ZOOM_SECTION("zoomCommand")` 匹配的段，使 `_zoom_cmd_start` / `_zoom_cmd_end` 等符号正确。

官方 demo 片段示例（见 [demo/esp-idf/README.md](../demo/esp-idf/README.md)）：

```
[sections:zoom_cmd]
entries:
    .zoom_command

[scheme:zoom_cmd_default]
entries:
    zoom_cmd -> flash_rodata
```

具体名称以你使用的 ESP-IDF 版本与 `linker.lf` 为准；若链接报错找不到 `_zoom_cmd_start`，说明段未进入最终镜像，需检查 `sdkconfig` 与自定义组件的 `ld` 片段。

---

## 5. UART 与任务

- 在 `zoom_shell_port.c` 中实现 `read`/`write`：通常 `uart_read_bytes` 带超时、`uart_write_bytes`。  
- 在独立任务中运行 `zoom_shell_run`，栈大小建议 ≥ 4096 字节（随 `ZOOM_CMD_MAX_LENGTH`、历史条数与扩展增加）。  
- 若使用 WiFi 与 `ZOOM_USING_AI_BRIDGE`，在 `zoom_ai_bridge_set_post` 中可用 `esp_http_client` 实现 POST。

---

## 6. C++ 组件

若命令在 `.cpp` 中导出，包含 [extensions/cpp_support/zoom_shell_cpp.h](../extensions/cpp_support/zoom_shell_cpp.h)，并注意 `extern "C"` 与段属性。

---

## 7. 验证

1. 烧录后串口出现 `zoom>`（或自定义提示符）。  
2. `help`、`version` 正常。  
3. 若有自定义 `ZOOM_EXPORT_CMD`，确认命令出现在列表中；若缺失，优先查链接段与 `KEEP`。

更多通用项见 [porting_general.md](porting_general.md)。
