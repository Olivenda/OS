# OS

Welcome to the **OS** repository. This project aims to build a small experimental operating system for educational purposes.

## Features

- Minimal boot sequence
- Simple command-line interface
- Modular design to add new components easily

## Getting Started

Clone this repository and run the build script:

```bash
make
```

You can then run the resulting image with a virtual machine such as QEMU:

```bash
qemu-system-x86_64 -kernel build/os-image
```

## Contributing

Contributions are welcome! Feel free to open issues or submit pull requests if you have improvements or bug fixes.

## Disclaimer

This project is a learning exercise and is not intended for production use.


