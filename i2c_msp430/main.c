#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <string.h>
#include <libdigiapix/i2c.h>

#define I2C_TIMEOUT 1

#define I2C_BUS_ALIAS            "DEFAULT_I2C_BUS"
#define I2C_REGISTER_WRITE       0x40
#define I2C_REGISTER_READ        0x41
#define DEFAULT_I2C_ADDRESS      0x20

static i2c_t *i2c_bus;

/* Initialize the I2C bus */
static int init_i2c(void) {
    int i2c_bus_nb = ldx_i2c_get_bus(I2C_BUS_ALIAS);
    if (i2c_bus_nb < 0) {
        printf("Failed to get I2C bus\n");
        return EXIT_FAILURE;
    }

    i2c_bus = ldx_i2c_request(i2c_bus_nb);
    if (!i2c_bus) {
        printf("Failed to initialize I2C\n");
        return EXIT_FAILURE;
    }

    if (ldx_i2c_set_timeout(i2c_bus, I2C_TIMEOUT) != EXIT_SUCCESS) {
        printf("Failed to set I2C timeout\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/*
 * Write N bytes from the I2C slave.
 * slave_address: Slave Address.
 * data: Buffer write data.
 * len: lenght buffer data
 */
static int write_register(uint8_t slave_address, uint8_t *data, int len) {
    uint8_t buffer[len + 1];
    buffer[0] = I2C_REGISTER_WRITE;  // First byte is the register address
    for (int i = 0; i < len; i++) {
        buffer[i + 1] = data[i];
    }

    if (ldx_i2c_write(i2c_bus, slave_address, buffer, len + 1) != EXIT_SUCCESS) {
        printf("Failed to write data to I2C\n");
        return EXIT_FAILURE;
    }

    printf("Data written to register 0x40 successfully\n");
    return EXIT_SUCCESS;
}

/*
 * Read N bytes from the I2C slave.
 * slave_address: Slave Address.
 * data: Buffer read data.
 * len: lenght buffer data
 */
static int read_register(uint8_t slave_address, uint8_t *data, int len) {
    uint8_t command = I2C_REGISTER_READ;

    if (ldx_i2c_write(i2c_bus, slave_address, &command, 1) != EXIT_SUCCESS) {
        printf("Failed to send read command\n");
        return EXIT_FAILURE;
    }

    if (ldx_i2c_read(i2c_bus, slave_address, data, len) != EXIT_SUCCESS) {
        printf("Failed to read data from I2C\n");
        return EXIT_FAILURE;
    }

    printf("Data read from register 0x41 successfully\n");
    return EXIT_SUCCESS;
}

static const char *short_options = "a:d:l:wr";
static struct option long_options[] = 
{
	{"address", required_argument, 0, 'a'},
	{"data",    required_argument, 0, 'd'},
	{"len",     required_argument, 0, 'l'},
	{"write",   no_argument,       0, 'w'},
	{"read",    no_argument,       0, 'r'},
	{0, 0, 0, 0}
};

/*
 * cleanup() - Frees all the allocated memory before exiting
 */
static void cleanup(void)
{
	/* Free i2c */
	ldx_i2c_free(i2c_bus);
}

/*
 * sigaction_handler() - Handler to execute after receiving a signal
 *
 * @signum:	Received signal.
 */
static void sigaction_handler(int signum)
{
	/* 'atexit' executes the cleanup function */
	exit(EXIT_FAILURE);
}

/*
 * register_signals() - Registers program signals
 */
static void register_signals(void)
{
	struct sigaction action;

	action.sa_handler = sigaction_handler;
	action.sa_flags = 0;
	sigemptyset(&action.sa_mask);

	sigaction(SIGHUP, &action, NULL);
	sigaction(SIGINT, &action, NULL);
	sigaction(SIGTERM, &action, NULL);
}

int main(int argc, char **argv) 
{
    uint8_t slave_address = DEFAULT_I2C_ADDRESS;
    uint8_t *data = NULL;
	int operation = 0; // 1 for write, 2 for read
    int len = 0;
    int opt;

	fprintf(stderr, "Usage: %s [-a address] [-d data] [-l len] [-w (write)] [-r (read)]\n", argv[0]);
	fprintf(stderr, "Example:\n");
	fprintf(stderr, "Read: [-a address] [-r] [-l len] \n");
	fprintf(stderr, "%s -a 0x20 -r -l 10\n", argv[0]);
	fprintf(stderr, "Write: [-a address] [-w] [-d data][-l len]\n");
	fprintf(stderr, "%s -a 0x20 -w -d DEAD -l 2\n", argv[0]);

	if(argc < 2){
		fprintf(stderr, "Not parameter\n");
		return EXIT_FAILURE;
	}

    while ((opt = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
        switch (opt) {
            case 'a':
                slave_address = (uint8_t)strtol(optarg, NULL, 16);
                break;
            case 'd':
                len = strlen(optarg) / 2; // Assuming data is provided as hex string
                data = malloc(len);
                for (int i = 0; i < len; i++) {
                    sscanf(optarg + 2*i, "%2hhx", &data[i]);
                }
                break;
            case 'l':
                len = atoi(optarg);
                break;
            case 'w':
                operation = 1; // Write operation
                break;
            case 'r':
                operation = 2; // Read operation
                break;
            default:
                fprintf(stderr, "Usage: %s [-a address] [-d data] [-l len] [-w (write)] [-r (read)]\n", argv[0]);
                return EXIT_FAILURE;
        }
    }

    if (operation == 0) {
        fprintf(stderr, "You must specify an operation (-w for write, -r for read).\n");
        return EXIT_FAILURE;
    }

    if (operation == 1 && data == NULL) {
        fprintf(stderr, "Data buffer is required for write operation. Use -d option.\n");
        return EXIT_FAILURE;
    }

	/* Register signals and exit cleanup function */
	atexit(cleanup);
	register_signals();

    if (init_i2c() != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    if (operation == 1) {
        if (write_register(slave_address, data, len) != EXIT_SUCCESS) {
            free(data);
            return EXIT_FAILURE;
        }
    } else if (operation == 2) {
        uint8_t read_data[len];
        if (read_register(slave_address, read_data, len) != EXIT_SUCCESS) {
            return EXIT_FAILURE;
        }

        printf("Received data: ");
        for (int i = 0; i < len; i++) {
            printf("0x%X ", read_data[i]);
        }
        printf("\n");
    }

	if (data){
		printf("Free data\n");
		free(data);
	}
    // ldx_i2c_free(i2c_bus);
    return EXIT_SUCCESS;
}