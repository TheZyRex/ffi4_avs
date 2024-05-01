#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>	
#include <sys/types.h>
#include <unistd.h>

void init_sem(int sem_id)
{
  /* according to Faccis guide */
  union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* array;
    struct seminfo *__buf;
  } sem_arg;
  sem_arg.val = 1;

  /* second argument is which semaphore of the set is to be configured -> 0 for the first and only one */
  if (semctl(sem_id, 0, SETVAL, sem_arg) < 0)
  {
    perror("Error calling semctl()\n");
    exit(EXIT_FAILURE);
  }
}

/* V Operation */
void sem_up(int sem_id)
{
  struct sembuf semaphore;
  semaphore.sem_num = 0;
  semaphore.sem_op = 1;
  semaphore.sem_flg =~ (IPC_NOWAIT | SEM_UNDO);

  if (semop(sem_id, &semaphore, 1) < 0)
  {
    perror("Error calling semop() V Operation\n");
    exit(EXIT_FAILURE);
  }
}

/* P Operation */
void sem_down(int sem_id)
{
  struct sembuf semaphore;
  semaphore.sem_num = 0;
  semaphore.sem_op = -1;
  semaphore.sem_flg =~ (IPC_NOWAIT | SEM_UNDO);

  if (semop(sem_id, &semaphore, 1) < 0)
  {
    perror("Error calling semop() P Operation\n");
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char** argv)
{
  const int PROC_NUM = 3;


  key_t sem_key = ftok("/home/jopel/Documents/01_Coding/ffi4_avs/01/build", 1);
  if (sem_key < 0)
  {
    perror("Error calling ftok()\n");
    exit(EXIT_FAILURE);
  }

  /* second argument is the number of semaphores to be created -> nsems */
  int sem_id = semget(sem_key, 1, IPC_CREAT | 0660);
  if (sem_id < 0)
  {
    perror("Error calling semget()\n");
    exit(EXIT_FAILURE);
  }

  /* init semaphore */
  init_sem(sem_id);

  for (uint8_t i = 1; i <= PROC_NUM; ++i)
  {
    pid_t pid = fork();

    switch(pid) {
      case -1:
        perror("Failed to spawn child proc\n");
        exit(EXIT_FAILURE);

      case 0: /* Child Proc */
        for (uint8_t k = 0; k < 3; ++k)
        {
          sem_down(sem_id);
          printf("Child <%d> with pid: [%d] entering critial section\n", i, getpid());
          sleep(1);
          printf("Child <%d> with pid: [%d] leaving critical section\n\n", i, getpid());
          sem_up(sem_id);
        }
        exit(EXIT_SUCCESS);

      default:
        break;
    }
  }

  for (uint8_t i = 0; i < 3; ++i)
  {
    int status;
    pid_t pid = waitpid(-1, &status, 0);
    if (pid == -1) {
      perror("waitpid");
      exit(EXIT_FAILURE);
    }
    if (WIFEXITED(status)) {
      printf("Child %d exited with status %d\n", pid, WEXITSTATUS(status));
    } else {
      printf("Child %d terminated abnormally\n", pid);
    }
  }

  // Remove the semaphore
  if (semctl(sem_id, 0, IPC_RMID) == -1) {
    perror("Error calling semctl()");
    exit(EXIT_FAILURE);
  }

  printf("Semaphore removed\n");

  printf("Parent proc finished\n");

  return EXIT_SUCCESS;
}