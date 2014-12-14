
#ifndef _FRAMR_H_
#define _FRAMR_H_

#include <biosal.h>

#define SCRIPT_FRAMR 0x74c20115

#define ACTION_FRAMR_HELLO 0x00003fa9
#define ACTION_FRAMR_HELLO_REPLY 0x00004791

typedef struct {
    struct core_vector spawners;
    int completed;
} framr_t;

extern struct thorium_script framr_script;

void framr_init(struct thorium_actor *actor);
void framr_destroy(struct thorium_actor *actor);
void framr_receive(struct thorium_actor *actor, struct thorium_message *message);

void framr_start(struct thorium_actor *actor, struct thorium_message *message);
void framr_hello(struct thorium_actor *actor, struct thorium_message *message);
void framr_hello_reply(struct thorium_actor *actor, struct thorium_message *message);
void framr_notify(struct thorium_actor *actor, struct thorium_message *message);
void framr_ask_to_stop(struct thorium_actor *actor, struct thorium_message *message);

#endif /* _FRAMR_H_ */
