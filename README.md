# axTruder
Open Source Firmware for DIY Filament Extruder based on ESP32.
See Full instructions on daniel-strohbach.de/diy-filamentextruder-teil-1

Concept:
Shred your failed Prints and remelt the Plastic to new Filament. Inspired by the Lyman Extruder.

    -Based on the ESP32-Webserver Work by FipsOK - https://fipsok.de/Esp32-Webserver/Esp32
    - Digital Caliper ReadOut based on Work of MGX3D - See https://github.com/MGX3D/EspDRO for Details
    

Features:

    - Filament Sensor from hacked digital calipers (WIP)
    - Filament Width Control via speed difference (WIP)
    - PID Heat Control (PID Tuning required individually)
    - Web Interface via Wifi
    - using highcharts js to visualise temperature and diameter in graphs
    - MQTT Data Logging (See MQTT Tab for Topics)
    - Preheat Protection (do not rotate untill nozzle temp is reached)

To Do
   - PID Temperature Control testing and optimizing controller values
   - mDNS
   - captive Portal
   - OTA Updates
   - Wifi Multi
   - thermal runaway protection
   - integrate JSON better
   - add support more Thermistors
   - manual rotation time setting in ui
   - speed presets in material dropdown

Parts needed:

    - ESP32 Board
    - PSU 12V DC 30A
    - K-Type Thermistor WENT01
    - MAX6675 SPI Converter
    - SSR 25A 3-32V DC 24-380V AC (Suitable for inductive loads)
    - Heaterband 230V 25x25cm
    - L298N Motordriver
    - 12V DC Gearmotor
    - Various other Hardwareparts, as Screws, Pipes etc.

Coded in Arduino IDE - requirements:

    - 
