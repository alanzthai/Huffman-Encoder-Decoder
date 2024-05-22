#include <criterion/criterion.h>
#include <criterion/logging.h>
#include "global.h"

Test(basecode_tests_suite, validargs_help_test) {
    int argc = 2;
    char *argv[] = {"bin/huff", "-h", NULL};
    int ret = validargs(argc, argv);
    int exp_ret = 0;
    int opt = global_options;
    int flag = 0x1;
    cr_assert_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d",
		 ret, exp_ret);
    cr_assert_eq(opt & flag, flag, "Correct bit (0x1) not set for -h. Got: %x", opt);
}

Test(basecode_tests_suite, validargs_compress_test) {
    int argc = 4;
    char *argv[] = {"bin/huff", "-c", "-b", "1024", NULL};
    int ret = validargs(argc, argv);
    int exp_ret = 0;
    int opt = global_options;
    int flag = 0x2;
    int exp_size = 1024;
    int size = ((opt >> 16) & 0xffff) + 1;
    cr_assert_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d",
		 ret, exp_ret);
    cr_assert(opt & flag, "Compress mode bit wasn't set. Got: %x", opt);
    cr_assert_eq(exp_size, size, "Block size not properly set. Got: %d | Expected: %d",
		 exp_size, size);
}

Test(basecode_tests_suite, validargs_error_test) {
    int argc = 4;
    char *argv[] = {"bin/huff", "-d", "-b", "1024", NULL};
    int ret = validargs(argc, argv);
    int exp_ret = -1;
    cr_assert_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d",
		 ret, exp_ret);
}

Test(basecode_tests_suite, help_system_test) {
    char *cmd = "bin/huff -h";
    int return_code = WEXITSTATUS(system(cmd));

    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program exited with %d instead of EXIT_SUCCESS",
		 return_code);
}

// Adding more tests for valid args
Test(basecode_tests_suite, validargs_compress_test2) {
    int argc = 4;
    char *argv[] = {"bin/huff", "-c", "-b", "5555", NULL};
    int ret = validargs(argc, argv);
    int exp_ret = 0;
    int opt = global_options;
    int flag = 0x2;
    int exp_size = 5555;
    int size = ((opt >> 16) & 0xffff) + 1;
    cr_assert_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d",
         ret, exp_ret);
    cr_assert(opt & flag, "Compress mode bit wasn't set. Got: %x", opt);
    cr_assert_eq(exp_size, size, "Block size not properly set. Got: %d | Expected: %d",
         exp_size, size);
}

Test(basecode_tests_suite, validargs_compress_test3) {
    int argc = 2;
    char *argv[] = {"bin/huff", "-d", NULL};
    int ret = validargs(argc, argv);
    int exp_ret = 0;
    int opt = global_options;
    int flag = 0x4;
    int exp_size = 65536;
    int size = ((opt >> 16) & 0xffff) + 1;
    cr_assert_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d",
         ret, exp_ret);
    cr_assert(opt & flag, "Compress mode bit wasn't set. Got: %x", opt);
    cr_assert_eq(exp_size, size, "Block size not properly set. Got: %d | Expected: %d",
         exp_size, size);
}

Test(basecode_tests_suite, validargs_error_test2) {
    int argc = 4;
    char *argv[] = {"bin/huff", "-c", "-b", "1k", NULL};
    int ret = validargs(argc, argv);
    int exp_ret = -1;
    cr_assert_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d",
         ret, exp_ret);
}

Test(basecode_tests_suite, validargs_error_test3) {
    int argc = 4;
    char *argv[] = {"bin/huff", "-d", "BLAH", "BLAH", NULL};
    int ret = validargs(argc, argv);
    int exp_ret = -1;
    cr_assert_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d",
         ret, exp_ret);
}

Test(basecode_tests_suite, validargs_help_test2) {
    int argc = 4;
    char *argv[] = {"bin/huff", "-h", "BLAH", "BLAH", NULL};
    int ret = validargs(argc, argv);
    int exp_ret = 0;
    int opt = global_options;
    int flag = 0x1;
    cr_assert_eq(ret, exp_ret, "Invalid return for valid args.  Got: %d | Expected: %d",
         ret, exp_ret);
    cr_assert_eq(opt & flag, flag, "Correct bit (0x1) not set for -h. Got: %x", opt);
}