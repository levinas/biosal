
#include "target.h"

#include <stdio.h>

static void target_init(struct thorium_actor *self);
static void target_destroy(struct thorium_actor *self);
static void target_receive(struct thorium_actor *self, struct thorium_message *message);

struct thorium_script script_target = {
    .identifier = SCRIPT_LATENCY_TARGET,
    .init = target_init,
    .destroy = target_destroy,
    .receive = target_receive,
    .size = sizeof(struct target),
    .name = "target"
};

static void target_init(struct thorium_actor *self)
{
    struct target *concrete_self;

    concrete_self = (struct target *)thorium_actor_concrete_actor(self);
    concrete_self->received = 0;
}

static void target_destroy(struct thorium_actor *self)
{
    struct target *concrete_self;

    concrete_self = (struct target *)thorium_actor_concrete_actor(self);
    concrete_self->received = 0;
}

static void target_receive(struct thorium_actor *self, struct thorium_message *message)
{
    int action;
    void *buffer;
    struct target *concrete_self;
    int source;
    int count;
    int name;

    concrete_self = (struct target *)thorium_actor_concrete_actor(self);
    action = thorium_message_action(message);
    buffer = thorium_message_buffer(message);
    source = thorium_message_source(message);
    count = thorium_message_count(message);
    name = thorium_actor_name(self);

    if (action == ACTION_ASK_TO_STOP) {

        printf("target %d received %d ACTION_PING messages\n",
                        name, concrete_self->received);

        thorium_actor_send_to_self_empty(self, ACTION_STOP);

    } else if (action == ACTION_PING) {

        if (concrete_self->received == 0) {
            thorium_actor_send_to_self_empty(self, ACTION_ENABLE_MULTIPLEXER);
        }
        ++concrete_self->received;

#ifdef CORE_DEBUGGER_ASSERT_ENABLED
        if (count != 0) {
            printf("Error, count is %d but should be %d.\n",
                            count, 0);

            thorium_message_print(message);
        }
#endif

        CORE_DEBUGGER_ASSERT_IS_EQUAL_INT(count, 0);
        CORE_DEBUGGER_ASSERT_IS_NULL(buffer);

        thorium_actor_send_reply_empty(self, ACTION_PING_REPLY);

    }
}
