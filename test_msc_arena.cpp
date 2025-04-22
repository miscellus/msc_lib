
#define MSC_ARENA_IMPLEMENTATION
#include "msc_arena.h"

static msc_arena g_arena_frame;
static msc_arena g_arena_permanent;

int main(int argc, char **argv) {

    msc_arena arena = msc_arena_create(64ULL<<30ULL);
    
    double *thing = msc_arena_push(&arena, double);

    //usize of = msc_offsetof((struct { char x; msc_arena thing; })[], thing);
    //usize al = msc_alignof(msc_arena);

    return 0;
}
