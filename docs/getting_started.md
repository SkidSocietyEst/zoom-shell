# 快速开始与构建

## 环境要求

- **GCC**（或兼容的 C99 编译器）
- **GNU Make**
- **x86 Linux** 运行 demo 时用于终端 raw 模式（`demo/x86-gcc`）

## 获取代码后

```bash
cd zoom-shell
```

## 编译 x86 演示程序

```bash
make demo
./build/zoom_shell_demo
```

演示程序使用 `demo/x86-gcc/zoom_shell_cfg_user.h` 覆盖默认配置，功能开关较全（含扩展模块）。退出可输入 `exit` 或按 Ctrl+D（视终端行为而定）。

## 运行自动化测试

```bash
make test
```

测试使用 `test/zoom_shell_cfg_test.h`，与 demo 配置相互独立。全部通过时进程退出码为 `0`。

**注意**：修改 `zoom_shell_cfg_user.h` 后若出现链接错误，请先执行 `make clean` 再编译，以便所有翻译单元按新宏重新编译。

## 目录结构（简要）

```
zoom-shell/
├── include/           # zoom_shell.h、zoom_shell_cfg.h
├── src/               # 核心：core / cmds / var
├── extensions/        # 可选扩展（hexdump、log、calc、ai_bridge 等）
├── demo/x86-gcc/      # PC 终端 demo + link.lds
├── demo/stm32-freertos/ 等   # 其他平台模板
├── test/              # 单元测试
└── docs/              # 说明文档（本目录）
```

## 配置方式

1. 复制或参考 `demo/x86-gcc/zoom_shell_cfg_user.h` 编写你的 **`zoom_shell_cfg_user.h`**。
2. 编译时定义宏，让配置头被包含：

   ```bash
   -DZOOM_SHELL_CFG_USER='"zoom_shell_cfg_user.h"'
   ```

3. 在 `zoom_shell_cfg_user.h` 中设置 `ZOOM_CMD_MAX_LENGTH`、`ZOOM_GET_TICK()` 等；各功能用 `ZOOM_USING_*` 宏开关。

详见主 [README.md](../README.md) 中的「移植到你的 MCU」与「链接脚本」章节；扩展列表见 [extensions_overview.md](extensions_overview.md)，通用移植步骤见 [porting_general.md](porting_general.md)。

## 静态库（可选）

```bash
make lib
```

生成 `build/libzoomshell.a`（核心 + 各扩展目标文件，具体以 Makefile 为准）。
