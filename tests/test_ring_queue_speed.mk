TEST_RING_QUEUE_SPEED_NAME=ring_queue_speed
TEST_RING_QUEUE_SPEED_EXECUTABLE=tests/test_$(TEST_RING_QUEUE_SPEED_NAME)
TEST_RING_QUEUE_SPEED_OBJECTS=tests/test_$(TEST_RING_QUEUE_SPEED_NAME).o
TEST_EXECUTABLES+=$(TEST_RING_QUEUE_SPEED_EXECUTABLE)
TEST_OBJECTS+=$(TEST_RING_QUEUE_SPEED_OBJECTS)
$(TEST_RING_QUEUE_SPEED_EXECUTABLE): $(LIBRARY_OBJECTS) $(TEST_RING_QUEUE_SPEED_OBJECTS) $(TEST_LIBRARY_OBJECTS)
	$(Q)$(ECHO) "  LD $@"
	$(Q)$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(CONFIG_LDFLAGS)
TEST_RING_QUEUE_SPEED_RUN=test_run_$(TEST_RING_QUEUE_SPEED_NAME)
$(TEST_RING_QUEUE_SPEED_RUN): $(TEST_RING_QUEUE_SPEED_EXECUTABLE)
	./$^
#TEST_RUNS+=$(TEST_RING_QUEUE_SPEED_RUN)

