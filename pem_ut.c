#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pem.h"
#include<signal.h>
#include<unistd.h>

int g_new_input = 0;

static void 
pem_ut_sig_handler(int signo)
{
	if (signo == SIGINT) {
		printf("received SIGINT\n");
		if (g_new_input != 0) {
			printf ("Last round of input processing not complete. Try again later.\n");
			return;
		}
		g_new_input = 1;
	}
}


typedef enum {
	PEM_UT_CMD_CP=0,
	PEM_UT_CMD_DP,
	PEM_UT_CMD_ET,
	PEM_UT_CMD_DL,
	PEM_UT_CMD_MAX
} pem_ut_cmd;

char *pem_ut_cmd_str [] = {
	"CP",
	"DP",
	"ET",
	"DL",
	"NA"
};

typedef enum {
	PEM_UT_TOKEN_profile_name=0,
	PEM_UT_TOKEN_duration,
	PEM_UT_TOKEN_trig,
	PEM_UT_TOKEN_trig_delay,
	PEM_UT_TOKEN_action_repeat,
	PEM_UT_TOKEN_rearm_delay,
	PEM_UT_TOKEN_action_type,
	PEM_UT_TOKEN_action_bin,
	PEM_UT_TOKEN_action_report,
	PEM_UT_TOKEN_action_cb,
	PEM_UT_TOKEN_delay,
	PEM_UT_TOKEN_interval,
	PEM_UT_TOKEN_run_mode,
	PEM_UT_TOKEN_trigger_mode,
	PEM_UT_TOKEN_eventtriggername,
	PEM_UT_TOKEN_eventarg1,
	PEM_UT_TOKEN_eventarg2,
	PEM_UT_TOKEN_eventarg3,
	PEM_UT_TOKEN_eventarg4,
	PEM_UT_TOKEN_eventarg5,
	PEM_UT_TOKEN_eventarg6,
	PEM_UT_TOKEN_sleep,
	PEM_UT_TOKEN_MAX
} pem_ut_token;


char *pem_ut_token_str [] = {
	"profile_name",
	"duration",
	"trig",
	"trig_delay",
	"action_repeat",
	"rearm_delay",
	"action_type",
	"action_bin",
	"action_report",
	"action_cb",
	"delay",
	"interval",
	"run_mode",
	"trigger_mode",
	"eventtriggername",
	"eventarg1",
	"eventarg2",
	"eventarg3",
	"eventarg4",
	"eventarg5",
	"eventarg6",
	"sleep",
	"NA"
};

int cmd_context = 0;


typedef struct _event_trigger {
	char trigger[32];
	char arg1[32];
	char arg2[32];
	char arg3[32];
	char arg4[32];
	char arg5[32];
	char arg6[32];
} event_trigger_t;


static void
get_token_value (char *token, int cmd, void *inout, unsigned int *argbitmap)
{
	int ret;
	int ii;
	int match = 0;
	int newstrlen = 0;
	char newstr[256] = {0};
	char newtoken[256] = {0};
	char newvalue[256] = {0};
	pem_event_reg_t *profile = (pem_event_reg_t *)inout;
	event_trigger_t *event = (event_trigger_t *)inout;
	unsigned int *ut_sleep = (unsigned int *)inout;

	/* Remove new line */
	newstrlen = strlen (token);
	if (token[newstrlen-1] == '\n') token[newstrlen-1] = '\0';

	for (ii = 0; ii < PEM_UT_CMD_MAX; ii++) {
		ret = strncmp (token, pem_ut_cmd_str[ii], 2);
		if (ret == 0) return;
	}

	strcpy (newstr, token);

	newstrlen = strlen (newstr);
	for (ii = 0; ii < newstrlen; ii++) {
		if (newstr[ii] == '=') {
			match = 1;
			break;
		}
	}

	if (match == 0) {
		printf ("Invalid token format: %s\n", token);
		return;
	}

	strncpy (newtoken, newstr, ii);
	newtoken[ii] = 0;
	strncpy (newvalue, newstr+ii+1, newstrlen-ii);
	//printf ("Token:%s\n", newtoken);
	//printf ("Token value:%s\n", newvalue);

	for (ii = 0; ii < PEM_UT_TOKEN_MAX; ii++) {
		ret = strcmp (newtoken, pem_ut_token_str[ii]);
		if (ret == 0) {
			match = 1;
			break;
		}
	}

	if (match == 0) {
		printf ("Unknown token: %s\n", newtoken);
		return;
	}

	switch (ii) {
	case PEM_UT_TOKEN_profile_name:
		strcpy (profile->profile_name, newvalue);
		*argbitmap |= (1 << PEM_UT_TOKEN_profile_name);
		break;

	case PEM_UT_TOKEN_duration:
		*argbitmap |= (1 << PEM_UT_TOKEN_duration);
		profile->duration = atoi (newvalue);
		break;

	case PEM_UT_TOKEN_trig:
		*argbitmap |= (1 << PEM_UT_TOKEN_trig);
		profile->trig = atoi (newvalue);
		break;

	case PEM_UT_TOKEN_trig_delay:
		*argbitmap |= (1 << PEM_UT_TOKEN_trig_delay);
		profile->trig_delay = atoi (newvalue);
		break;

	case PEM_UT_TOKEN_action_repeat:
		*argbitmap |= (1 << PEM_UT_TOKEN_action_repeat);
		profile->action_repeat = atoi (newvalue);
		break;

	case PEM_UT_TOKEN_rearm_delay:
		*argbitmap |= (1 << PEM_UT_TOKEN_rearm_delay);
		profile->rearm_delay = atoi (newvalue);
		break;

	case PEM_UT_TOKEN_action_type:
		*argbitmap |= (1 << PEM_UT_TOKEN_action_type);
		profile->action_type = atoi (newvalue);
		break;

	case PEM_UT_TOKEN_action_bin:
		*argbitmap |= (1 << PEM_UT_TOKEN_action_bin);
		strcpy (profile->action_bin, newvalue);
		break;

	case PEM_UT_TOKEN_action_report:
		*argbitmap |= (1 << PEM_UT_TOKEN_action_report);
		profile->action_report = atoi (newvalue);
		break;

	case PEM_UT_TOKEN_action_cb:
		*argbitmap |= (1 << PEM_UT_TOKEN_action_cb);
		//profile->action_cb = atoi (newvalue);
		break;

	case PEM_UT_TOKEN_delay:
		*argbitmap |= (1 << PEM_UT_TOKEN_delay);
		profile->delay = (unsigned int)atoi (newvalue);
		break;

	case PEM_UT_TOKEN_interval:
		*argbitmap |= (1 << PEM_UT_TOKEN_interval);
		profile->interval = (unsigned int)atoi (newvalue);
		break;

	case PEM_UT_TOKEN_run_mode:
		*argbitmap |= (1 << PEM_UT_TOKEN_run_mode);
		profile->run_mode = (unsigned int)atoi (newvalue);
		break;

	case PEM_UT_TOKEN_trigger_mode:
		*argbitmap |= (1 << PEM_UT_TOKEN_trigger_mode);
		profile->trigger_mode = (unsigned int)atoi (newvalue);
		break;

	case PEM_UT_TOKEN_eventtriggername:
		*argbitmap |= (1 << PEM_UT_TOKEN_eventtriggername);
		strcpy (event->trigger, newvalue);
		break;

	case PEM_UT_TOKEN_eventarg1:
		*argbitmap |= (1 << PEM_UT_TOKEN_eventarg1);
		strcpy (event->arg1, newvalue);
		break;

	case PEM_UT_TOKEN_eventarg2:
		*argbitmap |= (1 << PEM_UT_TOKEN_eventarg2);
		strcpy (event->arg2, newvalue);
		break;

	case PEM_UT_TOKEN_eventarg3:
		*argbitmap |= (1 << PEM_UT_TOKEN_eventarg3);
		strcpy (event->arg3, newvalue);
		break;

	case PEM_UT_TOKEN_eventarg4:
		*argbitmap |= (1 << PEM_UT_TOKEN_eventarg4);
		strcpy (event->arg4, newvalue);
		break;

	case PEM_UT_TOKEN_eventarg5:
		*argbitmap |= (1 << PEM_UT_TOKEN_eventarg5);
		strcpy (event->arg5, newvalue);
		break;

	case PEM_UT_TOKEN_eventarg6:
		*argbitmap |= (1 << PEM_UT_TOKEN_eventarg6);
		strcpy (event->arg6, newvalue);
		break;

	case PEM_UT_TOKEN_sleep:
		*argbitmap |= (1 << PEM_UT_TOKEN_sleep);
		*ut_sleep = atoi (newvalue);
		break;

	default:
	case PEM_UT_TOKEN_MAX:
		break;
	}
}

typedef union _pem_ut_inout_union {
	pem_event_reg_t profile;
	event_trigger_t event;
	unsigned int ut_sleep;
} pem_ut_inout_union;

pem_ut_inout_union inout;

static void
parse_input (char *cmd_str)
{
	int ret;
	int ii;
	char *newstr;
	char *token;
	char *value;
	unsigned int argbitmap = 0x0;
	pem_event_reg_t *profile = (pem_event_reg_t *)(&inout);
	event_trigger_t *event = (event_trigger_t *)(&inout);
	unsigned int *ut_sleep = (unsigned int *)(&inout);

	memset (&inout, 0, sizeof(pem_ut_inout_union));

	if (cmd_str[0] == '#') return;
	if (cmd_str[0] == '\n') return;

	for (ii = 0; ii < PEM_UT_CMD_MAX; ii++) {
		ret = strncmp (cmd_str, pem_ut_cmd_str[ii], 2);
		if (ret == 0) break;
	}

	if (ii == PEM_UT_CMD_MAX) {
		printf ("Invalid Command: %s", cmd_str);
		return;
	}

	cmd_context = ii;

	newstr = malloc (1024);
	strcpy (newstr, cmd_str);
	token = strtok (newstr, " ");
	if (token) get_token_value (token, cmd_context, &inout, &argbitmap);

	while (token != NULL) {
		token = strtok (NULL, " ");
		if (token) get_token_value (token, cmd_context, &inout, &argbitmap);
	}

	switch (cmd_context) {
	case PEM_UT_CMD_CP:
		printf ("Invoking new_profile %s API\n", profile->profile_name);
		break;

	case PEM_UT_CMD_DP:
		printf ("Invoking del_profile %s API\n", profile->profile_name);
		break;

	case PEM_UT_CMD_ET:
		printf ("Invoking event trigger %s API\n", event->trigger);
		break;

	case PEM_UT_CMD_DL:
		printf ("Delaying UT by %d seconds\n", *ut_sleep);
		break;

	case PEM_UT_CMD_MAX:
	default:
		printf ("Invalid command\n");
	}
}

static void
process_input (void)
{
    FILE *fp;
    char *line;
    size_t len = 256;
    ssize_t read;
	char filename[256] = "pem_ut.config";

    fp = fopen(filename, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

	line = malloc (len);
    while ((read = getline(&line, &len, fp)) != -1) {
		parse_input (line);
    }

	free(line);
	fclose (fp);
	return;
}

int
main(int argc, char **argv)
{

	if (signal(SIGINT, pem_ut_sig_handler) == SIG_ERR) {
		printf("\nPEM_UT: Can't catch SIGINT\n");
		exit (1);
	}

	while (1) {
		sleep (10);
		if (g_new_input) {
			process_input ();
			g_new_input = 0;
		}
		else {
			printf ("PEM_UT: ZZZZZ\n");
		}
	}

    exit(EXIT_SUCCESS);
}
