set(io_dos_SRCS
    dos/dpmi.c
    dos/findfile.c
    dos/ipx.c
    dos/joy.asm         dos/joyc.c
    dos/key.c
    dos/mono.c
    dos/mouse.c
    dos/rbaudio.c
    dos/timer.asm
)

set(io_dos_OTHERS
    dos/key.asm         dos/keys.inc        dos/oldkey.asm
    dos/mouse.asm
    dos/rbaudio.new
)

set_source_files_properties(${io_dos_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

target_sources(io PRIVATE ${io_dos_SRCS} ${io_dos_OTHERS})
