## Строим LLVM из исходников

Нужны: git, gcc, g++, cmake, ninja, libzstd, lld

Для установки libzstd на Ubuntu:
```sh
$ sudo apt install libzstd-dev
```

### Клонируем LLVM (версия [19.1.0](https://github.com/llvm/llvm-project/commit/a4bf6cd7cfb1a1421ba92bca9d017b49936c55e4))
```sh
$ mkdir llvm-project
$ cd llvm-project
$ git init
$ git remote add origin https://github.com/llvm/llvm-project.git
$ git fetch --depth 1 origin a4bf6cd7cfb1a1421ba92bca9d017b49936c55e4
$ git checkout FETCH_HEAD
```
Занимает 2.2 GB.

### Строим LLVM
```sh
$ cmake -S llvm -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DLLVM_ENABLE_ASSERTIONS=ON \
    -DLLVM_TARGETS_TO_BUILD=X86 \
    -DLLVM_PARALLEL_LINK_JOBS=1 \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DLLVM_CCACHE_BUILD=ON \
    -DLLVM_BUILD_LLVM_DYLIB=ON
```

- Если строите на ARM, то вместо `-DLLVM_TARGETS_TO_BUILD=X86` указывайте `-DLLVM_TARGETS_TO_BUILD=AArch64`.
- `-DLLVM_CCACHE_BUILD=ON` опционально. Ускоряет построение кэшированием компиляций. Для работы нужен `ccache`.

Затем:
```
$ cd build
$ ninja
```

- Если билд падает с Out of Memory, то перезапустите еще раз. Можно попробовать строить в один поток с этого момента: `ninja -j1`

### Скачиваем clang

clang строится долго и занимает много места.  
Вместо ручного билда, скачаем готовый с https://github.com/llvm/llvm-project/releases/tag/llvmorg-19.1.0.

На Linux нужен файл LLVM-19.1.0-Linux-X64.tar.xz,  
на macOS - LLVM-19.1.0-macOS-X64.tar.xz

Создаём отдельную папку, качаем и разархивируем:
```sh
$ mkdir /path/to/clang/install
$ cd /path/to/clang/install
$ wget https://github.com/llvm/llvm-project/releases/download/llvmorg-19.1.0/LLVM-19.1.0-Linux-X64.tar.xz
$ tar -xzvf LLVM-19.1.0-Linux-X64.tar.xz
$ cp /path/to/cjit/llvm-patches/ClangTargets.cmake LLVM-19.1.0-Linux-X64/lib/cmake/clang
```

### Ubuntu/Debian:
```sh
$ wget https://apt.llvm.org/llvm.sh
$ chmod +x llvm.sh
$ sudo ./llvm.sh 19
$ sudo apt install llvm-19-dev libclang-common-19-dev libclang-19-dev libclang-cpp19-dev
```

## Строим cjit

В корне cjit:

```bash
cmake -S . -B build -G Ninja \
    -DLLVM_DIR=~/work/projects/llvm-project/build/lib/cmake/llvm \
    -DClang_DIR=~/work/projects/cjit/LLVM-19.1.0-Linux-X64/lib/cmake/clang \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

cmake -S . -B build -G Ninja \
    -DLLVM_DIR=/usr/lib/llvm-19/lib/llvm/clang \
    -DClang_DIR=/usr/lib/llvm-19/lib/cmake/clang \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

mac:
brew install llvm@19
/opt/homebrew/opt/llvm/lib/cmake/llvm
/opt/homebrew/opt/llvm/lib/cmake/clang