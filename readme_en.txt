① Introduction
M5CoreHamCAT is a system that uses a Raspberry Pi Zero2W connected to a transceiver and an M5CoreS3SE to perform CAT control of the radio. The M5CoreS3SE communicates with the Raspberry Pi Zero2W to retrieve and control radio information.

Technically, the system wraps Hamlib running on the Raspberry Pi Zero2W with FastAPI, allowing the M5CoreS3SE to send requests to FastAPI to operate the radio and retrieve its status.

The system supports displaying and controlling radio information, but does not support transmitting or receiving audio or CW signals.
For audio reception, please use an FM transmitter and radio, or low-power wireless audio devices.
For CW transmission, consider using Wifi_Keyer, Remotekeyer, or other external solutions.

February 21, 2026  
We are releasing a prototype version of M5CoreHamCAT_Speaker.
This version allows the M5CoreS3SE, when on the radio connection screen, to send the SSID, password, and Raspberry Pi address to an ATOMS3Lite (ATOMIC SPK) connected to Port B.
It then works with the audio streaming service on the Raspberry Pi to output the radio audio through a speaker connected to the ATOMIC SPK.

LED color meanings:

Orange: Information not yet retrieved

Green: Information retrieved

Blue: Connected

Once the LED turns green, press and hold the A button to initiate the connection.
To disconnect the speaker, press and hold the A button again.
※ Please note: this feature is still unstable. It may take over 10 seconds to respond, or fail to connect on the first try.

Currently, the system has only been tested with the Yaesu FT-991A and IC-705.
Operation with other radios is untested.
Compatibility with M5CoreS3, M5CoreS3Lite, or other M5Core series devices is also unverified.

② Required Components
To run this system, you will need:

M5CoreS3SE

Raspberry Pi Zero2W

Wi-Fi router (both devices must be on the same network)

Unit Encoder (M5 original SKU: U135)
→ Optional, but improves usability

Battery bottom for M5Stack CoreS3
→ Optional, but improves convenience

MicroSD card (16GB or more, high reliability recommended)

USB cables for powering and connecting the radio, M5CoreS3SE, and Raspberry Pi Zero2W

③ Setup Instructions (Raspberry Pi Zero2W)
Please refer to the following guide:
https://github.com/ji1ore/M5CoreHamCAT/blob/main/RaspberryPiSetup/readme.txt

Main steps:

Install Raspberry Pi Imager

Create an image with Wi-Fi SSID and user password configured

SSH login

Run the required setup commands (a shell script is provided, though it may take some time)

④ Setup Instructions (M5CoreS3SE)
Use M5Burner to flash the firmware onto the M5CoreS3SE.
Source code is available here:
https://github.com/ji1ore/M5CoreHamCAT/main/M5CoreHamCAT

The source is intended to be compiled using PlatformIO on Visual Studio Code.

Firmware flashing steps:

Download and install M5Burner

Launch M5Burner and register an account

Download M5CoreHamCAT

Connect M5CoreS3SE to your computer via USB and burn the firmware

You can search for “M5CoreHamCAT” in M5Burner

⑤ Setup Instructions (AtomS3Lite)
Use M5Burner to flash the firmware onto the AtomS3Lite as well.
Search for “M5CoreHamCAT” in M5Burner, and you’ll find M5CoreHamCAT_Speaker.

⑥ Notes
The system is still quite unstable. If it doesn’t work properly, try restarting the devices a few times.
If the radio information stops updating, SSH into the Raspberry Pi Zero2W and reboot it using the following command:

bash
sudo reboot now
That’s all! 