①はじめに
M5CoreHamCATは、Raspberry Pi Zero2WとM5CoreS3SEを用い、無線機に接続したRaspberry Pi Zero2Wを使って
無線機をCAT操作し、その情報の取得、及び操作をM5CoreS3SE上で行うシステムです。
技術的には、Raspberry Pi Zero2W上のHamlibをFastAPIでWrapし、M5CoreS3SEからFastAPIを叩いて無線機の操作や
無線機の情報を取得する、ということを行っています。
できることは、無線機の情報の表示、操作です。
音声信号やCW信号の送受信を行うことはできません。
音の受信はFMトランスミッター及びラジオ、特定小電力無線による音声の送受信、
CW信号の送信はWifi_KeyerやRemotekeyerなど別の手段を使ってください。

現在のところ、Yaesu FT-991Aのみで動作確認を行っており、他の無線機での動作は未検証です。
また、 M5CoreS3やM5CoreS3Lite、他のM5Coreシリーズで動作するかは未検証です。

②必要なもの
当システムを動作させるために必要なものは以下のとおりです。
・M5CoreS3SE
・Raspberry Pi Zero2W
・Wifiルータ(上記２つの端末が同一Wifiネットワーク上に存在することを前提とします。)
・Unit Encoder(M5純正 SKU:U135) 
 なくても動作しますが、操作性が向上します。
・M5Stack CoreS3用バッテリーボトム
 なくても動作しますが、利便性があがりますので。
・MicroSDカード(16G以上、信頼性の高いもの)
・その他無線機やM5CoreS3SE、Raspberry Pi Zero2Wへの電源取得やCATデータ取得に用いるUSBケーブル類

③セットアップ手順(Raspberry Pi Zero2W)
https://github.com/ji1ore/M5CoreHamCAT/blob/main/RaspberryPiSetup/readme.txt
を参照してセットアップを行ってください。
主な手順は以下の通りです。
・Raspberry Pi Imagerのインストール
・Raspberry Pi Imagerの作成(ここでWifi SSIDの指定やユーザパスワードを指定します)
・SSHログイン
・必要コマンドの実施(シェルファイルを用意してありますので簡単ですが時間がかかります)

④セットアップ手順(M5CoreS3SE)
M5CoreS3SEでは、M5Burnerを用いてファームウェアの読み込みを行ってください。
Git上の以下フォルダにソースは公開します。
https://github.com/ji1ore/M5CoreHamCAT/main/M5CoreHamCAT
ソースはVisual Studio Code上のPlatformI/O上でのコンパイルを前提にしています。
ファームウェアの読み込み手順は以下のとおりです。
・M5Burnerをダウンロードし、インストールします。
・M5Burnerを起動します。ユーザー登録を行います。
・M5CoreHamCATをDownloadします。
・コンピュータにM5CoreS3SEをUSBケーブルで接続し、Burnします。

⑤注意点
まだ結構不安定です。
無線機の情報が取れなくなったら、Raspberry Pi Zero2WにSSH接続をしてリブートを行ってください。
リブートのコマンドは以下になります。

sudo reboot now 
