## Repo for INA219 current sensor with custom shunt

You will need to configure the board, I was using a nodeMCU ESP32 controller. But any "arduino" board should do.

You will also need to manually add the setCalibration_12V_10A(float rval) definition to the Adafruit_INA219.h file.

