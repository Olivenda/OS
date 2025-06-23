
# Simple OS

This project contains a very small hobby operating system with a basic bootloader and kernel. The bootloader now switches the processor into 64-bit long mode before jumping to the kernel. The kernel provides simple text output, keyboard input, a small in-memory file system and a minimal shell with a few commands.

## Building

```
make
```

This produces an `os-image.iso` file which can be booted with an emulator such as `qemu-system-x86_64`:

```
make run
```

## Commands
- `help` – show available commands
- `ls` – list files in the in-memory file system
- `cat <name>` – display a file's contents
- `echo <text>` – print text to the console
- `clear` – clear the screen
- `halt` – halt the system
- `reboot` – reboot the system

# OS

Welcome to the **OS** repository. This project aims to build a small experimental operating system for educational purposes.

## Features

- Minimal boot sequence
- Simple command-line interface
- Modular design to add new components easily
- Linux-like shell with commands such as `echo`, `clear` and `reboot`

## Getting Started

Clone this repository and run the build script:

```bash
make
```

You can then run the resulting image with a virtual machine such as QEMU:

```bash
qemu-system-x86_64 -cdrom os-image.iso
```

## Contributing

Contributions are welcome! Feel free to open issues or submit pull requests if you have improvements or bug fixes.

## Disclaimer

This project is a learning exercise and is not intended for production use.


