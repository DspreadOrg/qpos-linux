# Device Application Build & Installation Guide

This guide is for **customers who have received the device and need to build and install the application
themselves**. There are only three steps: **prepare the tools → build the APK → install it on the device**.

---

## Contents

1. [What you need](#1-what-you-need)
2. [Platform types](#2-platform-types)
3. [How to build](#3-how-to-build)
4. [How to install on the device](#4-how-to-install-on-the-device)
5. [Switching platforms](#5-switching-platforms)
6. [Troubleshooting](#6-troubleshooting)
7. [Command reference](#7-command-reference)

---

## 1. What you need

| Item                           | Requirement                                                       |
| ------------------------------ | ----------------------------------------------------------------- |
| Computer                       | Windows 10 / 11 (64-bit)                                          |
| Device                         | The target device and a USB cable                                 |
| Build tools                    | CMake, make, cross toolchain, zip (see below)                     |
| adb (optional but recommended) | Lets the scripts detect the device type and install automatically |

Tool checklist (each must run directly from the command line):

| Tool                  | Purpose                      | Check command                            |
| --------------------- | ---------------------------- | ---------------------------------------- |
| CMake (3.10 or newer) | Generate the build files     | `cmake --version`                        |
| make                  | Run the build                | `make --version`                         |
| ARM cross toolchain   | Build QposPlus / QposTrio    | `arm-none-linux-gnueabihf-gcc --version` |
| MIPS cross toolchain  | Build D70                    | `mips-linux-gnu-gcc --version`           |
| zip                   | Create the APK               | `where zip`                              |
| adb (recommended)     | Detect device type / install | `adb devices`                            |

Notes:

- **Building does not require adb** (without it the script asks you to pick the platform once), but
  **installing with the script does require adb**.

- **If you only work with one model, install only that platform's cross toolchain**
  (MIPS for D70, ARM for QposPlus / QposTrio).

---

## 2. Platform types

The project builds for two platforms. **The platform must match the device model** — a package built for
the wrong platform installs but will not run.

| Platform | Devices              | Output                                             |
| -------- | -------------------- | -------------------------------------------------- |
| `ARM`    | QposPlus, QposTrio   | `QposPlus_Linux_App.apk`, `QposTrio_Linux_App.apk` |
| `MIPS`   | D70 (Ingenic X2600E) | `D70_Linux_App.apk`                                |

Keep in mind:

- The two ARM packages (`QposPlus_...` and `QposTrio_...`) have **identical content, only the file name
  differs**; each is used for one model, either one works.
- All APKs are created in the **project root** (the folder containing `CMakeLists.txt`).
- The platform is remembered by the scripts; you never need to edit any file by hand.

---

## 3. How to build

### 3.1 Open the project folder

Open a Command Prompt and change to the project root (the folder with `CMakeLists.txt` and `pkg_bat`):

```bat
cd /d D:\git\Linux_Pos\linux_pos_app
```

### 3.2 Build with one command

**Recommended: connect the device over USB first, then run**

```bat
pkg_bat\compile.bat
```

The script detects the device platform and builds and packages immediately — **it asks no questions**. On
success the last lines look like this:

```
[INFO] Device platform: MIPS  [adb shell uname -m]
[INFO] Target platform: MIPS
[100%] Built target linux_pos_app
[OK] Build finished: release\MIPS\bin\linux_pos_app
[INFO] Libraries packed from lib\MIPS: emv mqtt sdk
[INFO]   17 file(s); "os" is OS-provided and not packed
[INFO] Created D70_Linux_App.apk
[OK] compile.bat done for MIPS.
```

If you see `[OK] compile.bat done` and no `[ERROR]`, it succeeded and the APK is in the project root.

**Without a device:** if a platform was chosen before, it is reused silently. Only when no platform has
ever been chosen does a menu appear:

```
==========================================
   Select target platform
==========================================

   1. ARM    (QposPlus / QposTrio)
   2. MIPS   (D70)

  NOTE: this step is SKIPPED when a device is connected by
        USB/adb - the platform is then read from the device.

Enter 1 or 2:
```

Enter `1` (QposPlus / QposTrio) or `2` (D70) and press Enter; the choice is remembered and **you will not
be asked again**.

> The first build takes a few minutes. Please wait.
> Many `warning` messages are normal and do not affect the output.

### 3.3 After each code change

Same command, incremental build, usually tens of seconds:

```bat
pkg_bat\compile.bat
```

---

## 4. How to install on the device

### 4.1 Recommended: install with the script

> This step requires adb (building does not, installing does).

With the device connected over USB, run from the project root:

```bat
pkg_bat\install.bat
```

The script reads the device platform and installs the APK for **that** platform:

```
[INFO] Device platform: MIPS  [adb shell uname -m]
[INFO] APK: D70_Linux_App.apk  [MIPS]
[INFO] Copying D70_Linux_App.apk to the device...
[INFO] Installing D70_Linux_App.apk on the device...

[OK] Installed D70_Linux_App.apk on the MIPS device.
```

**Platform check before installing**: if the APK does not match the device, the script refuses to install
and tells you why:

```
[WARN] Platform mismatch, installation stopped.
[WARN]   device platform : ARM   [adb shell uname -m]
[WARN]   this device needs : ARM APK, e.g. QposPlus_Linux_App.apk
[WARN]   APK platform    : MIPS   [D70_Linux_App.apk]
```

Run `pkg_bat\compile.bat` to build the package for this device, then run `pkg_bat\install.bat` again.

To install a specific package name (the platform check still runs):

```bat
pkg_bat\install.bat QposTrio_Linux_App
```

### 4.2 Install manually

Use the install/upgrade tool shipped with your device and select the `.apk` for the matching model in the
project root.

---

## 5. Switching platforms

When you change models (for example D70 → QposPlus) the platform **must** change too, otherwise the
package will not run on the device.

### 5.1 Recommended: connect the new device and build

```bat
pkg_bat\compile.bat
```

The script reads the new device's platform, switches automatically and reports:

```
[INFO] Platform changed: MIPS -> ARM (device)
```

### 5.2 Without a device

```bat
pkg_bat\recompile.bat
```

It shows the same menu as the first run, then does a clean + full rebuild + packaging for the chosen
platform. `recompile.bat` is also the way to **start over completely** if a build result looks wrong.

### 5.3 Things to keep in mind

| Point                                               | Explanation                                                                                                                          |
| --------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------ |
| Do not edit `pkg_bat\target_arch.cfg` by hand       | The scripts maintain it; a typo here builds for the wrong platform                                                                   |
| The two platforms build independently               | `build\ARM` and `build\MIPS` are separate, switching does not damage either, and both kinds of packages can stay in the project root |
| `clean.bat` cleans only the current platform        | Packages of the other platform are kept                                                                                              |
| Want to choose manually while a device is connected | Run `set FORCE_ARCH_MENU=1` first, then run the script                                                                               |
| Wrong platform chosen (e.g. ARM for a D70)          | The package builds but will not run. Run `recompile.bat` and pick the right platform                                                 |

---

## 6. Troubleshooting

| Symptom                                                              | Cause and fix                                                                                                                  |
| -------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------ |
| `'pkg_bat\compile.bat' is not recognized ...`                        | Wrong working directory. Run `cd /d <project root>` first                                                                      |
| `[ERROR] cmake was not found in PATH.`                               | CMake is missing or not on `PATH`. Install it and **reopen** the command window                                                |
| `MIPS toolchain not found`                                           | The MIPS toolchain was not found. Set `MIPS_TOOLCHAIN_ROOT` (see Section 1) or add its `bin` to `PATH`                         |
| `[ERROR] Packaging failed.` with a zip message                       | `zip.exe` is missing. Install zip, add it to `PATH`, verify with `where zip`                                                   |
| The menu keeps appearing (`Invalid selection`)                       | Only `1` or `2` are accepted. Once chosen, the platform is remembered and not asked again                                      |
| A device is connected but the menu still appears                     | `adb shell uname -m` returned nothing usable. Check the USB cable and that adb is enabled (`adb devices` must list the device) |
| `No device platform detected`                                        | No device or adb unavailable — normal fallback: the saved platform is used, or the menu appears                                |
| adb hangs, or you do not want the scripts to touch adb               | Run `set NO_ADB_DETECT=1` before running the scripts                                                                           |
| `[ERROR] Executable not found: release\...`                          | Packaging ran before a build. Run `pkg_bat\compile.bat` (it builds and packages)                                               |
| `install.bat` reports `Platform mismatch`                            | The APK does not match the device; installation was refused. Run `compile.bat` for this device                                 |
| `install.bat` reports `No built APK for ...`                         | That platform has no package yet. Run `pkg_bat\compile.bat` first                                                              |
| `install.bat` reports `APK not found`                                | Wrong package name, or the APK is not in the project root                                                                      |
| `install.bat` reports `Installation failed` and prints device output | The device installer is busy or refused. Read the printed output; reboot the device and retry                                  |
| Many `warning` messages during the build                             | Normal. It succeeded as long as `[OK] ... done` appears and there is no `[ERROR]`                                              |
| The program does not run on the device                               | Wrong platform (e.g. an ARM package on a D70). Run `pkg_bat\recompile.bat`, pick the right platform, then reinstall            |
| The build result looks wrong, want a clean restart                   | Run `pkg_bat\recompile.bat` (clean + full rebuild + packaging)                                                                 |
| Source files added or deleted                                        | Nothing to configure and no `build` folder to delete — just run `pkg_bat\compile.bat`                                          |

---

## 7. Command reference

All commands run from the **project root**:

| I want to                            | Command                 | Description                                                                                          |
| ------------------------------------ | ----------------------- | ---------------------------------------------------------------------------------------------------- |
| Build and package (most common)      | `pkg_bat\compile.bat`   | Detects the platform when a device is connected; otherwise uses the saved platform or shows the menu |
| Install on the device                | `pkg_bat\install.bat`   | Checks the platform first, installs only on a match                                                  |
| Switch platform / full rebuild       | `pkg_bat\recompile.bat` | Clean + full rebuild + packaging                                                                     |
| Build only (no packaging)            | `pkg_bat\build.bat`     | Force a platform with `build.bat ARM` / `build.bat MIPS`                                             |
| Package only                         | `pkg_bat\package.bat`   | Packages the existing build output                                                                   |
| Clean intermediates and old packages | `pkg_bat\clean.bat`     | Cleans the current platform only                                                                     |
| Show the parameter help              | `pkg_bat\build.bat /?`  | Displays usage                                                                                       |

The everyday workflow is two commands:

```bat
pkg_bat\compile.bat
pkg_bat\install.bat
```

---

**Technical support**: if you still cannot produce a package after following this guide, contact us with:

1. The complete command you ran;
2. The complete command-line output (especially the `[ERROR]` lines and their context);
3. The device model;
4. The content of `pkg_bat\target_arch.cfg`.

> For deeper technical details (directory layout, script parameters, how the platform is detected), see
> `build_advanced.en.md`.
