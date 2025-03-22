#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include <libircclient.h>

#define BOT_NICK	"remindbot"
#define BOT_USER	"remindbot"
#define BOT_INFO	"A bot that gives out reminders"

#define SYSTEMLINE	"echo \"Running bot as $(whoami)\""

#define MAXINTCHAR	30
#define TIMEOUTMS	(1000 * 100) // 100 ms
#define INFOTEXTSZ	0x100
#define MAXHELLOSZ	0x400

#define ALARMCMD	"remindAlarm!"
#define MSGCMD		"remindMe!"
#define STOPCMD		"remindStop!"
#define STARTCMD	"remindStart!"
#define HELPCMD		"remindHelp!"
#define EDITCMD		"remindEdit!"
#define EXITCMD		"remindGoAway!"
#define INFOCMD		"remindInfo!"
#define HELLOCMD    "remindHi!"

#define HELPTEXT1	"Ok! So you can use \""MSGCMD" <time> <message>\" for a reminder after <time> seconds pass." 
#define HELPTEXT2	"Use \""ALARMCMD" <time>\" for an alarm after <time> seconds."
#define HELPTEXT3	"Use \""STARTCMD"\" to start a stopwatch and \""STOPCMD"\" to stop it and get the result."
#define HELPTEXT4	"Use \""INFOCMD"\" <id> to get information on an previous command."
#define HELPTEXT5	"Finally, you can change a previous command with \""EDITCMD" <id> [type=(alarm|reminder)] [time=<seconds>] [msg=newmsg])\""
#define HELPTEXT6	"Thanks!"

#define NEWENTRYTEXT	"Okay! id %d"

#define HELLOMSG	"Hey! Use \""MSGCMD" <seconds> <message>\" to get started, or type \""HELPCMD"\" for info! :)"

#define DEFAULTCHAN	"#botsrule"

#define ALARMTEXT	"Alarm for %s!"

#define MSGTEXT		"Reminder for %s: %s"

#define STOPWATCHTEXT	"Stopwatch: %ld seconds"

#define INVITEREPLY	"To have me join another channel, start another a copy of me"

#define DEFAULT_PORT	"6668"

#define USAGELINE	"\nUsage: %s [-h][options] server [port]\n"

#define HELPLINE	USAGELINE \
			"\t-h         : Print this information\n"\
			"\t-n <nick>  : Bot's Nickname (default " BOT_NICK ")\n"\
			"\t-u <user>  : Bot's Username (default " BOT_USER ")\n"\
			"\t-t <#chan> : Target (channel or username) to talk to\n"\
			"\t-m \"msg\" : The hello string the bot uses after connecting\n"\
			"\tserver     : Address of the irc server\n"\
			"\tport       : Optional port of irc server (default " DEFAULT_PORT ")\n"

const char* helptexts[] = {HELPTEXT1, HELPTEXT2, HELPTEXT3, HELPTEXT4, HELPTEXT5, HELPTEXT6};

// reminder types
typedef enum _reminder_type {
	REM_ALARM,
	REM_MSG,
	REM_STOPWATCH,
	REM_UNKNOWN
} reminder_type;

typedef void(*action_t)(char* user, char* chan, char* msg, irc_session_t* session); // some function pointer

typedef struct _reminder {
	struct _reminder* flink;
	reminder_type type;
	int id;
	time_t time;
	char* chan;
	char* user;
	char* msg;
	action_t action;
} reminder;

typedef struct _bot_ctx {
	reminder* reminder_head;
	int reminder_id_ctr;
	int doquit;
	int prevcount;
} bot_ctx;

// Callback functions

static void connect_handler(
	irc_session_t* session,
	const char* event,
	const char* origin,
	const char** params,
	unsigned int count
);
static void msg_handler(
	irc_session_t* session,
	const char* event,
	const char* origin,
	const char** params,
	unsigned int count
);
static void invite_handler(
	irc_session_t* session,
	const char* event,
	const char* origin,
	const char** params,
	unsigned int count
);
static void numeric_handler(
	irc_session_t* session,
	unsigned int event,
	const char* origin,
	const char** params,
	unsigned int count
);

static int processCommand(irc_session_t* session, const char* command, const char* user, const char* channel);

static int sendInfoEntryId(irc_session_t* session, const char* target, int id);

static int sendInfoEntry(irc_session_t* session, const char* target, reminder* r);

static int editReminder(irc_session_t* session, const char* target, int id, reminder_type type, time_t t, const char* newmsg);

static int addAlarm(irc_session_t* session, const char* user, const char* channel, time_t t);

static int addMsg(irc_session_t* session, const char* user, const char* channel, time_t t, const char* msg);

static int addStart(irc_session_t* session, const char* channel, time_t t);

static int doStop(irc_session_t* session, const char* channel);

static int sendHelpText(irc_session_t* session, const char* target);

static int updateReminders(irc_session_t* session);

static int parseTime(const char* secs, time_t* t, char**);

static int sendNewEntryId(irc_session_t* session, const char* target, int id);

static void freeReminder(reminder* r);

// reminder actions
static void alarm_action(char* user, char* chan, char* msg_unused, irc_session_t* session);
static void reminder_action(char* user, char* chan, char* msg, irc_session_t* session);
// no action for stopwatch

// Other helper functions
void hexdump(FILE* outstream, void* addr, size_t bytecount);

// Global variables
char* gtarget = DEFAULTCHAN; // user or channel the bot talks to
char ghello[MAXHELLOSZ] = HELLOMSG;

int main(int argc, char** argv) {
	int ret = 0;
	char* server_addr = NULL;
	short server_port = 0;
	char* nickname = BOT_NICK;
	char* username = BOT_USER;
	char* infoname = BOT_INFO;
	irc_callbacks_t callbacks = {0};
	irc_session_t* session = NULL;
	bot_ctx* ctx = NULL;
	int c = 0;
	struct timeval timeout = {0};
	int maxfd = 0;
	fd_set readfds = {0};
	fd_set writefds = {0};
	reminder* r = NULL;
	time_t prevt = 0;

	// parse arguments
	opterr = 0;
	while ((c = getopt(argc, argv, "hn:u:t:m:")) != -1) {
		switch (c) {
		case 'h':
			fprintf(stderr, HELPLINE, argv[0]);
			exit(0);
		case 'n':
			nickname = optarg;
			break;
		case 'u':
			username = optarg;
			break;
		case 't':
			gtarget = optarg;
			break;
		case 'm':
			strncpy(ghello, optarg, MAXHELLOSZ-1);
			break;
		case '?':
			fprintf(stderr, "Unknown option -%c\n", optopt);
			fprintf(stderr, USAGELINE, argv[0]);
			ret = -1;
			goto END;
		case ':':
			fprintf(stderr, "Missing an argument for -%c\n", optopt);
			fprintf(stderr, USAGELINE, argv[0]);
			ret = -1;
			goto END;
		default:
			fprintf(stderr, "Argument issue\n");
			fprintf(stderr, USAGELINE, argv[0]);
			ret = -1;
			goto END;
		}
	}

	if (optind >= argc) {
		fprintf(stderr, "Required argument \"server\"\n");
		fprintf(stderr, USAGELINE, argv[0]);
		ret = -1;
		goto END;
	}

	server_addr = argv[optind];

	if ((optind+1) >= argc) {
		server_port = strtoul(DEFAULT_PORT, NULL, 0);
	} else {
		server_port = strtoul(argv[optind+1], NULL, 0);
	}

	system(SYSTEMLINE);

	// start irc client
	// register more callbacks here if needed
	callbacks.event_connect = connect_handler;
	//callbacks.event_nick = event_handler;
	//callbacks.event_quit = event_handler;
	//callbacks.event_join = event_handler;
	//callbacks.event_part = event_handler;
	//callbacks.event_mode = event_handler;
	//callbacks.event_umode = event_handler;
	//callbacks.event_topic = event_handler;
	//callbacks.event_kick = event_handler;
	callbacks.event_channel = msg_handler;
	callbacks.event_privmsg = msg_handler;
	//callbacks.event_notice = event_handler;
	//callbacks.event_channel_notice = event_handler;
	callbacks.event_invite = invite_handler;
	//callbacks.event_unknown = event_handler;
	callbacks.event_numeric = numeric_handler;
	
	session = irc_create_session(&callbacks);
	if (!session) {
		fprintf(stderr, "Unable to create session. %s\n", irc_strerror(irc_errno(session)));
		ret = -1;
		goto END;
	}

	irc_option_set(session, LIBIRC_OPTION_SSL_NO_VERIFY);
	irc_option_set(session, LIBIRC_OPTION_STRIPNICKS);
	//irc_option_set(session, LIBIRC_OPTION_DEBUG);
	
	ctx = (bot_ctx*)calloc(1, sizeof(bot_ctx));
	if (!ctx) {
		fprintf(stderr, "Unable to allocate bot context\n");
		ret = -1;
		goto END;
	}

	ctx->reminder_head = NULL;
	ctx->reminder_id_ctr = 1;
	ctx->doquit = 0;
	ctx->prevcount = 0;
	irc_set_ctx(session, (void*)ctx);

	fprintf(stderr, "Connecting to %s on port %u\n", server_addr, server_port);
	
	if (irc_connect(session, server_addr, server_port, NULL, nickname, username, infoname)) {
		fprintf(stderr, "Unable to connect to server. %s\n", irc_strerror(irc_errno(session)));
		ret = -1;
		goto END;
	}

	// our run loop
	timeout.tv_sec = 0;
	timeout.tv_usec = TIMEOUTMS;
	while (1) {
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);

		irc_add_select_descriptors(session, &readfds, &writefds, &maxfd);

		if (select(maxfd+1, &readfds, &writefds, NULL, &timeout) < 0) {
			// error
			fprintf(stderr, "Error during run loop. %s\n", strerror(errno));
			ret = -1; goto END;
		}
		
		if (prevt != time(NULL)) {
			prevt = time(NULL);
			// handle any reminders we have to handle
			if (updateReminders(session)) {
				fprintf(stderr, "Error while updating reminders.\n");
				ret = -1;
				goto END;
			}
		}

		if (ctx->doquit) {
			break;
		}

		// let the irc library do it's thing
		if (irc_process_select_descriptors(session, &readfds, &writefds)) {
			fprintf(stderr, "Error with irc run loop. %s\n", irc_strerror(irc_errno(session)));
			ret = -1;
			goto END;
		}
	}

	fprintf(stderr, "Shutting Down\n");

END:
	if (ctx) {
		while (ctx->reminder_head != NULL) {
			r = ctx->reminder_head;
			ctx->reminder_head = r->flink;
			freeReminder(r);
		}
		free(ctx);
	}
	if (session) {
		irc_destroy_session(session);
	}

	fprintf(stderr, "Done\n");

	return ret;
}

void connect_handler(
	irc_session_t* session,
	const char* event,
	const char* origin,
	const char** params,
	unsigned int count
) {
	(void)event;
	(void)count;
	(void)params;
	(void)origin;

	// join target channel (if it is a channel)
	if (gtarget && gtarget[0] == '#') {
		// join the target channel
		// NULL secret key
		irc_cmd_join(session, gtarget, NULL);
	}

	// say hello, if we have a "target"
	if (gtarget) {
		irc_cmd_msg(session, gtarget, ghello);
	}
}

void msg_handler(
	irc_session_t* session,
	const char* event,
	const char* origin,
	const char** params,
	unsigned int count
) {
	(void)event;

	char lastchar = 0;
	char* msg = NULL;
	const char* chan = NULL;

	if (count < 2) {
		fprintf(stderr, "Error, msg handler called with param count of < 2\n");
		return;
	}

	msg = irc_color_strip_from_mirc(params[1]);

	lastchar = msg[strlen(msg)-1];
	if (lastchar == '\r' || lastchar == '\n') {
		msg[strlen(msg)-1] = '\0';
	}

	chan = params[0];
	if (chan[0] != '#') {
		chan = NULL;
	}
	
	// handle messages
	if (processCommand(session, msg, origin, chan)) {
		fprintf(stderr, "Unable to process Command\n");
	}

	if (msg) {
		free(msg);
	}
}

void invite_handler(
	irc_session_t* session,
	const char* event,
	const char* origin,
	const char** params,
	unsigned int count
) {
	(void)event;
	(void)params;
	(void)count;

	// tell the inviter to use the "/bot" command
	if (irc_cmd_msg(session, origin, INVITEREPLY)) {
		fprintf(stderr, "Error replying to invite. %s\n", irc_strerror(irc_errno(session)));
	}
}

void numeric_handler(
	irc_session_t* session,
	unsigned int event,
	const char* origin,
	const char** params,
	unsigned int count
) {
	(void)session;
	(void)event;
	(void)origin;
	(void)params;
	(void)count;
	// handle numeric server responses here
	// like to see results of a command
	return;
}

int processCommand(irc_session_t* session, const char* command, const char* user, const char* channel) {
	int ret = 0;
	char* cmd = NULL;
	time_t t = 0;
	char* msg = NULL;
	int id = 0;
	reminder_type type = REM_UNKNOWN;
	char* cursor = NULL;
	bot_ctx* ctx = NULL;

	ctx = irc_get_ctx(session);

	// see if we got a command
	cmd = strstr(command, ALARMCMD);
	if (cmd) {
		// Got an alarm type
		fprintf(stderr, "Got an alarm command \"%s\"\n", command);
		// try to parse args
		if (!parseTime(cmd+sizeof(ALARMCMD), &t, &msg)) {
			addAlarm(session, user, channel, t);
		}
		goto END;
	}
	
	cmd = strstr(command, MSGCMD);
	if (cmd) {
		// got a msg type
		fprintf(stderr, "Got a msg command \"%s\"\n", command);
		// try to parse args
		if (!parseTime(cmd+sizeof(MSGCMD), &t, &msg)) {
			addMsg(session, user, channel, t, msg);
		}
		goto END;
	}
	
	cmd = strstr(command, STOPCMD);
	if (cmd) {
		// got a stopwatch command
		fprintf(stderr, "Got a stopwatch command \"%s\"\n", command);

		if (channel == NULL) {
			channel = user;
		}
		doStop(session, channel);

		goto END;
	}

	cmd = strstr(command, STARTCMD);
	if (cmd) {
		// got a startwatch command
		fprintf(stderr, "Got a startwatch command \"%s\"\n", command);

		if (channel == NULL) {
			channel = user;
		}
	
		addStart(session, channel, time(NULL));

		goto END;
	}

	cmd = strstr(command, HELPCMD);
	if (cmd) {
		// got a help command
		fprintf(stderr, "Got a help command \"%s\"\n", command);
		sendHelpText(session, (channel) ? channel : user);
		goto END;
	}

	cmd = strstr(command, EDITCMD);
	if (cmd) {
		// got a edit command
		fprintf(stderr, "Got an edit command \"%s\"\n", command);

		cursor = cmd + sizeof(EDITCMD);
		// parse id
		id = strtol(cursor, &cursor, 0);
		if (id == 0) {
			fprintf(stderr, "Unable to parse the id for an edit command\n");
			goto END;
		}

		// parse optional new type
		if (strstr(cursor, "type=alarm")) {
			// type should now be alarm
			type = REM_ALARM;
		} else if (strstr(cursor, "type=reminder")) {
			// type should be
			type = REM_MSG; 
		}

		// parse optional new time
		cursor = strstr(cursor, "time=");
		if (cursor != NULL) {
			t = strtol(cursor + strlen("time="), NULL, 0);
			if (t != 0) {
				t += time(NULL);
			}
		}

		// parse optional msg
		cursor = cmd + sizeof(EDITCMD);
		cursor = strstr(cursor, "msg=");
		if (cursor != NULL) {
			msg = cursor + strlen("msg=");
		}

		
		if (channel == NULL) {
			channel = user;
		}

		editReminder(session, channel, id, type, t, msg);
		goto END;
	}

	cmd = strstr(command, INFOCMD);
	if (cmd) {
		// got a info command
		fprintf(stderr, "Got an info command \"%s\"\n", command);

		// try to parse the id
		cursor = cmd + sizeof(INFOCMD);

		id = strtol(cursor, &cursor, 0);
		if (id == 0) {
			fprintf(stderr, "Unable to parse the id for an info command\n");
			goto END;
		}

		if (channel == NULL) {
			channel = user;
		}

		// find the reminder based on id
		sendInfoEntryId(session, channel, id);
	}

    cmd = strstr(command, HELLOCMD);
    if (cmd) {
        // got a hello command
        fprintf(stderr, "Got a hello command\n");

        cursor = cmd + (sizeof(HELLOCMD));
        if (cursor[-1] == '\0' || cursor[0] == '\0') {
            // say hello
            irc_cmd_msg(session, (channel) ? channel : user, ghello);
        } else {
            // change greeting
            strncpy(ghello, cursor, sizeof(ghello) - 1);
        }
    }
	
	cmd = strstr(command, EXITCMD);
	if (cmd) {
		ctx->doquit = 1;
	}

	// not a command (that we know)
END:
	return ret;
}

int editReminder(irc_session_t* session, const char* target, int id, reminder_type type, time_t t, const char* newmsg) {
	bot_ctx* ctx = NULL;
	int ret = 0;
	reminder* r = NULL;

	ctx = irc_get_ctx(session);

	r = ctx->reminder_head;

	while (r != NULL) {
		if (r->id == id) {
			if (type != REM_UNKNOWN) {
				r->type = type;
				fprintf(stderr, "Changed %d type to %d\n", id, type);
			}

			if (t != 0) {
				r->time = t;
				fprintf(stderr, "Changed %d time to %ld\n", id, t);
			}

			if (newmsg != NULL) {
				if (strlen(newmsg) > strlen(r->msg)) {
					r->msg = realloc(r->msg, strlen(newmsg)+1);
				}
				strcpy(r->msg, newmsg);
			}
			
			sendNewEntryId(session, target, id);
			
			break;
		}

		r = r->flink;
	}

	return ret;
}

int sendHelpText(irc_session_t* session, const char* target) {
	size_t i;
	
	for (i = 0; i < sizeof(helptexts) / sizeof(helptexts[0]); i++) {
		if (irc_cmd_msg(session, target, helptexts[i])) {
			fprintf(stderr, "Unable to send help text %lu to server. %s\n", i, irc_strerror(irc_errno(session)));
			return -1;
		}
	}

	return 0;
}

int addAlarm(irc_session_t* session, const char* user, const char* channel, time_t t) {
	bot_ctx* ctx = NULL;
	int ret = 0;	
	reminder* r = NULL;
	char* copied_chan = NULL;
	char* copied_user = NULL;
	
	ctx = irc_get_ctx(session);

	r = malloc(sizeof(reminder));
	if (r == NULL) {
		fprintf(stderr, "Unable to allocate reminder for alarm\n");
		ret = -1;
		goto END;
	}

	if (channel) {
		copied_chan = malloc(strlen(channel)+1);
		if (copied_chan == NULL) {
			fprintf(stderr, "Unable to allocated channel string\n");
			ret = -1;
			goto END;
		}

		strcpy(copied_chan, channel);
	} else {
		copied_chan = NULL;
	}

	copied_user = malloc(strlen(user)+1);
	if (copied_user == NULL) {
		fprintf(stderr, "Unable to allocate user string\n");
		ret = -1;
		goto END;
	}

	strcpy(copied_user, user);

	r->type = REM_ALARM;
	r->id = ctx->reminder_id_ctr++;
	r->time = t;
	r->chan = copied_chan;
	r->user = copied_user;
	r->action = alarm_action;

	// finally link the reminder
	r->flink = ctx->reminder_head;
	ctx->reminder_head = r;

	sendNewEntryId(session, (r->chan) ? r->chan : r->user, r->id);

END:
	if (ret == -1) {
		if (r) {
			free(r);
		}
		
		if (copied_chan) {
			free(copied_chan);
		}

		if (copied_user) {
			free(copied_user);
		}
	}
	return ret;
}

int addMsg(irc_session_t* session, const char* user, const char* channel, time_t t, const char* msg) {
	bot_ctx* ctx = NULL;
	int ret = 0;	
	reminder* r = NULL;
	char* copied_chan = NULL;
	char* copied_user = NULL;
	char* copied_msg = NULL;
	
	ctx = irc_get_ctx(session);

	r = malloc(sizeof(reminder));
	if (r == NULL) {
		fprintf(stderr, "Unable to allocate reminder for alarm\n");
		ret = -1;
		goto END;
	}

	if (channel) {
		copied_chan = malloc(strlen(channel)+1);
		if (copied_chan == NULL) {
			fprintf(stderr, "Unable to allocated channel string\n");
			ret = -1;
			goto END;
		}

		strcpy(copied_chan, channel);
	} else {
		copied_chan = NULL;
	}

	copied_user = malloc(strlen(user)+1);
	if (copied_user == NULL) {
		fprintf(stderr, "Unable to allocate user string\n");
		ret = -1;
		goto END;
	}

	strcpy(copied_user, user);

	copied_msg = malloc(strlen(msg)+1);
	if (copied_msg == NULL) {
		fprintf(stderr, "Unable to allocate msg string\n");
		ret = -1;
		goto END;
	}

	strcpy(copied_msg, msg);

	r->type = REM_MSG;
	r->id = ctx->reminder_id_ctr++;
	r->time = t;
	r->chan = copied_chan;
	r->user = copied_user;
	r->msg = copied_msg;
	r->action = reminder_action;

	// finally link the reminder
	r->flink = ctx->reminder_head;
	ctx->reminder_head = r;

	sendNewEntryId(session, (r->chan) ? r->chan : r->user, r->id);

END:
	if (ret == -1) {
		if (r) {
			free(r);
		}
		
		if (copied_chan) {
			free(copied_chan);
		}

		if (copied_user) {
			free(copied_user);
		}

		if (copied_msg) {
			free(copied_msg);
		}
	}
	return ret;
}

int addStart(irc_session_t* session, const char* channel, time_t t) {
	bot_ctx* ctx = NULL;
	int ret = 0;	
	reminder* r = NULL;
	char* copied_chan = NULL;
	
	ctx = irc_get_ctx(session);

	r = malloc(sizeof(reminder));
	if (r == NULL) {
		fprintf(stderr, "Unable to allocate reminder for alarm\n");
		ret = -1;
		goto END;
	}

	if (channel) {
		copied_chan = malloc(strlen(channel)+1);
		if (copied_chan == NULL) {
			fprintf(stderr, "Unable to allocated channel string\n");
			ret = -1;
			goto END;
		}

		strcpy(copied_chan, channel);
	} else {
		copied_chan = NULL;
	}

	r->type = REM_STOPWATCH;
	r->id = ctx->reminder_id_ctr++;
	r->time = t;
	r->chan = copied_chan;

	// finally link the reminder
	r->flink = ctx->reminder_head;
	ctx->reminder_head = r;

	sendNewEntryId(session, r->chan, r->id);

END:
	if (ret == -1) {
		if (r) {
			free(r);
		}
		
		if (copied_chan) {
			free(copied_chan);
		}
	}
	return ret;
}


int doStop(irc_session_t* session, const char* channel) {
	int ret = 0;
	time_t delta;
	bot_ctx* ctx = NULL;
	reminder* r = NULL;
	reminder* prev_r = NULL;
	reminder* next_r = NULL;
	char* resp = NULL;
	size_t resplen = 0;

	ctx = irc_get_ctx(session);
	r = ctx->reminder_head;
	
	while (r != NULL) {
		next_r = r->flink;

		if (r->type == REM_STOPWATCH && !strcmp(channel, r->chan)) {
			// found the associated stopwatch start
			// free the entry
			// print out the response
			delta = time(NULL) - r->time;
			
			resplen = sizeof(STOPWATCHTEXT) + MAXINTCHAR;
			resp = malloc(resplen);
			if (!resp) {
				fprintf(stderr, "Unable to allocate stopwatch response\n");
				ret = -1;
				goto END;
			}
			
			if (snprintf(resp, resplen - 1, STOPWATCHTEXT, delta) == -1) {
				fprintf(stderr, "Unable to format text for stopwatch response\n");
				ret = -1;
				goto END;
			}

			if (irc_cmd_msg(session, channel, resp)) {
				fprintf(stderr, "Error sending stopwatch text. %s\n", irc_strerror(irc_errno(session)));
				goto END;
			}

			// remove this entry
			if (prev_r != NULL) {
				// set previous to point to next
				prev_r->flink = next_r;
			} else {
				// this was the head
				// so make the next one the head
				ctx->reminder_head = next_r;
			}
			
			// free it
			freeReminder(r);
			r = NULL;
			
			if (resp != NULL) {
				free(resp);
				resp = NULL;
			}
		} else {
			// update the previous if we didn't remove one
			prev_r = r;
		}

		r = next_r;
	}

END:
	if (resp) {
		free(resp);
	}
	return ret;
}

int updateReminders(irc_session_t* session) {
	bot_ctx* ctx = NULL;
	int i = 0;
	reminder* r = NULL;
	reminder* prev_r = NULL;
	reminder* next_r = NULL;

	ctx = irc_get_ctx(session);
	r = ctx->reminder_head;
	
	while (r != NULL) {
		next_r = r->flink;

		if (r->type != REM_STOPWATCH && r->time <= time(NULL)) {
			// call the action
			r->action(r->user, r->chan, r->msg, session);
			// remove this entry
			if (prev_r != NULL) {
				// set previous to point to next
				prev_r->flink = next_r;
			} else {
				// this was the head
				// so make the next one the head
				ctx->reminder_head = next_r;
			}
			
			// free the entry
			freeReminder(r);
			r = NULL;
		} else {
			// update the previous if we didn't remove one
			prev_r = r;
		}

		r = next_r;
		i++;
	}

	if (i != ctx->prevcount) {
		fprintf(stderr, "There are %d items in the list\n", i);
		ctx->prevcount = i;
	}
	
	return 0;
}

void alarm_action(char* user, char* chan, char* msg_unused, irc_session_t* session) {
	(void)msg_unused;

	char* target = chan;
	char* alarmtxt = NULL;
	size_t txtsz = 0;

	if (target == NULL) {
		target = user;
	}

	txtsz = sizeof(ALARMTEXT) + strlen(user);
	alarmtxt = malloc(txtsz);
	if (alarmtxt == NULL) {
		fprintf(stderr, "Unable to allocate alarm text\n");
		goto END;
	}

	if (snprintf(alarmtxt, txtsz - 1, ALARMTEXT, user) <= -1) {
		fprintf(stderr, "Unable to craft alarm text. %s\n", strerror(errno));
		goto END;
	}

	if (irc_cmd_msg(session, target, alarmtxt)) {
		fprintf(stderr, "Error sending alarm text. %s\n", irc_strerror(irc_errno(session)));
		goto END;
	}

END:
	if (alarmtxt) {
		free(alarmtxt);
	}
}

void reminder_action(char* user, char* chan, char* msg, irc_session_t* session) {
	char* target = chan;
	char* msgtxt = NULL;
	size_t txtsz = 0;

	if (target == NULL) {
		target = user;
	}

	txtsz = sizeof(MSGTEXT) + strlen(user) + strlen(msg);
	msgtxt = malloc(txtsz);
	if (msgtxt == NULL) {
		fprintf(stderr, "Unable to allocate reminder text\n");
		goto END;
	}

	if (snprintf(msgtxt, txtsz - 1, MSGTEXT, user, msg) <= -1) {
		fprintf(stderr, "Unable to craft reminder text. %s\n", strerror(errno));
		goto END;
	}

	if (irc_cmd_msg(session, target, msgtxt)) {
		fprintf(stderr, "Error sending reminder text. %s\n", irc_strerror(irc_errno(session)));
		goto END;
	}

END:
	if (msgtxt) {
		free(msgtxt);
	}
	
}

int parseTime(const char* secs, time_t* t, char** endptr) {
	time_t now;
	time_t delta;

	delta = strtoll(secs, endptr, 0);
	if (delta == 0) {
		return -1;
	}

	now = time(NULL);
	*t = now + delta;

	return 0;
}

int sendNewEntryId(irc_session_t* session, const char* target, int id) {
	int ret = 0;
	char* msgtxt = NULL;
	size_t txtsz = 0;

	txtsz = sizeof(NEWENTRYTEXT) + MAXINTCHAR;
	msgtxt = malloc(txtsz);
	if (msgtxt == NULL) {
		fprintf(stderr, "Unable to allocate reminder text\n");
		ret = -1;
		goto END;
	}

	if (snprintf(msgtxt, txtsz - 1, NEWENTRYTEXT, id) <= -1) {
		fprintf(stderr, "Unable to craft new id text. %s\n", strerror(errno));
		ret = -1;
		goto END;
	}

	if (irc_cmd_msg(session, target, msgtxt)) {
		fprintf(stderr, "Error sending reminder text. %s\n", irc_strerror(irc_errno(session)));
		ret = -1;
		goto END;
	}

END:
	if (msgtxt) {
		free(msgtxt);
	}
	return ret;
}

int sendInfoEntryId(irc_session_t* session, const char* target, int id) {
	bot_ctx* ctx = NULL;
	int ret = 0;
	reminder* r = NULL;

	ctx = irc_get_ctx(session);

	r = ctx->reminder_head;

	while (r != NULL) {
		if (r->id == id) {
			sendInfoEntry(session, target, r);
			
			break;
		}

		r = r->flink;
	}

	return ret;
}

int sendInfoEntry(irc_session_t* session, const char* target, reminder* r) {
	char infotext[INFOTEXTSZ] = {0};
	// write out information about the reminder
	
	if (r->type == REM_MSG) {
		snprintf(infotext, INFOTEXTSZ-1, "%d: Reminder for %s, msg %s", r->id, r->user, r->msg); 
	} else if (r->type == REM_ALARM) {
		snprintf(infotext, INFOTEXTSZ-1, "%d: Alarm for %s", r->id, r->user); 
	} else if (r->type == REM_STOPWATCH) {
		snprintf(infotext, INFOTEXTSZ-1, "%d: Start", r->id); 
	} else {
		fprintf(stderr, "Unknown entry type to print info for!\n");
		return -1;
	}
	
	if(irc_cmd_msg(session, target, infotext)) {
		fprintf(stderr, "Unable to info text to server. %s\n", irc_strerror(irc_errno(session)));
		return -1;
	}

	return 0;
}

void freeReminder(reminder* r) {
	if (r->type == REM_ALARM) {
		free(r->user);
		free(r->chan);
	} else if (r->type == REM_MSG) {
		free(r->user);
		free(r->chan);
		free(r->msg);
	} else if (r->type == REM_STOPWATCH) {
		free(r->chan);
	} else {
		fprintf(stderr, "ERROR! Trying to free an unknown type of reminder!\n");
	}
	free(r);
}

void hexdump(FILE* outstream, void* addr, size_t bytecount) {
	size_t i = 0;
	size_t j = 0;
	size_t lineoff = 0;
	unsigned char* buf = addr;
	
	// print the address
	fprintf(outstream, "%016llx ", (unsigned long long)&buf[0]);

	for (i = 0; i < bytecount; i++) {
		fprintf(outstream, "%02x ", buf[i]);

		if (i == (bytecount - 1)) {
			// get line offset so far
			lineoff = (i % 0x10) * 3;
			if ((i % 0x10) >= 0x8) {
				lineoff += 3; // add size of "-- "
			}

			for (j = lineoff; j < ((0x10 * 3)+4); j++) {
				fprintf(outstream, " "); // get to the ascii part
			}
			
			for (j = (0x10 * (i/0x10)); j <= i; j++) {
				fprintf(outstream, (isprint(buf[j])) ? "%c" : ".", buf[j]);
			}
			fprintf(outstream, "\n");
		} else if ((i % 0x10) == 0xf) {
			fprintf(outstream, "    ");
			for (j = i-0xf; j <= i; j++) {
				fprintf(outstream, (isprint(buf[j])) ? "%c" : ".", buf[j]);
			}
			fprintf(outstream, "\n");
			// print the address
			fprintf(outstream, "%016llx ", (unsigned long long)&buf[i+1]);
		} else if ((i % 8) == 7) {
			fprintf(outstream, "-- ");
		}
	}
}
