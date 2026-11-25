#ifndef _LINUX_ALARMTIMER_H
#define _LINUX_ALARMTIMER_H

#include <linux/time.h>
#include <linux/hrtimer.h>
#include <linux/timerqueue.h>
#include <linux/rtc.h>

enum alarmtimer_type {
	ALARM_REALTIME,
	ALARM_BOOTTIME,

	ALARM_NUMTYPE,
};

enum alarmtimer_restart {
	ALARMTIMER_NORESTART,
	ALARMTIMER_RESTART,
};


#define ALARMTIMER_STATE_INACTIVE	0x00
#define ALARMTIMER_STATE_ENQUEUED	0x01
#define ALARMTIMER_STATE_CALLBACK	0x02

/**
 * struct alarm - Alarm timer structure
 * @node:	timerqueue node for adding to the event list this value
 *		also includes the expiration time.
 * @period:	Period for recuring alarms
 * @function:	Function pointer to be executed when the timer fires.
 * @type:	Alarm type (BOOTTIME/REALTIME)
 * @enabled:	Flag that represents if the alarm is set to fire or not
 * @data:	Internal data value.
 */
struct k_alarm {
	struct timerqueue_node	node;
	enum alarmtimer_restart	(*function)(struct k_alarm *, ktime_t now);
	enum alarmtimer_type	type;
	int			state;
	void			*data;
};

void k_alarm_init(struct k_alarm *alarm, enum alarmtimer_type type,
		enum alarmtimer_restart (*function)(struct k_alarm *, ktime_t));
void k_alarm_start(struct k_alarm *alarm, ktime_t start);
void k_alarm_start_relative(struct k_alarm *alarm, ktime_t start);
void k_alarm_restart(struct k_alarm *alarm);
int k_alarm_try_to_cancel(struct k_alarm *alarm);
int k_alarm_cancel(struct k_alarm *alarm);

u64 k_alarm_forward(struct k_alarm *alarm, ktime_t now, ktime_t interval);
u64 k_alarm_forward_now(struct k_alarm *alarm, ktime_t interval);
ktime_t k_alarm_expires_remaining(const struct k_alarm *alarm);

/*
 * A alarmtimer is active, when it is enqueued into timerqueue or the
 * callback function is running.
 */
static inline int alarmtimer_active(const struct k_alarm *timer)
{
	return timer->state != ALARMTIMER_STATE_INACTIVE;
}

/*
 * Helper function to check, whether the timer is on one of the queues
 */
static inline int alarmtimer_is_queued(struct k_alarm *timer)
{
	return timer->state & ALARMTIMER_STATE_ENQUEUED;
}

/*
 * Helper function to check, whether the timer is running the callback
 * function
 */
static inline int alarmtimer_callback_running(struct k_alarm *timer)
{
	return timer->state & ALARMTIMER_STATE_CALLBACK;
}


/* Provide way to access the rtc device being used by alarmtimers */
struct rtc_device *alarmtimer_get_rtcdev(void);

#endif
