# 通用移植清单

适用于裸机、RTOS 等场景的 **Checklist**。ESP-IDF 另有专文：[porting_esp_idf.md](porting_esp_idf.md)。

---

## 1. 文件与编译选项

| 步骤 | 说明 |
|------|------|
| 加入源码 | 至少 `include/`，`src/zoom_shell_core.c`、`zoom_shell_cmds.c`、`zoom_shell_var.c`；按需加入 `extensions/` 下 `.c` 文件。 |
| 用户配置 | 编写 `zoom_shell_cfg_user.h`，编译定义 `-DZOOM_SHELL_CFG_USER='"zoom_shell_cfg_user.h"'`。 |
| 包含路径 | `-I` 指向 `include`；若使用带独立头文件的扩展（如 `log`、`keybind`、`progress`），增加对应 `extensions/...` 目录。 |

---

## 2. 链接脚本（`ZOOM_USING_CMD_EXPORT == 1`）

必须将命令/变量/用户等段放入 **可执行 Flash/ROM**，并导出 `_zoom_cmd_start` / `_zoom_cmd_end` 等符号（名称以 [demo/x86-gcc/link.lds](../demo/x86-gcc/link.lds) 为准）。

典型段名：

- `zoomCommand` — 命令表  
- `zoomVar` — 变量表（若使用 `ZOOM_EXPORT_VAR`）  
- `zoomUser` — 静态用户（若使用 `ZOOM_EXPORT_USER`）  

**对齐**：段内按 4 字节对齐，避免 MCU 上未对齐访问。

---

## 3. 平台必须实现的接口

| 项目 | 说明 |
|------|------|
| `ZOOM_GET_TICK()` | 在 `zoom_shell_cfg_user.h` 中定义为返回毫秒级的 `uint32_t`（如 `HAL_GetTick()`、`rt_tick_get()`、`esp_log_timestamp()`）。 |
| `shell->read` | 阻塞读取至少 1 字节；若无可读数据可返回 0（视 `zoom_shell_run` 轮询策略而定）。 |
| `shell->write` | 写出到 UART 等。 |
| 缓冲区 | `zoom_shell_init(shell, buf, size)` 的 `size` 不小于 `zoom_shell_cfg.h` 中按历史条数计算的最小值。 |

可选：`lock` / `unlock`（`ZOOM_USING_LOCK`）。

---

## 4. 运行模式二选一

| 模式 | 调用方式 |
|------|----------|
| 阻塞 | `zoom_shell_run(&shell)` 内部循环 `read` + `zoom_shell_input`。 |
| 中断/轮询 | 主循环或 UART 中断里对每字节调用 `zoom_shell_input(&shell, byte)`。 |

---

## 5. STM32 + FreeRTOS（模板）

仓库模板：[demo/stm32-freertos/](../demo/stm32-freertos/)。

要点：

1. 在任务中调用 `zoom_shell_init` + `zoom_shell_run`，或在中断 + 主循环中使用 `zoom_shell_input`。  
2. 链接脚本添加 zoom 段（模板 README 中有片段）。  
3. 任务栈建议 ≥ 1～2 KB（取决于历史长度与扩展）。  

详见 [demo/stm32-freertos/README.md](../demo/stm32-freertos/README.md)。

---

## 6. RT-Thread（模板）

仓库模板：[demo/rt-thread/](../demo/rt-thread/)。

要点：`rt_device_read` / `rt_device_write` 对接串口；链接脚本与 STM32 类似。详见 [demo/rt-thread/README.md](../demo/rt-thread/README.md)。

---

## 7. 扩展与端口初始化

若使用扩展（如 `ZOOM_USING_KEYBIND`、`ZOOM_USING_LOG`、`ZOOM_USING_AI_BRIDGE`），在 `zoom_shell_init` **成功之后**于 `port` 中调用文档要求的初始化（例如 `zoom_shell_keybind_init`、`zoom_log_init`、`zoom_ai_bridge_set_post`）。参考 [demo/x86-gcc/zoom_shell_port.c](../demo/x86-gcc/zoom_shell_port.c)。

---

## 8. 验证

- 主机上运行 `make test`（x86 + 测试配置）作回归参考。  
- 目标板：上电后串口应出现欢迎信息与提示符，输入 `help`、`version` 验证。
