RaspberryPiのセットアップについて
①raspberryPiイメージャーのインストール
https://www.raspberrypi.com/software/

②起動用のMicroSDカード作成
・対象のイメージはRaspberry Pi OS Lite(64bit)
・Hostnameは「raspizero」だと以降の設定が楽
・設定上はユーザ名piを想定
・SSIDはイメージ作成時点で設定
・SSHを使ってセットアップを行う
ことを想定します。

なお、RaspberryPiZero2WのWifiですが、Wifiルータの
高速ローミング(11r)がONになっていると接続できないことがあります。
RaspberryPiZero2WがWifiに繋がらない場合、この可能性を疑ってください。

③SSHログイン
Dos窓やPowerShellに、

ssh pi@raspizero

とうち、RaspberryPiZero2Wにログインします。

④各種インストール
以下コマンドを実施してください。

sudo apt -y update
sudo apt -y install bash
wget https://raw.githubusercontent.com/ji1ore/M5CoreHamCAT/main/RaspberryPiSetup/setup_fastapi_radio.sh
chmod +x setup_fastapi_radio.sh
./setup_fastapi_radio.sh

sudo systemctl status fastapi

