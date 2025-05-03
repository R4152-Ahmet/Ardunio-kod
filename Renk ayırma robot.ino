/* Renk Ayıklama Robotu */

#include <Servo.h>      //Servo motorlar için servo kütüphanesi programa dahil edildi

Servo servo_ust;        //üstteki motor için servo_ust isimli bir nesne tanımlandı
Servo servo_alt;        //alttaki motor için servo_alt isimli bir nesne tanımlandı

/*Sensör üzerindeki pinlerin bağlı oldu arduino pinleri tanımlandı*/
int S0 = 5, S1 = 6, S2 = 3, S3 = 4;
int OUT = 2;
int LED = 7;    // OE pini

/* Renk yüzdeleri tanımlanıyor. Bu sayılar ortama ve sensöre göre değişiklik gösterebilir. */
int RenkYuzdesi[5][3] = { 
{40,40,33},     // Sarı renk için (Kırmızı, Yeşil ve Mavi yüzdeleri. Renkler bu ana renklerden oluşmaktadır)
{33,40,28},     // Mavi renk
{40,50,33},     // Beyaz renk
{40,40,28},     // Kırmızı renk
{33,40,33} };   // Yeşil renk

//Renkler için bir dizi oluşturuldu. Nesnenin rengi buradan çağırılır ve yazdırılır
String Renkler[5]={"Sari", "Mavi", "Beyaz", "Kirmizi", "Yesil"};  

int aralik = 4;     // Sensör hassasiyeti. Bu değer yüksek olursa renkler karışabilir
int KirmiziYuzdesi, YesilYuzdesi, MaviYuzdesi;  //Renk yüzdeleri için değişkenler oluşturuldu

int renk;         //renk isimli değişken oluşturuldu
int okunduMu;     //okunduMu isimli değişken oluşturuldu

void setup() {
Serial.begin(9600);   //seri monitör için başlangıç ayarları yapıldı
pinMode(S0,OUTPUT);   //Sensör pinleri çıkış olarak tanımlandı  
pinMode(S1,OUTPUT);
pinMode(S2,OUTPUT);
pinMode(S3,OUTPUT); 
pinMode(LED,OUTPUT);
pinMode(OUT,INPUT);   //OUT pini giriş olarak tanımlandı

servo_ust.attach(9);   //üstteki servo motorun sinyal pini 9 numralı arduino pinine bağlandı
servo_alt.attach(10);  //alttaki servo motorun sinyal pini 10 numralı arduino pinine bağlandı

servo_ust.write(95);   //başlangıçta üst servo motor açısı 95 derecede konumlansın
servo_alt.write(80);   //başlangıçta alt servo motor açısı 80 derecede konumlansın
}

void loop() {
//üstteki servo motor 95 ile 50 derece arasında gidip gelsin ve borudan pulu alıp sensöre getirsin
servo_ust.write(95);  //Borunun konumu
delay(500);           //500 milisaniye bekleme
servo_ust.write(50);  //Renk sensörünün konumu
delay(500);

RengiTanimla();       //Pulun rengini tespit etmek için RengiTanimla fonksiyonu çağırıldı ve çalıştırıldı
delay(1000);          //1 saniye bekleme

/* pulların renklerine göre alttaki servo motor doğru bardağa konumlanıyor*/
if(renk==0)servo_alt.write(35);     //Pulun rengi sarı ise alttaki servo motor 35 dereceye konumlansın
if(renk==1)servo_alt.write(60);     //Pulun rengi mavi ise alttaki servo motor 60 dereceye konumlansın
if(renk==3)servo_alt.write(85);     //Pulun rengi kırmızı ise alttaki servo motor 85 dereceye konumlansın
if(renk==4)servo_alt.write(110);    //Pulun rengi yeşil ise alttaki servo motor 110 dereceye konumlansın
delay(500);    //500 milisaniye bekleme

if(okunduMu==1){        //pulun rengi tespit edildi ise üstteki servo motor 5 dereceye gelsin ve holuğa pulu bıraksın
  servo_ust.write(5);
  delay(500);
}

if(okunduMu==0){        //pulun rengi tespit edilemediyse üstteki servo motor 95 dereceye yani boru hizasına gidip pul alsın
  servo_ust.write(95);
  delay(500);
}
}

/*TCS3200 renk sensörü üzerindeki pinler aktif edilerek renkler tanımlanıyor. Renk tanımlanması için gerekli fonksiyonlar*/
void TCS3200_Ac() {       //sensörü açmak için gerekli fonksiyon
digitalWrite(LED,HIGH);   //LED pini aktif yani 5V gönderildi
digitalWrite(S0,HIGH);    //S0 pini aktif
digitalWrite(S1,HIGH);    //S1 pini aktif
delay(10);
}

void TCS3200_Kapat() {    //sensörü kapatmak için gerekli fonksiyon
digitalWrite(LED,LOW);    //LED pini pasif yani kapalı
digitalWrite(S0,LOW);     //S0 pini pasif
digitalWrite(S1,LOW);     //S1 pini pasif
delay(5);
}

void Filtresiz() {        //Filtresiz fonksiyonu 
digitalWrite(S2,HIGH);
digitalWrite(S3,LOW);
delay(10);
}

void KirmiziFiltre() {    //KirmiziFiltre fonksiyonu
digitalWrite(S2,LOW);
digitalWrite(S3,LOW);
delay(10);
}

void YesilFiltre() {      //YesilFiltre fonksiyonu
digitalWrite(S2,HIGH);
digitalWrite(S3,HIGH);
delay(10);
}

void MaviFiltre() {       //MaviFiltre fonksiyonu
digitalWrite(S2,LOW);
digitalWrite(S3,HIGH);
delay(10);
}

void RengiTanimla() {     //Renk tanımlamasının yapıldığı foksiyon. Renk yüzdeleri tespit ediliyor
float BeyazFrekansi,KirmiziFrekansi,YesilFrekansi,MaviFrekansi;
TCS3200_Ac();
Filtresiz();
BeyazFrekansi = float(pulseIn(OUT,LOW,40000));
KirmiziFiltre();
KirmiziFrekansi = float(pulseIn(OUT,LOW,40000));
YesilFiltre();
YesilFrekansi = float(pulseIn(OUT,LOW,40000));
MaviFiltre();
MaviFrekansi = float(pulseIn(OUT,LOW,40000));

TCS3200_Kapat();
KirmiziYuzdesi = int((BeyazFrekansi / KirmiziFrekansi) * 100.0);
YesilYuzdesi = int((BeyazFrekansi / YesilFrekansi) * 100.0);
MaviYuzdesi = int((BeyazFrekansi / MaviFrekansi) * 100.0);
RengiBul();
}

void RengiBul() {     //Renk yüzdelerine göre seri ekrana rengin yazdırılması
Serial.println(" Renk Yuzdeleri");
Serial.print(" Kirmizi= ");
Serial.print(KirmiziYuzdesi);
Serial.print(" Mavi= ");
Serial.print(MaviYuzdesi);
Serial.print(" Yesil= ");
Serial.println(YesilYuzdesi);
Serial.println();
Serial.print(" Okunan Renk= ");

okunduMu=0;   //okunduMu değişkeni 0 yapılıyor

//for döngüsü ile tespit edilen renk yüzdeleri ve rengin ismi yazdırılıyor
for(renk =0; renk < 5; renk ++) {     
if(KirmiziYuzdesi > RenkYuzdesi[renk][0] - aralik && KirmiziYuzdesi < RenkYuzdesi[renk][0] + aralik &&
MaviYuzdesi > RenkYuzdesi[renk][1] - aralik && MaviYuzdesi < RenkYuzdesi[renk][1] + aralik &&
YesilYuzdesi > RenkYuzdesi[renk][2] - aralik && YesilYuzdesi < RenkYuzdesi[renk][2] + aralik ){
Serial.println(Renkler[renk]);
okunduMu=1;   //okunduMu değişkeni 1 yapılıyor
break;        //döngüden çıkılıyor
}
}

if(okunduMu == 0)     //okunduMu değişkeni 0 ise ekrana Renk Algılanamadı yazılıyor
Serial.println("Renk Algilanamadi");
Serial.println();
}
