#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#define RE 12
#define DE 13
LiquidCrystal_I2C lcd(0x27, 16, 2); 
//const byte code[]= {0x01, 0x03, 0x00, 0x1e, 0x00, 0x03, 0x65, 0xCD};
const byte nitro[] = {0x01,0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
const byte phos[] = {0x01,0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
const byte pota[] = {0x01,0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};
 
const int selectButtonPin = 19; 

int selectedNumber1 = 0;
int selectedNumber2 = 0;
int Selection = 0;
int result = selectedNumber1 * selectedNumber2;
int LandSizeStatus = 0;


const int increaseButtonPin = 17;
const int decreaseButtonPin = 18;

bool goToSoilTest = false;

const int ControlPlotTestRepetition = 5;
int ControlPlotTest = 0;
int ControlPlotN[ControlPlotTestRepetition];
int ControlPlotP[ControlPlotTestRepetition];
int ControlPlotK[ControlPlotTestRepetition];
int ControlPlotNmean;
int ControlPlotPmean;
int ControlPlotKmean;
int ControlPlotStatus = 0;

const int TreatedPlotTestRepetition = 5;
int TreatedPlotTest = 0;
int TreatedPlotN[TreatedPlotTestRepetition];
int TreatedPlotP[TreatedPlotTestRepetition];
int TreatedPlotK[TreatedPlotTestRepetition];
int TreatedPlotNmean;
int TreatedPlotPmean;
int TreatedPlotKmean;
int TreatedPlotStatus = 0;


int SoilPlotTestRepetition = result / 100;
int SoilPlotTest = 0;
int* SoilPlotN = nullptr;
int* SoilPlotP = nullptr;
int* SoilPlotK = nullptr;

int SoilPlotNmean;
int SoilPlotPmean;
int SoilPlotKmean;
int SoilPlotStatus = 0;

double NitrogenTotalNutrientUptake = 17;
double PhosphorusTotalNutrientUptake = 5;
double PotassiumTotalNutrientUptake = 17;

double NitrogenTotalUptakeTreadedPlot = 15;
double PhosphorusTotalUptakeTreatedPlot = 2.6;
double PotassiumTotalUptakeTreatedPlot = 15;

double NitrogenTotalUptakeControlPlot = 12;
double PhosphorusTotalUptakeControlPlot = 2;
double PotassiumTotalUptakeControlPlot = 12;

int NFertilizerDose = 100;
int PFertilizerDose = 30;
int KFertilizerDose = 30;

int Yield = 30;
int T = 40;



byte values[11];
SoftwareSerial mod(14,15);
 
void setup() {
  Serial.begin(9600);
  mod.begin(9600);
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);
  lcd.init();
  lcd.backlight();
  pinMode(selectButtonPin, INPUT_PULLUP);

  pinMode(increaseButtonPin, INPUT_PULLUP);
  pinMode(decreaseButtonPin, INPUT_PULLUP);
  
  delay(1000);
}
 
void loop() {

  int soilMoisture = readSoilMoisture();
  if (soilMoisture < 4000){
  byte val1,val2,val3;
  val1 = nitrogen();
  delay(250);
  val2 = phosphorous();
  delay(250);
  val3 = potassium();
  delay(250);


  if (ControlPlotStatus == 0 && TreatedPlotStatus == 0 && SoilPlotStatus == 0){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ControlPlotTest");
    lcd.setCursor(0, 1);
    lcd.print("Test: ");
    lcd.print(ControlPlotTest + 1);
    lcd.print("/");
    lcd.print(ControlPlotTestRepetition);

    if ( digitalRead(selectButtonPin) == LOW){
      ControlPlotN[ControlPlotTest] = nitrogen();  // Read nitrogen value
      delay(250);
      ControlPlotP[ControlPlotTest] = phosphorous();  // Read phosphorous value
      delay(250);
      ControlPlotK[ControlPlotTest] = potassium();  // Read potassium value
      delay(250);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("N: ");
    lcd.print(nitrogen());
    lcd.setCursor(7, 0);
    lcd.print("P: ");
    lcd.print(phosphorous());
    lcd.setCursor(5, 1);
    lcd.print("K: ");
    lcd.print(potassium());
    delay(1000);
      ControlPlotTest++;
    }
    if (ControlPlotTest == ControlPlotTestRepetition){
      ControlPlotStatus = 1;
    }
  }
  if (ControlPlotStatus == 1 && TreatedPlotStatus == 0 && SoilPlotStatus == 0){
    ControlPlotNmean = calculateMean(ControlPlotN, ControlPlotTestRepetition);
    ControlPlotPmean = calculateMean(ControlPlotP, ControlPlotTestRepetition);
    ControlPlotKmean = calculateMean(ControlPlotK, ControlPlotTestRepetition);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("N: ");
    lcd.print(ControlPlotNmean);
    lcd.setCursor(7, 0);
    lcd.print("P: ");
    lcd.print(ControlPlotPmean);
    lcd.setCursor(5, 1);
    lcd.print("K: ");
    lcd.print(ControlPlotKmean);
    delay(2000);
    ControlPlotStatus = 2;
  }

  if (ControlPlotStatus == 2 && TreatedPlotStatus == 0 && SoilPlotStatus == 0){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TreatedPlotTest");
    lcd.setCursor(0, 1);
    lcd.print("Test: ");
    lcd.print(TreatedPlotTest + 1);
    lcd.print("/");
    lcd.print(TreatedPlotTestRepetition);

    if ( digitalRead(selectButtonPin) == LOW){
      TreatedPlotN[TreatedPlotTest] = nitrogen();  // Read nitrogen value
      delay(250);
      TreatedPlotP[TreatedPlotTest] = phosphorous();  // Read phosphorous value
      delay(250);
      TreatedPlotK[TreatedPlotTest] = potassium();  // Read potassium value
      delay(250);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("N: ");
    lcd.print(nitrogen());
    lcd.setCursor(7, 0);
    lcd.print("P: ");
    lcd.print(phosphorous());
    lcd.setCursor(5, 1);
    lcd.print("K: ");
    lcd.print(potassium());
    delay(1000);
      TreatedPlotTest++;
    }
    if (TreatedPlotTest == TreatedPlotTestRepetition){
      TreatedPlotStatus = 1;
    }
  }

  if (ControlPlotStatus == 2 && TreatedPlotStatus == 1 && SoilPlotStatus == 0){
    TreatedPlotNmean = calculateMean(TreatedPlotN, TreatedPlotTestRepetition);
    TreatedPlotPmean = calculateMean(TreatedPlotP, TreatedPlotTestRepetition);
    TreatedPlotKmean = calculateMean(TreatedPlotK, TreatedPlotTestRepetition);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("N: ");
    lcd.print(TreatedPlotNmean);
    lcd.setCursor(7, 0);
    lcd.print("P: ");
    lcd.print(TreatedPlotPmean);
    lcd.setCursor(5, 1);
    lcd.print("K: ");
    lcd.print(TreatedPlotKmean);
    delay(2000);
    TreatedPlotStatus = 2;
  }

      if (ControlPlotStatus == 2 && TreatedPlotStatus == 2 && SoilPlotStatus == 0 && LandSizeStatus == 0){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter Land Size");
    delay(2000); 
    LandSizeStatus = 1;
    }

    if (ControlPlotStatus == 2 && TreatedPlotStatus == 2 && SoilPlotStatus == 0 && LandSizeStatus == 1){
    displaySelectedNumbers();
    LandSizeSelection();
    
    }
    if (ControlPlotStatus == 2 && TreatedPlotStatus == 2 && SoilPlotStatus == 0 && LandSizeStatus == 2){
      
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Soil Test");
    lcd.setCursor(0, 1);
    lcd.print("Test: ");
    lcd.print(SoilPlotTest + 1);
    lcd.print("/");
    lcd.print(SoilPlotTestRepetition);

    if ( digitalRead(selectButtonPin) == LOW){
      SoilPlotN[SoilPlotTest] = nitrogen();  // Read nitrogen value
      delay(250);
      SoilPlotP[SoilPlotTest] = phosphorous();  // Read phosphorous value
      delay(250);
      SoilPlotK[SoilPlotTest] = potassium();  // Read potassium value
      delay(250);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("N: ");
    lcd.print(nitrogen());
    lcd.setCursor(7, 0);
    lcd.print("P: ");
    lcd.print(phosphorous());
    lcd.setCursor(5, 1);
    lcd.print("K: ");
    lcd.print(potassium());
    delay(1000);
      SoilPlotTest++;
    }
    if (SoilPlotTest == SoilPlotTestRepetition){
      SoilPlotStatus = 1;
    }
  }

  if (ControlPlotStatus == 2 && TreatedPlotStatus == 2 && SoilPlotStatus == 1 && LandSizeStatus == 2){
    SoilPlotNmean = calculateMean(SoilPlotN, SoilPlotTestRepetition);
    SoilPlotPmean = calculateMean(SoilPlotP, SoilPlotTestRepetition);
    SoilPlotKmean = calculateMean(SoilPlotK, SoilPlotTestRepetition);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("N: ");
    lcd.print(SoilPlotNmean);
    lcd.setCursor(7, 0);
    lcd.print("P: ");
    lcd.print(SoilPlotPmean);
    lcd.setCursor(5, 1);
    lcd.print("K: ");
    lcd.print(SoilPlotKmean);
    delay(2000);

    SoilPlotStatus = 2;
  }

    } if (soilMoisture > 4000 && SoilPlotStatus != 2) {
      Serial.println(soilMoisture);
    // Display a message on the LCD indicating low soil moisture
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SoilMoisture Low");
    lcd.setCursor(0, 1);
    lcd.print("Water the soil!");
    delay(1000);
  }
  if (ControlPlotStatus == 2 && TreatedPlotStatus == 2 && SoilPlotStatus == 2  && LandSizeStatus == 2){
    double NitrogenNR = NitrogenTotalNutrientUptake / Yield;
double NitrogenCSpercent = (NitrogenTotalUptakeControlPlot / ControlPlotNmean) * 100;
double NitrogenCF = NitrogenTotalUptakeTreadedPlot - (TreatedPlotNmean * NitrogenCSpercent / 100);
double NitrogenCFpercent = (NitrogenCF / NFertilizerDose) * 100;
double NitrogenAConstant = (NitrogenNR / NitrogenCFpercent) * 100;
double NitrogenBConstant = (NitrogenCSpercent / NitrogenCFpercent);
double NitrogenFD = (NitrogenAConstant * T) - (NitrogenBConstant * SoilPlotNmean);

double PhosphorusNR = PhosphorusTotalNutrientUptake / Yield;
double PhosphorusCSpercent = (PhosphorusTotalUptakeControlPlot / ControlPlotPmean) * 100;
double PhosphorusCF = PhosphorusTotalUptakeTreatedPlot - (TreatedPlotPmean * PhosphorusCSpercent / 100);
double PhosphorusCFpercent = (PhosphorusCF / PFertilizerDose) * 100;
double PhosphorusAConstant = (PhosphorusNR / PhosphorusCFpercent) * 100;
double PhosphorusBConstant = (PhosphorusCSpercent / PhosphorusCFpercent);
double PhosphorusFD = (PhosphorusAConstant * T) - (PhosphorusBConstant * SoilPlotPmean);

double PotassiumNR = PotassiumTotalNutrientUptake / Yield;
double PotassiumCSpercent = (PotassiumTotalUptakeControlPlot / ControlPlotKmean) * 100;
double PotassiumCF = PotassiumTotalUptakeTreatedPlot   - (TreatedPlotKmean * PotassiumCSpercent / 100);
double PotassiumCFpercent = (PotassiumCF / KFertilizerDose) * 100;
double PotassiumAConstant = (PotassiumNR / PotassiumCFpercent) * 100;
double PotassiumBConstant = (PotassiumCSpercent / PotassiumCFpercent);
double PotassiumFD = (PotassiumAConstant * T) - (PotassiumBConstant * SoilPlotKmean);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Fertilizer");
    lcd.setCursor(0, 1);
    lcd.print("Recommendation");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("N: ");
    lcd.print(NitrogenFD
    );
    lcd.setCursor(0, 1);
    lcd.print("P: ");
    lcd.print(PhosphorusFD);
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("K: ");
    lcd.print(PotassiumFD);
    delay(2000);


  if (digitalRead(selectButtonPin) == LOW) {
      goToSoilTest = true;
    }

  }

  if (goToSoilTest) {
    // Reset relevant variables to go back to the soil testing code block
    SoilPlotStatus = 0;
    SoilPlotTest = 0;
    LandSizeStatus = 0;
delete[] SoilPlotN;
delete[] SoilPlotP;
delete[] SoilPlotK;

// Set the pointers to nullptr to avoid using them after deallocation
SoilPlotN = nullptr;
SoilPlotP = nullptr;
SoilPlotK = nullptr;


    goToSoilTest = false; // Reset the flag
  }

}

int readSoilMoisture() {
  int soilMoistureValue = analogRead(16); // Assuming the sensor is connected to A0
  return soilMoistureValue;
}

int calculateMean(int arr[], int length) {
  int sum = 0;
  for (int i = 0; i < length; i++) {
    sum += arr[i];
  }
  return sum / length;
}

 
byte nitrogen(){
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(mod.write(nitro,sizeof(nitro))==8){;
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
    //Serial.print(mod.read(),HEX);
    values[i] = mod.read();
    }
  }
  return values[4];
}
 
byte phosphorous(){
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(mod.write(phos,sizeof(phos))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
    //Serial.print(mod.read(),HEX);
    values[i] = mod.read();
    }
  }
  return values[4];
}
 
byte potassium(){
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(mod.write(pota,sizeof(pota))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
    //Serial.print(mod.read(),HEX);
    values[i] = mod.read();
    }
  }
  return values[4];
}

void displaySelectedNumbers() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Land Length: ");
  lcd.print(selectedNumber1);
  lcd.setCursor(0, 1);
  lcd.print("Land Width: ");
  lcd.print(selectedNumber2);
}

void LandSizeSelection() {
  if (digitalRead(increaseButtonPin) == LOW) {
    if (Selection == 0) {
      selectedNumber1 = (selectedNumber1 + 1) % 51; // Increment by 1 and limit to 50
    } else {
      selectedNumber2 = (selectedNumber2 + 1) % 51; // Increment by 1 and limit to 50
    }
    displaySelectedNumbers();
    delay(300);
  }

  if (digitalRead(decreaseButtonPin) == LOW) {
    if (Selection == 0) {
      selectedNumber1 = (selectedNumber1 - 0 + 50) % 51; // Decrement by 1 and limit to 50
    } else {
      selectedNumber2 = (selectedNumber2 - 0 + 50) % 51; // Decrement by 1 and limit to 50
    }
    displaySelectedNumbers();
    delay(300);
  }


  if (digitalRead(selectButtonPin) == LOW) {
    if (Selection == 0) {
      Selection = 1;

    } else {
      result = selectedNumber1 * selectedNumber2;
        lcd.clear();
        lcd.setCursor(3, 0);
        lcd.print("Land Size");
        lcd.setCursor(0, 1);
        lcd.print(result);
        lcd.print("sq ft");
        delay(2000);
      Selection = 0;
      LandSizeStatus = 2;
      SoilPlotTestRepetition = result / 100;  // Calculate the size based on land size

// Allocate memory for the arrays
SoilPlotN = new int[SoilPlotTestRepetition];
SoilPlotP = new int[SoilPlotTestRepetition];
SoilPlotK = new int[SoilPlotTestRepetition];

    }
    delay(300);
  }
}