#
# Makefile for user-interface functions
#

set(ui_SRCS
    button.c
    checkbox.c
    file.c              func.c
    gadget.c
    harderr.c
    icon.c              inputbox.c
    keypad.c            keypress.c          keytrap.c
    listbox.c
    menu.c              menubar.c           message.c           mouse.c
    popup.c
    radio.c
    scroll.c
    ui.c                uidraw.c            userbox.c
    window.c
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

#add_executable(testg testg.c)
#target_link_libraries(testg io gr ui misc mem readfile iff func cflib cfile)
#TESTG_LFLAGS = option stack=50k
