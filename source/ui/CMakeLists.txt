#
# Makefile for user-interface functions
#

set(ui_SRCS
    ui.c mouse.c window.c uidraw.c button.c
    gadget.c listbox.c message.c popup.c scroll.c
    radio.c checkbox.c inputbox.c userbox.c menu.c
    harderr.c file.c keytrap.c icon.c keypress.c
    menubar.c keypad.c func.c
)

set(ui_OTHERS
    barbox.c
    func.h
    makefile
    medfunc.c
    number.c
    ui.h
)
set_source_files_properties(${ui_OTHERS} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(ui ${ui_SRCS} ${ui_OTHERS})

add_executable(testg testg.c)
target_link_libraries(testg io gr ui misc mem readfile iff func cflib cfile
#TESTG_LFLAGS = option stack=50k
