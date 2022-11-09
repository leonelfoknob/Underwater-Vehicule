
#include <SoftwareSerial.h>
#include <Servo.h>
/******************************** aracı şekil**********************
 *           *
 *          * *
 * M8  O---*   *---O M1
 *     |   *   *   |
 * M7   O  *   *  O  M2
 *     |   *   *   |
 * M6   O  *   *  O  M3
 *     |   *   *   |
 * M5  O---*   *---O M4
 *         *---*
 *  
 *  Motorlar isimlendirilmiş şekil
 *  
 *  
 */

//yeşil kablo rx
//mavi kablo tx

Servo M1;
Servo M2;
Servo M3;
Servo M4;
Servo M5;
Servo M6;
Servo M7;
Servo M8;

#define MIN_PULSE_LENGTH 1000
#define MAX_PULSE_LENGTH 2000

int sabit_hiz_ilere=140 ;
int sabit_hiz_geri=90 ;
int durmak_hizi=0 ;

unsigned char data_buffer1[4] = {0}; // Array to store incoming serial data
unsigned char data_buffer2[4] = {0}; // Array to store incoming serial data
int distance1 = 0;// Integer to store distance
unsigned char CS1;// Variable to hold checksum
int distance2 = 0;// Integer to store distance
unsigned char CS2;// Variable to hold checksum
int on_sensor_messafe;
int alt_sensor_messafe;

int su_sensor = A0;
int su_seviye_val;

void setup() {
  
  Serial.begin(115200);// Set up serial monitor
  Serial1.begin(9600);// Set up software serial port 1
  Serial2.begin(9600);// Set up software serial port 2
  M1.attach(2);
  M2.attach(3);
  M3.attach(4);
  M4.attach(5);
  M5.attach(6);
  M6.attach(7);
  M7.attach(8);
  M8.attach(9);
  
  delay(3000);
  M1.writeMicroseconds(MIN_PULSE_LENGTH);
  M2.writeMicroseconds(MIN_PULSE_LENGTH);
  M3.writeMicroseconds(MIN_PULSE_LENGTH);
  M4.writeMicroseconds(MIN_PULSE_LENGTH);
  M5.writeMicroseconds(MIN_PULSE_LENGTH);
  M6.writeMicroseconds(MIN_PULSE_LENGTH);
  M7.writeMicroseconds(MIN_PULSE_LENGTH);
  M8.writeMicroseconds(MIN_PULSE_LENGTH);
  delay(3000);
  M1.writeMicroseconds(MAX_PULSE_LENGTH);
  M2.writeMicroseconds(MAX_PULSE_LENGTH);
  M3.writeMicroseconds(MAX_PULSE_LENGTH);
  M4.writeMicroseconds(MAX_PULSE_LENGTH);
  M5.writeMicroseconds(MAX_PULSE_LENGTH);
  M6.writeMicroseconds(MAX_PULSE_LENGTH);
  M7.writeMicroseconds(MAX_PULSE_LENGTH);
  M8.writeMicroseconds(MAX_PULSE_LENGTH);
  delay(3000);
}

void loop() {

  // Run if data available
  if (Serial1.available() > 0 || Serial2.available() > 0) {

    delay(4);

    // Check for packet header character 0xff
    if (Serial1.read() == 0xff || Serial2.read() == 0xff) {
      // Insert header into array
      data_buffer1[0] = 0xff;
      data_buffer2[0] = 0xff;
      // Read remaining 3 characters of data and insert into array
      for (int i = 1; i < 4; i++) {
        data_buffer1[i] = Serial1.read();
        data_buffer2[i] = Serial2.read();
      }

      //Compute checksum
      CS1 = data_buffer1[0] + data_buffer1[1] + data_buffer1[2];
      CS2 = data_buffer2[0] + data_buffer2[1] + data_buffer2[2];
      // If checksum is valid compose distance from data
      if (data_buffer1[3] == CS1) { // distance avant
        distance1 = (data_buffer1[1] << 8) + data_buffer1[2];
      }
      if (data_buffer2[3] == CS2) { //distance du bas 
        distance2 = (data_buffer2[1] << 8) + data_buffer2[2];
      }
    }
  }
  
  on_sensor_messafe = distance1;
  alt_sensor_messafe = distance2;
  su_seviye_val = su_seviye_sensor();
  print_sensor_value(on_sensor_messafe,alt_sensor_messafe,su_seviye_val);
/**************************************************** CODE ****************************************************************************************/

  if(su_seviye_val >900){
    dur();
  }
  
  else{
    if(alt_sensor_messafe > 1000){
      asag();
    }
    else{
     dur_asag();
      if(on_sensor_messafe > 1500){
        ilere();
      }
      else{
        sag_ileri();
      }
    }
  }
}

//print sensor value funksyion
void print_sensor_value(int val1,int val2,int val3){
  Serial.print("On messafe sensor : ");
  Serial.print(val1);
  Serial.print(" mm");
  Serial.print(" | ");
  Serial.print("Alt messafe sensor : ");
  Serial.print(val2);
  Serial.print(" mm");
  Serial.print(" | ");
  Serial.print("su seviye : ");
  Serial.print(val3);
  Serial.println("seviye");
}
// Su seviye sensor fonksiyonu

int su_seviye_sensor(){
  int suseviye_val = analogRead(su_sensor);
  return suseviye_val;
}

//hareket fonksyonları

void ilere(){
  M1.write(sabit_hiz_ilere);
  M4.write(sabit_hiz_ilere);
  M5.write(sabit_hiz_ilere);
  M8.write(sabit_hiz_ilere);
  Serial.println(" Ilere yöne hareket ediyor...");
}

void  geri(){
  M1.write(sabit_hiz_geri);
  M4.write(sabit_hiz_geri);
  M5.write(sabit_hiz_geri);
  M8.write(sabit_hiz_geri);
  Serial.println(" geriye yöne hareket ediyor...");
}

void sag_ileri(){ //ilere hiz ile
  M1.write(durmak_hizi);
  M4.write(durmak_hizi);
  M5.write(sabit_hiz_ilere);
  M8.write(sabit_hiz_ilere);
  Serial.println(" Sağ ilere yöne hareket ediyor...");
}

void sag_geri(){//geri hiz ile
  M1.write(sabit_hiz_geri);
  M4.write(sabit_hiz_geri);
  M5.write(durmak_hizi);
  M8.write(durmak_hizi);
  Serial.println(" Sağ geriye yöne hareket ediyor...");
}

void spin_sag(){//spin saga geri
  M1.write(sabit_hiz_geri);
  M4.write(sabit_hiz_geri);
  M5.write(sabit_hiz_ilere);
  M8.write(sabit_hiz_ilere);
  Serial.println(" Sağ tarafa kendin etrafında dönüyor...");
}

void sol_ileri(){//ilere hiz ile
  M1.write(sabit_hiz_ilere);
  M4.write(sabit_hiz_ilere);
  M5.write(durmak_hizi);
  M8.write(durmak_hizi);
  Serial.println(" Sol ilere yöne hareket ediyor...");
}

void sol_geri(){ //geri hiz ile
  M1.write(durmak_hizi);
  M4.write(durmak_hizi);
  M5.write(sabit_hiz_geri);
  M8.write(sabit_hiz_geri);
  Serial.println(" Sol geriye yöne hareket ediyor...");
}

void spin_sol(){
  M1.write(sabit_hiz_ilere);
  M4.write(sabit_hiz_ilere);
  M5.write(sabit_hiz_geri);
  M8.write(sabit_hiz_geri);
  Serial.println(" Sol tarafa kendin etrafında dönüyor...");
}

void yukari(){
  M2.write(sabit_hiz_ilere);
  M3.write(sabit_hiz_ilere);
  M6.write(sabit_hiz_ilere);
  M7.write(sabit_hiz_ilere);
  Serial.println(" yukari yöne hareket ediyor...");
}

void asag(){
  M2.write(sabit_hiz_geri);
  M3.write(sabit_hiz_geri);
  M6.write(sabit_hiz_geri);
  M7.write(sabit_hiz_geri);
  Serial.println(" Aşağa yöne hareket ediyor...");
}
void dur_asag(){
  M2.write(durmak_hizi);
  M3.write(durmak_hizi);
  M6.write(durmak_hizi);
  M7.write(durmak_hizi);
  Serial.println("Batırma motorlar durdu ...");
}
void dur(){
  M1.write(durmak_hizi);
  M2.write(durmak_hizi);
  M3.write(durmak_hizi);
  M4.write(durmak_hizi);
  M5.write(durmak_hizi);
  M6.write(durmak_hizi);
  M7.write(durmak_hizi);
  M8.write(durmak_hizi);
  Serial.println(" Tüm motorla duruyor...");
}


// penser a ajouter les autres fonctions de control
