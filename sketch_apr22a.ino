#include <Timers.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <Wire.h>

Timer timer;
Timer timer2;
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

//Przyciski
int przycisk_01 = 6;
int przycisk_02 = 7;
int przycisk_03 = 9;
int przycisk_04 = 8;

//Czujniki
int wilgotnosc = 10000;
//Odbiorniki
int pompa = 11;

int max_wilg_eeprom = 3;
int max_wilg = EEPROM.read(max_wilg_eeprom);
int min_wilgotnosc_eeprom = 0;
int min_wilgotnosc = EEPROM.read(min_wilgotnosc_eeprom);


int stan_menu = 0;
int licznik = 0;

bool zapis = false;
bool menu = true;
int czas_eeprom = 1;
int czas = EEPROM.read(czas_eeprom);
int l = 0;

void setup() {
  Serial.begin(9600);
  pinMode(przycisk_01, INPUT_PULLUP);
  pinMode(przycisk_02, INPUT_PULLUP);
  pinMode(przycisk_03, INPUT_PULLUP);
  pinMode(przycisk_04, INPUT_PULLUP);
  pinMode(pompa, OUTPUT);
  pinMode(A0, INPUT);
  //wilgotnosc = analogRead(A0);
  lcd.begin(16, 2);
  lcd.backlight(); 
  lcd.print("Witaj w GardenOS");
  lcd.setCursor(0, 1);
  lcd.print("");
  timer.begin(czas*1000);
}

void loop() {
  wilgotnosc = analogRead(A2);
  float wilg_po_przeliczeniu = (float(wilgotnosc)/float(max_wilg))*float(100);
  Serial.println(wilg_po_przeliczeniu);
  Serial.println("w:" + String(wilgotnosc));
  Serial.println("minw:" + String(min_wilgotnosc));
  Serial.println("maxw:" + String(max_wilg));
  Serial.println("czas:" + String(czas));
  if(menu == true) {
  if(digitalRead(przycisk_01) == LOW) {
    licznik++;
    if(licznik > 3) {
      licznik = 0;
    }
    delay(200);
  } 
  
    switch(licznik) {
      case 0:
        lcd.setCursor(0,1);
        lcd.print("   Kliknij #1   ");
        stan_menu = 0;
        break;
      case 1:
        lcd.setCursor(0,1);
        lcd.print(">wilgotnosc   ");
        if(digitalRead(przycisk_04) == LOW)
        {
          stan_menu = 1;
        }
        break;
      case 2:
        lcd.setCursor(0,1);
        lcd.print(">czas podlewania  ");
        if(digitalRead(przycisk_04) == LOW)
        {
          stan_menu = 3;
        }
        break;
      case 3:
        lcd.setCursor(0,1);
        lcd.print(">czujnik max          ");
        if(digitalRead(przycisk_04) == LOW)
        {
          stan_menu = 5;
        }
    }
  }
  if(digitalRead(przycisk_04) == LOW && stan_menu == 5 && licznik == 3) {
    menu = false;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Podaj wartosc:  ");
    lcd.setCursor(0,1);
    lcd.print(String(max_wilg) + "             ");
    stan_menu = 6;
  }
  while(digitalRead(przycisk_02) == LOW && stan_menu == 6 && licznik == 3) {
    menu = false;
    if(max_wilg >= 0) {
        max_wilg += 1;
        delay(100);
    }
    lcd.setCursor(0,1);
    lcd.print(String(max_wilg) + "          ");
    delay(100);
    zapis = true;
    EEPROM.write(max_wilg_eeprom, max_wilg);
    }
    while(digitalRead(przycisk_03) == LOW && digitalRead(przycisk_02) == HIGH && stan_menu == 6 && licznik == 3) {
      menu = false;
      if(max_wilg > 0) {
          max_wilg--;
          delay(100);
      }
      lcd.setCursor(0,1);
      lcd.print(String(max_wilg) + "          ");
      delay(100);
      zapis = true;
      EEPROM.write(max_wilg_eeprom, max_wilg);
      } 
  // ------------------------- WILGOTNOSC --------------------------------- 
  if(digitalRead(przycisk_04) == LOW && stan_menu == 1 && licznik == 1) {
    menu = false;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Podaj wartosc:  ");
    lcd.setCursor(0,1);
    lcd.print(String(min_wilgotnosc) + "%");
    stan_menu = 2;
  }
  while(digitalRead(przycisk_02) == LOW && stan_menu == 2) {
    menu = false;
    if(min_wilgotnosc > 100) {
      min_wilgotnosc = 100;
    }
    if(min_wilgotnosc < 100) {
        min_wilgotnosc += 1;
        delay(100);
    }
    if(min_wilgotnosc < 10) {
        lcd.setCursor(0,1);
        lcd.print("0");
        lcd.setCursor(1,1);
        lcd.print(min_wilgotnosc);
        lcd.setCursor(2,1);
        lcd.print("%    ");
    }
    else {
        lcd.setCursor(0,1);
        lcd.print(min_wilgotnosc);
        lcd.setCursor(2,1);
        lcd.print("%   ");
    }
    if(min_wilgotnosc == 100) {
      lcd.setCursor(0,1);
        lcd.print(min_wilgotnosc);
        lcd.setCursor(3,1);
        lcd.print("%   ");
    }
    delay(100);
    zapis = true;
    EEPROM.write(min_wilgotnosc_eeprom, min_wilgotnosc);
    }
  while(digitalRead(przycisk_03) == LOW && digitalRead(przycisk_02) == HIGH && stan_menu == 2) {
    menu = false;
    if(min_wilgotnosc > 0) {
        min_wilgotnosc -= 1;
        delay(100);
    }
    if(min_wilgotnosc < 10) {
      lcd.setCursor(0,1);
        lcd.print("0");
        lcd.setCursor(1,1);
        lcd.print(min_wilgotnosc);
        lcd.setCursor(2,1);
        lcd.print("%   ");
    }
    else {
    lcd.setCursor(0,1);
        lcd.print(min_wilgotnosc);
        lcd.setCursor(2,1);
        lcd.print("%   ");
    }
    if(min_wilgotnosc == 100) {
      lcd.setCursor(0,1);
        lcd.print(min_wilgotnosc);
        lcd.setCursor(3,1);
        lcd.print("%   ");
    }
    delay(100);
    zapis = true;
    EEPROM.write(min_wilgotnosc_eeprom, min_wilgotnosc);
    }
  if(digitalRead(przycisk_04) == LOW && zapis == true && (stan_menu == 2 || stan_menu == 4 || stan_menu == 6)) {
    lcd.clear();
    lcd.begin(16, 2);
    lcd.print("Witaj w GardenOS");
    lcd.setCursor(0, 1);
    lcd.print("Stan: zapisano");
    EEPROM.write(min_wilgotnosc_eeprom, min_wilgotnosc);
    delay(500);
    lcd.setCursor(0, 1);
    lcd.print("                    ");
    licznik = 0;
    stan_menu = 0;
    menu = true;
    zapis = false;
  }
  // ---------------------------------- CZAS -------------------------------------------
  if(digitalRead(przycisk_04) == LOW && stan_menu == 3 && licznik == 2) {
    menu = false;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Podaj wartosc:  ");
    lcd.setCursor(0,1);
    lcd.print(String(czas) + "s");
    stan_menu = 4;
  }
  while(digitalRead(przycisk_02) == LOW && stan_menu == 4 && licznik == 2) {
    menu = false;
    if(czas >= 0) {
        czas += 1;
        delay(100);
    }
    if(czas < 10) {
      lcd.setCursor(0,1);
        lcd.print("0");
        lcd.setCursor(1,1);
        lcd.print(czas);
        lcd.setCursor(2,1);
        lcd.print("s   ");
    }
    else {
    lcd.setCursor(0,1);
        lcd.print(czas);
        lcd.setCursor(2,1);
        lcd.print("s   ");
    }
    if(czas >= 100) {
          lcd.setCursor(0,1);
          lcd.print(czas);
          lcd.setCursor(3,1);
          lcd.print("s   ");
      }
      else if(czas >=1000) {
          lcd.setCursor(0,1);
          lcd.print(czas);
          lcd.setCursor(4,1);
          lcd.print("s   ");
      }
    delay(100);
    zapis = true;
    timer.begin(czas*1000);
    EEPROM.write(czas_eeprom, czas);
    }
    while(digitalRead(przycisk_03) == LOW && digitalRead(przycisk_02) == HIGH && stan_menu == 4 && licznik == 2) {
      menu = false;
      if(czas > 0) {
          czas--;
          delay(100);
      }
      if(czas < 10) {
          lcd.setCursor(0,1);
          lcd.print("0");
          lcd.setCursor(1,1);
          lcd.print(czas);
          lcd.setCursor(2,1);
          lcd.print("s   ");
      }
      else {
          lcd.setCursor(0,1);
          lcd.print(czas);
          lcd.setCursor(2,1);
          lcd.print("s   ");
      }
      if(czas >= 100) {
          lcd.setCursor(0,1);
          lcd.print(czas);
          lcd.setCursor(3,1);
          lcd.print("s   ");
      }
      if(czas >=1000) {
          lcd.setCursor(0,1);
          lcd.print(czas);
          lcd.setCursor(4,1);
          lcd.print("s   ");
      }
      delay(100);
      zapis = true;
      timer.begin(czas*1000);
      EEPROM.write(czas_eeprom, czas);
      } 
  //Podlewanie 
  if(int(wilg_po_przeliczeniu) < min_wilgotnosc && stan_menu == 0 && menu == true) {
     if (timer.available() == true && l == 0){
        digitalWrite(pompa, HIGH);
        timer.restart();
        l=1;
    }
    else if(timer.available() == true && l == 1) {
        digitalWrite(pompa, LOW);
        timer.restart();
        l=0;
    }
  }
  if(int(wilg_po_przeliczeniu) > min_wilgotnosc) {
    digitalWrite(pompa, HIGH);
    l=0;
  }

}
