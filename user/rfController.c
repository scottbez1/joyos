// Include headers from OS
#include <joyos.h>

int usetup (void) {
    return 0;
}

packet_buffer buf;

#define S 0.9
float ax = 0;
float ay = 0;
float az = 0;

#define MID 350
#define VARIANCE 150

#define RANGE 1024

static inline int clamp(int x, int min, int max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}
int umain (void) {
    buf.type = POSITION;
    buf.board = 0;
    buf.seq_no = 0;
    buf.payload.coords[0].id=9;

    for(;;) {
        uint16_t x = analog_read(21);
        uint16_t y = analog_read(22);
        //uint16_t z = analog_read(23);

        ax = S*ax + (1-S)*x;
        ay = S*ay + (1-S)*y;
        //az = S*az + (1-S)*z;
        printf("%04f   %04f\n", (ax-MID)/VARIANCE,(ay-MID)/VARIANCE);
        int16_t valx = (int)((ax-MID)/VARIANCE*1024);
        int16_t valy = (int)((ay-MID)/VARIANCE*1024);
        valx = clamp(valx, -1024,1024);
        valy = clamp(valy, -1024,1024);
        buf.payload.coords[0].x = valx+1024;
        buf.payload.coords[0].y = valy+1024;
        buf.payload.coords[0].theta = frob_read();
        cli();
        LED_COMM(1);
        rf_send_packet(0xE7, &buf, sizeof(buf));
        LED_COMM(0);
        sei();
        pause(10);
    }

    return 0;
}
