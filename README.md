# driverbot-AbbMarPin
driverbot-AbbMarPin created by GitHub Classroom

## Vad är detta?

Detta är min bil. Den har en tt motor för drivning, en sg90 servo för styrning och 3D utskrivna delar för att koppla ihop lego:t med motorn och servot.

Bilen kan köras med hjälp av en hemsida och en gamepad kontroll om man vill. Den kan köra framåt, bakåt och svänga och den har också ett inbyggt batteri för att kunna köra i flera timmar.

I detta projekt finns det också en simulator som simulerar bilens rörelser och visar en pil där bilen skulle vara om den kördes i verkligheten. 


## Hur funkar den?

När en användare an startat hemsidan och kopplat upp sig till samma MQTT server som arduinon börjar hemisan skicka ett förflyttnings och styrpaket via MQTT till arduinon fem gånger per sekund.

I dessa förflyttningspaket finns tre värden:
* Av eller På (1 eller 0)
* Hastiget (0-100)
* Riktning (1 eller 0)

Tillsammans ser de ut såhär: (1, 50, 1); Bilen kör fram på halvfart

I ett styrpaket finns det endast ett värde:
* styrningsvinkel (180-0)

Ett stryrpaket kan se ut såhär: (90); bilen stryr rakt fram

### Arduino / simulatorn
Arduinos jobb är att ta emot förflyttnings och styrpaket från MQTT servern och kontrollera motorn och servot utefter det.
Om Arduinon inte får ett förflyttnings eller styrpaket på 500 millisekunder kan den anta att ingen kontrollerar den och stänger därför av sig
Arduinon använder WiFiManager för att lätt kunna koppla upp till nya WiFi nätverk om den inte hittar någon den känner igen med hjälp av en telefon.

Simulatorn är skrivet i python och använder turle bibloteket för att visa vart bilen är i verkligheten. Den gör sitt bästa för att simulera hur bilen skulle åka och har samma svängradie och hastighet.



# Loggbok

## 2020-03-23 
Idag har jag konverterat från min förra repo och försökt fixa med store

## 2020-03-24
Idag strukturerade och kommenterade jag kod  

## 2020-03-24
Idag lade jag till en av/på spak och en mqttkonfigpanel
