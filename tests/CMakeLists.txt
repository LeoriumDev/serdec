add_executable(serdec_tests
  test_main.c
  test_json_parser.c
  test_json_writer.c
)

target_link_libraries(serdec_tests PRIVATE serdec)

add_test(NAME serdec.subtask1 COMMAND serdec_tests subtask1)
add_test(NAME serdec.subtask2 COMMAND serdec_tests subtask2)
add_test(NAME serdec.all      COMMAND serdec_tests all)
