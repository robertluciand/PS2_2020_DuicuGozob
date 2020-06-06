#define LEFT_SENSORPIN 7
#define CENTER_SENSORPIN 8
#define RIGHT_SENSORPIN 9
#define MOTOR_SPEED 125
#define EN_A 10
#define EN_B 11
#define FORWARD 11
#define LEFT 12
#define RIGHT 13
#define STOP 14
#define PHOTORESISTOR_PIN A0
#define ECHO_PIN 5
#define TRIG_PIN 6
#define DELAY 10


class Led {
  private:
    int r, g, b;
  public:
    Led(int r, int g, int b) {
      this->r = r;
      this->g = g;
      this->b = b;
    }
    void red() {
      off();
      PORTD |= r;
    }
    void green() {
      off();
      PORTD |= g;
    }
    void blue() {
      off();
      PORTD |= b;
    }
    void yellow() {
      off();
      PORTD |= r | g;
    }
    void rgb() {
      PORTD |= r | g | b;
    }
    void off() {
      PORTD &= ~(r | g | b);
    }
};

Led *led = new Led((1 << 2), (1 << 3), (1 << 4));

void setup()
{
  Serial.begin(9600);
  // senzorul de linie
  pinMode(LEFT_SENSORPIN,INPUT);
  pinMode(CENTER_SENSORPIN,INPUT);
  pinMode(RIGHT_SENSORPIN,INPUT);
  // driver l298n
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(EN_A, OUTPUT);
  pinMode(EN_B, OUTPUT);
  analogWrite(EN_A, MOTOR_SPEED);
  analogWrite(EN_B, MOTOR_SPEED);
  // senzor cu fotorezistor si led rgb
  DDRD = (1 << 2) | (1 << 3) | (1 << 4);
  pinMode(PHOTORESISTOR_PIN, INPUT);
  // senzor ultrasunete
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void change_dir(int dir) {
  if(dir==FORWARD) {
    digitalWrite(A1, 1);
    digitalWrite(A2, 0);
    digitalWrite(A3, 1);
    digitalWrite(A4, 0);
  }
  else if(dir==LEFT) {
    digitalWrite(A1, 0);
    digitalWrite(A2, 0);
    digitalWrite(A3, 1);
    digitalWrite(A4, 0);
    delay(2);
  }
  else if(dir==RIGHT) {
    digitalWrite(A1, 1);
    digitalWrite(A2, 0);
    digitalWrite(A3, 0);
    digitalWrite(A4, 0);
    delay(2);
  }
  else if(dir==STOP) {
    digitalWrite(A1, 0);
    digitalWrite(A2, 0);
    digitalWrite(A3, 0);
    digitalWrite(A4, 0);
  }
}

void check_color() {
  led->red();
  delay(DELAY); // photoresistor lag time ~10ms - wikipedia
  int vred = analogRead(PHOTORESISTOR_PIN);
  
  led->yellow();
  delay(DELAY);
  int vyellow = analogRead(PHOTORESISTOR_PIN);

  led->blue();
  delay(DELAY);
  int vblue = analogRead(PHOTORESISTOR_PIN);

  led->off();

  if(vred >= 100 && vred <= 160 && vyellow >= 150 && vyellow <= 220 && vblue <= 120 && vblue >= 70 && vblue < vred && vyellow > vred) {
    Serial.println("red ");
    change_dir(STOP);
    delay(3000);
    change_dir(FORWARD);
    delay(750);
  }
  if(vyellow >= 280 && vyellow <=350 && vred >= 130 && vred <= 200 && vblue <= 110 && vblue >= 80 && vyellow > vblue && vyellow > vred) {
    Serial.println("yellow ");
    change_dir(RIGHT);
    delay(1200);
    change_dir(FORWARD);
    delay(250);
  }
  if(vblue >= 100 && vblue <= 150 && vred <= 50 && vblue > vred && vblue > vyellow) {
    Serial.println("blue ");
    change_dir(FORWARD);
  }
  /*Serial.print("R:");
  Serial.print(vred);
  Serial.print(" Y:");
  Serial.print(vyellow);
  Serial.print(" B:");
  Serial.println(vblue);*/
}

void adjust_direction() {
  byte leftSensor = !digitalRead(LEFT_SENSORPIN);
  byte centerSensor = !digitalRead(CENTER_SENSORPIN);
  byte rightSensor = !digitalRead(RIGHT_SENSORPIN);
  if(!leftSensor && !centerSensor && !rightSensor) {
    check_color();
  }
  if(leftSensor && !rightSensor) {
    change_dir(LEFT);
  }
  else if(!leftSensor && rightSensor) {
    change_dir(RIGHT);
  }
  else {
    change_dir(FORWARD);
  }
}

void check_obstacle() {
  long duration;
  int distance;

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration*0.034/2;

  // Serial.print("distance: ");
  // Serial.println(distance);
  if(distance < 15) {
    change_dir(LEFT);
    delay(500);
    change_dir(FORWARD);
    delay(500);
    change_dir(RIGHT);
    delay(500);
    change_dir(FORWARD);
  }
}

void loop()
{
  adjust_direction();
  check_obstacle();
}
