#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int ph = A0;
int turbidity = A2;
int tds = A3;
int water_flow = 2;
int relay = 3;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Constants for calculations
volatile int pulseCount;
const float pulsePerLiter = 450.0;
float tdsValue = 0.0;
float turbidityValue = 0.0;
float rainValue = 0.0;
float phValue = 0.0;
float flowRate = 0.0;
float volume = 0.0;
float tdsOffset = 50.0;  


void countPulses();
void setup() {
  pinMode(ph, INPUT);
  pinMode(turbidity, INPUT);
  pinMode(tds, INPUT);
  pinMode(water_flow, INPUT);
  attachInterrupt(digitalPinToInterrupt(water_flow), countPulses, RISING);
  pinMode(relay, OUTPUT);
  
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Smart Water");
  lcd.setCursor(0, 1);
  lcd.print("Quality Monitoring");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("System");
  delay(1000);
  Serial.println("Smart water quality Monitoring System");
}

void loop() {
  // Read and calculate sensor values
  tdsValue = analogRead(tds) * (5.0 / 1023.0) * 1000.0 + tdsOffset; // Adjusted TDS calculation
  int turbidityValueRaw = analogRead(turbidity);

  // Remap turbidity values 
    if (turbidityValueRaw >= 640 && turbidityValueRaw <= 1000) {
    turbidityValue = map(turbidityValueRaw, 640, 1000, -10, 5); 
  }
  else if (turbidityValueRaw >= 0 && turbidityValueRaw <= 630) {
    turbidityValue = map(turbidityValueRaw, 0, 630, 5, 20);
  }

  int phRawValue = analogRead(ph);
  float PHValue = map(phRawValue, 700, 300, 4, 8);

  int totalLitres = pulseCount / pulsePerLiter;

  // Print values to Serial Monitor
  Serial.print("TDS: ");
  Serial.print(tdsValue);
  Serial.println(" ppm");

  Serial.print("Turbidity: ");
  Serial.print(turbidityValue);
  Serial.println(" NTU");

  Serial.print("pH: ");
  Serial.println(PHValue);

  Serial.print("Water Consumed: ");
  Serial.print(totalLitres);
  Serial.println(" L");

  // Safety checks and alerts
  if (tdsValue > 900) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DANGER! Not safe");
    lcd.setCursor(0,1);
    lcd.print("TDS:");
    lcd.print(tdsValue, 0);
    lcd.print("ppm");
    digitalWrite(relay, HIGH);
    delay(3000);
    lcd.clear();
  }
  if (turbidityValue > 5) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("DANGER! Not safe");
    lcd.setCursor(0,1);
    lcd.print("Turb:");
    lcd.print(turbidityValue);
    lcd.print("NTU");
    digitalWrite(relay, HIGH);
    delay(3000);
    lcd.clear();
  }
  if (PHValue > 7) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DANGER! Not safe");
    lcd.setCursor(0, 1);
    lcd.print("pH:");
    lcd.print(PHValue, 0);
    digitalWrite(relay, HIGH);
    delay(3000);
  }

  // Safe condition
  if ((tdsValue <= 50) && (turbidityValue <= 1) && (PHValue <= 7)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Water Safe");
    digitalWrite(relay, LOW);
    delay(2000);
  }
  else {
    lcd.setCursor(0, 0);
    lcd.print("TDS:");
    lcd.print(tdsValue);
    lcd.print("ppm");
    delay(1000);

    lcd.setCursor(0, 1);
    lcd.print("TURB: ");
    lcd.print(turbidityValue);
    lcd.print("NTU");
    delay(1000);
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Ph:");
    lcd.print(PHValue, 1);
    delay(3000);
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Water_consumed");
    lcd.setCursor(0, 1);
    lcd.print(":");
    lcd.print(totalLitres);
    lcd.print("L");
    delay(1000);
    lcd.clear();
  }
}

// Pulse counter ISR
void countPulses() {
  pulseCount++;
}