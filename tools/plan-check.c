#include <stdio.h>

#include <libitute164.h>

int main(int argc, char **argv)
{
    if(argc != 2) {
        fprintf(stderr, "usage: %s <e164-plan.txt>\n", argv[0]);
        return 2;
    }

    if(itu_t_e164_load_plan_file(argv[1]) != 0) {
        const char *error = itu_t_e164_plan_error();
        fprintf(stderr, "%s: invalid numbering plan", argv[1]);
        if(error != NULL)
            fprintf(stderr, ": %s", error);
        fprintf(stderr, "\n");
        return 1;
    }

    return 0;
}
