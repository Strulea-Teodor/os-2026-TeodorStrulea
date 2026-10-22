#include "kernel/types.h"
#include "user/user.h"

// One stage of the sieve: reads numbers from fd `in`. The first number is
// a prime p; every following number that p does not divide is forwarded
// to a child stage through a new pipe.
void
stage(int in)
{
  int p, n;

  if(read(in, &p, sizeof(p)) != sizeof(p)){   // no numbers left: end of the chain
    close(in);
    return;
  }
  printf("prime %d\n", p);

  int pfd[2];
  pipe(pfd);
  if(fork() == 0){
    close(in);                                 // the child reads only from the new pipe
    close(pfd[1]);
    stage(pfd[0]);
    exit(0);
  }

  close(pfd[0]);                               // the parent writes only into the new pipe
  while(read(in, &n, sizeof(n)) == sizeof(n)){
    if(n % p != 0)
      write(pfd[1], &n, sizeof(n));
  }
  close(in);
  close(pfd[1]);                               // EOF for the child
  wait(0);
}

int
main(int argc, char *argv[])
{
  int pfd[2];
  pipe(pfd);
  if(fork() == 0){
    close(pfd[1]);
    stage(pfd[0]);
    exit(0);
  }
  close(pfd[0]);
  for(int i = 2; i <= 35; i++)
    write(pfd[1], &i, sizeof(i));
  close(pfd[1]);
  wait(0);
  exit(0);
}
