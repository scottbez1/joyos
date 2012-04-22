// Include headers from OS
#include <joyos.h>
#include <math.h>

#define LSB_US_PER_DEG 1400000

extern volatile uint8_t robot_id;

// usetup is called during the calibration period. 
int usetup (void) {
    gyro_init(20, LSB_US_PER_DEG, 1000);
    robot_id = 9;
    return 0;
}

float wrap_angle(float deg) {
    return deg - 360*floor(0.5 + deg/(360));
}

int clamp(int x, int min, int max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

// Entry point to contestant code.
int umain (void) {

    int strafe_v = 0;
    int ROT_GAIN = 5;

    float theta_desired = 0;
    float theta_strafe = 45;

    while(1) {
        copy_objects();
        if (get_time_us() - position_microtime[0] > 500000) {
            motor_set_vel(0,0);
            motor_set_vel(1,0);
            motor_set_vel(2,0);
            motor_set_vel(3,0);
            pause(400);
            yield();
            continue;
        }

        int x = -(objects[0].x-1024);
        int y = objects[0].y-1024;

        //theta_strafe = atan2(objects[2].y - objects[0].y, objects[2].x - objects[0].x) * 180 / M_PI;
        theta_strafe = atan2(y,x) * 180 / M_PI;
        strafe_v = clamp( (abs(x) + abs(y))/5, 0, 200);
        theta_desired = (objects[0].theta - 512) * 0.35;


        float theta_head = gyro_get_degrees();//(float)objects[0].theta * 180. / 2047.;
        
        float angle_error = wrap_angle(theta_head - theta_desired);
        
        int rot_v = clamp((int)(angle_error * ROT_GAIN), -150, 150);

        
        float theta_head_strafe_rad = wrap_angle(theta_head - theta_strafe) / 180 * M_PI;

        int L = rot_v  + strafe_v * cos(theta_head_strafe_rad);
        int R = -rot_v + strafe_v * cos(theta_head_strafe_rad);
        int F = rot_v  + strafe_v * sin(theta_head_strafe_rad);
        int B = -rot_v + strafe_v * sin(theta_head_strafe_rad);

        motor_set_vel(1, clamp(L,-255,255));
        motor_set_vel(0, clamp(R,-255,255));
        motor_set_vel(3, clamp(-F,-255,255));
        motor_set_vel(2, clamp(-B,-255,255));

        yield();
    }

    // Will never return, but the compiler complains without a return
    // statement.
    return 0;
}
