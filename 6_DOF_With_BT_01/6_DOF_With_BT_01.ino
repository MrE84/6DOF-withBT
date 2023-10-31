#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <SoftwareSerial.h>
 
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
SoftwareSerial bt(2,3); // TX, RX

const int MIN_PULSE_WIDTH = 500;
const int MAX_PULSE_WIDTH = 2500;
const int DEFAULT_PULSE_WIDTH = 1500;
const int FREQUENCY = 50;
const float FREQUENCY_SCALE = (float)FREQUENCY * 4096 / 1000000;

int pulseWidth(int angle) {
    int pulse_wide = map(angle, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
    int analog_value = int(pulse_wide * FREQUENCY_SCALE);
    Serial.println(analog_value);
    return analog_value;
}

void setup() {
    Serial.begin(9600);
    bt.begin(9600);

    pwm.begin();
    pwm.setPWMFreq(FREQUENCY);
    pwm.setPWM(1,0,pulseWidth(120));
    pwm.setPWM(2,0,pulseWidth(120));
    pwm.setPWM(3,0,pulseWidth(97));
    pwm.setPWM(4,0,pulseWidth(88));
    pwm.setPWM(5,0,pulseWidth(0));
    pwm.setPWM(6,0,pulseWidth(67));
}

void loop() {
    String command = "";

    if (Serial.available()) {
        command = Serial.readStringUntil('\n');
    } else if (bt.available()) {
        command = bt.readStringUntil('\n');
        Serial.println(command); // echo command from bt to Serial
    }

    if (command != "") {
        executeCommand(command);
    }
}

void executeCommand(String command) {
    struct ServoCommand {
        const char* name;
        int channel;
    };

    ServoCommand commands[] = {
        {"HIP ", 1},
        {"WAIST ", 2},
        {"SHOULDER ", 3},
        {"ELBOW ", 4},
        {"WRIST ", 5},
        {"CRAW ", 6}
    };

    for (const auto& cmd : commands) {
        if (command.startsWith(cmd.name)) {
            int angle = command.substring(strlen(cmd.name)).toInt();
            pwm.setPWM(cmd.channel, 0, pulseWidth(angle));
            Serial.println(String(cmd.name) + "moved to " + String(angle) + " degrees");
            return;
        }
    }

    Serial.println("Unknown command: " + command);
}
