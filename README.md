# Gesture-controlled-Robotic-Arm
The project builds a gesture-controlled robotic arm where the movements of the robotic arm are controlled by human hand gestures.

- Two sensors, including the MPU6050 (accelerometer + gyroscope) and Flex sensor, are used to record hand positions and motions in 3D space. The sensor readings are then sent to the microcontroller of the robotic arm via the WiFi module of the ESP32.

- The main microcontroller used is the ATmega328PB, which does not have built-in WiFi communication. Therefore, the ESP32 is used to enable WiFi communication.

Code files include:
- 'arm_control': the bare metal C code to map the sensor readings into robotic movements.
- 'glove_sensors_reading': code to read the sensors data, including ADC and I2C libraries.
- 'ESP32_data_receiving': receiving data at the arm.
- 'ESP32_data_sending': sending data from sensors to the arm.

Real implementation:
- ![image](https://github.com/winston-nguyenn/Gesture-controlled-Robotic-Arm/assets/116818321/b43ba270-4c8a-44c6-9a75-abccfcd39931)

