#define P1 32
#define D1 33

#define P2 26
#define D2 25

int speed = 255;

void setup() {
  Serial.begin(115200);

  pinMode(P1, OUTPUT);
  pinMode(D1, OUTPUT);

  pinMode(P2, OUTPUT);
  pinMode(D2, OUTPUT);
  
}

char input = ' ';

void loop() {
  if (Serial.available() > 0) {
    input = Serial.read();
    Serial.println(input);
    switch (input){
      case 'w':
        digitalWrite(D1, HIGH);
        digitalWrite(D2, HIGH);


        analogWrite(P1, speed);
        analogWrite(P2, speed);
        Serial.println("forward");
        break;

      case 's':
        digitalWrite(D1, LOW);
        digitalWrite(D2, LOW);


        analogWrite(P1, speed);
        analogWrite(P2, speed);

        Serial.println("backward");
        break;
      case 'd':
        digitalWrite(D1, HIGH);
        digitalWrite(D2, LOW);
        


        analogWrite(P1, speed);
        analogWrite(P2, speed);

        Serial.println("right");
        break;
      case 'a':
        digitalWrite(D1, LOW);
        digitalWrite(D2, HIGH);


        analogWrite(P1, speed);
        analogWrite(P2, speed);

        Serial.println("left");
        break;
      case 'x':
        analogWrite(P1, 0);
        analogWrite(P2, 0);

        Serial.println("stopped");
        break;
    }
  }
}
