

#include "mock_actor.h"
#include <biosal.h>

int main(int argc, char **argv)
{
    struct mock_actor mock_actor;
    struct bsal_actor actor;
    struct bsal_node node;
    int threads;

    threads = 4;
    bsal_node_construct(&node, threads, &argc, &argv);

    bsal_actor_construct(&actor, &mock_actor, &mock_actor_vtable);
    bsal_node_spawn_actor(&node, &actor);

    bsal_node_start(&node);

    bsal_node_destruct(&node);
    bsal_actor_destruct(&actor);

    return 0;
}
