# switch-psdk
This project is an early-stage port of the Pokemon SDK framework to Atmosphere, the CFW for the Nintendo Switch.

It should allow you to compile and run Pokemon SDK demos on your Switch or emulator. It is very much in its experimental stage, you've been warned.

## Requirements
- CMake >= 3.28
- DevkitA64 == r29.2-1
- libnx == 4.12.0-1
- Ninja

## Build
### Prepare the demo/project
You need to compile the demo/project you want to port with the appropriate PSDK patch.

Clone [this fork](https://gitlab.com/maxmarsc/pokemonsdk) and use the `switch-support` branch.
Then compile the demo/project with the `--no-yarb` flag to prevent YARB arch conflict.

```shell
cd <DEMO_FOLDER>
git clone -b switch-support https://gitlab.com/maxmarsc/pokemonsdk.git
./game-linux.sh --util=project_compilation --no-yarb
```

### Build with CMake
With `<DEMO_FOLDER>` being the path of a compiled demo folder (above `Release`, but `Release` must exists)

```shell
# Source the devkitA64 environment
source /etc/profile.d/devkit-env.sh

cmake -B build -DCMAKE_TOOLCHAIN_FILE=DevkitA64.cmake -DCMAKE_BUILD_TYPE=Release -DPROJECT_FOLDER=<DEMO_FOLDER> -G Ninja .
cmake --build build --target psdk_demo.nro
```

You should then be able to load the NRO file on your Switch or emulator.