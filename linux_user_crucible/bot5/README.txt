Note: module focus == VR&E

This bot is vulnerable by using uninitialized data, provided they do a bit of memory grooming

It is a bot that sets up reminders for users
Users can set, remove, and edit reminders
	- edit message (if appropriate type)
	- edit time
	- edit reminder type (vulnerable)

struct _reminder {
	enum rtype remindertype
	time_t time
	char* usr
	char* msg
	action
}

Types:
	- alarm:
		alarm_action(usr); "Alarm for {user}"
	- reminder:
		reminder_action(usr, msg); "Reminder for {user}: \"{msg}\""
	- stopwatch:
		no associated action, because it doesn't go "off" based on time, it is just a start timestamp

Vulnerability relies on uninitialized memory and editing a reminder to change it's type:
	1. change: alarm -> reminder
			prints from uninitialized 'msg' (peek)
	2. change: stopwatch -> alarm
			- goes to use 'action'
			- 'action' replaced with 'system', 'usr' as argument, boom!

