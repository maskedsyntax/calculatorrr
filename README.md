
<p align="center">
  <img src="./assets/calculatorrr.png" alt="Calculatorrr Logo" width="160" />
</p>

<h1 align="center">Calculatorrr</h1>

<p align="center">
  <b> Simple GTK Calculator </b>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Platform-Linux-blue" />
  <img src="https://img.shields.io/badge/Built_with-C++-blueviolet" />
  <img src="https://img.shields.io/badge/UI-GTK-8bc34a" />
  <img src="https://img.shields.io/badge/License-MIT-blue.svg" />
  <img src="https://img.shields.io/badge/Status-In%20Progress-orange" />
</p>

## Compilation instructions:

```bash
g++ -o calculatorrr calculatorrr.cpp `pkg-config --cflags --libs gtk4`
```
 
### Make sure to have GTK4 development libraries installed:
- Ubuntu/Debian:
  ```bash
  sudo apt install libgtk-4-dev
  ```
- Fedora:
  ```bash
  sudo dnf install gtk4-devel
  ```
- Arch:
  ```bash
  sudo pacman -S gtk4
  ```
