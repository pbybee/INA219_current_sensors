#include <Wire.h>
#include <Adafruit_INA219.h>


Adafruit_INA219 ina219_1(0x40);
Adafruit_INA219 ina219_2(0x41);
Adafruit_INA219 ina219_3(0x44);
Adafruit_INA219 ina219_4(0x45);

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
    Serial.println("Failed to find INA219 1 chip");
    while (1) { delay(10); }
  }

   if (! ina219_2.begin()) {
    Serial.println("Failed to find INA219 2 chip");
    while (1) { delay(10); }
  }

  if (! ina219_3.begin()) {
    Serial.println("Failed to find INA219 3 chip");
    while (1) { delay(10); }
  }

  if (! ina219_4.begin()) {
    Serial.println("Failed to find INA219 4 chip");
    while (1) { delay(10); }
  }

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
  ina219_2.setCalibration_12V_10A(0.00315);
  ina219_3.setCalibration_12V_10A(0.00315);
  ina219_4.setCalibration_12V_10A(0.00315);

  Serial.println("Measuring voltage and current with INA219 ...");
}

void loop(void) 
{
  float a_shuntvoltage = 0;
  float a_busvoltage = 0;
  float a_current_mA = 0;
  float a_loadvoltage = 0;
  float a_power_mW = 0;

  float b_shuntvoltage = 0;
  float b_busvoltage = 0;
  float b_current_mA = 0;
  float b_loadvoltage = 0;
  float b_power_mW = 0;

  float c_shuntvoltage = 0;
  float c_busvoltage = 0;
  float c_current_mA = 0;
  float c_loadvoltage = 0;
  float c_power_mW = 0;

  float d_shuntvoltage = 0;
  float d_busvoltage = 0;
  float d_current_mA = 0;
  float d_loadvoltage = 0;
  float d_power_mW = 0;

  a_shuntvoltage = ina219_1.getShuntVoltage_mV();
  a_busvoltage = ina219_1.getBusVoltage_V();
  a_current_mA = ina219_1.getCurrent_mA();
  a_power_mW = ina219_1.getPower_mW();
  a_loadvoltage = a_busvoltage + (a_shuntvoltage / 1000);

  b_shuntvoltage = ina219_2.getShuntVoltage_mV();
  b_busvoltage = ina219_2.getBusVoltage_V();
  b_current_mA = ina219_2.getCurrent_mA();
  b_power_mW = ina219_2.getPower_mW();
  b_loadvoltage = b_busvoltage + (b_shuntvoltage / 1000);

  c_shuntvoltage = ina219_3.getShuntVoltage_mV();
  c_busvoltage = ina219_3.getBusVoltage_V();
  c_current_mA = ina219_3.getCurrent_mA();
  c_power_mW = ina219_3.getPower_mW();
  c_loadvoltage = c_busvoltage + (c_shuntvoltage / 1000);

  d_shuntvoltage = ina219_4.getShuntVoltage_mV();
  d_busvoltage = ina219_4.getBusVoltage_V();
  d_current_mA = ina219_4.getCurrent_mA();
  d_power_mW = ina219_4.getPower_mW();
  d_loadvoltage = d_busvoltage + (d_shuntvoltage / 1000);

  char buffer[200];
  
  Serial.println("  I2C ID  |    Bus V     |    Shunt V   |    Load V    |  Current mA  |   Power mW");
  sprintf(buffer, "   0x40   |%14.02f|%14.02f|%14.02f|%14.02f|%14.02f", a_busvoltage, a_shuntvoltage, a_loadvoltage, a_current_mA, a_power_mW);
  Serial.println(buffer);
  sprintf(buffer, "   0x41   |%14.02f|%14.02f|%14.02f|%14.02f|%14.02f", b_busvoltage, b_shuntvoltage, b_loadvoltage, b_current_mA, b_power_mW);
  Serial.println(buffer);
  sprintf(buffer, "   0x44   |%14.02f|%14.02f|%14.02f|%14.02f|%14.02f", c_busvoltage, c_shuntvoltage, c_loadvoltage, c_current_mA, c_power_mW);
  Serial.println(buffer);
  sprintf(buffer, "   0x45   |%14.02f|%14.02f|%14.02f|%14.02f|%14.02f", d_busvoltage, d_shuntvoltage, d_loadvoltage, d_current_mA, d_power_mW);
  Serial.println(buffer);
  Serial.println("");

  delay(1000);
}
