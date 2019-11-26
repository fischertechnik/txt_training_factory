# Build Instructions

## TXT Tool Chain
Download and extract the TXT Tool Chain for [Linux](https://github.com/fischertechnik/txt_training_factory/releases/download/v0.7.0/gcc-linaro-7.2.1-2017.11-x86_64_arm-linux-gnueabihf.tar.xz) or [Windows](https://github.com/fischertechnik/txt_training_factory/releases/download/v0.7.0/gcc-linaro-7.2.1-2017.11-i686-mingw32_arm-linux-gnueabihf.zip)

## Using make and Makefile
```
export TOOLCHAIN_BIN_PATH=/path_to_txt_toolchain_bin
make clean
make
```

## Using Eclipse CDT
The following diagram shows the structure of the eclipse workspace.
![SW Layer](SW_Layer.PNG "SW Layer")

### Import txt_traning_factory
Download [txt_traning_factory](https://github.com/fischertechnik/txt_training_factory/archive/master.zip), extract and import the workspace in [eclipse CDT](https://www.eclipse.org/cdt/downloads.php) as `General -> Existing Projects into Workspace`
![IDE_Setup_Import](IDE_Setup_Import.PNG "IDE Setup Import")
![IDE_Setup_Import2](IDE_Setup_Import2.PNG "IDE Setup Import2")
![IDE_Setup_Import3](IDE_Setup_Import3.PNG "IDE Setup Import3")

### Config txt_traning_factory
Change the paths in the properties for all projects in `Properties -> C/C++ Build -> Settings -> Cross Settings -> Path` to the TXT Tool Chain path. Select `[All configurations]` and repeat for all subprojects.
![IDE_Setup_Config](IDE_Setup_Config.PNG "IDE Setup Config")
![IDE_Setup_Config2](IDE_Setup_Config2.PNG "IDE Setup Config2")

### Build
Build all projects
