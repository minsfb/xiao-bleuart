// void setup() 
// {
//   Serial.begin(115200);
//   while(!Serial);

//   pinMode(PIN_VBAT, INPUT);    //Battery Voltage monitoring pin

//   pinMode(P0_13, OUTPUT);   //Charge Current setting pin
//   pinMode(P0_14, OUTPUT);   //Enable Battery Voltage monitoring pin
//   digitalWrite(P0_13, LOW); //Charge Current 100mA   
//   digitalWrite(P0_14, LOW); //Enable

//   analogReference(AR_VDD);  //Vref=3.3V
//   analogReadResolution(12);         //12bits
// }

// void loop() 
// {
//   digitalWrite(LED_BLUE, LOW);
//   delay(500);
  
//   int Vadc = analogRead(PIN_VBAT);
//   Serial.print(Vadc);Serial.print(", ");

//   float Vbatt = ((510e3 + 1000e3) / 510e3) * 3.3 * Vadc / 4095;
//   Serial.print("0x");Serial.print(Vadc, HEX);
//   Serial.print(", "); 
//   Serial.println(Vbatt, 3); 
  
//   digitalWrite(LED_BLUE, HIGH);
//   delay(500);
// }