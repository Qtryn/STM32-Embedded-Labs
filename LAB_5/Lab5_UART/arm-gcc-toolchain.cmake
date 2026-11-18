set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)


set(ARM_TOOLCHAIN_PATH
    "C:/Program Files/Arm/GNU Toolchain mingw-w64-x86_64-arm-none-eabi/bin"
)


set(CMAKE_C_COMPILER
    "${ARM_TOOLCHAIN_PATH}/arm-none-eabi-gcc.exe"
)


set(CMAKE_ASM_COMPILER
    "${ARM_TOOLCHAIN_PATH}/arm-none-eabi-gcc.exe"
)


set(CMAKE_OBJCOPY
    "${ARM_TOOLCHAIN_PATH}/arm-none-eabi-objcopy.exe"
)