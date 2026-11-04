foreach(example handoff empty self_loop)
    execute_process(COMMAND "${PROGRAM}"
        INPUT_FILE "${EXAMPLES}/${example}.txt"
        OUTPUT_VARIABLE actual ERROR_VARIABLE error RESULT_VARIABLE status)
    file(READ "${EXAMPLES}/${example}.expected.txt" expected)
    if(NOT status EQUAL 0 OR NOT actual STREQUAL expected OR NOT error STREQUAL "")
        message(FATAL_ERROR "Console example ${example} failed: ${status}: ${error}\n${actual}")
    endif()
endforeach()

foreach(input "1 A 1 A Unknown" "2 A A 0" "0 -1" "1 A 1 A" "0 0 extra")
    file(WRITE "${TEST_DIR}/invalid-input.txt" "${input}\n")
    execute_process(COMMAND "${PROGRAM}"
        INPUT_FILE "${TEST_DIR}/invalid-input.txt"
        OUTPUT_VARIABLE actual ERROR_VARIABLE error RESULT_VARIABLE status)
    if(NOT status EQUAL 1 OR NOT error MATCHES "^Error:" OR NOT actual STREQUAL "")
        message(FATAL_ERROR "Invalid console input was not correctly rejected: ${input}")
    endif()
endforeach()
file(REMOVE "${TEST_DIR}/invalid-input.txt")

execute_process(COMMAND "${PROGRAM}" --help OUTPUT_VARIABLE help RESULT_VARIABLE status)
if(NOT status EQUAL 0 OR NOT help MATCHES "Usage:")
    message(FATAL_ERROR "Console help failed")
endif()
execute_process(COMMAND "${PROGRAM}" --unknown ERROR_VARIABLE error RESULT_VARIABLE status)
if(NOT status EQUAL 1 OR NOT error MATCHES "Usage:")
    message(FATAL_ERROR "Unknown command-line argument was not rejected")
endif()
