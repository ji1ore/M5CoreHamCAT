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

と入力し、RaspberryPiZero2Wにログインします。

④各種インストール
以下コマンドを実施してください。
なお、結構時間がかかります。

sudo apt -y update
sudo apt -y install bash
wget https://raw.githubusercontent.com/ji1ore/M5CoreHamCAT/main/RaspberryPiSetup/setup_netwk.sh
chmod +x setup_netwk.sh

※ここでネットワークが切れますので再度SSHログインしてください。

ipconfig /all

※IPv4アドレスが取得できてることを確認してください。

wget https://raw.githubusercontent.com/ji1ore/M5CoreHamCAT/main/RaspberryPiSetup/setup_fastapi_radio.sh
chmod +x setup_fastapi_radio.sh
bash setup_fastapi_radio.sh

sudo systemctl status fastapi
rm setup_fastapi_radio.sh

⑤確認
インストールが完了すると以下のようなメッセージが出るはずです。

