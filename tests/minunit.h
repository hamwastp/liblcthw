#undef NDEBUG
#ifndef _minunit_h
#define _minunit_h
#include <lcthw/dbg.h>
#include <stdio.h>

#include <stdlib.h>

#define mu_suite_start() char* message = NULL

#define mu_assert(test, message)          \
    if (!(test)) {                        \
        /* __ADD__ */                     \
        fprintf(stderr,                   \
            "\033[41;1m[FAIL]\033[0m");   \
        /* __END__ */                     \
        log_err(message);                 \
    } /* __ADD__ */                       \
    else {                                \
        fprintf(stderr,                   \
            "\033[32;1m[PASS]\n\033[0m"); \
    }
/* __END__ */

#define mu_run_test(test)          \
    debug("\n-----%s", " " #test); \
    message = test();              \
    tests_run++;                   \
    if (message)                   \
        return message;

// __ADD__
#define mu_run_test_ex(test, casename)     \
    debug("\n-----testcase:%s", casename); \
    test();                                \
    tests_run++;
// __END__

#define RUN_TESTS(name)                         \
    int main(int argc, char* argv[])            \
    {                                           \
        debug("----- RUNNING: %s", argv[0]);    \
        printf("----\nRUNNING: %s\n", argv[0]); \
        char* result = name();                  \
        if (result != 0) {                      \
            printf("FAILED: %s\n", result);     \
        } else {                                \
            printf("ALL TESTS PASSED\n");       \
        }                                       \
        printf("Tests run: %d\n", tests_run);   \
        exit(result != 0);                      \
    }

int tests_run;

// __ADD__
#define mu_run_all_test()                                           \
    MU_TestInfo* p_mu_alltest = mu_alltest_list;                    \
    while (p_mu_alltest != NULL) {                                  \
        mu_run_test_ex(p_mu_alltest->testcase, p_mu_alltest->name); \
        p_mu_alltest = p_mu_alltest->next;                          \
    }

#define RUN_TESTS_EX()     \
    char* all_tests()      \
    {                      \
        mu_suite_start();  \
        mu_run_all_test(); \
        return NULL;       \
    }                      \
                           \
    RUN_TESTS(all_tests);
// __END__

// __ADD__
typedef char* (*testcase_func)(void);

typedef struct _MU_TestInfo {
    testcase_func testcase;
    const char* name;
    struct _MU_TestInfo* next;
} MU_TestInfo;

MU_TestInfo* mu_alltest_list = NULL;
MU_TestInfo* mu_alltest = NULL;

void register_testcase(testcase_func f, const char* name);

#define MU_TEST(name)                                         \
    char* mutest_##name();                                    \
    __attribute((constructor)) void register_testcase##name() \
    {                                                         \
        register_testcase(mutest_##name, "mutest_" #name ""); \
        return;                                               \
    };                                                        \
    char* mutest_##name()

void register_testcase(testcase_func f, const char* name)
{
    if (mu_alltest == NULL) {
        mu_alltest = (MU_TestInfo*)malloc(sizeof(MU_TestInfo*));
        if (mu_alltest == NULL) {
            return;
        }

        mu_alltest_list = mu_alltest;
    } else {

        mu_alltest->next = (MU_TestInfo*)malloc(sizeof(MU_TestInfo*));
        if (mu_alltest->next == NULL) {
            return;
        }

        mu_alltest = mu_alltest->next;
    }

    printf("register_testcase success:%s\n", name);

    mu_alltest->testcase = f;
    mu_alltest->name = name;
    mu_alltest->next = NULL;

    return;
}
// __END__

// __ADD__
__attribute((destructor)) void mutestcases_destroy()
{
    MU_TestInfo* p_mu_alltest = mu_alltest_list;
    while (p_mu_alltest != NULL) {
        free(p_mu_alltest);
        p_mu_alltest = p_mu_alltest->next;
    }
    return;
};
    // __END__

#endif
