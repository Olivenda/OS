
# Simple OS

This project contains a very small hobby operating system with a basic kernel and UEFI loader. The boot process switches the processor into 64‑bit long mode before jumping to the kernel. The kernel provides simple text output, keyboard input, a tiny disk-backed file system and a minimal shell with a few commands.

## Building

```
make
```

This produces an `os-image.iso` file which can be booted with an emulator such as `qemu-system-x86_64` using an UEFI firmware (for example `OVMF.fd`):

```
make run
```

## Commands
Nach dem Booten des Betriebssystems landest du in einer einfachen Shell. Dort kannst du die folgenden Befehle nutzen:
- `help` – show available commands
- `ls` – list files in the persistent file system
- `cat <name>` – display a file's contents
- `echo <text>` – print text to the console
- `clear` – clear the screen
- `halt` – halt the system
- `reboot` – reboot the system
- `mkdir <name>` – create a directory (demo only)
- `sync` – manually write changes to disk
- `nano <name>` – edit a file
- `gui` – launch a simple text-based GUI (type `exit` to return)

Damit lassen sich Dateien anzeigen, Texte ausgeben, der Bildschirm leeren und das System anhalten oder neu starten.

# OS

Welcome to the **OS** repository. This project aims to build a small experimental operating system for educational purposes.

## Features

- Minimal boot sequence
- Simple command-line interface
- Modular design to add new components easily
- Linux-like shell with commands such as `echo`, `clear` and `reboot`
- Simple persistent file system stored on disk
- Optional simple text-based GUI with a single command line

## Getting Started

Clone this repository and run the build script:

```bash
make
```

You can then run the resulting image with a virtual machine such as QEMU:

```bash
qemu-system-x86_64 -bios OVMF.fd -cdrom os-image.iso
```

To write the ISO to a USB drive for real hardware testing you can use the `install.sh` helper script. Provide the target device as parameter (e.g. `/dev/sdX`):

```bash
./install.sh /dev/sdX
```

## Contributing

Contributions are welcome! Feel free to open issues or submit pull requests if you have improvements or bug fixes.

## Disclaimer

This project is a learning exercise and is not intended for production use.


