#ifndef STATS_H
#define STATS_H

void stats_init(int num_producers);
void stats_cleanup(void);
void stats_record_produced(int factory_number);
void stats_record_consumed(int factory_number, double delay_in_ms);
double min_delay(int factory_number);
double max_delay(int factory_number);
double avg_delay(int factory_number);
void stats_display(void);

#endif