# Versão mínima do CMake necessária
cmake_minimum_required(VERSION 3.12)

# Nome do projeto
project(TemperatureMonitor C CXX ASM)

# Inclui as configurações padrão do Raspberry Pi Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Inicializa o SDK do Raspberry Pi Pico
pico_sdk_init()

# Adiciona o executável do projeto
add_executable(TemperatureMonitor
    main.c
)

# Vincula o executável ao SDK do Raspberry Pi Pico
target_link_libraries(TemperatureMonitor pico_stdlib)

# Habilita a saída de depuração (USB/UART)
pico_enable_stdio_usb(TemperatureMonitor 1)
pico_enable_stdio_uart(TemperatureMonitor 1)

# Adiciona bibliotecas específicas para Wi-Fi (se necessário)
target_link_libraries(TemperatureMonitor hardware_pwm)
target_link_libraries(TemperatureMonitor hardware_adc)

# Configurações adicionais para o Pico W (Wi-Fi)
if (PICO_BOARD STREQUAL "pico_w")
    target_link_libraries(TemperatureMonitor pico_cyw43_arch_none)
    target_compile_definitions(TemperatureMonitor PUBLIC PICO_CYW43_ARCH_POLL=1)
endif()

# Configurações de otimização e depuração
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g")
