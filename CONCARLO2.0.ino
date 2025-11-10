#include <Servo.h>
#include <Wire.h>
#include <PS2X_lib.h>
unsigned long thoigian;
int khoangcach;
Servo myServo;
Servo servo;
#define PS2_DAT 13  // data
#define PS2_CMD 11  //command
#define PS2_CS 10   // attention
#define PS2_CLK 12  //clock
#define LED 2       // LED thông báo
#define servoPin 3
#define ENA 8
#define IN_1 7
#define IN_2 6
#define IN_3 5
#define IN_4 4
#define ENB 9
#define irRight_BN1 A0
#define irRight_BN2 A1
#define irLEFT_BN3 A2
#define irLEFT_BN4 A3
#define trig A4
#define echo A5
//#define pressures   true
#define pressures false
//#define rumble      true
#define rumble false
PS2X ps2x;  // tạo PS2 điều khiển lớp
int error = 0;
byte type = 0;
byte vibrate = 0;
//Biến cho analog
int temp1;
int temp2;
int temp3;
int temp4;
int temp33;
int temp44;
bool Ps2 = false;
bool Auto = false;
bool Right;
bool Left;
int Speed;
int Speed2;
void setup() {
  Serial.begin(57600);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  myServo.attach(servoPin);
  pinMode(ENA, OUTPUT);
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(IN_3, OUTPUT);
  pinMode(IN_4, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(irRight_BN1, INPUT);
  pinMode(irRight_BN2, INPUT);
  pinMode(irLEFT_BN3, INPUT);
  pinMode(irLEFT_BN4, INPUT);
  pinMode(LED, OUTPUT);
  myServo.write(90);
  myServo.writeMicroseconds(1500);
  delay(300);
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_CS, PS2_DAT, pressures, rumble);
  if (error == 0) {
    Serial.print("Đã tìm thấy bộ điều khiển ");
    Serial.print("pressures = ");
    if (pressures)
      Serial.println("true ");
    else
      Serial.println("false");
    Serial.print("rumble = ");
    if (rumble)
      Serial.println("true)");
    else
      Serial.println("false");
    Serial.println("Hãy thử tất cả các nút;");
    Serial.println("giữ L1 hoặc R1 sẽ in ra các giá trị analog.");
  } else if (error == 1)
    Serial.println("Không kết nối đc, thử lại...");

  else if (error == 2)
    Serial.println("Bộ điều khiển tìm thấy nhưng không chấp nhận lệnh");

  else if (error == 3)
    Serial.println("Bộ điều khiển từ chối để vào chế độ Pressures ");

  //  Serial.print(ps2x.Analog(1), HEX);
  type = ps2x.readType();
  switch (type) {
    case 0:
      Serial.print("Tay điều khiển không phù hợp ");
      break;
    case 1:
      Serial.print("Đã tìm thấy DualShock ");
      break;
    case 2:
      Serial.print("Đã tìm thấy GuitarHero ");
      break;
    case 3:
      Serial.print("Không dây của Sony DualShock điều khiển tìm thấy ");
      break;
  }
}

void moveForward() {  // Tiến
  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
}

void moveBackward() {  //Lui
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
}

void turnRight() {  //Quẹo trái
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
}

void turnLeft() {  //Quẹo phải
  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
}

void stopMoving() {  //Dừng
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, LOW);
  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, LOW);
}
int calculatekhoangcach() {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  // Phát xung siêu âm với thời gian là 10 Micro giây
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);  // Ngưng phát xung
  thoigian = pulseIn(echo, HIGH);
  khoangcach = int(thoigian / 2 / 29.412);  // Tính khoảng cách từ siêu âm đến vật cản ( vì vận tốc của xung siêu âm là 344m/s...và sau khi đập vào vật cản rồi dội lại, nên /2) ==>> khoảng cách = thời gian*vận tốc/2
  return khoangcach;
}
void CheckMap() {
  ps2x.read_gamepad();

  for (int i = 90; i <= 180; i += 5) {
    Serial.print("khoangcach: ");
    Serial.print(khoangcach);
    Serial.println("cm");
    delay(50);
    int pulse = map(i, 0, 180, 1000, 2000);
    myServo.writeMicroseconds(pulse);
    delay(15);
    khoangcach = calculatekhoangcach();  // Gọi hàm tính khoảng cách
    Serial.print(i);                     // Gửi giá trị i ( góc quay của Servo) đến Serial Port
    Serial.print(",");                   // Gửi ","
    Serial.print(khoangcach);            // Tiếp theo gửi các giá trị khoảng cách tới Serial Port
    Serial.print(".");
    Serial.println(pulse);
    delay(500);
    // Hiển thị trạng thái
    if (i >= 130) {
      if (khoangcach < 15) {
        Right = true;
        Left = false;
        break;
      }
    } else {
      Right = false;
      Left = false;
      stopMoving();
    }
  }
  if (Right == true && Left == false) {
    turnRight();
    delay(100);
    Serial.print("Xoay phải");
  }
  for (int i = 0; i < 90; i += 5) {
    Serial.print("khoangcach: ");
    Serial.print(khoangcach);
    Serial.println("cm");
    delay(50);
    int pulse = map(i, 0, 180, 1000, 2000);
    myServo.writeMicroseconds(pulse);
    delay(15);
    khoangcach = calculatekhoangcach();
    Serial.print(i);           // Gửi giá trị i ( góc quay của Servo) đến Serial Port
    Serial.print(",");         // Gửi ","
    Serial.print(khoangcach);  // Tiếp theo gửi các giá trị khoảng cách tới Serial Port
    Serial.print(".");
    Serial.println(pulse);
    if (i <= 30) {
      if (khoangcach < 15) {
        Left = true;  // turnRight();  // Hoặc turnLeft();
        Right = false;
        break;
      }
    } else {
      Right = false;
      Left = false;
      stopMoving();
    }
  }

  if (Left == true && Right == false) {
    turnLeft();
    delay(100);
    Serial.print("Xoay trái");
  } else if (Right == true && Left == true) {
    moveBackward();
    delay(500);
    turnRight();
    delay(100);
    Serial.print("Chuyển hướng");
    Right = false;
    Left = false;
  } else {
    Right = false;
    Left = false;
  }

  delay(500);
}
void CONTROLPS2() {

  //Cho về 0 xét lại
  temp1 = 0;  // LUI
  temp2 = 0;  // TIẾN
  temp3 = 0;  // TRÁI
  temp4 = 0;  // PHẢI
  if (error == 1)
    return;

  if (type == 2) {
    ps2x.read_gamepad();
  } else {                              //DualShock Controller
    ps2x.read_gamepad(false, vibrate);  //read controller and set large motor to spin at 'vibrate' speed

    if (ps2x.Analog(PSS_LY) > 128) {
      temp1 = (ps2x.Analog(PSS_LY) - 128) * 2;
    } else if (ps2x.Analog(PSS_LY) < 128) {
      temp2 = abs(ps2x.Analog(PSS_LY) - 128) * 2;
      if (temp2 > 255)
        temp2 = 255;
    } else {
      temp2 = 0;
    }
    if (ps2x.Analog(PSS_RX) > 128) {
      temp3 = (ps2x.Analog(PSS_RX) - 128) * 2;
    } else if (ps2x.Analog(PSS_RX) < 128) {
      temp4 = abs(ps2x.Analog(PSS_RX) - 128) * 2;
      if (temp4 > 255)
        temp4 = 255;
    } else {
      temp4 = 0;
    }
    Serial.print("temp1: ");
    Serial.println(temp1);
    Serial.print("temp2: ");
    Serial.println(temp2);
    Serial.print("temp3: ");
    Serial.println(temp3);
    Serial.print("temp4: ");
    Serial.println(temp4);
  }
  //Quy định
  // + pin 3 - pin 5 1 động cơ bên trái quay tới
  // + pin 6 - pin 9 1 động cơ bên phải quay tới
  if ((temp2 > 2) && temp3 == 0 && temp4 == 0) {
    analogWrite(ENA, temp2);  //tiến ********************
    analogWrite(ENB, temp2);  //tiến **      / \       **
    moveForward();            //tiến **     /   \      **
                              //tiến **       |        **
                              //tiến **       |        **
                              //tiến ********************
    Serial.println("Xe di thang");
  } else {
    if ((temp1 > 0) && temp3 == 0 && temp4 == 0) {

      analogWrite(ENA, temp1);      //lui ********************
      analogWrite(ENB, temp1 + 1);  //lui **       |        **
      moveBackward();               //lui **       |        **
                                    //lui **     \   /      **
                                    //lui **      \ /       **
                                    //lui ********************
      Serial.println("Xe di lui");
    } else {
      if ((temp2 > 2) && temp4 > 0) {
        temp44 = (temp2 - temp4);
        if (temp44 < 0) {
          temp44 = 0;
        }
        analogWrite(ENA, temp2 * 2);
        analogWrite(ENB, temp44);
        moveForward();
        Serial.println("temp44");
        Serial.println(temp44);
        Serial.println("Xe queo Trai");
      } else {
        if ((temp2 > 2) && temp3 > 0) {
          temp33 = (temp2 - temp3);
          if (temp33 < 0) {
            temp33 = 0;
          }
          analogWrite(ENA, temp33);
          analogWrite(ENB, temp2);
          moveForward();
          Serial.println("temp33");
          Serial.println(temp33);
          Serial.println("temp3");
          Serial.println(temp3);
          Serial.println("Xe queo Phai");
        } else {
          if ((temp1 > 0) && temp4 > 0) {
            temp44 = (temp1 - temp4);
            if (temp44 < 0) {
              temp44 = 0;
            }
            analogWrite(ENA, temp1);
            analogWrite(ENB, temp44);
            moveBackward();
            Serial.println("temp44");
            Serial.println(temp44);
            Serial.println("temp4");
            Serial.println(temp4);
            Serial.println("temp1");
            Serial.println(temp1);
            Serial.println("Xe lui phai");
          } else {
            if ((temp1 > 0) && temp3 > 0) {
              temp33 = (temp1 - temp3);
              if (temp33 < 0) {
                temp33 = 0;
              }
              analogWrite(ENA, temp33);
              analogWrite(ENB, temp1 + 1);
              moveBackward();
              Serial.println("temp33");
              Serial.println(temp33);
              Serial.println("temp3");
              Serial.println(temp3);
              Serial.println("Xe lui Trai");
            } else {
              digitalWrite(IN_1, LOW);
              digitalWrite(IN_2, LOW);
              digitalWrite(IN_3, LOW);
              digitalWrite(IN_4, LOW);
            }
          }
        }
      }
    }
  }
}
void loop() {
  ps2x.read_gamepad();
  if (ps2x.ButtonPressed(PSB_RED)) {  //Nhấn nút O(RED) trên PS2 chuyển chế độ điều khiển bằng PS2
    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);
    delay(200);
    Serial.println("Circle just pressed");
    Ps2 = true;
    Auto = false;
    Serial.print("CHẾ ĐỘ ĐIỀU KHIỂN BẰNG PS2!!!!!!!!");
  }
  if (ps2x.ButtonReleased(PSB_PINK)) {  //Nhấn nút |_|(PINK) trên PS2 tắt các chế độ
    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);
    delay(200);
    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);
    delay(200);
    Serial.println("Square just released");
    stopMoving();
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
    delay(1000);
    Ps2 = false;
    Auto = false;
  }
  if (ps2x.NewButtonState(PSB_BLUE)) {  //Nhấn nút X(BLUE) trên PS2 chuyển chế độ auto
    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);
    delay(200);
    Serial.println("X just changed");
    Ps2 = false;
    Auto = true;
    Serial.print("Chế độ xe tự hành!!!!");
  }
  if (Ps2 == true && Auto == false) {
    CONTROLPS2();
  } else if (Auto == true && Ps2 == false) {  // Nhấn X(Blue) trên Ps2 ở chế độ tự hành
    analogWrite(ENB, Speed2);
    analogWrite(ENA, Speed);
    khoangcach = calculatekhoangcach();
    digitalWrite(trig, 0);
    delayMicroseconds(2);
    digitalWrite(trig, 1);
    delayMicroseconds(10);
    Serial.print("khoangcach: ");
    Serial.print(khoangcach);
    Serial.println("cm");

    delay(500);
    myServo.writeMicroseconds(1500);
    // if (khoangcach <= 35 && khoangcach > 10) {
    //   float ratio = (float)(khoangcach - 10) / (30 - 10);
    //   Speed = (int)(ratio * 255);
    //   Speed2 = (int)(ratio * 255);

    // }
    if (khoangcach <= 20) {
      stopMoving();
      CheckMap();
    } else {
      ps2x.read_gamepad();
      Speed = 255;
      Speed2 = 255;
      moveForward();
    }
  } else {
    Ps2 = false;
    Auto = false;
  }
}
