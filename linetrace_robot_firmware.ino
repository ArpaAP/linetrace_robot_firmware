#include <SoftwareSerial.h>
#define TX_PIN 2
#define RX_PIN 3
#define TRIG 12
#define ECHO 13
#define IR_TRACE_LEFT 4
#define IR_TRACE_RIGHT 5
#define STATUS_LED 7

double duration, distance;

SoftwareSerial BTSerial(TX_PIN, RX_PIN);
int auto_enabled = 0;
int is_stopped = 0;

String splitString(String string, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = string.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(string.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found > index ? string.substring(strIndex[0], strIndex[1]) : "";
}

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(IR_TRACE_LEFT, INPUT);
  pinMode(IR_TRACE_RIGHT, INPUT);
}

void loop() {
  if (auto_enabled) {
    int ir_left = digitalRead(IR_TRACE_LEFT);
    int ir_right = digitalRead(IR_TRACE_RIGHT);

    Serial.print("IR: ");
    Serial.print(ir_left);
    Serial.println(ir_right);

    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);
  
    duration = pulseIn (ECHO, HIGH);
    distance = duration * 17 / 1000; 
  
    Serial.print("Distance : ");
    Serial.print(distance);
    Serial.println("cm");

    if (distance > 10) {
      is_stopped = 0;
      digitalWrite(STATUS_LED, LOW);
      if (ir_left == 0 && ir_right == 0) {
        digitalWrite(8, HIGH);
        digitalWrite(9, LOW);
        digitalWrite(10, HIGH);
        digitalWrite(11, LOW);
        delay(30);
        digitalWrite(10, LOW);
        digitalWrite(11, LOW);
        delay(5);
        digitalWrite(8, LOW);
        digitalWrite(9, LOW);
      }
      else if (ir_left == 0) {
        digitalWrite(8, HIGH);
        digitalWrite(9, LOW);
        delay(30);
        digitalWrite(8, LOW);
        digitalWrite(9, LOW);
      }
      else if (ir_right == 0) {
        digitalWrite(10, HIGH);
        digitalWrite(11, LOW);
        delay(30);
        digitalWrite(10, LOW);
        digitalWrite(11, LOW);
      }
    }
    else {
      if (is_stopped == 0) {
        is_stopped = 1;
        digitalWrite(STATUS_LED, HIGH);
        tone(6, 659.25);
        delay(200);
        noTone(6);
      }
      Serial.println("Too close. stop running");
    }
  }
  
  if (BTSerial.available()) {
    String data = BTSerial.readStringUntil('\n');
    data.replace("\r", "");
    Serial.println(data);
    
    do { 
      BTSerial.read();
    } while (BTSerial.available());
    

    String key = splitString(data, '=', 0);
    String value = splitString(data, '=', 1);

    Serial.print("KEY: ");
    Serial.println(key);
    Serial.print("VALUE: ");
    Serial.println(value);
    Serial.println();

    // 자동주행 모드 변경
    if (key == "automode") {
      auto_enabled = value.toInt();
    }

    if (key == "forward") {
      digitalWrite(8, HIGH);
      digitalWrite(9, LOW);
      digitalWrite(10, HIGH);
      digitalWrite(11, LOW);
      delay(200);
      digitalWrite(10, LOW);
      digitalWrite(11, LOW);
      delay(25);
      digitalWrite(8, LOW);
      digitalWrite(9, LOW);
    }
    if (key == "left") {
      digitalWrite(10, HIGH);
      digitalWrite(11, LOW);
      delay(200);
      digitalWrite(10, LOW);
      digitalWrite(11, LOW);
    }
    if (key == "right") {
      digitalWrite(8, HIGH);
      digitalWrite(9, LOW);
      delay(200);
      digitalWrite(8, LOW);
      digitalWrite(9, LOW);
    }
    if (key == "backward") {
      digitalWrite(8, LOW);
      digitalWrite(9, HIGH);
      digitalWrite(10, LOW);
      digitalWrite(11, HIGH);
      delay(200);
      digitalWrite(8, LOW);
      digitalWrite(9, LOW);
      delay(25);
      digitalWrite(10, LOW);
      digitalWrite(11, LOW);
    }
  }
  if (Serial.available()) {
    BTSerial.write(Serial.read());
  }
}
