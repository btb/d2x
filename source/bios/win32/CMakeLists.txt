set(io_win32_SRCS
    findfile.c
    ipx.c
    joyc.c
    key.c
    mono.c
    mouse.c
    rbaudio.c
    timer.c
)

set(io_win32_OTHERS
    winmono.c
)

set_source_files_properties(${io_win32_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

target_sources(io PRIVATE ${io_win32_SRCS} ${io_win32_OTHERS})
