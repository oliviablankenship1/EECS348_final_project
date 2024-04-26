#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "eecs388_lib.h"

void auto_brake(int devid)
{
    // Task-1: 
    // Your code here (Use Lab 02 - Lab 04 for reference)
    // Use the directions given in the project document

    /*
    Distance                Action              LED Color
    ======================================================
    >200cm                  No breaking         Green
    100cm < d <= 200cm      Break lightly       Red and green
    60cm < d <= 100cm       Break hard          Red
    d <= 60cm               Must stop           Flashing red (100ms blinks)
    */

    uint8_t dist_L = 0;
    uint8_t dist_H = 0;
    uint16_t dist = 0;
    while (1) {
        if ('Y' == ser_read(devid) && 'Y' == ser_read(devid)) {

            dist_L = ser_read(devid);
            dist_H = ser_read(devid);
            dist = (dist_H << 8) | dist_L;

            if (dist > 200) {
                gpio_write(GREEN_LED, ON);
                gpio_write(RED_LED, OFF);
            } else if (200 > dist && dist >= 100) {
                gpio_write(GREEN_LED, ON);
                gpio_write(RED_LED, ON);
            } else if (100 > dist && dist > 60) {
                gpio_write(GREEN_LED, OFF);
                gpio_write(RED_LED, ON);
            } else {
                gpio_write(GREEN_LED, OFF);
                gpio_write(RED_LED, ON);
                delay(100);
                gpio_write(RED_LED, OFF);
                delay(100);
            }
            printf("d = %d \n", dist);
        }
    }
}
int read_from_pi(int devid)
{
    // Task-2: 
    // You code goes here (Use Lab 09 for reference)
    // After performing Task-2 at dnn.py code, modify this part to read angle values from Raspberry Pi.

    char buffer[10];
    int angle;
    ser_readline(devid, sizeof(buffer)/sizeof(buffer[0]), buffer);

    if (ser_isready(devid)) {
         sscanf(buffer, "%d", &angle);
        //  printf("From PI to HIFIVE: read(%d) => %d \n", read_data, angle);
         return angle;
    } else {
         return -1;
    }
}

void steering(int gpio, int pos)
{
    // Task-3: 
    // Your code goes here (Use Lab 05 for reference)
    // Check the project document to understand the task
    int SERVO_PULSE_MAX = 2400;
    int SERVO_PULSE_MIN = 544;
    int SERVO_PERIOD = 20000;

   int pulse = (((SERVO_PULSE_MAX - SERVO_PULSE_MIN)*pos) / 180) + SERVO_PULSE_MIN;
   gpio_write(gpio, ON);
   delay_usec(pulse);
   gpio_write(gpio, OFF);
   delay_usec(SERVO_PERIOD - pulse);
}

int main() { 
    // initialize UART channels
    ser_setup(0); // uart0
    ser_setup(1); // uart1
    int pi_to_hifive = 1; //The connection with Pi uses uart 1
    int lidar_to_hifive = 0; //the lidar uses uart 0
    
    printf("\nUsing UART %d for Pi -> HiFive", pi_to_hifive);
    printf("\nUsing UART %d for Lidar -> HiFive", lidar_to_hifive);
    
    //Initializing PINs
    gpio_mode(PIN_19, OUTPUT);
    gpio_mode(RED_LED, OUTPUT);
    gpio_mode(BLUE_LED, OUTPUT);
    gpio_mode(GREEN_LED, OUTPUT);

    printf("Setup completed.\n");
    printf("Begin the main loop.\n");

    while (1) {


        // printf("Entered while loop");
        auto_brake(lidar_to_hifive); // measuring distance using lidar and braking
        int angle = read_from_pi(pi_to_hifive); //getting turn direction from pi
        printf("\ta=%d\n", angle) 
        int gpio = PIN_19; 
        for (int i = 0; i < 5; i++){
            // Here, we set the angle to 180 if the prediction from the DNN is a positive angle
            // and 0 if the prediction is a negative angle.
            // This is so that it is easier to see the movement of the servo.
            // You are welcome to pass the angle values directly to the steering function.
            // If the servo function is written correctly, it should still work,
            // only the movements of the servo will be more subtle
            if(angle>0){
                steering(gpio, 180);
            }
            else {
                steering(gpio,0);
            }
            
            // Uncomment the line below to see the actual angles on the servo.
            // Remember to comment out the if-else statement above!
            // steering(gpio, angle);
        }

    }
    return 0;
}
