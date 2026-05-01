#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <libitute164.h>

#define DEFAULT_ITERATIONS 100000

struct bench_case {
    const char *name;
    const char *value;
    itu_t_e164_context_t context;
};

static uint64_t monotonic_ns(void)
{
    struct timespec ts;

    if(clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }

    return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

static unsigned long parse_iterations(int argc, char **argv)
{
    char *end;
    unsigned long iterations;

    if(argc < 2)
        return DEFAULT_ITERATIONS;

    errno = 0;
    iterations = strtoul(argv[1], &end, 10);
    if(errno != 0 || *end != 0 || iterations == 0) {
        fprintf(stderr, "usage: %s [iterations]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    return iterations;
}

static void load_plan(void)
{
    if(itu_t_e164_load_default_plan() == 0)
        return;

    if(itu_t_e164_load_plan_file("data/e164-plan.txt") == 0)
        return;

    if(itu_t_e164_load_plan_file("../data/e164-plan.txt") == 0)
        return;

    fprintf(stderr, "could not load numbering plan");
    if(itu_t_e164_plan_error() != NULL)
        fprintf(stderr, ": %s", itu_t_e164_plan_error());
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}

static uint64_t run_case(const struct bench_case *bench, unsigned long iterations, unsigned long *accepted)
{
    unsigned long i;
    uint64_t start;
    uint64_t elapsed;
    volatile unsigned long checksum = 0;
    itu_t_e164_t e164;

    *accepted = 0;
    itu_t_e164_init(&e164);
    itu_t_e164_set_context(&e164, &bench->context);

    start = monotonic_ns();
    for(i = 0; i < iterations; i++) {
        itu_t_e164_set_value(&e164, bench->value);

        checksum += (unsigned long)e164.pos;
        checksum += (unsigned long)e164.cc.value;
        checksum += (unsigned long)e164.raw_phone;
        if(e164.pos > 0 && e164.cc.type == ITU_T_NUMBER)
            (*accepted)++;
    }
    elapsed = monotonic_ns() - start;

    if(checksum == 0)
        fprintf(stderr, "checksum: %lu\n", checksum);

    return elapsed;
}

int main(int argc, char **argv)
{
    static const struct bench_case cases[] = {
        {"br-international", "+5519912345678", {0, 0, ITU_T_E164_CONTEXT_RESTRICT_NONE, 0}},
        {"br-local-context", "912345678", {55, 19, ITU_T_E164_CONTEXT_RESTRICT_NONE, 0}},
        {"br-area-restricted", "912345678", {55, 19, ITU_T_E164_CONTEXT_RESTRICT_AREA, 0}},
        {"br-rejected-area", "+5511912345678", {55, 19, ITU_T_E164_CONTEXT_RESTRICT_AREA, 0}},
        {"us-international", "+12010001234", {0, 0, ITU_T_E164_CONTEXT_RESTRICT_NONE, 0}},
        {"alpha-context", "9FLOWERS", {55, 19, ITU_T_E164_CONTEXT_RESTRICT_AREA, 1}},
        {"uy-international", "+59812345678", {0, 0, ITU_T_E164_CONTEXT_RESTRICT_NONE, 0}},
        {"invalid-country", "+999123456789", {0, 0, ITU_T_E164_CONTEXT_RESTRICT_NONE, 0}},
    };
    unsigned long iterations;
    size_t i;
    uint64_t total_elapsed = 0;
    unsigned long total_ops = 0;
    unsigned long total_accepted = 0;

    iterations = parse_iterations(argc, argv);
    load_plan();

    printf("iterations_per_case=%lu\n", iterations);
    printf("%-20s %12s %14s %12s\n", "case", "ops", "ns/op", "accepted");
    for(i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        unsigned long accepted;
        uint64_t elapsed = run_case(&cases[i], iterations, &accepted);
        double ns_per_op = (double)elapsed / (double)iterations;

        total_elapsed += elapsed;
        total_ops += iterations;
        total_accepted += accepted;
        printf("%-20s %12lu %14.1f %12lu\n", cases[i].name, iterations, ns_per_op, accepted);
    }

    printf("%-20s %12lu %14.1f %12lu\n",
           "total",
           total_ops,
           (double)total_elapsed / (double)total_ops,
           total_accepted);

    return 0;
}
