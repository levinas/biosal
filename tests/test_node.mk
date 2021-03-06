TEST_NODE_NAME=node
TEST_NODE_EXECUTABLE=tests/test_$(TEST_NODE_NAME)
TEST_NODE_OBJECTS=tests/test_$(TEST_NODE_NAME).o
TEST_EXECUTABLES+=$(TEST_NODE_EXECUTABLE)
TEST_OBJECTS+=$(TEST_NODE_OBJECTS)
$(TEST_NODE_EXECUTABLE): $(LIBRARY_OBJECTS) $(TEST_NODE_OBJECTS) $(TEST_LIBRARY_OBJECTS)
	$(Q)$(ECHO) "  LD $@"
	$(Q)$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(CONFIG_LDFLAGS)
TEST_NODE_RUN=test_run_$(TEST_NODE_NAME)
$(TEST_NODE_RUN): $(TEST_NODE_EXECUTABLE)
	./$^
TEST_RUNS+=$(TEST_NODE_RUN)

