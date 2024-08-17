#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>

#define BUFFER_SIZE			1024
#define BUFFER_READ_SIZE	BUFFER_SIZE - 1
#define AT_COMMAND_FILE		"/dev/smd7"

int open_file(char *filename, int mode) {
	int file = open(filename, mode);
	if (file < 0) {
		printf("Error: Could not open file %s\n", filename);
		return file;
	}
	return file;
}

void fd_nonblock(int fd) {
	int flags;

	flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main(int argc, char *argv[]) {
	char *file_path = NULL;
	int bytes_write_count = 0;
	int file_out = 0;
	int file_in = 0;
	int flags;
	int ret = 0;
	int i;
	int n;

	file_path = AT_COMMAND_FILE;
	if (argc == 2) {
		file_path = argv[1];
	}

	/* Open /dev/smd7 for reading */
	file_in = open_file(file_path, O_RDONLY);
	if (file_in < 0) {
		fprintf(stderr, "Error: Could not open AT command file\n");
		ret = 1;
		goto out_free;
	}

	/* Open /dev/smd7 for writing */
	file_out = open_file(file_path, O_WRONLY);
	if (file_out < 0) {
		fprintf(stderr, "Error: Could not open AT command file\n");
		ret = 1;
		goto out_free;
	}

	/* Set stdin non-blocking */
	fd_nonblock(STDIN_FILENO);

	/* Set AT-Interface non-blocking */
	fd_nonblock(file_in);

	while (1) {
		char buf[BUFFER_SIZE];

		/* Read non-blocking from file */
		n = read(file_in, buf, BUFFER_READ_SIZE);
		if (n < 0 && errno != EAGAIN) {
			ret = 1;
			fprintf(stderr, "Error: Could not read from AT command file\n");
			goto out_free;
		} else if (n > 0) {	
			buf[n] = '\0';
			fprintf(stdout, "%s", buf);
		}

		/* Read non-blocking from stdin */
		n = read(STDIN_FILENO, buf, BUFFER_READ_SIZE);
		if (n < 0) {
			if (errno == EAGAIN) 
				continue;
			ret = 1;
			fprintf(stderr, "Error: Could not read from AT command file\n");
			goto out_free;
		}
		
		/* Ensure proper termination */
		buf[n - 1] = '\r';
		buf[n] = '\n';

		/* Determine number of characters to write */
		bytes_write_count = n + 1;

		//printf("Writing %d bytes to file\n", n);
		if (write(file_out, buf, bytes_write_count) != bytes_write_count) {
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