#include <DRV8835MotorShield.h>
#include <NewPing.h>
#include <Wire.h>
#include <PCF8574.h>
#include <Servo.h>

PCF8574 expander;
Servo servo;

DRV8835MotorShield motors; //definiowanie silnikow

#define DATA_BUFFER_SIZE 5 //ilosc zmiennych w tablicy odczytu bluetooth
#define trig 4 //czuj odleglosci
#define echo 5 //czuj odleglosci
#define max_distance 100 //max odczyt czuj odleglosci
#define button 13 //przycisk zmiana trybu
#define czerwona 0 //dioda
#define zielona 1 //dioda
#define niebieska 2 //dioda
#define zolta 3 //dioda

NewPing czujnik(trig, echo, max_distance); //definiowanie czujnika odleglosci

int cm; //odczyt czuj odleglosci
char data[DATA_BUFFER_SIZE]; //odczyt bt
char znak; //aktualny znak odczyt bt
byte x; //miejsce w tablicy odczyt bt
bool started; //start odczytu bt
int maxprzod; //max predkosc jazdy w przod
int maxtyl;   //max predkosc jazdy w tyl
int maxbok;   //max predkosc skretu
int roznicalewe;
int roznicaprawe;
boolean stanczujnik; //zatrzymanie po wykryciu przeszkody tryb bt
boolean tryb; //tryb jazdy
byte strona; //strona skrecania tryb autonomiczny
boolean rozpedzenie; //czy pojazd sie rozpedzil tryb autonomiczny
unsigned long int czas; //czas uzyty w diodzie czerwonej i zielonej tryb bt
unsigned long int poprzedniczas; //czas uzyty w diodzie czerwonej i zielonej tryb bt

void setup()
{
  Serial.begin(9600);
  expander.begin(0x20);
  servo.attach(2);
  maxprzod = 400;
  maxtyl = -1;
  maxbok = 325;
  started = false;
  stanczujnik = 1;
  tryb=0;
  strona=0;
  rozpedzenie = 0;
  
  motors.setM1Speed(0); //zatrzymanie silnikow
  motors.setM2Speed(0);
  
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(button, INPUT_PULLUP);
  expander.pinMode(czerwona, OUTPUT);
  expander.pinMode(zielona, OUTPUT);
  expander.pinMode(niebieska, OUTPUT);
  expander.pinMode(zolta, OUTPUT);
  expander.digitalWrite(zolta, HIGH);
  expander.digitalWrite(niebieska, LOW);
  servo.write(90);
}

void przod()
{
  int predkosclewe=275; //najmniejsza predkosc jazdy w przod lewego silnika
  int predkoscprawe=275; //najmniejsza predkosc jazdy w przod prawego silnika
  for(predkosclewe>275; predkosclewe<maxprzod; predkosclewe++)
  {
    predkoscprawe++;
     motors.setM1Speed(predkoscprawe-roznicaprawe);
     motors.setM2Speed(predkosclewe-roznicalewe);
     delay(10);                         //przyspieszanie
     data[0] = Serial.read();
     if(data[0] == '0') //hamowanie gdy jest przeszkoda podczas rozpedzania
     {
       predkosclewe = 1000;
       stop();
       reset(); //reset odczytu bluetooth
     }
  }
}

void tyl()
{
  int predkosclewe=-325; //najmniejsza predkosc jazdy w przod
  int predkoscprawe=-325;
  for(predkosclewe > -325; predkosclewe<maxtyl; predkosclewe++)
  {
    predkoscprawe++;
   motors.setM1Speed(predkoscprawe-roznicaprawe);
   motors.setM2Speed(predkosclewe-roznicalewe);
   delay(4);                    //przyspieszanie
   data[0] = Serial.read();
     if(data[0] == '0') //hamowanie gdy jest przeszkoda podczas rozpedzania
     { 
       predkosclewe = 500;
       stop();
       reset();
     }
  }
}

void lewo()
{
  int predkosc=275; //minimalna predkosc jazdy w bok
  for(predkosc>275; predkosc<maxbok; predkosc++)
  {
    motors.setM1Speed(predkosc);
    delay(7);                   //rozpedzanie
    data[0] = Serial.read();
     if(data[0] == '0')   //hamowanie gdy jest przeszkoda podczas rozpedzania
     { 
       predkosc = 1000;
       stop();
       reset();
     }
  }
}

void prawo()
{
  int predkosc=275;
  for(predkosc>275; predkosc<maxbok; predkosc++)
  {
   motors.setM2Speed(predkosc);
   delay(7);
   data[0] = Serial.read();
     if(data[0] == '0')
     { 
       predkosc = 1000;
       stop();
       reset();
     }
  }
}

void przodl()
{
  int predkosclewe=250; //minimalna predkosc jazdy w przod prawego silnika
  int predkoscprawe=300; //minimalna predkosc jazdy w przod prawego silnika
  for(predkosclewe>250; predkosclewe<maxbok; predkosclewe++)
  {
    if(predkoscprawe<350)
    {
      predkoscprawe++;
    }
     motors.setM1Speed(predkoscprawe);
     motors.setM2Speed(predkosclewe);
     delay(10);
     if(Serial.available())
     {
       data[0] = Serial.read();
     }
     if(data[0] == '0')
     {
       predkosclewe = 1000;
       stop();
       reset();
     }
  }
}


void przodp()
{
  int predkosclewe=300;
  int predkoscprawe=250;
  for(predkoscprawe>250; predkoscprawe<maxbok; predkoscprawe++)
  {
    if(predkosclewe<350)
    {
      predkosclewe++;
    }
     motors.setM1Speed(predkoscprawe);
     motors.setM2Speed(predkosclewe);
     delay(10);
     if(Serial.available())
     {
       data[0] = Serial.read();
     }
     if(data[0] == '0')
     {
       predkoscprawe = 1000;
       stop();
       reset();
     }
  }
}

void tyll()
{
  int predkosclewe=-250;
  int predkoscprawe=-325;
  for(predkoscprawe > -325; predkoscprawe<maxtyl; predkoscprawe++)
  {
    predkosclewe++;
     motors.setM1Speed(predkoscprawe);
     motors.setM2Speed(predkosclewe);
     delay(4);
     if(Serial.available())
     {
       data[0] = Serial.read();
     }
     if(data[0] == '0')
     {
       predkoscprawe = 500;
       stop();
       reset();
     }
  }
}

void tylp()
{
  int predkosclewe=-325;
  int predkoscprawe=-250;
  for(predkosclewe > -325; predkosclewe<maxtyl; predkosclewe++)
  {
      predkoscprawe++;
     motors.setM1Speed(predkoscprawe);
     motors.setM2Speed(predkosclewe);
     delay(4);
     if(Serial.available())
     {
       data[0] = Serial.read();
     }
     if(data[0] == '0')
     {
       predkosclewe = 500;
       stop();
       reset();
     }
  }
}

void stop()   //zatrzymywanie
{
  motors.setM1Speed(0);
  motors.setM2Speed(0);
}

void reset() //reset odczytu bluetooth
{
  x=0;
  memset(data, 0, sizeof(data));
  started = false;
}

void sprawdz() //sprawdzanie odczytu bluetooth
{
 if(tryb==0)
 {
  if(data[0]=='G')
  {
    if(data[1]=='L')
    {
      przodl();
    }
    else if(data[1]=='P')
    {
      przodp();
    }
    else
    {
      przod();
    }
  }

  else if(data[0]=='D')
  {
    if(data[1]=='L')
    {
      tyll();
    }
    else if(data[1]=='P')
    {
      tylp();
    }
    else
    {
      tyl();
    }
  }

  else if(data[0]=='L')
  {
    lewo();
  }

  else if(data[0]=='P') 
  {
    prawo();  
  }

  else if (data[0] == '0')
  {
    stop();
  }
 }

  if(data[0] == '+')
  {
   maxprzod = ((1 * data[3] -48) + (10* (data[2] - 48)) + (100 * (data[1]-48))); //zamiana char na int

   if(maxprzod < 325)
   {
    maxbok = maxprzod; //zamiana predkosci przod na predkosc skrecania
   }
   else if (maxprzod > 325)
   {
    maxbok = 325; //zamiana predkosci przod na predkosc skrecania
   }
   rozpedzenie=0;
  }

  else if(data[0] == '-')
  {
    if(x == 4)
    {
     maxtyl = ((1 * data[3] -48) + (10* (data[2] - 48)) + (100 * (data[1]-48))); //zamiana char na int przy 3 znakach
     maxtyl = maxtyl * -1;
    }
    else
    {
     maxtyl = ((1 * data[2] -48) + (10* (data[1] - 48))); //zamiana char na int przy 2 znakach
     maxtyl = maxtyl * -1;
    }
  }
  else if(data[0] == 'T') //zamiana trybu jazdy
  {
    if(tryb==0)
    {
      tryb=1;
      rozpedzenie = 0;
      stop();
      digitalWrite(zolta, LOW);
      digitalWrite(niebieska, HIGH);
      delay(500);
    }
    else if(tryb==1)
    {
      tryb=0;
      stop();
      digitalWrite(zolta, HIGH);
      digitalWrite(niebieska, LOW);
      delay(500);
    }
  }
  else if(data[0]=='Z')
  {
    if(data[1]=='L')
    {
      if(x==3)
      {
        roznicalewe = data[2] - 48;
      }
      else if(x==4)
      {
        roznicalewe = ((1 * data[3] -48) + (10* (data[2] - 48)));
      }
    }
    else if(data[1]=='P')
    {
      if(x==3)
      {
        roznicaprawe = data[2] - 48;
      }
      else if(x==4)
      {
        roznicaprawe = ((1 * data[3] -48) + (10* (data[2] - 48)));
      }
    }
      Serial.println(roznicalewe);
      Serial.println(roznicaprawe);
  }
}

void odczytbt()
{
  if(Serial.available() > 0) //jesli serial odczytuje
  {
   znak = Serial.read(); //odczyt aktualnego znaku bt
   if(znak == '/')
   {
     reset(); //reset odczytu i zaczecie zapisywania znakow
     started = true;
   }
   else if(znak=='.')
   {
    sprawdz(); //sprawdzenie odczytu i zakonczenie zapisywania znakow
    reset();
   }
   else if(started)
   {
     data[x]=znak; //zapis znakow do tablicy char
     x++;
     if(x>=DATA_BUFFER_SIZE)
     {
       Serial.println("Blad, koniec bufora."); //zbyt duzo odczytanych znakow
       reset();
     }
   }
  }
}

void odczytczujnik()
{
  /*
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  
  odczyt = pulseIn(echo, HIGH);
  cm = odczyt / 58;
  */
  if((stanczujnik==1)&&(czujnik.ping_cm()<50)&&(czujnik.ping_cm() > 0)) //jesli przeszkoda wykryta
  {
    stop();
    stanczujnik = 0;
    poprzedniczas = millis();
    expander.digitalWrite(czerwona, LOW);
    expander.digitalWrite(zielona, HIGH);
  }
  else if((stanczujnik==0)&&(czujnik.ping_cm()>55)) //jesli oddalono od przeszkody
  {
    stanczujnik = 1;
  }

  if(czas - poprzedniczas == 1000) //zgaszenie czerwonej diody po 1s
  {
    expander.digitalWrite(czerwona, HIGH);
    expander.digitalWrite(zielona, LOW);
    
  }
}

void automat()
{
  byte cm;
  int predkosclewe=275;
  int predkoscprawe=275;
  cm = czujnik.ping_cm(); //odczyt czujnika odleglosci
  if((cm>50)||(cm==0)) //jazda w przod
  {
    expander.digitalWrite(czerwona, HIGH);
    expander.digitalWrite(zielona, LOW);
    if(rozpedzenie==0)
    {
      for(predkosclewe>275; predkosclewe<maxprzod; predkosclewe++)
      {
        predkoscprawe++;
        motors.setM1Speed(predkoscprawe-roznicaprawe);
        motors.setM2Speed(predkosclewe-roznicalewe);
        delay(5);
        cm = czujnik.ping_cm();
        if((cm<50)&&(cm>0)) //jesli wykryto przeszkode podczas przyspieszania zatrzymaj sie
        {
          predkosclewe=1000;
        }
      }
      rozpedzenie=1; //skonczenie rozpedzania
    }
  }
  else if((cm<50)&&(strona==0)||(strona==1)&&(cm>0)) //wykryto przeszkode i skret w lewo
  {
    predkosclewe = -325;
    predkoscprawe = -325;
    expander.digitalWrite(czerwona, LOW);
    expander.digitalWrite(zielona, HIGH);
    stop();
    delay(200);
    for(predkosclewe>-325; predkosclewe<-100; predkosclewe++)
    {
      predkoscprawe++;
      motors.setM1Speed(predkoscprawe-roznicaprawe);
      motors.setM2Speed(predkosclewe-roznicalewe); //rozpedzanie w tyl
      delay(3);
    }
    stop();
    delay(400);
    motors.setM1Speed(300);
    motors.setM2Speed(0);  //skret w lewo
    delay(300);
    stop();
    delay(200);
    
    if(strona==0)
    {
      strona=1;
    }
    else if(strona==1)  //zamiana zmiennej strony skretu
    {
      strona=2;
    }
    rozpedzenie=0; //wlaczenie funkcji rozpedzania w przod
  }
  else if((cm<50)&&(strona==2)&&(cm>0)) //wykryto przeszkode i skret w prawo
  {
    predkosclewe = -325;
    predkoscprawe = -325;
    expander.digitalWrite(czerwona, LOW);
    expander.digitalWrite(zielona, HIGH);
    stop();
    delay(200);
    for(predkosclewe>-325; predkosclewe<-100; predkosclewe++)
    {
      predkoscprawe++;
      motors.setM1Speed(predkoscprawe - roznicaprawe);
      motors.setM2Speed(predkosclewe - roznicalewe); //rozpedzanie w tyl
      delay(3);
    }
    stop();
    delay(400);
    motors.setM1Speed(0);
    motors.setM2Speed(300); //skret w prawo
    delay(300);
    stop();
    delay(200);
    strona=0; //zamiana zmiennej strony skretu
    rozpedzenie=0; //wlaczenie funkcji rozpedzania w przod
  }
}



void loop()
{
  czas = millis(); //pobranie aktualnego czasu
  if(digitalRead(button) == LOW) //jesli przycisk do zmiany trybu nacisniety
  {
    delay(20);
    if(tryb == 0) //zamiana trybu
    { 
      tryb = 1; 
      rozpedzenie = 0; //wlaczenie funkcji rozpedzania w przod w trybie autonomicznym
      stop();
      expander.digitalWrite(zolta, LOW);
      expander.digitalWrite(niebieska, HIGH);
      expander.digitalWrite(czerwona, HIGH);
      expander.digitalWrite(zielona, LOW);
    }
    else if(tryb == 1)
    { 
     tryb = 0; 
     stop();
     expander.digitalWrite(zolta, HIGH);
     expander.digitalWrite(niebieska, LOW);
    }
    delay(1000);
  }
  if(tryb==0) //funkcje wykonywane dla trybu bluetooth
  {
    odczytczujnik();
  }
  else if(tryb==1) //funkcje wykonywane dla trybu autonomicznego
  {
    automat();
  }
  odczytbt(); //funkcja wykonywana dla obu trybow
}
