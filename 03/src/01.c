#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>	
#include <sys/types.h>
#include <unistd.h>

const int READER = 5;
const int WRITER = 2;
const int READER_I = 0;
const int WRITER_I = 1;
const int SHARED_I = 2;

void init_sem(int sem_id, int index, int value)
{
  /* second argument is which semaphore of the set is to be configured -> 0 for the first and only one */
  if ( semctl(sem_id, index, SETVAL, value) < 0)
  {
    perror("Error calling semctl()\n");
    exit(EXIT_FAILURE);
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
void sem_up(int sem_id, int index)
{
  struct sembuf semaphore;
  semaphore.sem_num = index;
  semaphore.sem_op = 1;
  semaphore.sem_flg =~ (IPC_NOWAIT | SEM_UNDO);

  if (semop(sem_id, &semaphore, 1) < 0)
  {
    perror("Error calling semop() V Operation\n");
    exit(EXIT_FAILURE);
  }
}

/* P Operation */
void sem_down(int sem_id, int index)
{
  struct sembuf semaphore;
  semaphore.sem_num = index;
  semaphore.sem_op = -1;
  semaphore.sem_flg =~ (IPC_NOWAIT | SEM_UNDO);

  if (semop(sem_id, &semaphore, 1) < 0)
  {
    perror("Error calling semop() P Operation\n");
    exit(EXIT_FAILURE);
  }
}

void reader(int shared)
{
  int shared_val = 0;

  for (uint8_t i = 0; i < 3; ++i) {
    sem_up(shared, READER_I); // reader +1
    shared_val = semctl(shared, SHARED_I, GETVAL);
    sem_down(shared, READER_I);
    //semctl(shared, 1, SETVAL, shared_val-1);
    printf("[%d] READER read shared_val to be: %d\n", getpid(), shared_val);
    sleep(1);
    
    //

    sleep(1);
  }
}

void writer(int shared)
{
  int shared_val = 0;
  for (uint8_t i = 0; i < 3; ++i) {
    while (semctl(shared, READER_I, GETVAL) > 0) { sleep(0.1); }
    sem_down(shared, SHARED_I);
    shared_val = semctl(shared, 1, GETVAL);
    semctl(shared, 1, SETVAL, shared_val+1);
    sem_up(shared, SHARED_I);
    printf("[%d] WRITER Incremented shared_val to be: %d\n", getpid(), shared_val+1);
    sleep(1);

    // unkritisch

    sleep(1);
  }

}

int main()
{
  key_t sem_key = ftok("/home/jopel/Documents/01_Coding/ffi4_avs/03/build", 1);
  if (sem_key < 0)
  {
    perror("Error calling ftok()\n");
    exit(EXIT_FAILURE);
  }

  /* second argument is the number of semaphores to be created -> nsems */
  /* 5 semaphores because we got 5 forks */
  int sem = semget(sem_key, 3, IPC_CREAT | 0660);
  if (sem < 0)
  {
    perror("Error calling semget()\n");
    exit(EXIT_FAILURE);
  }
  
  /* init semaphore */
  init_sem(sem, SHARED_I, 1);
  init_sem(sem, READER_I, 0);
  init_sem(sem, WRITER_I, 0);


  for (uint8_t i = 0; i < READER; i++)
  {
    if (fork() == 0) {
      printf("Spawned READER: [%d]\n", getpid());
      reader(sem);
      exit(EXIT_SUCCESS);
    }
  }

  for (uint8_t i = 0; i < WRITER; i++)
  {
    if (fork() == 0)
    {
      printf("Spawned WRITER: [%d]\n", getpid());
      writer(sem);
      exit(EXIT_SUCCESS);
    }
  }

  for (uint8_t i = 0; i < READER + WRITER; i++)
  {
    wait(NULL);
  }

  clean_sem(sem);

  return EXIT_SUCCESS;
}