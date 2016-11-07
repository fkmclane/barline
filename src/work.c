#include <stdlib.h>
#include <string.h>

#include "barline.h"
#include "bspwm.h"

#include "work.h"

int work_count(work_t * work, char * bspwmline) {
	int count = 0;

	char ctrl;

	char * pos = bspwmline;

	pos++;

	bspwm_monitor(&pos, work->monitor);

	for (;;) {
		sscanf(pos, "%c%*63[^:]", &ctrl);

		if (ctrl == 'L')
			break;

		if (work->value == ALL) {
			count++;
		}
		else if (work->value == ACTIVE) {
			if (ctrl == 'O' || ctrl == 'o' || ctrl == 'F')
				count++;
		}
		else if (work->value == CURRENT) {
			if (ctrl == 'O')
				count++;
		}
		else {
			count++;
		}

		bspwm_next(&pos);
	}

	return count;
}

void work_all(work_t * work, char * bspwmline, char ** vector, size_t inner, size_t size) {
	char ctrl;

	char buf[64];

	buf[0] = '\0';

	char * pos = bspwmline;

	pos++;

	bspwm_monitor(&pos, work->monitor);

	for (size_t idx = 0; idx < size; idx++) {
		sscanf(pos, "%c%63[^:]", &ctrl, buf);

		if (ctrl == 'L')
			break;

		snprintf(vector[idx], inner, "%s", buf);

		bspwm_next(&pos);
	}
}

void work_active(work_t * work, char * bspwmline, char ** vector, size_t inner, size_t size) {
	char ctrl;

	char buf[64];

	buf[0] = '\0';

	char * pos = bspwmline;

	pos++;

	bspwm_monitor(&pos, work->monitor);

	for (size_t idx = 0; idx < size;) {
		sscanf(pos, "%c%63[^:]", &ctrl, buf);

		if (ctrl == 'L')
			break;

		if (ctrl == 'O' || ctrl == 'o' || ctrl == 'F') {
			snprintf(vector[idx], inner, "%s", buf);
			idx++;
		}

		bspwm_next(&pos);
	}
}

void work_current(work_t * work, char * bspwmline, char * current, size_t size) {
	char ctrl;

	char buf[64];

	buf[0] = '\0';

	char * pos = bspwmline;

	pos++;

	bspwm_monitor(&pos, work->monitor);

	for (size_t idx = 0; idx < size;) {
		sscanf(pos, "%c%63[^:]", &ctrl, buf);

		if (ctrl == 'L')
			break;

		if (ctrl == 'O') {
			snprintf(current, size, "%s", buf);
			return;
		}

		bspwm_next(&pos);
	}
}

void work_parse(const char * fmt, work_t * work) {
	char type;

	int ret = sscanf(fmt, "%c:%63s:%63s", &type, work->separator, work->monitor);

	if (ret <= 0) {
		work->value = ACTIVE;
		strncpy(work->separator, "  ", sizeof(work->separator));
		strncpy(work->monitor, monitor, sizeof(work->monitor));
	}
	else if (ret == 1) {
		work->value = type == 'C' ? COUNT : type == 'A' ? ALL : type == 'T' ? ACTIVE : CURRENT;
		strncpy(work->separator, "  ", sizeof(work->separator));
		strncpy(work->monitor, monitor, sizeof(work->monitor));
	}
	else if (ret == 2) {
		work->value = type == 'C' ? COUNT : type == 'A' ? ALL : type == 'T' ? ACTIVE : CURRENT;
		strncpy(work->monitor, monitor, sizeof(work->monitor));
	}
	else if (ret == 3) {
		work->value = type == 'C' ? COUNT : type == 'A' ? ALL : type == 'T' ? ACTIVE : CURRENT;
	}

	work->bspwmfd = -1;
}

int work_poll(work_t * work) {
	if (work->bspwmfd < 0)
		work->bspwmfd = bspwm_subscribe("report");

	return work->bspwmfd;
}

size_t work_format(work_t * work, char * buf, size_t size) {
	char current[64];

	char bspwmline[256];

	bspwm_readline(work->bspwmfd, bspwmline, sizeof(bspwmline));

	int count = work_count(work, bspwmline);

	size_t chars = 0;

	if (work->value == COUNT) {
		chars = snprintf(buf, size, "%d", count);
	}
	else if (work->value == ALL) {
		char ** vector = malloc(count*sizeof(current));

		work_all(work, bspwmline, vector, sizeof(current), count);

		chars += snprintf(buf, size - chars, "%s", vector[0]);
		for (int ws = 1; ws < count; ws++)
			chars += snprintf(buf, size - chars, "%s%s", work->separator, vector[ws]);

		free(vector);
	}
	else if (work->value == ACTIVE) {
		char ** vector = malloc(count*sizeof(current));

		work_active(work, bspwmline, vector, sizeof(current), count);

		chars += snprintf(buf, size - chars, "%s", vector[0]);
		for (int ws = 1; ws < count; ws++)
			chars += snprintf(buf, size - chars, "%s%s", work->separator, vector[ws]);

		free(vector);
	}
	else if (work->value == CURRENT) {
		work_current(work, bspwmline, current, sizeof(current));

		chars = snprintf(buf, size, "%s", current);
	}

	return chars;
}
