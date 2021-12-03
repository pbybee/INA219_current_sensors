#include <Wire.h>
#include <Adafruit_INA219.h>


Adafruit_INA219 ina219_1(0x40);
// Adafruit_INA219 ina219_2(0x41);

void Adafruit_INA219::setCalibration_12V_10A(float rval) {

  float VBUS_MAX = 14;
  float VSHUNT_MAX = 0.06;
  float RSHUNT = 0.00315;

  // 1. Determine max possible current
  float MaxPossible_I = VSHUNT_MAX / RSHUNT;
  // MaxPossible_I = 0.4A

  // 2. Determine max expected current
  float MaxExpected_I = 10;

  // 3. Calculate possible range of LSBs (Min = 15-bit, Max = 12-bit)
  float MinimumLSB = MaxExpected_I/32767;
  // MinimumLSB = 0.0000122              (12uA per bit)
  // MaximumLSB = MaxExpected_I/4096
  // MaximumLSB = 0.0000977              (98uA per bit)

  // 4. Choose an LSB between the min and max values
  //    (Preferrably a roundish number close to MinLSB)
  float CurrentLSB = 0.0007;// (50uA per bit)

  // 5. Compute the calibration register
  uint32_t Cal = trunc (0.04096 / (CurrentLSB * RSHUNT));
  // Cal = 8192 (0x2000)

  ina219_calValue = Cal;

  // Set multipliers to convert raw current/power values
  ina219_currentDivider_mA = 1;    // Current LSB = 50uA per bit (1000/50 = 20)
  ina219_powerMultiplier_mW = 1.0f; // Power LSB = 1mW per bit

  // Set Calibration register to 'Cal' calculated above
  Adafruit_BusIO_Register calibration_reg =
      Adafruit_BusIO_Register(i2c_dev, INA219_REG_CALIBRATION, 2, MSBFIRST);
  calibration_reg.write(ina219_calValue, 2);
  // Set Config register to take into account the settings above
  uint16_t config = INA219_CONFIG_BVOLTAGERANGE_16V |
                    INA219_CONFIG_GAIN_1_40MV | INA219_CONFIG_BADCRES_12BIT |
                    INA219_CONFIG_SADCRES_12BIT_1S_532US |
                    INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;

  Adafruit_BusIO_Register config_reg =
      Adafruit_BusIO_Register(i2c_dev, INA219_REG_CONFIG, 2, MSBFIRST);
  _success = config_reg.write(config, 2);
}


void setup(void) 
{
  Serial.begin(115200);
  while (!Serial) {
      // will pause Zero, Leonardo, etc until serial console opens
      delay(1);
  }

  //uint32_t currentFrequency;
    
  Serial.println("Hello!");
  
  if (! ina219_1.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }

  //  if (! ina219_2.begin()) {
  //   Serial.println("Failed to find INA219 chip");
  //   while (1) { delay(10); }
  // }

  // To use a slightly lower 32V, 1A range (higher precision on amps):
  //ina219.setCalibration_32V_1A();
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
  // IDs and values to use as RSHUNT
  /*
  ID     RSHUNT
  --     ------
  0x40   0.00315
  0x41   
  
  
  */
  ina219_1.setCalibration_12V_10A(0.00315);
  // ina219_2.setCalibration_12V_10A(0.00315);

  Serial.println("Measuring voltage and current with INA219 ...");
}

void loop(void) 
{
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;

  shuntvoltage = ina219_1.getShuntVoltage_mV();
  busvoltage = ina219_1.getBusVoltage_V();
  current_mA = ina219_1.getCurrent_mA();
  power_mW = ina219_1.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  
  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
  Serial.println("");

  // shuntvoltage = ina219_2.getShuntVoltage_mV();
  // busvoltage = ina219_2.getBusVoltage_V();
  // current_mA = ina219_2.getCurrent_mA();
  // power_mW = ina219_2.getPower_mW();
  // loadvoltage = busvoltage + (shuntvoltage / 1000);
  
  // Serial.print("2 Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  // Serial.print("2 Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
  // Serial.print("2 Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  // Serial.print("2 Current:       "); Serial.print(current_mA); Serial.println(" mA");
  // Serial.print("2 Power:         "); Serial.print(power_mW); Serial.println(" mW");
  // Serial.println("");

  delay(2000);
}
