#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
  const int PROC_NUM = 3;


  for (uint8_t i = 1; i <= PROC_NUM; ++i) {
    pid_t pid = fork();
    switch (pid)
    {
    case 0: /* Child Proc */
      printf("Spanwed Child %d with pid: [%d]\n", i, getpid());

      for (uint8_t k = 0; k < 3; ++k) {
        printf("child <%d> entered critical area\n", i);
        sleep(1);
        printf("child <%d> left critical area\n", i);
      }

      if (i == 1)
      {
        printf("child <%d> entered noncritical area\n", i);
        sleep(1);
        printf("child <%d> left noncritical area\n", i);
      }
      else if (i == 2)
      {
        printf("child <%d> entered noncritical area\n", i);
        sleep(1);
        printf("child <%d> left noncritical area\n", i);

      }
      else if (i == 3) 
      {
        printf("child <%d> entered noncritical area\n", i);
        sleep(1);
        printf("child <%d> left noncritical area\n", i);
      }

      exit(0);
    
    case -1:
      perror("Error spawning child\n");
      exit(EXIT_FAILURE);
    
    default: /* Main Proc */
      ;
    }
  }

  printf("Parent proc stopped\n");

  return EXIT_SUCCESS;
}