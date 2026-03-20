# 扩展模块总览

所有扩展均为可选，通过 `zoom_shell_cfg_user.h` 中的 `ZOOM_USING_*` 宏开关，默认多为 `0`（以 [zoom_shell_cfg.h](../include/zoom_shell_cfg.h) 为准）。

**核心库**（`src/`）不依赖扩展；扩展编译进工程后，由链接器段或静态表注册命令。

---

## 1. 核心功能开关（非扩展目录）

| 宏 | 默认 | 说明 |
|----|------|------|
| `ZOOM_USING_CMD_EXPORT` | 1 | 使用链接器段导出命令；为 0 时用静态数组模式 |
| `ZOOM_USING_HISTORY` | 1 | 命令历史 |
| `ZOOM_USING_USER` | 1 | 用户/登录/权限 |
| `ZOOM_USING_VAR` | 1 | 变量观测 `var` |
| `ZOOM_USING_PLUGIN` | 0 | 静态插件表 |
| `ZOOM_USING_LOCK` | 0 | shell 互斥回调 |
| `ZOOM_USING_PERF` | 1 | 命令计时 `perf` |
| `ZOOM_USING_ANSI` | 1 | 颜色、方向键、Tab 补全等 |
| `ZOOM_USING_AUDIT_LOG` | 0 | 审计日志 |

---

## 2. 扩展模块一览

| 宏 | 源码路径 | Shell / API 摘要 |
|----|----------|------------------|
| `ZOOM_USING_HEXDUMP` | [extensions/hexdump/zoom_shell_hexdump.c](../extensions/hexdump/zoom_shell_hexdump.c) | `mem read/write/dump` 内存查看（危险，慎用） |
| `ZOOM_USING_LOG` | [extensions/log/](../extensions/log/) | `zoom_log.h`：`ZOOM_LOGI` 等；命令 `log level/on/off/test` |
| `ZOOM_USING_CALC` | [extensions/calc/zoom_shell_calc.c](../extensions/calc/zoom_shell_calc.c) | `calc <表达式>` |
| `ZOOM_USING_PASSTHROUGH` | [extensions/passthrough/zoom_shell_passthrough.c](../extensions/passthrough/zoom_shell_passthrough.c) | `passthrough`；Ctrl+D 退出；需注册回调 |
| `ZOOM_USING_REPEAT` | [extensions/repeat/zoom_shell_repeat.c](../extensions/repeat/zoom_shell_repeat.c) | `repeat <ms> <cmd>` / `repeat stop`；与 `zoom_shell_run` 内 tick 配合 |
| `ZOOM_USING_KEYBIND` | [extensions/keybind/](../extensions/keybind/) | Ctrl 组合键；`keybind list`；`zoom_shell_keybind_init` |
| `ZOOM_USING_ALIAS` | [extensions/alias/zoom_shell_alias.c](../extensions/alias/zoom_shell_alias.c) | `alias`；首词别名展开 |
| `ZOOM_USING_SCRIPT` | [extensions/script/zoom_shell_script.c](../extensions/script/zoom_shell_script.c) | `script define/run/list/delete` |
| `ZOOM_USING_PROGRESS` | [extensions/progress/](../extensions/progress/) | `progress demo`；API `zoom_progress_bar` 等 |
| `ZOOM_USING_GAME` | [extensions/game/](../extensions/game/) | Shell 命令：`game_2048`、`pushbox`、`snake` |
| `ZOOM_USING_AI_BRIDGE` | [extensions/ai_bridge/zoom_shell_ai_bridge.c](../extensions/ai_bridge/zoom_shell_ai_bridge.c) | `ai url/ask/status`；见 [ai_bridge.md](ai_bridge.md) |
| （无单独宏，头文件） | [extensions/cpp_support/zoom_shell_cpp.h](../extensions/cpp_support/zoom_shell_cpp.h) | C++ 中导出命令时包含此头 |
| （依赖网络栈） | [extensions/telnet/](../extensions/telnet/) | Telnet 服务；需实现 socket 回调 |

---

## 3. 容量相关宏（与扩展相关）

| 宏 | 含义 |
|----|------|
| `ZOOM_ALIAS_MAX` | 别名个数 |
| `ZOOM_SCRIPT_MAX_LINES` / `ZOOM_SCRIPT_MAX` | 脚本行数、脚本个数 |
| `ZOOM_KEYBIND_MAX` | 按键绑定条数 |
| `ZOOM_LOG_BUFFER_SIZE` | 日志单条缓冲 |
| `ZOOM_AI_URL_MAX` / `ZOOM_AI_RESP_MAX` | AI 桥 URL 与响应长度 |

---

## 4. 编译进工程

- 参考官方 [Makefile](../Makefile) 中的 `EXT_SRCS`，按需加入你的构建系统。
- 若使用 `ZOOM_USING_CMD_EXPORT`，必须在链接脚本中保留 `zoomCommand` / `zoomVar` / `zoomUser` 等段（见 [porting_general.md](porting_general.md)）。
- 修改 `zoom_shell_cfg_user.h` 后建议 **`make clean`** 再全量编译，避免宏与目标文件不一致。

---

## 5. 与 core 的耦合点

部分扩展在 [zoom_shell_core.c](../src/zoom_shell_core.c) 中有钩子（透传、repeat、别名、脚本、按键绑定等）。关闭对应宏时，相关代码不参与编译。
