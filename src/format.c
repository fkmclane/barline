#include "format.h"

format_t * format_load(const char * fmt) {
	int num = 0;
	int idx = 0;

	char buf[256];
	int bufidx = 0;

	format_t * format = malloc(sizeof(format_t));

	const char * pos = fmt;
	while (*pos != '\0') {
		if (*pos == '%') {
			if (bufidx > 0) {
				num++;
			}

			num++;

			while(*pos != '}') {
				if (*pos == '\0')
					break;

				pos++;
			}
		}
		else {
			bufidx++;

			if (bufidx >= sizeof(buf) - 1) {
				bufidx = 0;

				num++;
			}

			pos++;
		}
	}

	format->num_widgets = num;
	format->widgets = malloc(format->num_widgets*sizeof(widget_t));

	pos = fmt;
	while (*pos != '\0') {
		if (*pos == '%') {
			if (bufidx > 0) {
				buf[bufidx] = '\0';
				bufidx = 0;

				widget_text(buf, &format->widgets[idx])

				idx++;
			}

			sscanf(pos, "%%{%255[^}]}", buf);

			widget_parse(buf, &format->widgets[idx]);

			idx++;

			while(*pos != '}') {
				if (*pos == '\0')
					break;

				pos++;
			}
		}
		else {
			buf[bufidx] = *pos;
			bufidx++;

			if (bufidx >= sizeof(buf) - 1) {
				buf[bufidx] = '\0';
				bufidx = 0;

				widget_text(buf, &format->widgets[idx])

				idx++;
			}

			pos++;
		}
	}

	return format;
}

void format_poll(format_t * format, int timeout, char * buf, size_t size) {
}

void format_free(format_t * format) {
	for (int widget = 0; widget < format->num_widgets; widget++)
		free(format->widgets[widget]);

	free(format);
}
