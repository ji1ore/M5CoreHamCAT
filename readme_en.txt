① Introduction
M5CoreHamCAT is a system that uses a Raspberry Pi Zero2W connected to a transceiver and an M5CoreS3SE to perform CAT (Computer Aided Transceiver) control of the radio. It allows you to both retrieve information from and operate the transceiver via the M5CoreS3SE.

Technically, it wraps Hamlib running on the Raspberry Pi Zero2W with FastAPI, and the M5CoreS3SE communicates with it via FastAPI to control the radio and retrieve its information.

Currently, the system supports displaying radio information and receiving audio signals. Transmitting audio is not supported.

[Update: March 1, 2026]  
Since M5CoreHamCAT_Speaker can now output sound via Module Audio, it has been deprecated.

At present, operation has only been confirmed with the Yaesu FT-991A and IC-705. Compatibility with other transceivers has not been tested.
Additionally, operation on M5CoreS3, M5CoreS3Lite, or other M5Core series devices has not been verified.

② Required Components
To run this system, you will need the following:

M5CoreS3SE or M5Core2 (M5CoreS3SE is recommended for smoother operation)

Module Audio (Official M5 product, SKU: M144)

Raspberry Pi Zero2W

Wi-Fi router (Both devices must be on the same Wi-Fi network)

Unit Encoder (Official M5 product, SKU: U135)
Optional, but improves usability

Battery Bottom for M5Stack CoreS3  
Optional, but adds convenience

MicroSD card (16GB or larger, reliable brand recommended)

Various USB cables for powering the devices and connecting to the transceiver for CAT data

③ Setup Instructions (Raspberry Pi Zero2W)
Please refer to the following guide for setup:
https://github.com/ji1ore/M5CoreHamCAT/blob/main/RaspberryPiSetup/readme.txt

Main steps include:

Install Raspberry Pi Imager

Create an image using Raspberry Pi Imager (set Wi-Fi SSID and user password here)

Log in via SSH

Run the required commands (a shell script is provided, making the process easy but time-consuming)

④ Setup Instructions (M5CoreS3SE)
For the M5CoreS3SE, use M5Burner to flash the firmware.
The source code is available in the following GitHub folder:
https://github.com/ji1ore/M5CoreHamCAT/main/M5CoreHamCAT

The source is intended to be compiled using PlatformIO on Visual Studio Code.

Firmware flashing steps:

Download and install M5Burner

Launch M5Burner and register an account

Download M5CoreHamCAT

Connect the M5CoreS3SE to your computer via USB and click Burn  
(You can search for "M5CoreHamCAT" within M5Burner)

⑤ Notes
The system is still somewhat unstable. If it doesn’t work properly, try restarting it a few times.
If the transceiver information stops updating, connect to the Raspberry Pi Zero2W via SSH and reboot it using the following command:

sudo reboot now

Be careful not to select the wrong CAT device, or the connection will fail.
Repeated tapping may cause the system to reboot.
To prevent audio delay, the system reconnects every 10 minutes, which may cause a few seconds of silence.
When using M5Core2, you may need to press and hold the main screen controls a bit longer.

That’s all! Let me know if you want help with the GitHub setup or a diagram of the system 