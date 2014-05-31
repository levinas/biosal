
#include "worker.h"

#include "work.h"
#include "message.h"
#include "node.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*#define BSAL_THREAD_DEBUG*/

void bsal_worker_init(struct bsal_worker *worker, int name, struct bsal_node *node)
{
    bsal_fifo_init(&worker->works, 16, sizeof(struct bsal_work));
    bsal_fifo_init(&worker->messages, 16, sizeof(struct bsal_message));

    worker->node = node;
    worker->name = name;
    worker->dead = 0;

#ifdef BSAL_THREAD_USE_LOCK
    pthread_spin_init(&worker->work_lock, 0);
    pthread_spin_init(&worker->message_lock, 0);
#endif
}

void bsal_worker_destroy(struct bsal_worker *worker)
{
#ifdef BSAL_THREAD_USE_LOCK
    pthread_spin_destroy(&worker->message_lock);
    pthread_spin_destroy(&worker->work_lock);
#endif

    worker->node = NULL;

    bsal_fifo_destroy(&worker->messages);
    bsal_fifo_destroy(&worker->works);
    worker->name = -1;
    worker->dead = 1;
}

struct bsal_fifo *bsal_worker_works(struct bsal_worker *worker)
{
    return &worker->works;
}

struct bsal_fifo *bsal_worker_messages(struct bsal_worker *worker)
{
    return &worker->messages;
}

void bsal_worker_run(struct bsal_worker *worker)
{
    struct bsal_work work;

    /* check for messages in inbound FIFO */
    if (bsal_worker_pull_work(worker, &work)) {

        /* dispatch message to a worker */
        bsal_worker_work(worker, &work);
    }
}

void bsal_worker_work(struct bsal_worker *worker, struct bsal_work *work)
{
    struct bsal_actor *actor;
    struct bsal_message *message;
    bsal_actor_receive_fn_t receive;
    int dead;
    char *buffer;

    actor = bsal_work_actor(work);

    /* lock the actor to prevent another worker from making work
     * on the same actor at the same time
     */
    bsal_actor_lock(actor);

    bsal_actor_set_worker(actor, worker);
    message = bsal_work_message(work);

    bsal_actor_increase_received_messages(actor);
    receive = bsal_actor_get_receive(actor);

    /* Store the buffer location before calling the user
     * code because the user may change the message buffer
     * pointer. We need to free the buffer regardless if the
     * actor code changes it.
     */
    buffer = bsal_message_buffer(message);

    /* call the actor receive code
     */
    receive(actor, message);

    bsal_actor_set_worker(actor, NULL);
    dead = bsal_actor_dead(actor);

    if (dead) {
        bsal_node_notify_death(worker->node, actor);
    }

    /* Unlock the actor.
     * This does not do anything if a death notification
     * was sent to the node
     */
    bsal_actor_unlock(actor);

#ifdef BSAL_THREAD_DEBUG
    printf("bsal_worker_work Freeing buffer %p %i tag %i\n",
                    buffer, bsal_message_count(message),
                    bsal_message_tag(message));
#endif

    /* TODO free the buffer with the slab allocator */
    free(buffer);

    /* TODO replace with slab allocator */
    free(message);
}

struct bsal_node *bsal_worker_node(struct bsal_worker *worker)
{
    return worker->node;
}

void bsal_worker_send(struct bsal_worker *worker, struct bsal_message *message)
{
    struct bsal_message copy;
    char *buffer;
    int count;
    int metadata_size;
    int all;

    memcpy(&copy, message, sizeof(struct bsal_message));
    count = bsal_message_count(&copy);
    metadata_size = bsal_message_metadata_size(message);
    all = count + metadata_size;

    /* TODO use slab allocator to allocate buffer... */
    buffer = (char *)malloc(all * sizeof(char));

#ifdef BSAL_THREAD_DEBUG
    printf("[bsal_worker_send] allocated %i bytes (%i + %i) for buffer %p\n",
                    all, count, metadata_size, buffer);

    printf("bsal_worker_send old buffer: %p\n",
                    bsal_message_buffer(message));
#endif

    /* according to
     * http://stackoverflow.com/questions/3751797/can-i-call-memcpy-and-memmove-with-number-of-bytes-set-to-zero
     * memcpy works with a count of 0, but the addresses must be valid
     * nonetheless
     *
     * Copy the message data.
     */
    if (count > 0) {
        memcpy(buffer, bsal_message_buffer(message), count);
    }

    bsal_message_set_buffer(&copy, buffer);
    bsal_message_write_metadata(&copy);

    bsal_worker_push_message(worker, &copy);
}

void bsal_worker_start(struct bsal_worker *worker)
{
    /*
     * http://pubs.opengroup.org/onlinepubs/7908799/xsh/pthread_create.html
     */

    pthread_create(bsal_worker_thread(worker), NULL, bsal_worker_main,
                    worker);
}

void *bsal_worker_main(void *pointer)
{
    struct bsal_worker *worker;

    worker = (struct bsal_worker*)pointer;

#ifdef BSAL_THREAD_DEBUG
    bsal_worker_display(worker);
    printf("Starting worker thread\n");
#endif

    while (!worker->dead) {

        bsal_worker_run(worker);
    }

    return NULL;
}

void bsal_worker_display(struct bsal_worker *worker)
{
    printf("[bsal_worker_main] node %i worker %i\n",
                    bsal_node_name(worker->node),
                    bsal_worker_name(worker));
}

int bsal_worker_name(struct bsal_worker *worker)
{
    return worker->name;
}

void bsal_worker_stop(struct bsal_worker *worker)
{
#ifdef BSAL_THREAD_DEBUG
    bsal_worker_display(worker);
    printf("stopping thread!\n");
#endif

    /*
     * worker->dead is volatile and will be read
     * by the running thread.
     */
    worker->dead = 1;

    /* http://man7.org/linux/man-pages/man3/pthread_join.3.html
     */
    pthread_join(worker->thread, NULL);
}

pthread_t *bsal_worker_thread(struct bsal_worker *worker)
{
    return &worker->thread;
}

void bsal_worker_push_work(struct bsal_worker *worker, struct bsal_work *work)
{
#ifdef BSAL_THREAD_USE_LOCK
    pthread_spin_lock(&worker->work_lock);
#endif

    bsal_fifo_push(bsal_worker_works(worker), work);

#ifdef BSAL_THREAD_USE_LOCK
    pthread_spin_unlock(&worker->work_lock);
#endif
}

int bsal_worker_pull_work(struct bsal_worker *worker, struct bsal_work *work)
{
    int value;

    /* avoid the spinlock by checking first if
     * there is something to actually pull...
     */
    if (bsal_fifo_empty(bsal_worker_works(worker))) {
        return 0;
    }

#ifdef BSAL_THREAD_USE_LOCK
    pthread_spin_lock(&worker->work_lock);
#endif

    value = bsal_fifo_pop(bsal_worker_works(worker), work);

#ifdef BSAL_THREAD_USE_LOCK
    pthread_spin_unlock(&worker->work_lock);
#endif

    return value;
}

void bsal_worker_push_message(struct bsal_worker *worker, struct bsal_message *message)
{
#ifdef BSAL_THREAD_USE_LOCK
    pthread_spin_lock(&worker->message_lock);
#endif

#ifdef BSAL_THREAD_DEBUG
    printf("bsal_worker_push_message message %p buffer %p\n",
                    (void *)message, bsal_message_buffer(message));
#endif

    bsal_fifo_push(bsal_worker_messages(worker), message);

#ifdef BSAL_THREAD_USE_LOCK
    pthread_spin_unlock(&worker->message_lock);
#endif
}

int bsal_worker_pull_message(struct bsal_worker *worker, struct bsal_message *message)
{
    int value;

    /* avoid the spinlock by checking first if
     * there is something to actually pull...
     */
    if (bsal_fifo_empty(bsal_worker_messages(worker))) {
        return 0;
    }

#ifdef BSAL_THREAD_USE_LOCK
    pthread_spin_lock(&worker->message_lock);
#endif

    value = bsal_fifo_pop(bsal_worker_messages(worker), message);

#ifdef BSAL_THREAD_USE_LOCK
    pthread_spin_unlock(&worker->message_lock);
#endif

    return value;
}