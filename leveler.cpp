#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <BluetoothSerial.h>
#include <ili9341.h>
#define BT_NAME "ESP32" //Can be changed to any name you like

BluetoothSerial ESP_BT;
int song = 0;
int played = 0;

Adafruit_MPU6050 mpu;
int buf[8] = { 0 };
void setdevice() {
    delay(20);
    buf[0] = 0x7e;
    buf[1] = 0xff;
    buf[2] = 0x06;
    buf[3] = 9;
    buf[4] = 0x00;//0x00 = no feedback, 0x01 = feedback
    buf[5] = 0;
    buf[6] = 2;
    buf[7] = 0xef; //ending byte
}

void set() {
    delay(20);
    buf[0] = 0x7e; //starting byte
    buf[1] = 0xff; //version
    buf[2] = 0x06; //the number of bytes of the command without starting byte and ending byte
    buf[3] = 9;
    buf[4] = 0x00;//0x00 = no feedback, 0x01 = feedback
    buf[5] = 0;
    buf[6] = 2;
    buf[7] = 0xef; //ending byte
    for (int i = 0; i < 8; i++)
    {
        Serial2.write(buf[i]);
    }
}
void sendwords()
{
    for (int i = 0; i < 8; i++)
    {
        Serial2.write(buf[i]);
    }
}

void playsong(int song)
{
    delay(20);
    buf[0] = 0x7e; //starting byte
    buf[1] = 0xff; //version
    buf[2] = 0x06;
    buf[3] = 3;
    buf[4] = 0x00;//0x00 = no feedback, 0x01 = feedback
    buf[5] = 0;
    buf[6] = song;
    buf[7] = 0xef; //ending byte
    for (int i = 0; i < 8; i++)//
        Serial2.write(buf[i]);
}

void setup(void) {
    tft.init();
    tft.setRotation(1);
    delay(20);
    Serial.begin(115200);
    Serial2.begin(9600);//RXD2=16; TXD2=17
    set();//select the TF card  
    setdevice();
    ESP_BT.begin("peles1.0");
    while (!Serial)
        delay(10); // will pause Zero, Leonardo, etc until serial console opens

    Serial.println("Adafruit MPU6050 test!");

    // Try to initialize!
    if (!mpu.begin()) {
        Serial.println("Failed to find MPU6050 chip");
        while (1) {
            delay(10);
        }
    }


    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);

    mpu.setGyroRange(MPU6050_RANGE_500_DEG);


    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);


    Serial.println("");
    delay(100);
}

void loop() {
    delay(10);
    /* Get new sensor events with the readings */
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    /* Print out the values */
    Serial.print("Acceleration X: ");
    Serial.print(a.acceleration.x);

    Serial.println(" m/s^2");
    if (ESP_BT.available() == 0);
    ESP_BT.println(a.acceleration.x * 9);
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(100, 100);
    tft.setTextSize(4);
    tft.print(a.acceleration.x * 9);


    float deg = a.acceleration.x * 9;

    if (deg > 6 && deg < 25) {
        //close lift left side up

        song = 1;
    }
    if (deg > 25) {
        //right

        song = 2;
    }
    if (deg < -6 && deg > -25) {
        //close lift left side up

        song = 3;
    }
    if (deg > -5 && deg < 5) {
        //bool

        song = 4;
    }
    if (deg < -25) {
        //left

        song = 5;
    }

    if (song != played) {
        played = song;
        playsong(song);
        sendwords();
    }
}