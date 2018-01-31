
#include <RCSwitch.h>
#include <LiquidCrystal.h>
#define sendPin 2
#define left 4
#define right 3
#define up 5
#define down 6
#define lcdon 13
#define nlights 17
#define sleepmillis 30000

RCSwitch Sender = RCSwitch();
LiquidCrystal lcd(12,11,10,9,8,7);

int i = 0;
bool upd, downd, leftd, rightd, sleep;
long delaym = 0;
long smillis = 0;
char dbuffer[30] = "Debug Mode on";
bool debug = true;
unsigned long codeon, codeoff, bits, proto;
int pulse[] = {0,427,686};
int repeat[] = {0,6,6};

// Name, OnCode, OffCode, Proto, Bits -- in HEX
char* light[][5] = {
                   // Wohnbereich
                   {"Couch","151555","151554","1","18"},       //1,1
                   {"Wohnzimmer","141555","141554","1","18"},  //5.1
                   {"Thai Lampe","514555","514554","1","18"},  //3,2
                   {"Spot WZ","155155","155154","1","18"},     //1,3                 
                   {"Esstisch","451555","451554","1","18"},    //2,1  
                   // Küchenberech
                   {"Kueche","454555","454554","1","18"},      //2,2
                   {"Bar","515155","515154","1","18"},         //3,3
                   {"Kueche Gesamt","99707000","A9707000","2","20"},                   
                   // Schlafzimmer
                   {"Stehlampe","154555","154554","1","18"},   //1,2
                   {"Bett","144555","144554","1","18"},        //5.2
                   {"Schlafzimmer","541555","541554","1","18"},//4,1
                   // Vorzimme
                   {"Vorzimmer","155455","155454","1","18"},   //1,4
                   {"Abstellraum","455455","455454","1","18"}, //2,4
                   {"Toilette","455155","455154","1","18"},    //2,3
                   // Badezimmer
                   {"Badezimmer","511555","511554","1","18"},  //3,1                   
                   {"Schrankraum","515455","515454","1","18"}, //3,4
                   // Gruppen
                   {"Radio An/Aus","145455","145454","1","18"},//5.4
                   {"Alle","145155","145154","1","18"}         //5,3
                 };

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  lcd.begin(16,2);
  lcd.print("Initializing...");
  pinMode(left, INPUT);
  pinMode(right, INPUT);
  pinMode(up, INPUT);
  pinMode(down, INPUT);
  pinMode(lcdon, OUTPUT);
  Sender.enableTransmit(sendPin);
  changeLight();
}

// the loop function runs over and over again forever
void loop() {
    smillis = millis();
    
    if (digitalRead(up) == HIGH && upd == false) {
        Sender.setProtocol(proto);
        Sender.setPulseLength(pulse[proto]);
        Sender.setRepeatTransmit(repeat[proto]);
        Sender.send(codeon,bits);
        if (debug) {
          sprintf(dbuffer,"%lu,%lu,%lu",codeon,proto,bits);
          changeLight();
        }
        upd = true;
        //delaym = 0;
    }
    
    if (digitalRead(up) == LOW) {
      upd = false;
    }
    
    if (digitalRead(down) == HIGH && downd == false) {
        Sender.setProtocol(proto);
        Sender.setPulseLength(pulse[proto]);
        Sender.setRepeatTransmit(repeat[proto]);
        Sender.send(codeoff,bits);
        if (debug) {
          sprintf(dbuffer,"%lu,%lu,%lu",codeoff, proto, bits);
          changeLight();
        }
        downd = true;
        //delaym = 0;
    }
    
    if (digitalRead(down) == LOW) {
      downd = false;
    }
    
    if (digitalRead(left) == HIGH && leftd == false)  {
       if (sleep == false) {
         i--;
         if ( i == -1 ) { i = nlights; }
         leftd = true;
         changeLight(); 
       }
       delaym = 0;
       delay(200);
    }
    
    if (digitalRead(left) == LOW) {
      leftd = false;
    }
    
    if (digitalRead(right) == HIGH && rightd == false) {
       if ( sleep == false ) {
         i++;
         if ( i > nlights ) { i = 0; }
         rightd = true;
         changeLight();
       }
       delaym = 0;
       delay(200);
    }
    
    if (digitalRead(right) == LOW) {
     rightd = false;
    }
     
    delaym = delaym + (millis() - smillis);
    
    if ( delaym > sleepmillis ) {
      digitalWrite(lcdon, LOW);
      sleep = true;
    } else {
      digitalWrite(lcdon, HIGH);
      sleep = false;
    }    
}

void changeLight() {
  proto = x2i(light[i][3]);
  bits = x2i(light[i][4]);
  if ( proto == 2 ) {
    codeon = x2i(convhex32(light[i][1]));
    codeoff = x2i(convhex32(light[i][2]));
  } else {
    codeon = x2i(light[i][1]);
    codeoff = x2i(light[i][2]);
  }
  lcd.clear();
  if (debug == false) {
    lcd.setCursor(5,0);
    lcd.print("Licht");
  } else {
    lcd.setCursor(0,0);
    lcd.print(dbuffer);
  }
  String text = light[i][0];
  int pos = (16 - text.length()) / 2;
  lcd.setCursor(pos,1);
  lcd.print(text); 
}

unsigned long x2i(char *s) 
{

 unsigned long x = 0;
 for(;;) {
   char c = *s;
   if (c >= '0' && c <= '9') {
      x *= 16;
      x += c - '0'; 
   }
   else if (c >= 'A' && c <= 'F') {
      x *= 16;
      x += (c - 'A') + 10; 
   }
   else break;
   s++;
 }
 return x;
}

static char* convhex32(char* hexvalue){
  static char retvalue[8];  
  static char convtable[17] = {"084C2A6E195D3B7F"};
  static int bytetable[8] = {1,0,3,2,5,4,7,6};
  
  for (unsigned int i = 0; i < strlen(hexvalue); i++) {
  	char temp[1];
  	temp[0] = hexvalue[i];
  	char *ptr;
    int num = (int)strtol(temp, &ptr, 16);
    if ( i == 1 ) { num = num & 14;}
    retvalue[bytetable[i]] = convtable[num];   
  }
  return retvalue;
}
