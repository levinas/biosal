TEST_BUFFERED_READER_NAME=buffered_reader
TEST_BUFFERED_READER_EXECUTABLE=tests/test_$(TEST_BUFFERED_READER_NAME)
TEST_BUFFERED_READER_OBJECTS=tests/test_$(TEST_BUFFERED_READER_NAME).o
TEST_EXECUTABLES+=$(TEST_BUFFERED_READER_EXECUTABLE)
TEST_OBJECTS+=$(TEST_BUFFERED_READER_OBJECTS)
$(TEST_BUFFERED_READER_EXECUTABLE): $(LIBRARY_OBJECTS) $(TEST_BUFFERED_READER_OBJECTS) $(TEST_LIBRARY_OBJECTS)
	$(Q)$(ECHO) "  LD $@"
	$(Q)$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
TEST_BUFFERED_READER_RUN=test_run_$(TEST_BUFFERED_READER_NAME)
$(TEST_BUFFERED_READER_RUN): $(TEST_BUFFERED_READER_EXECUTABLE)
	./$^
#TEST_RUNS+=$(TEST_BUFFERED_READER_RUN)

