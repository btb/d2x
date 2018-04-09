set(io_win32_SRCS
    win32/findfile.c
    win32/ipx.c
    win32/joyc.c
    win32/key.c
    win32/mono.c
    win32/mouse.c
    win32/rbaudio.c
    win32/timer.c
)

set(io_win32_OTHERS
    win32/winmono.c
)

set_source_files_properties(${io_win32_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

target_sources(io PRIVATE ${io_win32_SRCS} ${io_win32_OTHERS})
