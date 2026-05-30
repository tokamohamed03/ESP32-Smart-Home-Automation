#include <DHT.h>
#include <BluetoothSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pin Definitions
const int LAMP_RELAY_PIN = 18;
const int OUTLET_RELAY_PIN = 19;
const int BUZZER_PIN = 32;
const int MANUAL_SWITCH_PIN = 34;
const int TEMP_SENSOR_PIN = 27;
const int DOOR_SENSOR_PIN = 33;
const int SCL_LCD = 22;
const int SDA_LCD = 21;
const int PUSH_BUTTON_HIGHTEMP = 17;
const int PUSH_BUTTON_LOWTEMP = 4;

// Constants
const int DHTTYPE = DHT11;
const int DEFAULT_TEMP_THRESHOLD = 40;
const int TEMP_THRESHOLD_STEP = 1;
const int MIN_TEMP_THRESHOLD = 20;
const int MAX_TEMP_THRESHOLD = 60;
const int UPDATE_INTERVAL = 250;
const int SENSOR_UPDATE_INTERVAL = 1000;
const int BUTTON_DEBOUNCE_DELAY = 50;

BluetoothSerial SerialBT;
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(TEMP_SENSOR_PIN, DHTTYPE);

bool lampState = false;
bool outletState = false;
bool doorClosed = true;
bool tempAlert = false;
float temperature = 0;
int tempThreshold = DEFAULT_TEMP_THRESHOLD;

unsigned long lastSensorRead = 0;
unsigned long lastBuzzerToggle = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long lastSensorSend = 0;
bool buzzerOn = false;

bool lastSwitchState = HIGH;
bool switchToggleDetected = false;
bool bluetoothConnected = false;

bool lastDoorState = true;
bool doorStateChanged = false;

bool lastHighTempButtonState = HIGH;
bool lastLowTempButtonState = HIGH;
unsigned long lastHighTempButtonTime = 0;
unsigned long lastLowTempButtonTime = 0;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("SmartHomeController");
  
  pinMode(LAMP_RELAY_PIN, OUTPUT);
  pinMode(OUTLET_RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MANUAL_SWITCH_PIN, INPUT_PULLUP);
  pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP);
  pinMode(PUSH_BUTTON_HIGHTEMP, INPUT_PULLUP);
  pinMode(PUSH_BUTTON_LOWTEMP, INPUT_PULLUP);
  
  digitalWrite(LAMP_RELAY_PIN, HIGH);
  digitalWrite(OUTLET_RELAY_PIN, HIGH);
  digitalWrite(BUZZER_PIN, LOW);
  
  Wire.begin(SDA_LCD, SCL_LCD);
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  delay(1000);
  
  dht.begin();
  delay(2000);
  
  temperature = dht.readTemperature();
  if (isnan(temperature)) temperature = 0;
  tempAlert = (temperature > tempThreshold);
  
  doorClosed = (digitalRead(DOOR_SENSOR_PIN) == LOW);
  lastDoorState = doorClosed;
  
  lastSwitchState = digitalRead(MANUAL_SWITCH_PIN);
  lastHighTempButtonState = digitalRead(PUSH_BUTTON_HIGHTEMP);
  lastLowTempButtonState = digitalRead(PUSH_BUTTON_LOWTEMP);
  
  Serial.println("Smart Home Controller Started");
  updateLCD();
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (SerialBT.hasClient() != bluetoothConnected) {
    bluetoothConnected = SerialBT.hasClient();
    if (bluetoothConnected) {
      Serial.println("Bluetooth connected!");
      sendSensorData();
    }
  }
  
  bool currentSwitchState = digitalRead(MANUAL_SWITCH_PIN);
  if (currentSwitchState != lastSwitchState) {
    delay(20);
    if (digitalRead(MANUAL_SWITCH_PIN) == currentSwitchState) {
      lampState = !lampState;
      switchToggleDetected = true;
      if (bluetoothConnected) sendSensorData();
    }
    lastSwitchState = currentSwitchState;
  }
  
  if (currentMillis - lastSensorRead >= 2000) {
    float temp = dht.readTemperature();
    if (!isnan(temp)) {
      temperature = temp;
      tempAlert = (temperature > tempThreshold);
    }
    lastSensorRead = currentMillis;
  }
  
  bool newDoorState = (digitalRead(DOOR_SENSOR_PIN) == LOW);
  if (newDoorState != doorClosed) {
    doorClosed = newDoorState;
    if (!doorClosed && lastDoorState) doorStateChanged = true;
    lastDoorState = doorClosed;
    if (bluetoothConnected) sendSensorData();
  }
  
  bool currentHighTempButtonState = digitalRead(PUSH_BUTTON_HIGHTEMP);
  if (currentHighTempButtonState == LOW && lastHighTempButtonState == HIGH) {
    if (currentMillis - lastHighTempButtonTime > BUTTON_DEBOUNCE_DELAY) {
      if (tempThreshold < MAX_TEMP_THRESHOLD) {
        tempThreshold += TEMP_THRESHOLD_STEP;
        tempAlert = (temperature > tempThreshold);
        Serial.print("Threshold increased to: ");
        Serial.println(tempThreshold);
        updateLCD();
        if (bluetoothConnected) sendSensorData();
      }
      lastHighTempButtonTime = currentMillis;
    }
  }
  lastHighTempButtonState = currentHighTempButtonState;
  
  bool currentLowTempButtonState = digitalRead(PUSH_BUTTON_LOWTEMP);
  if (currentLowTempButtonState == LOW && lastLowTempButtonState == HIGH) {
    if (currentMillis - lastLowTempButtonTime > BUTTON_DEBOUNCE_DELAY) {
      if (tempThreshold > MIN_TEMP_THRESHOLD) {
        tempThreshold -= TEMP_THRESHOLD_STEP;
        tempAlert = (temperature > tempThreshold);
        Serial.print("Threshold decreased to: ");
        Serial.println(tempThreshold);
        updateLCD();
        if (bluetoothConnected) sendSensorData();
      }
      lastLowTempButtonTime = currentMillis;
    }
  }
  lastLowTempButtonState = currentLowTempButtonState;
  
  digitalWrite(LAMP_RELAY_PIN, lampState ? LOW : HIGH);
  digitalWrite(OUTLET_RELAY_PIN, outletState ? LOW : HIGH);
  
  if (tempAlert) {
    if (currentMillis - lastBuzzerToggle >= 500) {
      buzzerOn = !buzzerOn;
      digitalWrite(BUZZER_PIN, buzzerOn ? HIGH : LOW);
      lastBuzzerToggle = currentMillis;
    }
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    buzzerOn = false;
  }
  
  if (SerialBT.available() > 0) {
    String cmd = SerialBT.readStringUntil('\n');
    cmd.trim();
    cmd.toUpperCase();
    
    Serial.print("Bluetooth Command: ");
    Serial.println(cmd);
    
    if (cmd == "LAMPTOGGLE") {
      lampState = !lampState;
      Serial.println("Bluetooth: Lamp toggled");
      sendSensorData();
    } 
    else if (cmd == "OUTLETONAPP") {
      outletState = true;
      Serial.println("Bluetooth: Outlet turned ON");
      sendSensorData();
    } 
    else if (cmd == "OUTLETOFFAPP") {
      outletState = false;
      Serial.println("Bluetooth: Outlet turned OFF");
      sendSensorData();
    }
  }
  
  if (bluetoothConnected && (currentMillis - lastSensorSend >= SENSOR_UPDATE_INTERVAL)) {
    sendSensorData();
    lastSensorSend = currentMillis;
  }
  
  if (currentMillis - lastDisplayUpdate >= UPDATE_INTERVAL) {
    updateDisplay();
    updateLCD();
    lastDisplayUpdate = currentMillis;
  }
  
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toUpperCase();
    
    if (cmd == "LAMP ON") {
      lampState = true;
      Serial.println("Command: Lamp turned ON");
      if (bluetoothConnected) sendSensorData();
    } 
    else if (cmd == "LAMP OFF") {
      lampState = false;
      Serial.println("Command: Lamp turned OFF");
      if (bluetoothConnected) sendSensorData();
    } 
    else if (cmd == "OUTLET ON") {
      outletState = true;
      Serial.println("Command: Outlet turned ON");
      if (bluetoothConnected) sendSensorData();
    } 
    else if (cmd == "OUTLET OFF") {
      outletState = false;
      Serial.println("Command: Outlet turned OFF");
      if (bluetoothConnected) sendSensorData();
    }
    updateDisplay();
    updateLCD();
  }
}

void sendSensorData() {
  while (SerialBT.available() > 0) SerialBT.read();
  
  String dataString = "";
  dataString += lampState ? "ON" : "OFF";
  dataString += "~";
  dataString += outletState ? "ON" : "OFF";
  dataString += "~";
  dataString += String(temperature, 1);
  dataString += "~";
  dataString += doorClosed ? "CLOSED" : "OPEN";
  dataString += "~";
  dataString += String(tempThreshold);
  dataString += "~";
  dataString += doorStateChanged ? "TOGGLE" : ".";
  dataString += "~";
  
  if (doorStateChanged) doorStateChanged = false;
  
  SerialBT.print(dataString);
  SerialBT.print("\n");
  Serial.print("Sent: '");
  Serial.print(dataString);
  Serial.println("'");
}

void updateDisplay() {
  static bool firstUpdate = true;
  if (!firstUpdate) Serial.print("\033[9A");
  firstUpdate = false;
  
  Serial.println("=== SMART HOME STATUS ===");
  Serial.print("Bluetooth: ");
  Serial.println(bluetoothConnected ? "CONNECTED" : "DISCONNECTED");
  Serial.print("Temperature: ");
  Serial.print(temperature, 1);
  Serial.print("°C  Threshold: ");
  Serial.print(tempThreshold);
  Serial.print("°C  ");
  Serial.println(tempAlert ? "[ALERT!]" : "[OK]");
  Serial.print("Door: ");
  Serial.println(doorClosed ? "CLOSED" : "OPEN");
  Serial.print("Lamp: ");
  Serial.print(lampState ? "ON " : "OFF");
  if (switchToggleDetected) {
    Serial.println("  [Switch toggled]");
    switchToggleDetected = false;
  } else Serial.println();
  Serial.print("Outlet: ");
  Serial.println(outletState ? "ON" : "OFF");
  Serial.print("Switch: ");
  Serial.println(digitalRead(MANUAL_SWITCH_PIN) == LOW ? "ON (LOW)" : "OFF (HIGH)");
  Serial.print("Buttons: High=");
  Serial.print(digitalRead(PUSH_BUTTON_HIGHTEMP) == LOW ? "PRESSED" : "RELEASED");
  Serial.print(" Low=");
  Serial.println(digitalRead(PUSH_BUTTON_LOWTEMP) == LOW ? "PRESSED" : "RELEASED");
  Serial.println("-------------------------");
}

void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(temperature, 1);
  lcd.print("C");
  
  if (tempAlert) {
    lcd.setCursor(12, 0);
    lcd.print("ALERT!");
  } else {
    lcd.setCursor(13, 0);
    lcd.print("OK");
  }
  
  lcd.setCursor(0, 1);
  lcd.print("Thr:");
  lcd.print(tempThreshold);
  lcd.print("C Door:");
  lcd.print(doorClosed ? "CLSD" : "OPEN");
}