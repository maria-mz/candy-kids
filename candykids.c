#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include "bbuff.h"
#include "stats.h"

#define BUFFER_SIZE 10

typedef struct {
  int factory_number;
  pthread_t tid;
} factory_t;

typedef struct  {
  int factory_number;
  double creation_ts_ms;
} candy_t;

bool times_up;
int threads_finished;

pthread_mutex_t buffer_mutex;
pthread_mutex_t thread_mutex;
sem_t full;
sem_t empty;

factory_t *factory_ids;
pthread_t *kid_ids;

double current_time_in_ms(void) {
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  return now.tv_sec * 1000.0 + now.tv_nsec/1000000.0;
}

candy_t *produce_candy(int factory) {
  candy_t *new_candy = (candy_t *)malloc(sizeof(candy_t));
  new_candy->factory_number = factory;
  new_candy->creation_ts_ms = current_time_in_ms();
  return new_candy;
}

void *factory_runner(void *factory_number) {
  int factory = *((int *)(factory_number));

  while (!times_up) {
    int wait = rand() % 4;
    candy_t *candy = produce_candy(factory);
    printf("Factory %d ships candy & waits %ds\n", factory, wait);
    
    sem_wait(&empty);
    pthread_mutex_lock(&buffer_mutex);

    // Critical section start
    bbuff_blocking_insert(candy);
    stats_record_produced(factory);
    // Critical section end

    pthread_mutex_unlock(&buffer_mutex);
    sem_post(&full);

    for (int i = 0; i < wait; i++) {
      sleep(1);
    }
  }

  pthread_mutex_lock(&thread_mutex);

  // Critical section start
  threads_finished++;
  // Critical section end

  pthread_mutex_unlock(&thread_mutex);

  printf("Candy-factory %d done\n", factory);
  return NULL;
}

void consume_candy() {
  candy_t* candy = (candy_t *)bbuff_blocking_extract();
  double delay = (double)(current_time_in_ms() - candy->creation_ts_ms);
  stats_record_consumed(candy->factory_number, delay);
  free(candy);
}

void *kid_runner() {
  while (1) {
    int wait = rand() % 2;
    sem_wait(&full);
    pthread_mutex_lock(&buffer_mutex);

    // Critical section start
    consume_candy();
    // Critical section end

    pthread_mutex_unlock(&buffer_mutex);
    sem_post(&empty);

    for (int i = 0; i < wait; i++) {
      sleep(1);
    }
  }
  return NULL;
}

void launch_factory_threads(int num_factories) {
  factory_ids = (factory_t *)malloc(num_factories * sizeof(factory_t));
  for (int i = 0; i < num_factories; i++) {
    factory_ids[i].factory_number = i;
    pthread_create(&factory_ids[i].tid, NULL, factory_runner, &factory_ids[i].factory_number);
  }
}

void launch_kid_threads(int num_kids) {
  kid_ids = (pthread_t *)malloc(num_kids * sizeof(pthread_t));
  for (int i = 0; i < num_kids; i++) {
    pthread_create(kid_ids + i, NULL, kid_runner, NULL);
  }
}

int main(int argc, char *argv[]) {
  // 1.  Extract arguments
  int num_factories = atoi(argv[1]);
  int num_kids = atoi(argv[2]);
  int num_seconds = atoi(argv[3]);

  if (num_factories <= 0 || num_kids <= 0 || num_seconds <= 0) {
    printf("ERROR: Arguments must be greater than 0\n");
    exit(0);
  }

  // 2.  Initialization of modules
  pthread_mutex_init(&buffer_mutex, NULL);
  pthread_mutex_init(&thread_mutex, NULL);
  sem_init(&empty, 0, BUFFER_SIZE);
  sem_init(&full, 0, 0);
  bbuff_init();
  stats_init(num_factories);

  times_up = false;
  threads_finished = 0;

  // 3.  Launch candy-factory threads
  launch_factory_threads(num_factories);

  // 4.  Launch kid threads
  launch_kid_threads(num_kids);

  // 5.  Wait for requested time
  for (int i = 0; i < num_seconds; i++) {
    sleep(1);
    printf("Time %ds\n", i + 1);
  }

  // 6.  Stop candy-factory threads
  times_up = true;
  while (threads_finished < num_factories) {
    sleep(1);
  }
  for (int i = 0; i < num_factories; i++) {
    pthread_join(factory_ids[i].tid, NULL);
  }

  // 7.  Wait until there is no more candies
  while (!bbuff_is_empty()) {
    printf("Waiting for all candies to be consumed\n");
    sleep(1);
  }

  // 8.  Stop kid threads
  for (int i = 0; i < num_kids; i++) {
    pthread_cancel(kid_ids[i]);
    pthread_join(kid_ids[i], NULL);
  }

  // 9.  Print statistics
  stats_display();

  // 10. Cleanup any allocated memory
  pthread_mutex_destroy(&buffer_mutex);
  pthread_mutex_destroy(&thread_mutex);
  sem_destroy(&empty);
  sem_destroy(&full);
  free(factory_ids);
  free(kid_ids);
  stats_cleanup();
}
