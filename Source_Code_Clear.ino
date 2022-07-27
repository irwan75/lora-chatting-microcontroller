//Inisialisasi Library
#include <BluetoothSerial.h>
#include "heltec.h"

//Inisialisasi Frequency Radio
#define BAND    921E6  

//Pengecekan Keaktifan Bluetooth
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

//Inisialisasi object Bluetooth
BluetoothSerial SerialBT;

//Pendeklarasian variabel penyimpan pesan
String packSize = "--";
String packet ;

//Pendeklarasian nomor dan nama LoRa ESP32
String number="555";
String nama="Lora5";

//Method setup utama pada mikrokontroller
void setup() {

  Heltec.begin(true, true, true, true, BAND);
  SerialBT.begin(nama);
  delay(1000);

  //LoRa.onReceive(cbk);
  LoRa.receive();
}


//Method ketika menerima pesan dari gelombang radio
void terimaPesanLora(int packetSize) {
  //listing code untuk menyimpan pesan ke dalam variable
  packet ="";
  packSize = String(packetSize,DEC);
  for (int i = 0; i < packetSize; i++) {
    packet += (char) LoRa.read(); 
  }
  
    String nomor;
    String words = "";
    //listing code untuk memecah pesan menjadi 2 bagian, yaitu nomor dan isi pesan 
    for(auto x : packet){
      if(x=='^'){
        nomor=words;
        words="";
      }
      else{
        words=words+x;
      }
    }
  //listing code pengecekan nomor tujuan yang terdapat pada pesan yang ditangkap  
  if(nomor.equals(number)){
    kirimPesanBluetooth(words);
  }else{
    kirimPesanLora(packet);    
  }
}

//Method untuk mengirim pesan yang ditangkap ke smartphone melalui bluetooth
void kirimPesanBluetooth(String text){
  SerialBT.println(text);
}

//Method untuk membroadcast pesan melalui gelombang radio secara broadcast
void kirimPesanLora(String text){ 
    LoRa.beginPacket();

    LoRa.setTxPower(14,RF_PACONFIG_PASELECT_PABOOST);
    LoRa.print(text);
    LoRa.endPacket();
}


//Method looping utama pada mikrokontroller
void loop() {

  //Pendeklarasian variabel kondisi penerima pesan melalui gelombang radio
  int packetSize = LoRa.parsePacket();

  //jika pesan dari hasil capture melalui gelomban radio
  if (packetSize) {
      terimaPesanLora(packetSize); 
  }


  //jika ada pesan dari smartphone ke LoRa ESP32 melalui Bluetooth
  if (SerialBT.available()) {
    String text = SerialBT.readString();
    kirimPesanLora(text+"~"+number);
  }
}
