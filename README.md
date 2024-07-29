# Custom Container Runtime

A simple container runtime written in C, implementing namespace and cgroup isolation.

## Features

- Namespace isolation (PID, UTS, mount)
- Cgroup resource limits (memory and CPU)
- Custom root filesystem

## Prerequisites

- **GCC Compiler**: Ensure you have the GNU Compiler Collection installed.
- **libcap Library**: Required for setting capabilities.
- **Alpine Mini Root Filesystem**: Used as the container’s root filesystem.

## Setup Instructions

### 1. Clone the Repository

```sh
git  clone  https://github.com/yourusername/custom-container-runtime.git
cd  custom-container-runtime
```

### 2. Download the Alpine Mini Root Filesystem

To run the container runtime, you need to download the Alpine mini root filesystem. You can obtain it from the [Alpine Linux website](https://alpinelinux.org/downloads/).

- Choose the appropriate mini root filesystem tarball from the `mini` directory.
- Download the tarball to your local machine.

### 3. Extract the Filesystem

Extract the downloaded tarball into the **CONTAINER_PATH** directory. This directory is where the container runtime will look for the root filesystem.

### 4. Build the Project

```sh
make
```

### 5. Run the Container Runtime

```sh
./container_runtime  run <command>
```
