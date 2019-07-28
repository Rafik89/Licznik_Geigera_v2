#include <MemoryFree.h>
#include <pgmStrToRAM.h>

#define BUZZER_PIN          2               // BUZZER

#define LED_COUNTER_PIN     3               // LED_COUNTER
#define LED_ALARM_PIN       4               // LED_ALARM

#define GEIGER_PIN          10              // GEIGER_PIN
#define BATTERY_PIN         A0              // BATTERY_PIN

#define CPM1                45
#define CPM2                95
#define CPM3                200
#define CPM4                400
#define CPM5                600

#define PIN_STATE_ON        LOW
#define PIN_STATE_OFF       HIGH

int   ledArray []                      = {5, 6, 7, 8, 9};
int   toneArray[]                      = {100, 2100, 500, 1000, 1500, 2500, 1000, 100};

float faktor                           = 0.006666;
float radiationValue                   = 0.0;

unsigned long  count                   = 0;
unsigned long  countPerMinute          = 0;
unsigned long  timePrevious            = 0;
unsigned long  timePreviousMeassure    = 0;
unsigned long  timeActualMeassure      = 0;
unsigned long  countPrevious           = 0;

unsigned long geiger_results           = 0;

int alarm                              = 0;
unsigned long timeAlarm                = 0;
unsigned long timeAlarmOFF             = 10000;            //10 s

boolean isLow = false;

void setup()
{

  pinMode(LED_COUNTER_PIN,      OUTPUT);
  pinMode(LED_ALARM_PIN,        OUTPUT);

  pinMode(BUZZER_PIN,           OUTPUT);
  pinMode(BATTERY_PIN,          INPUT);
  pinMode(GEIGER_PIN,           INPUT);

  for (int i = 0; i < 5; i++) {
    pinMode(ledArray[i], OUTPUT);
  }

  Serial.begin(115200);
  
  startUpSeqence();

  timeActualMeassure = millis();
  
  isLow = true;
}

void loop()
{

  float voltage = (analogRead(BATTERY_PIN) * 5) / 1023.00;
  
  geiger_results = digitalRead(GEIGER_PIN);

  if (geiger_results > 0 && isLow)
  {
    count++;
    timePreviousMeassure = (float) (millis()-timeActualMeassure)/1000;
    countPerMinute = (float) 60.0 * (count / timePreviousMeassure);
    radiationValue = countPerMinute * faktor;

    analogWrite(BUZZER_PIN, 127);
    Serial.print("cpm = ");
    Serial.print(countPerMinute, DEC);
    Serial.print(" - ");
    Serial.print("uSv/h = ");
    Serial.print(radiationValue, 4);
    Serial.print(" - ");
    Serial.print("voltage = ");
    Serial.print(voltage, 3);
    Serial.print(" - ");
    Serial.print("PIN STATUS = ");
    Serial.print(geiger_results, 4);
    Serial.print(" - ");
    Serial.print("MEM = ");
    Serial.print(freeMemory(), DEC);
    Serial.print(" - ");
    Serial.print("timeActualMeassure = ");
    Serial.print(timeActualMeassure);
    Serial.print(" - ");
    Serial.print("timePreviousMeassure = ");
    Serial.println(timePreviousMeassure);
    
    if (countPerMinute <= CPM1) ledStrip(0);
    if ((countPerMinute <= CPM2) && (countPerMinute > CPM1)) ledStrip(1);
    if ((countPerMinute <= CPM3) && (countPerMinute > CPM2)) ledStrip(2);
    if ((countPerMinute <= CPM4) && (countPerMinute > CPM3)) ledStrip(3);
    if ((countPerMinute <= CPM5) && (countPerMinute > CPM4)) ledStrip(4);
    if (countPerMinute > CPM5) {
      timeAlarm = millis();
      ledStrip(5); 
      digitalWrite(LED_ALARM_PIN, PIN_STATE_ON);
    }


    digitalWrite(LED_COUNTER_PIN, PIN_STATE_ON);
    isLow = false;
  } else {
    if (millis() - timeAlarm > timeAlarmOFF) {
        digitalWrite(LED_ALARM_PIN, PIN_STATE_OFF);
        timeAlarm = millis();
    }
    analogWrite(BUZZER_PIN, 0);
    digitalWrite(LED_COUNTER_PIN, PIN_STATE_OFF);
    isLow = true;
  }
  
}

void startUpSeqence()
{
  for (int i = 0; i <= 5; i++) {
    digitalWrite(ledArray[i], PIN_STATE_ON);
  }
  delay(2000);
  for (int i = 0; i <= 5; i++) {
    digitalWrite(ledArray[i], PIN_STATE_OFF);
    delay(100);
  }
  for (int i = 0; i <= 5; i++) {
    digitalWrite(ledArray[i], PIN_STATE_ON);
    delay(100);
  }
  for (int i = 0; i <= 5; i++) {
    digitalWrite(ledArray[i], PIN_STATE_OFF);
    delay(100);
  }

  digitalWrite(LED_COUNTER_PIN, PIN_STATE_ON);
  delay(100);
  digitalWrite(LED_COUNTER_PIN, PIN_STATE_OFF);
  digitalWrite(LED_ALARM_PIN, PIN_STATE_ON);
  delay(100);
  digitalWrite(LED_ALARM_PIN, PIN_STATE_OFF);
  digitalWrite(LED_COUNTER_PIN, PIN_STATE_ON);
  delay(100);
  digitalWrite(LED_COUNTER_PIN, PIN_STATE_OFF);
  digitalWrite(LED_ALARM_PIN, PIN_STATE_ON);
  delay(100);
  digitalWrite(LED_ALARM_PIN, PIN_STATE_OFF);

  for (int i = 0; i <= 8; i++) {
    if (i < 8) {
      tone(BUZZER_PIN, toneArray[i]);
      delay(100);
    } else {
      noTone(BUZZER_PIN);
    }
  }
}

void ledStrip(int value) {
  if (value > 0) {
    for (int i = 0; i <= value; i++) {
      digitalWrite(ledArray[i], PIN_STATE_ON);
    }
    for (int i = 5; i >= value; i--) {
      digitalWrite(ledArray[i], PIN_STATE_OFF);
    }
  } else {
    for (int i = 5; i >= 0; i--) {
      digitalWrite(ledArray[i], PIN_STATE_ON);
    }
  }
}
