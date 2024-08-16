#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>

#define AT_COMMAND_FILE "/dev/smd7"

int open_file(char *filename, int mode) {
	int file = open(filename, mode);
	if (file < 0) {
		printf("Error: Could not open file %s\n", filename);
		return file;
	}
	return file;
}

int main(int argc, char *argv[]) {
	char *file_path = NULL;
	int file_in = 0, file_out = 0;
	int ret = 0;
	int i;
	int n;

	file_path = AT_COMMAND_FILE;
	if (argc == 2) {
		file_path = argv[1];
	}

	/* Set stdin non-blocking */
	int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

	/* Open /dev/smd7 */
	file_in = open_file(file_path, O_RDONLY);
	if (file_in < 0) {
		fprintf(stderr, "Error: Could not open AT command file\n");
		ret = 1;
		goto out_free;
	}
	flags = fcntl(file_in, F_GETFL, 0);
	fcntl(file_in, F_SETFL, flags | O_NONBLOCK);

	file_out = open_file(file_path, O_WRONLY);
	if (file_out < 0) {
		fprintf(stderr, "Error: Could not open AT command file\n");
		ret = 1;
		goto out_free;
	}

	while (1) {
		char buf[1024];

		/* Read non-blocking from file */
		n = read(file_in, buf, sizeof(buf));
		if (n < 0 && errno != EAGAIN) {
			ret = 1;
			fprintf(stderr, "Error: Could not read from AT command file\n");
			goto out_free;
		} else if (n > 0) {	
			buf[n] = '\0';
			fprintf(stdout, "%s", buf);
		}

		/* Read non-blocking from stdin */
		n = read(STDIN_FILENO, buf, sizeof(buf));
		if (n < 0) {
			if (errno == EAGAIN) 
				continue;
			ret = 1;
			fprintf(stderr, "Error: Could not read from AT command file\n");
			goto out_free;
		}
		
		/* Print all character codes */
		buf[n - 1] = '\r';
		buf[n] = '\n';

		//printf("Writing %d bytes to file\n", n);
		if (write(file_out, buf, n + 1) != n + 1) {
			fprintf(stderr, "Error: Could not write to AT command file\n");
			ret = 1;
			goto out_free;
		}
	}
	
out_free:
	if (file_in > 0) {
		close(file_in);
	}

	if (file_out > 0) {
		close(file_out);
	}
	return 0;
}