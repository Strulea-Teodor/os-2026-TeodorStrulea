// trytest.c: shows both answers of sem_trydown (optional part of Lab 13).
#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  sem_init(4, 1);                       // one unit available
  printf("first  trydown -> %d  (took the unit)\n", sem_trydown(4));
  printf("second trydown -> %d  (nothing left, did not sleep)\n", sem_trydown(4));
  sem_up(4);                            // give it back
  printf("after sem_up, trydown -> %d\n", sem_trydown(4));
  exit(0);
}
