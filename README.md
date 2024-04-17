# esp32_thermostat_telegram
esp32 - Mijia (LYWSD03MMC) - telegram thermostat

Bu projeyi yaparken yararlandığım kaynaklar;
* [Mucit Pilot youtube kanalı - esp32 telegram bağlantısını gösteriyor.](youtube.com/watch?v=g6W4bdM9Vhg) 
* [Mi Termometre cihazımıza bu özelleştirilmiş yazılımı yüklememiz gerekiyor. ](github.com/atc1441/ATC_MiThermometer) 
* [Termometremizden veri çekmemizi sağlayan bu kütüphaneyi kullanıyoruz.](github.com/matthias-bs/ATC_MiThermometer)
  
## Kullanığım malzemeler:
* ESP32
* Mi Sıcaklık ve Nem Ölçer 2
* 5v Röle
  
## Proje Neyi Amaçlıyor?
Oda sıcaklığına bağlı olarak kazanı veya kombiyi açıp kapayan, yakıt tasarrufu sağlamayı amaçlayan bir cihaz. Standart termometrelerden farkı telegram üzerinden sistemi kontrol edebilmemiz (min-max dereceler ve sistemi komple açıp kapama gibi). Verileri bluetooth ile Mi termometre üzerinden alır ve esp32 kart üzerinde işler.
Ben bunu ECA Elanus pelet yakıtlı kazan için kullandım ancak standart kombiler içinde uygundur.

## Nerelerde Kullanılabilir?
* Kombi (kombiler için Min-Max derece ayarı arasındaki fark azaltılabilir.)
* Termostat çıkışı olan katı yakıt kazanları.(pelet sobasında denendi.)
* Kuluçka makinesi için kullanılabilir.

## Nasıl Çalışıyor?
* ESP32 ilk açıldığında sistem kapalı durumdadır, /aktif komutu ile aktif etmemiz gerekiyor. (Sistemi kapatmak için /kapali)
* Kodda Minimum ve Maksimum dereceler 21.5-22.5 olarak ayarlanmıştır isterseniz /min xx, /max xx şeklinde dereceler ayarlanabilir. (ESP32 yeniden başladığında koddaki derecelere geri döner, aktif kullanmak istenilen derece ayarı koddan değiştirilmeli.)
* /bilgi yazarak komutlar listesi ve sistem durumu görülebilir.
* /olc komutu ile anlık ölçümler ve termometremizin batarya durumunu görebiliriz.

![termostat](https://github.com/ehlullah/esp32_thermostat_telegram/assets/29586225/2e8f11ec-b5b4-45d3-91a8-648c70bee08a)

