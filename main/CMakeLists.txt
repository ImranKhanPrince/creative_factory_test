set(SOURCES 
    "src/AI.c"
    "src/AO.c"
    "src/GPIO.c"
    "src/uart0.c"
    "src/uart1.c"
    "src/command_parser.c"
    "src/nvs.c"
  )

idf_component_register(SRCS "main.c" ${SOURCES}
                    INCLUDE_DIRS "." "include")
