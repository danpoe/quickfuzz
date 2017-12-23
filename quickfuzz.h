// Manual input and schedule fuzzing/steering

// qf_*: macros for public use
// qfi_*: internal macros/functions

// Exit codes:
// 134: normal assertion failure
// 135: qf user assertion failure
// 136: qf precondition failure

// Function name suffixes:
// <empty>: execute always
// _k: execute for matching symbolic thread name
// _c: execute if cond is true
// _kc: execute if both k and c (k is checked first, and c only if k holds)

#ifndef __QUICKFUZZ_H__
#define __QUICKFUZZ_H__

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include <pthread.h>

// -----------------------------------------------------------------------------
// Setup

#ifdef QUICKFUZZ
#define qfi_w(x) x
#else
#define qfi_w(x)
#endif

#define qfi_key_wrap(key, func) { if (qfi_chk_sym(#key)) { func; } }
#define qfi_con_wrap(con, func) { if (con) { func; } }
#define qfi_key_con_wrap(key, con, func) qfi_key_wrap(key, qfi_con_wrap(con,\
                                           func))

#define qfi_generic_chk(name, ret, con) { if (!(con)) { fprintf(stderr,\
  "%s:%d: %s: "#name" `"#con"' failed.\n", __FILE__, __LINE__, __func__);\
  exit(ret); } }

// -----------------------------------------------------------------------------
// User macros

// Init
#define qf_init(t) qfi_w(qfi_init(t))

// Create symbolic name for thread ID
#define qf_prepare_map()             qfi_w(qfi_prepare_map())
#define qf_prepare_map_c(con)        qfi_w(qfi_con_wrap(con, qfi_prepare_map()))
#define qf_make_map(key, tid)        qfi_w(qfi_make_map(#key, tid))
#define qf_make_map_c(con, key, tid) qfi_w(qfi_con_wrap(con, qfi_make_map(#key,\
                                       tid)))
#define qf_clear_map(tid)        qfi_w(qfi_clear_map(tid))
#define qf_clear_map_c(con, tid) qfi_w(qfi_con_wrap(con, qfi_clear_map(tid)))
#define qf_let_map()             qfi_w(qfi_let_map())

// Printing
#define qf_print(str)              qfi_w(qfi_print(str))
#define qf_print_k(key, str)       qfi_w(qfi_key_wrap(key, qfi_print(str)))
#define qf_print_c(con, str)       qfi_w(qfi_con_wrap(con, qfi_print(str)))
#define qf_print_kc(key, con, str) qfi_w(qfi_key_con_wrap(key, con,\
                                     qfi_print(str)))

// User assertions (to check for bugs)
#define qf_assert(user_con)              qfi_w(qfi_generic_chk(qf_assertion,\
                                           134, user_con))
#define qf_assert_k(key, user_con)       qfi_w(qfi_key_wrap(key,\
                                           qf_assert(user_con))
#define qf_assert_c(con, user_con)       qfi_w(qfi_con_wrap(con,\
                                           qf_assert(user_con))
#define qf_assert_kc(key, con, user_con) qfi_w(qfi_key_con_wrap(key, con,\
                                           qf_assert(user_con)))

// Precondition checks (to check preconditions of test)
#define qf_precon(precon)              qfi_w(qfi_generic_chk(qf_precondition,\
                                         134, precon))
#define qf_precon_k(key, precon)       qfi_w(qfi_key_wrap(key,\
                                         qf_precon(precon))
#define qf_precon_c(con, precon)       qfi_w(qfi_con_wrap(con,\
                                         qf_precon(precon))
#define qf_precon_kc(key, con, precon) qfi_w(qfi_key_con_wrap(key, con,\
                                         qf_precon(precon)))

// Wait/signal (for schedule steering)
#define qf_wait(sym)              qfi_w(qfi_wait(#sym))
#define qf_wait_k(key, sym)       qfi_w(qfi_key_wrap(key, qfi_wait(#sym)))
#define qf_wait_c(con, sym)       qfi_w(qfi_con_wrap(con, qfi_wait(#sym)))
#define qf_wait_kc(key, con, sym) qfi_w(qfi_key_con_wrap(key, con,\
                                    qfi_wait(#sym)))

#define qf_signal(sym)              qfi_w(qfi_signal(#sym))
#define qf_signal_k(key, sym)       qfi_w(qfi_key_wrap(key, qfi_signal(#sym)))
#define qf_signal_c(con, sym)       qfi_w(qfi_con_wrap(con, qfi_signal(#sym)))
#define qf_signal_kc(key, con, sym) qfi_w(qfi_key_con_wrap(key, con,\
                                    qfi_signal(#sym)))

// Code blocks
#define qf_code(code)              qfi_w({code;}) 
#define qf_code_k(key, code)       qfi_w(qfi_key_wrap(key, qf_code(code)))
#define qf_code_c(con, code)       qfi_w(qfi_con_wrap(con, qf_code(code)))
#define qf_code_kc(key, con, code) qfi_w(qfi_key_con_wrap(key, con,\
                                     qf_code(code)))

// -----------------------------------------------------------------------------
// Internal

void qfi_init(int t);
int qfi_chk_sym(char *key);

void qfi_prepare_map();
void qfi_make_map(char *key, pthread_t tid);
void qfi_clear_map(pthread_t tid);
void qfi_let_map();

void qfi_print(char *str);

void qfi_wait(char *sym);
void qfi_signal(char *sym);

#endif // __QUICKFUZZ_H__

