cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_C_COMPILER /usr/bin/gcc)

project(bootr)

set(EFIBIN "bootr.efi")

link_directories(/usr/local/lib /usr/lib)
include_directories(/usr/include/ /usr/include/efi)

set(COMMONFLAGS "-dgnu_efi_use_ms_abi -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -Wall")
set(LDFLAGS "-Wl,-nostdlib -Wl,-znocombreloc -Wl,-T,/usr/lib/elf_x86_64_efi.lds -Wl,-shared -Wl,-Bsymbolic,/usr/lib/crt0-efi-x86_64.o -Wl,-lefi -Wl,-lgnuefi") 

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 ${COMMONFLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=c11 ${COMMONFLAGS}")

file(GLOB_RECURSE sourcefiles
    "*.h"
    "*.c"
)

add_library(bootr SHARED ${sourcefiles})
target_link_libraries(bootr ${LDFLAGS})

add_custom_command(
    TARGET bootr POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -j .text -j .sdata -j .data -j .dynamic -j .dynsym  -j .rel -j .rela -j .reloc --target=efi-app-x86_64 libbootr.so ${EFIBIN} # efi-rtdrv for runtime driver
)
