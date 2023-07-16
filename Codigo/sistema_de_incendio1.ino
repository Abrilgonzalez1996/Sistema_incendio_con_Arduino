#include <Servo.h> //Libreria del motor
#include <LiquidCrystal.h> //Libreria del lcd
#include <IRremote.h> //Libreria sensor IR
#define led_red 6
#define led_green 5


//Crear el objeto LCD
LiquidCrystal lcd(7, 8, 9, 10, 12, 13);

//Variables
int lectura = 0;
float temperature = 0;
Servo servo;
int pin_sensor = 11;
IRrecv irrecv(pin_sensor);
int cont = 0;
String season;
bool flag_display = true;
String message1;
String message2;
long startTime = 0; // Tiempo de inicio
long currentTime; // Tiempo actual

//Funciones
String detect_seasons_of_the_year(float temperature);
void write_on_display(String message1, String message2);
void servo_motor(long startTime, long currentTime);
void power_led(int led, int estado);
void turn_on();

void setup(){
  Serial.begin(9600);//Iniciamos la comunicación serial
  pinMode(led_red, OUTPUT);
  pinMode(led_green, OUTPUT);
  IrReceiver.begin(pin_sensor); 
  irrecv.enableIRIn(); // Empezamos la recepción  por IR
  servo.attach(2);
  // Inicializar el LCD con el número de  columnas y filas del LCD
  lcd.begin(16, 2);
}

void loop(){
  currentTime = millis();

  //Control
  if(IrReceiver.decode()){    
    if(IrReceiver.decodedIRData.command == 0 and cont == 0){
      write_on_display("Turned on", "");//Imprimo mensaje de lcd ecendido
      delay(750);
      cont++;    
    } else if(IrReceiver.decodedIRData.command == 0){
      write_on_display("Turned off", "");//Imprimo mensaje de lcd apagado
      delay(750);
      cont ++;
    }
    IrReceiver.resume();//Recive el siguiente dato del control remoto
  }
   if(cont == 1){//Prendo la alarma     
     turn_on();  
   }else if(cont > 1){//Apago la alarma
     lcd.clear();    
     cont = 0;
     power_led(led_green, LOW);
     power_led(led_red, LOW);
   }
}

//Funciones

void turn_on(){
  lectura = analogRead(A0);//Tomamos la lectura del pin al que conectamos el sensor de temperatura
  temperature = map(lectura, 20, 350, -40, 125);//Obtengo la temperatura

  if(temperature > 60){
    power_led(led_green, LOW);//Apago el led verde
    power_led(led_red, HIGH);//Enciendo el led rojo
    message1 = "EMERGENCY!!";
    message2 = "There is a fire";
  } else{
    power_led(led_red, LOW);//Apago el led rojo
    power_led(led_green, HIGH);//Enciendo el led verde
    season = detect_seasons_of_the_year(temperature);//Busco la estacion del año segun la temperatura
    message1 = season + ":";
    message2 = String(temperature) + " C";
  }
  write_on_display(message1, message2);//Imprimo los mensajes en el lcd
  if(temperature > 60){ //Prendo el motor si la temperatura supera los 60 grados
    servo_motor(startTime, currentTime);
  }
}

void power_led(int led, int estado){
  digitalWrite(led, estado);//Encender o apagar el LED
}

void servo_motor(long startTime, long currentTime){
  int grados;
  //Motor
  if (currentTime - startTime < 500) {
    grados = 90;
    startTime = currentTime;
  } else if(grados == 180){
    grados = 0;
  }
  servo.write(grados); // Mover el servo a la posición 180
}

String detect_seasons_of_the_year(float temperature){
  String season;
  if(temperature < 9){ //Segun la temperatura es la estacion del año
    season = "Winter";
  }else if (temperature < 17){
   	season = "Autumn"; 
  } else if(temperature < 27){
    season = "Spring";
  } else{
   	season = "Summer"; 
  }
    
  return season; 
}

void write_on_display(String menssage1, String menssage2){
  	delay(500);
  	lcd.clear(); //Borra el mensaje del lcd
  	lcd.setCursor(0, 0);//Imprime en la primer fila
  	lcd.print(menssage1);//Imprime el primer mensaje
    lcd.setCursor(0, 1);//Imprime en la segunda fila 
  	lcd.print(menssage2);//Imprime el segundo mensaje
}
