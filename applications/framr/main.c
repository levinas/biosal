
#include "framr.h"

struct thorium_script framr_script;

int main(int argc, char *argv[])
{
    return biosal_thorium_engine_boot_initial_actor(&argc, &argv, SCRIPT_FRAMR, &framr_script);
}
