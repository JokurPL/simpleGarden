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

//czas podlewania
int minuty_eeprom = 1;
int sekundy_eeprom = 4;

int sekundy = EEPROM.read(sekundy_eeprom);
int minuty = EEPROM.read(minuty_eeprom);
int czas = minuty * 6000 + sekundy * 1000;
int l_czas = 0;

//czas czekania
int minuty2_eeprom = 5;
int sekundy2_eeprom = 6;


int sekundy2 = EEPROM.read(sekundy2_eeprom);
int minuty2 = EEPROM.read(minuty2_eeprom);
int czas2 = minuty2 * 6000 + sekundy2 * 1000;
int l_czas2 = 0;

bool zapis = false;
bool menu = true;

int l = 0;

void setup() {
  //EEPROM.write(minuty2_eeprom, 0);
  //EEPROM.write(sekundy2_eeprom, 0);
  Serial.begin(9600);
  pinMode(przycisk_01, INPUT_PULLUP);
  pinMode(przycisk_02, INPUT_PULLUP);
  pinMode(przycisk_03, INPUT_PULLUP);
  pinMode(przycisk_04, INPUT_PULLUP);
  pinMode(pompa, OUTPUT);
  pinMode(A0, INPUT);
  digitalWrite(pompa, HIGH);
  //wilgotnosc = analogRead(A0);
  lcd.begin(16, 2);
  lcd.backlight(); 
  lcd.print("Witaj w GardenOS");
  lcd.setCursor(0, 1);
  lcd.print("");
  timer.begin(czas);
}

void loop() {
  Serial.println("czas: " + String(timer.time()));
  Serial.println("czas2: " + String(timer2.time()));
  wilgotnosc = analogRead(A2);
  float wilg_po_przeliczeniu = (float(wilgotnosc)/float(max_wilg))*float(100);
//  Serial.println(wilg_po_przeliczeniu);
//  Serial.println("w:" + String(wilgotnosc));
//  Serial.println("minw:" + String(min_wilgotnosc));
//  Serial.println("maxw:" + String(max_wilg)); 
//  Serial.println("czas:" + String(czas));
  if(menu == true) {
  if(digitalRead(przycisk_01) == LOW) {
    licznik++;
    if(licznik > 4) {
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
        lcd.print(">czas czekania        ");
        if(digitalRead(przycisk_04) == LOW) {
          stan_menu = 10;
        }
        break;
      case 4:
        lcd.setCursor(0,1);
        lcd.print(">czujnik max          ");
        if(digitalRead(przycisk_04) == LOW)
        {
          stan_menu = 5;
        }
    }
  }
  //------------------------------ MAX WILG --------------------------------
  if(digitalRead(przycisk_04) == LOW && stan_menu == 5 && licznik == 4) {
    menu = false;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Podaj wartosc:  ");
    lcd.setCursor(0,1);
    lcd.print(String(max_wilg) + "             ");
    stan_menu = 6;
  }
  while(digitalRead(przycisk_02) == LOW && stan_menu == 6 && licznik == 4) {
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
    while(digitalRead(przycisk_03) == LOW && digitalRead(przycisk_02) == HIGH && stan_menu == 6 && licznik == 4) {
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
  if(digitalRead(przycisk_04) == LOW && zapis == true && (stan_menu == 2 || stan_menu == 4 || stan_menu == 6 || stan_menu == 11)) {
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
    l_czas = 0;
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
    lcd.print(String(minuty) + ":" + String(sekundy) + "                     ");
    if(minuty < 10) {
        lcd.setCursor(0,1);
        lcd.print("0" + String(minuty) + ":" + String(sekundy) + "                 ");
    }
    if(sekundy < 10) {
        lcd.setCursor(0,1);
        lcd.print(String(minuty) + ":0" + String(sekundy)+ "                         ");
    }
    if(minuty < 10 && sekundy < 10) {
        lcd.setCursor(0,1);
        lcd.print("0" + String(minuty) + ":0" + String(sekundy)+ "                  ");
    }
    stan_menu = 4;
  }
  if(digitalRead(przycisk_01) == LOW && stan_menu == 4) {
      l_czas++;
      if(l_czas > 1) {
        l_czas = 0;
      }  
      delay(100);
  }
  while(digitalRead(przycisk_02) == LOW && stan_menu == 4 && licznik == 2) {
    menu = false;
    if(l_czas == 0 && minuty >= 0 && minuty < 60 )
    {
      minuty += 1;
      delay(100);
    }
    if(l_czas == 1 && sekundy >= 0)
    {
      if(sekundy == 60) {
        sekundy = 0;
        minuty += 1;
      }
      sekundy += 1;
      delay(100);
    }
    if(minuty < 10) {
        lcd.setCursor(0,1);
        lcd.print("0");
        lcd.setCursor(1,1);
        lcd.print(minuty);
    }
    else {
        lcd.setCursor(0,1);
        lcd.print(minuty);
    }
    if(sekundy < 10) {
        lcd.setCursor(3,1);
        lcd.print("0");
        lcd.setCursor(4,1);
        lcd.print(String(sekundy) + "              ");
    }
    else {
        lcd.setCursor(3,1);
        lcd.print(String(sekundy) + "                            ");

    }
    delay(100);
    zapis = true;
    czas = minuty * 6000 + sekundy * 1000;
    timer.begin(czas);
    EEPROM.write(sekundy_eeprom, sekundy);
    EEPROM.write(minuty_eeprom, minuty);
    }
    while(digitalRead(przycisk_03) == LOW && digitalRead(przycisk_02) == HIGH && stan_menu == 4 && licznik == 2) {
    menu = false;
    lcd.setCursor(2,1);
    lcd.print(":");
    if(l_czas == 0 && minuty > 0)
    {
      minuty -= 1;
      delay(100);
    }
    if(l_czas == 1 && minuty >= 0 && sekundy >= 0)
    {
      if(sekundy == 0) {
        if(minuty > 0) {
          minuty -= 1;
        }
        sekundy = 60;
      }
      sekundy -= 1;
      delay(100);
    }
    if(minuty < 10) {
        lcd.setCursor(0,1);
        lcd.print("0");
        lcd.setCursor(1,1);
        lcd.print(minuty);
    }
    else {
        lcd.setCursor(0,1);
        lcd.print(minuty);
    }
    if(sekundy < 10) {
        lcd.setCursor(3,1);
        lcd.print("0");
        lcd.setCursor(4,1);
        lcd.print(String(sekundy) + "                ");
    }
    else {
        lcd.setCursor(3,1);
        lcd.print(String(sekundy) + "                      ");

    }
    delay(100);
    zapis = true;
    czas = minuty * 6000 + sekundy * 1000;
    timer.begin(czas);
    EEPROM.write(sekundy_eeprom, sekundy);
    EEPROM.write(minuty_eeprom, minuty);
    }
  //-------------------------------------- CZAS CZEKANIA --------------------------------------------
  if(digitalRead(przycisk_04) == LOW && stan_menu == 10 && licznik == 3) {
    menu = false;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Podaj wartosc:  ");
    lcd.setCursor(0,1);
    lcd.print(String(minuty2) + ":" + String(sekundy2) + "                     ");
    if(minuty < 10) {
        lcd.setCursor(0,1);
        lcd.print("0" + String(minuty2) + ":" + String(sekundy2) + "                 ");
    }
    if(sekundy < 10) {
        lcd.setCursor(0,1);
        lcd.print(String(minuty2) + ":0" + String(sekundy2)+ "                         ");
    }
    if(minuty2 < 10 && sekundy2 < 10) {
        lcd.setCursor(0,1);
        lcd.print("0" + String(minuty2) + ":0" + String(sekundy2)+ "                  ");
    }
    stan_menu = 11;
  }
  if(digitalRead(przycisk_01) == LOW && stan_menu == 11) {
      l_czas2++;
      if(l_czas2 > 1) {
        l_czas2 = 0;
      }  
      delay(100);
  }
  while(digitalRead(przycisk_02) == LOW && stan_menu == 11 && licznik == 3) {
    menu = false;
    lcd.setCursor(2,1);
    lcd.print(":");
    if(l_czas2 == 0 && minuty2 >= 0 && minuty2 < 60 )
    {
      minuty2 += 1;
      delay(100);
    }
    if(l_czas2 == 1 && sekundy2 >= 0)
    {
      if(sekundy2 == 60) {
        sekundy2 = 0;
        minuty2 += 1;
      }
      sekundy2 += 1;
      delay(100);
    }
    if(minuty2 < 10) {
        lcd.setCursor(0,1);
        lcd.print("0");
        lcd.setCursor(1,1);
        lcd.print(minuty2);
    }
    else {
        lcd.setCursor(0,1);
        lcd.print(minuty2);
    }
    if(sekundy2 < 10) {
        lcd.setCursor(3,1);
        lcd.print("0");
        lcd.setCursor(4,1);
        lcd.print(String(sekundy2) + "              ");
    }
    else {
        lcd.setCursor(3,1);
        lcd.print(String(sekundy2) + "                            ");

    }
    delay(100);
    zapis = true;
    czas2 = minuty2 * 6000 + sekundy2 * 1000;
    timer2.begin(czas);
    EEPROM.write(sekundy2_eeprom, sekundy2);
    EEPROM.write(minuty2_eeprom, minuty2);
    }
    while(digitalRead(przycisk_03) == LOW && digitalRead(przycisk_02) == HIGH && stan_menu == 11 && licznik == 3) {
    menu = false;
    lcd.setCursor(2,1);
    lcd.print(":");
    if(l_czas2 == 0 && minuty2 > 0)
    {
      minuty2 -= 1;
      delay(100);
    }
    if(l_czas2 == 1 && minuty2 >= 0 && sekundy2 >= 0)
    {
      if(sekundy2 == 0) {
        if(minuty2 > 0) {
          minuty2 -= 1;
        }
        sekundy2 = 60;
      }
      sekundy2 -= 1;
      delay(100);
    }
    if(minuty2 < 10) {
        lcd.setCursor(0,1);
        lcd.print("0");
        lcd.setCursor(1,1);
        lcd.print(minuty2);
    }
    else {
        lcd.setCursor(0,1);
        lcd.print(minuty2);
    }
    if(sekundy2 < 10) {
        lcd.setCursor(3,1);
        lcd.print("0");
        lcd.setCursor(4,1);
        lcd.print(String(sekundy2) + "                ");
    }
    else {
        lcd.setCursor(3,1);
        lcd.print(String(sekundy2) + "                      ");

    }
    delay(100);
    zapis = true;
    czas2 = minuty2 * 6000 + sekundy2 * 1000;
    timer2.begin(czas);
    EEPROM.write(sekundy2_eeprom, sekundy2);
    EEPROM.write(minuty2_eeprom, minuty2);
    }
  //Podlewanie 
  if(int(wilg_po_przeliczeniu) < min_wilgotnosc && stan_menu == 0 && menu == true) {
     if(timer.available() == true && l == 0){
        digitalWrite(pompa, HIGH);
        timer2.begin(czas2);
        l=1;
    }
     if(timer2.available() == true && l == 1) {
        digitalWrite(pompa, LOW);
        timer.begin(czas);
        l=0;
    }
  }
  if(int(wilg_po_przeliczeniu) > min_wilgotnosc) {
    digitalWrite(pompa, HIGH);
    l=0;
  }
}
