#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>	
#include <sys/types.h>
#include <unistd.h>

/* Number of Philosophers */
#define N 5

void init_sem(int sem_id, int nsems)
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
  for (uint8_t i = 0; i < nsems; i++) {
    if ( semctl(sem_id, i, SETVAL, 1) < 0)
    {
      perror("Error calling semctl()\n");
      exit(EXIT_FAILURE);
    }
  }
}

void clean_sem(int sem_id)
{
  // Remove the semaphore
  if (semctl(sem_id, 0, IPC_RMID) == -1) {
    perror("Error calling semctl()");
    exit(EXIT_FAILURE);
  }
}

/* V Operation */
void sem_up(int sem_id, int left, int right)
{
  struct sembuf operation[2];
  operation[0].sem_num = left;
  operation[0].sem_op = 1;
  operation[0].sem_flg =~ (IPC_NOWAIT | SEM_UNDO);
  
  operation[1].sem_num = right;
  operation[1].sem_op = 1;
  operation[1].sem_flg =~ (IPC_NOWAIT | SEM_UNDO);

  if (semop(sem_id, operation, 2) < 0)
  {
    perror("Error calling semop() V Operation\n");
    exit(EXIT_FAILURE);
  }
}

/* P Operation */
void sem_down(int sem_id, int left, int right)
{
  struct sembuf operation[2];
  operation[0].sem_num = left;
  operation[0].sem_op = -1;
  operation[0].sem_flg =~ (IPC_NOWAIT | SEM_UNDO);
  
  operation[1].sem_num = right;
  operation[1].sem_op = -1;
  operation[1].sem_flg =~ (IPC_NOWAIT | SEM_UNDO);

  if (semop(sem_id, operation, 2) < 0)
  {
    perror("Error calling semop() P Operation\n");
    exit(EXIT_FAILURE);
  }
}

void philosophers(int semid, int index)
{
  for (uint8_t i = 0; i < 5; i++) {
    printf("Philosopher %d is thinking.\n", index);
    /* Think for a random time between 1-5 */
    sleep(rand() % 5 + 1);

    /* pick up left and right fork */
    sem_down(semid, index, (index + 1) % N);
    printf("Philosopher %d is eating.\n", index);
    /* Eat for a random time between 1-5 */
    sleep(rand() % 5 + 1);
    
    printf("Philosopher %d is done eating.\n", index);
    fflush(stdout);
    /* put down left and right fork */
    sem_up(semid, index, (index + 1) % N);
  }

}

int main(int argc, char** argv)
{
  key_t sem_key = ftok("/home/jopel/Documents/01_Coding/ffi4_avs/01/build", 1);
  if (sem_key < 0)
  {
    perror("Error calling ftok()\n");
    exit(EXIT_FAILURE);
  }

  /* second argument is the number of semaphores to be created -> nsems */
  /* 5 semaphores because we got 5 forks */
  int sem_id = semget(sem_key, N, IPC_CREAT | 0660);
  if (sem_id < 0)
  {
    perror("Error calling semget()\n");
    exit(EXIT_FAILURE);
  }

  /* init semaphore */
  init_sem(sem_id, N);


  for (uint8_t i = 0; i < N; i++)
  {
    if (fork() == 0) {
      philosophers(sem_id, i);
      exit(EXIT_SUCCESS);
    }
  }

  for (uint8_t i = 0; i < N; i++)
  {
    wait(NULL);
  }

  clean_sem(sem_id);

  return EXIT_SUCCESS;
}