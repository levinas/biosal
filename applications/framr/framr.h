
#ifndef _FRAMR_H_
#define _FRAMR_H_

#include <biosal.h>

#define SCRIPT_FRAMR 0x74c20115

#define ACTION_FRAMR_HELLO 0x00003fa9
#define ACTION_FRAMR_HELLO_REPLY 0x00004791

typedef struct thorium_actor actor_t;
typedef struct thorium_message message_t;
typedef struct thorium_script script_t;

typedef struct {
    struct core_vector spawners;
    int completed;
} framr_t;

extern script_t framr_script;

void framr_init(actor_t *actor);
void framr_destroy(actor_t *actor);
void framr_receive(actor_t *actor, message_t *message);

/* Message handlers */

void framr_start(actor_t *actor, message_t *message);
void framr_hello(actor_t *actor, message_t *message);
void framr_hello_reply(actor_t *actor, message_t *message);
void framr_notify(actor_t *actor, message_t *message);
void framr_ask_to_stop(actor_t *actor, message_t *message);

/* Utility routines */

void framr_process_args(actor_t *actor);

#endif /* _FRAMR_H_ */
