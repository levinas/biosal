
#include "framr.h"

#include <stdio.h>

#include <core/system/command.h>

#define pmsg(L, ...) thorium_actor_log_with_level(actor, L, __VA_ARGS__)
#define pm(...) thorium_actor_log(actor, __VA_ARGS__)

script_t framr_script = {
    .identifier = SCRIPT_FRAMR,
    .init = framr_init,
    .destroy = framr_destroy,
    .receive = framr_receive,
    .size = sizeof(framr_t),
    .name = "framr",
    .author = "Fangfang Xia",
    .version = "0.0.0",
    .description = "Feature Research for Antibiotic Microbial Resistance"
};

void framr_init(actor_t *actor)
{
    framr_t *self;
    self = thorium_actor_concrete_actor(actor);
    core_vector_init(&self->spawners, sizeof(int));
    self->completed = 0;

    thorium_actor_add_action(actor, ACTION_START, framr_start);
    thorium_actor_add_action(actor, ACTION_FRAMR_HELLO, framr_hello);
    thorium_actor_add_action(actor, ACTION_FRAMR_HELLO_REPLY, framr_hello_reply);
    thorium_actor_add_action(actor, ACTION_NOTIFY, framr_notify);
    thorium_actor_add_action(actor, ACTION_ASK_TO_STOP, framr_ask_to_stop);
}

void framr_destroy(actor_t *actor)
{
    framr_t *self;
    self = thorium_actor_concrete_actor(actor);
    core_vector_destroy(&self->spawners);
}

void framr_receive(actor_t *actor, message_t *message)
{
    thorium_actor_take_action(actor, message);
}

/* Message handlers */

void framr_start(actor_t *actor, message_t *message)
{
    int name;
    int rank;
    int size;
    int neighbor_rank;
    int neighbor_name;
    void * buffer;

    framr_t *self;
    struct core_vector *spawners;

    framr_process_args(actor);

    self = thorium_actor_concrete_actor(actor);

    name = thorium_actor_name(actor);
    buffer = thorium_message_buffer(message);
    spawners = &self->spawners;
    size = core_vector_size(spawners);

    pm("received ACTION_START\n");

    core_vector_unpack(spawners, buffer);
    size = core_vector_size(spawners);
    rank = core_vector_index_of(spawners, &name);
    neighbor_rank = (rank + 1) % size;
    neighbor_name = core_vector_at_as_int(spawners, neighbor_rank);

    pm("Spawner world size = %d\n", size);

    pm("Spawner %d about to send hello to neighbor %d\n", rank, neighbor_rank);
    thorium_actor_send_empty(actor, neighbor_name, ACTION_FRAMR_HELLO);
    /* thorium_message_init(&new_message, ACTION_FRAMR_HELLO, 0, NULL); */
    /* thorium_actor_send(actor, neighbor_name, &new_message); */
    /* thorium_message_destroy(&new_message); */
}

void framr_hello(actor_t *actor, message_t *message)
{
    pm("received ACTION_FRAMR_HELLO\n");

    thorium_actor_send_reply_empty(actor, ACTION_FRAMR_HELLO_REPLY);
}

void framr_hello_reply(actor_t *actor, message_t *message)
{
    message_t new_message;
    int name;
    int source;
    int boss;

    framr_t *self;
    struct core_vector *spawners;

    self = thorium_actor_concrete_actor(actor);

    name = thorium_actor_name(actor);
    source = thorium_message_source(message);
    spawners = &self->spawners;

    pm("Actor %d is satisfied with a reply from the neighbor %d.\n", name, source);

    boss = core_vector_at_as_int(spawners, 0);
    thorium_message_init(&new_message, ACTION_NOTIFY, 0, NULL);
    thorium_actor_send(actor, boss, &new_message);
    thorium_message_destroy(&new_message);
}

void framr_notify(actor_t *actor, message_t *message)
{
    int size;

    framr_t *self;
    struct core_vector *spawners;

    self = thorium_actor_concrete_actor(actor);

    spawners = &self->spawners;
    size = core_vector_size(spawners);

    pm("Boss received NOTIFY\n");

    ++self->completed;
    if (self->completed == size) {
        thorium_actor_send_range_empty(actor, spawners, ACTION_ASK_TO_STOP);
    }
}

void framr_ask_to_stop(actor_t *actor, message_t *message)
{
    pm("received ASK_TO_STOP\n");
    thorium_actor_send_to_self_empty(actor, ACTION_STOP);
}


/* Utility routines */

void framr_process_args(actor_t *actor)
{
    int argc;
    char **argv;

    argc = thorium_actor_argc(actor);
    argv = thorium_actor_argv(actor);

    if (core_command_has_argument(argc, argv, "-v")) {
        thorium_actor_send_to_self_empty(actor, ACTION_ENABLE_LOG_LEVEL);
    }
}
