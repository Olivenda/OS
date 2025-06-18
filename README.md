# Simple OS

This project contains a very small hobby operating system with a basic bootloader and kernel. The bootloader loads the kernel into memory and switches to 32-bit protected mode. The kernel provides simple text output, keyboard input, a small in-memory file system and a minimal shell with a few commands.

## Building

```
make
```

This produces an `os-image` file which can be booted with an emulator such as `qemu-system-i386`:

```
make run
```

## Commands
- `help` – show available commands
- `ls` – list files in the in-memory file system
- `cat <name>` – display a file's contents

