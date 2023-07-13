#include <stdio.h>
#include <stdlib.h>
#include <float.h>

typedef struct {
  int produced;
  int consumed;
  double *times;
  int times_cap;  
} stat_t;

stat_t *stats;
int producers;

void stats_init(int num_producers) {
  producers = num_producers;
  stats = (stat_t *)malloc(sizeof(stat_t) * producers);
  for (int i = 0; i < producers; i++) {
    stats[i].produced = 0;
    stats[i].consumed = 0;
    stats[i].times = (double *)malloc(sizeof(double) * 4);
    stats[i].times_cap = 4;
  }
}

void stats_cleanup(void) {
  for (int i = 0; i < producers; i++) {
    free(stats[i].times);
  }
  free(stats);
}

void stats_record_produced(int factory_number) {
  stats[factory_number].produced++;
}

void stats_record_consumed(int factory_number, double delay_in_ms) {
  stat_t *stat =  &stats[factory_number];
  stat->consumed++;
  if (stat->consumed > stat->times_cap) {
    stat->times = realloc(stat->times, sizeof(double) * stat->times_cap * 2);
    stat->times_cap = stat->times_cap * 2;
  }
  stat->times[stat->consumed - 1] = delay_in_ms;
}

double min_delay(int factory_number) {
  stat_t *stat =  &stats[factory_number];
  double min = DBL_MAX;
  for (int i = 0; i < stat->consumed; i++) {
    if (stat->times[i] < min) {
      min = stat->times[i];
    }
  }
  return min;
}

double max_delay(int factory_number) {
  stat_t *stat =  &stats[factory_number];
  double max = DBL_MIN;
  for (int i = 0; i < stat->consumed; i++) {
    if (stat->times[i] > max) {
      max = stat->times[i];
    }
  }
  return max;
}

double avg_delay(int factory_number) {
  stat_t *stat =  &stats[factory_number];
  double sum = 0;
  for (int i = 0; i < stat->consumed; i++) {
    sum += stat->times[i];
  }
  return (double)(sum / ((double)(stat->consumed)));
}

void stats_display(void) {
  printf("%8s%10s%10s%20s%20s%20s\n", "Factory#", "#Made", "#Eaten", "Min Delay[ms]",
          "Avg Delay[ms]", "Max Delay[ms]");
  for (int i = 0; i < producers; i++) {
    printf("%8d%10d%10d%20.5f%20.5f%20.5f\n", i, stats[i].produced, stats[i].consumed,
            min_delay(i), avg_delay(i), max_delay(i));
    if (stats[i].produced != stats[i].consumed) {
      printf("ERROR: Mismatch between number made and number eaten.");
    }
  }
}