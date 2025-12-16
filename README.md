# BashForge ![C++](https://img.shields.io/badge/C%2B%2B-17-blue?style=flat&logo=c%2B%2B&logoColor=white) ![Windows](https://img.shields.io/badge/Windows-Native-blue?style=flat&logo=windows&logoColor=white)

**BashForge** is a robust, lightweight command-line utility that brings Unix-like command syntax and functionality to Windows using native WinAPI calls.

## ✨ Key Features

- **Comfort for Linux users**: Familiar syntax and flags for Linux users working in Windows consoles.
- **Native Performance**: Direct WinAPI calls (`CreateFile`, `GetFileTime`, `SetFileTime`) for maximum speed.
- **Comprehensive Flag Support**: Aims for functional parity with GNU/Linux (e.g., `ls -lhat`, `cat -bn`).
- **Clean Error Handling**: Unix-style error messages mapped from Windows API failures.

## 🔨 Implementation Status

### Currently Implemented

**Shell Basics:**
- `pwd` - Current working directory 
- `whoami` - Current username 
- `clear` - ANSI escape sequence screen clear
- `exit` - Graceful session termination

**Core Utilities:**
- **`ls`** - Full implementation with flags: `-a` (hidden), `-l` (long), `-h` (human-readable), `-r` (reverse), `-t` (time sort), `-R` (recursive)
- **`echo`** - Standard output with escape sequence handling (`-e`, `-E`, `-n`)
- **`cat`** - Advanced file display with:
  - Line numbering: `-n` (all lines), `-b` (non-blank only)
  - Non-printables: `-v` (^ notation for control chars)
  - Formatting: `-T` (tabs as ^I), `-E` (endlines as $), `-A` (both)
  - `-s` (squeeze empty lines)

### In Progress
- **`touch`** - Advanced file timestamp manipulation with flags `-a`, `-m`, `-c`, `-r`, `-d`, `-t`, `-h`

## 🎯 Future Roadmap

**1. Advanced Filesystem Commands**

cd, rm, mv, cp, mkdir, rmdir, find
chmod, chown (Windows ACL → POSIX mapping)



**2. Process & System Commands**

ps aux, pidof, top, kill (Windows Process APIs)



**3. Networking**

ping, curl, ssh, ifconfig



**4. Shell Advancements**

Piping (|) - Redirection (>, >>, <) - Aliases - History



## 🚀 How to Start this on YOUR machine

Clone & build

git clone https://github.com/mielTheCoolGirl/bashForge.git
Open bashForge.sln in Visual Studio
Build (x64/x86, Debug/Release)

---Run from Command Prompt or PowerShell---

To run you must take the exe
(./bashForge.exe) and run it via the command line in windows/press the exe to activate it

A run example:


    >pwd
    C:\Users\YourUser\Documents\BashForge

    >ls -lah
    total 12
    drwxrwxrwx 1 user group 4096 Nov 10 14:32 .
    -rwxrwxrwx 1 user group 1437 Nov 10 14:32 bashForge.sln

    >cat -n file.txt
    1 First line of file
    2 Second line here

    >whoami
    YourUser



## 💃🤝💃🏻🤝 Contributing 

1. Fork the project
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to branch (`git push origin feature/AmazingFeature`)
5. Open Pull Request

​
