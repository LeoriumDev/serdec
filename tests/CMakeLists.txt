add_executable(serdec_tests
  test_main.c
  test_buffer.c
  test_arena.c
  test_error.c
  test_lexer.c
)

target_link_libraries(serdec_tests PRIVATE serdec)

add_test(NAME serdec.buffer COMMAND serdec_tests buffer)                      
add_test(NAME serdec.arena COMMAND serdec_tests arena)
add_test(NAME serdec.error COMMAND serdec_tests error)
add_test(NAME serdec.lexer COMMAND serdec_tests lexer)
add_test(NAME serdec.all COMMAND serdec_tests all)
