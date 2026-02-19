add_executable(serdec_tests
  test_main.c
  test_buffer.c
  test_arena.c
  test_error.c
  test_lexer.c
  test_utf8.c
  test_string.c
)

target_link_libraries(serdec_tests PRIVATE serdec)

add_test(NAME serdec.buffer COMMAND serdec_tests buffer)                      
add_test(NAME serdec.arena COMMAND serdec_tests arena)
add_test(NAME serdec.error COMMAND serdec_tests error)
add_test(NAME serdec.lexer COMMAND serdec_tests lexer)
add_test(NAME serdec.utf8 COMMAND serdec_tests utf8)
add_test(NAME serdec.string COMMAND serdec_tests string)
add_test(NAME serdec.all COMMAND serdec_tests all)
