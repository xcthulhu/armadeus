#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <net/if.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#ifndef PF_CAN
#define PF_CAN 29
#endif

#ifndef AF_CAN
#define AF_CAN PF_CAN
#endif

#define SW2_MASK 0x01 /* Mask of the SW2 bit */

int listen_mode(int can_socket);
int led_flash(int can_socket);
int led_on(int can_socket);
int led_off(int can_socket);
int sw2_get(int can_socket);
int read_config(int can_socket);
void display_help();
void int_quit(int sig);

/* Global variable */
int quit;

int main(int argc, char **argv)
{
    struct ifreq ifr;
    struct sockaddr_can addr;
    int can_socket;

    quit=0;
    signal(SIGINT, int_quit); /* Capture the ctrl+C sequence */

    /* If no arguments is given */
    if (argc < 2) {
        display_help();
        return EXIT_SUCCESS;
    }

    /* Interface selection on can0 */
    strcpy(ifr.ifr_name, "can0");

    /* Socket creation */
    if ((can_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0 ) {
        perror("socket");
        return EXIT_FAILURE;
    }

    /* Association of the socket with the interface */
    ioctl(can_socket, SIOCGIFINDEX, &ifr);

    /* Socket param */
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    /* Bind of the socket */
    if(bind(can_socket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "listen") == 0) {
        listen_mode(can_socket);/* Entering in listen mode */
    } else if(strcmp(argv[1], "led-flash") == 0) {
        led_flash(can_socket);/* Entering in led flashing mode */
    } else if(strcmp(argv[1], "led-on") == 0) {
        led_on(can_socket);/* Entering in led on mode */
    } else if(strcmp(argv[1], "led-off") == 0) {
        led_off(can_socket);/* Entering in led off mode */
    } else if(strcmp(argv[1], "sw2-get") == 0) {
        sw2_get(can_socket);/* Entering in get state of the SW2 switch mode */
    } else if(strcmp(argv[1], "config") == 0) {
        read_config(can_socket);
    } else {
        display_help();/* Display display_help */
    }

    close(can_socket);

    return EXIT_SUCCESS;
}

int listen_mode(int can_socket)
{
    struct can_frame receivedCanFrame;
    int i, size;

    while (quit != 1) {
        /* Read the data received by the interface */
        if ((size = read(can_socket, &receivedCanFrame, sizeof(receivedCanFrame))) < 0) {
            perror("read");
            return size;
        }

        if (receivedCanFrame.can_id & CAN_RTR_FLAG) {/* If the last data received was a remote request frame */
            printf("ID: 0x%X is a remote request of %d byte(s) ", (receivedCanFrame.can_id ^ CAN_RTR_FLAG), receivedCanFrame.can_dlc);
        }
        else {/* If the last frame received was a data frame */
            printf("ID: 0x%X -> %d byte(s): ", receivedCanFrame.can_id, receivedCanFrame.can_dlc);

            for (i= 0; i<receivedCanFrame.can_dlc; i++)
                printf("0x%X ",receivedCanFrame.data[i]);
        }
        printf("\n");
    }

    return EXIT_SUCCESS;
}

int led_flash(int can_socket)
{
    int size;

    struct can_frame frameToSend;

    frameToSend.can_id = 0x500; /* ID */
    frameToSend.can_dlc = 3; /* We want to send 3 bytes */
    frameToSend.data[0] = 0x1E; /* Byte 1 */
    frameToSend.data[1] = 0x10; /* Byte 2 */
    frameToSend.data[2] = 0x10; /* Byte 3 */

    while (quit != 1) {
        frameToSend.data[2] ^= 0x10; /* Invert the output bit of the LED */

        if ((size = write(can_socket, &frameToSend, sizeof(frameToSend))) != sizeof(frameToSend)) {
            perror("led_flash write");
            return size;
        }

        usleep(200000); /* Wait 200ms ... */
    }

    return EXIT_SUCCESS;
}

int sw2_get(int can_socket)
{
    int size;

    struct can_frame frameToSend;
    struct can_frame receivedCanFrame;

    frameToSend.can_id = 0x280 | CAN_RTR_FLAG; /* Remote request frame */
    frameToSend.can_dlc = 8; /* The data received must contains 8 byte of datas */

    while (quit != 1) {
        /* Send the RTR frame */
        if ((size = write(can_socket, &frameToSend, sizeof(frameToSend))) != sizeof(frameToSend)) {
            perror("sw2_get write");
            return size;
        }

        /* Read the frame containing the information */
        if ((size = read(can_socket, &receivedCanFrame, sizeof(receivedCanFrame))) < 0) {
            perror("sw2_get read");
            return size;
        }


        if ((receivedCanFrame.data[1] & SW2_MASK) == 1) /* If the SW2 bit is on, */
            printf("SW2 is released\n"); /* the SW2 switch is released */
        else
            printf("SW2 is pushed\n");/* the SW2 switch is pushed */

        /* Wait 1s */
        sleep(1);
    }

    return EXIT_SUCCESS;
}

int led_on(int can_socket)
{
    int size;

    struct can_frame frameToSend;

    frameToSend.can_id = 0x500; /* ID */
    frameToSend.can_dlc = 3; /* We want to send 3 bytes */
    frameToSend.data[0] = 0x1E; /* Byte 1 */
    frameToSend.data[1] = 0x10; /* Byte 2 */
    frameToSend.data[2] = 0x00; /* Byte 3 (bit 4 = 0 to turn the led ON) */

    /* Send the frame */
    if ((size = write(can_socket, &frameToSend, sizeof(frameToSend))) != sizeof(frameToSend)) {
        perror("led_on write");
        return size;
    }

    return EXIT_SUCCESS;
}

int led_off(int can_socket)
{
    int size;

    struct can_frame frameToSend;

    frameToSend.can_id = 0x500; /* ID */
    frameToSend.can_dlc = 3; /* We want to send 3 bytes */
    frameToSend.data[0] = 0x1E; /* Byte 1 */
    frameToSend.data[1] = 0x10; /* Byte 2 */
    frameToSend.data[2] = 0x10; /* Byte 3 (bit 4 = 1 to turn the led OFF) */

    /* Send the frame */
    if ((size = write(can_socket, &frameToSend, sizeof(frameToSend))) != sizeof(frameToSend)) {
        perror("led_off write");
        return size;
    }

    return EXIT_SUCCESS;
}

int read_config(int can_socket)
{
    struct can_frame frameToSend;
    struct can_frame receivedCanFrame;

    int i,j, size;

    for (i = 0; i < 7; i++) {

        frameToSend.can_id = 0x280 | CAN_RTR_FLAG |  i; /* Remote request frame */
        switch (i) {

        case 1 :
            frameToSend.can_dlc = 7;
            break;

        case 2 :
            frameToSend.can_dlc = 5;
            break;

        case 3 :
            frameToSend.can_dlc = 3;
            break;

        case 4 :
            frameToSend.can_dlc = 6;
            break;

        default :
            frameToSend.can_dlc = 8;
        }

        /* Send the RTR frame */
        if ((size = write(can_socket, &frameToSend, sizeof(frameToSend))) != sizeof(frameToSend)) {
            perror("read_config write");
            return size;
        }

        /* Read the frame containing the information */
        if ((size = read(can_socket, &receivedCanFrame, sizeof(receivedCanFrame))) < 0) {
            perror("read_config read");
            return size;
        }

        printf("ID: 0x%X -> %d byte(s): ", receivedCanFrame.can_id, receivedCanFrame.can_dlc);

        for (j= 0; j<receivedCanFrame.can_dlc; j++)
            printf("0x%X ",receivedCanFrame.data[j]);

        printf("\n");
    }

    return EXIT_SUCCESS;
}



void display_help()
{/* Display display_help */
    printf("#################################\n");
    printf("#                               #\n");
    printf("#        Armadeus Systems       #\n");
    printf("#        Demo can_mcp25020      #\n");
    printf("#                               #\n");
    printf("#################################\n\n");
    printf("This sofware is a demonstration. It show how to communicate with a MICROCHIP MCP25020 using the can bus with socketCAN\n\n");
    printf("USE: can_mcp25020 [TEST]\nTEST are:\n");
    printf("\tlisten : Listen on the can bus\n");
    printf("\tled-flash : flash the D6 led (interval: 200ms)\n");
    printf("\tled-on : turn the D6 led on\n");
    printf("\tled-off : turn the D6 led off\n");
    printf("\tsw2-get : get the value of the switch SW2\n");
    printf("\tCONFIG : get the value of several config registers\n\n");
    printf("\tMore informations, please contact joly.kevin25@gmail.com\n");
}

void  int_quit(int sig)
{
    signal(sig, SIG_IGN); /* Ignore the ctrl+c sequence for the system */
    printf("Exiting...\n");
    quit=1;
}
