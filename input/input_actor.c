
#include "input_actor.h"

#include <data/dna_sequence.h>

#include <stdio.h>

/*
#define BSAL_INPUT_ACTOR_DEBUG
*/

struct bsal_actor_vtable bsal_input_actor_vtable = {
    .init = bsal_input_actor_init,
    .destroy = bsal_input_actor_destroy,
    .receive = bsal_input_actor_receive
};

void bsal_input_actor_init(struct bsal_actor *actor)
{
    struct bsal_input_actor *input;

    input = (struct bsal_input_actor *)bsal_actor_actor(actor);
    input->file_name = NULL;
}

void bsal_input_actor_destroy(struct bsal_actor *actor)
{
    struct bsal_input_actor *input;

    input = (struct bsal_input_actor *)bsal_actor_actor(actor);

    if (input->file_name != NULL) {
        /* free memory */
    }
}

void bsal_input_actor_receive(struct bsal_actor *actor, struct bsal_message *message)
{
    int tag;
    int source;
    int name;
    int count;
    struct bsal_input_actor *input;
    struct bsal_dna_sequence sequence;
    char *buffer;
    int i;
    int has_sequence;

    input = (struct bsal_input_actor *)bsal_actor_actor(actor);
    tag = bsal_message_tag(message);
    source = bsal_message_source(message);
    name = bsal_actor_name(actor);
    buffer = (char *)bsal_message_buffer(message);

    if (tag == BSAL_INPUT_ACTOR_OPEN) {

#ifdef BSAL_INPUT_ACTOR_DEBUG
        printf("DEBUG bsal_input_actor_receive open %s\n",
                        buffer);
#endif

        bsal_input_proxy_init(&input->proxy, buffer);
        bsal_message_set_tag(message, BSAL_INPUT_ACTOR_OPEN_OK);
        bsal_actor_send(actor, source, message);

    } else if (tag == BSAL_INPUT_ACTOR_COUNT) {
        /* count a little bit and yield the worker */

        i = 0;
        /* continue counting ... */
        has_sequence = 1;
        while (i < 1024 && has_sequence) {
            has_sequence = bsal_input_proxy_get_sequence(&input->proxy,
                            &sequence);
            i++;
        }

        if (has_sequence) {

            /*printf("DEBUG yield\n");*/
            bsal_message_set_tag(message, BSAL_INPUT_ACTOR_COUNT_YIELD);
            bsal_actor_send(actor, name, message);
        } else {
            bsal_message_set_tag(message, BSAL_INPUT_ACTOR_COUNT_READY);
            bsal_actor_send(actor, name, message);
        }

    } else if (tag == BSAL_INPUT_ACTOR_COUNT_YIELD) {
        bsal_message_set_tag(message, BSAL_INPUT_ACTOR_COUNT);
        bsal_actor_send(actor, source, message);

    } else if (tag == BSAL_INPUT_ACTOR_COUNT_READY) {

        count = bsal_input_proxy_size(&input->proxy);
        bsal_message_set_buffer(message, &count);
        bsal_message_set_count(message, sizeof(count));
        bsal_message_set_tag(message, BSAL_INPUT_ACTOR_COUNT_RESULT);
        bsal_actor_send(actor, bsal_actor_supervisor(actor), message);

    } else if (tag == BSAL_INPUT_ACTOR_CLOSE) {

#ifdef BSAL_INPUT_ACTOR_DEBUG
        printf("DEBUG destroy proxy\n");
#endif

        bsal_input_proxy_destroy(&input->proxy);
        bsal_actor_die(actor);
    }
}