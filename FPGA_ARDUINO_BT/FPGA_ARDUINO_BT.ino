
//--------------cámara------------
#include <Servo.h>
#define SIO_C 12
#define SIO_D 13
#define SIO_CLOCK_DELAY 100

bool i=false;
bool bad_write =false; 

//----------fin camara---------------

//--------------Bt--------------------

#include <SoftwareSerial.h>
SoftwareSerial EEBlue(7, 10); // RX | TX
int comandos;
int grados;

//------------fin Bt-----------------

//--------------ultrasonido 2

#define echoPin 2 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 8 // pwm

// defines variables


//---------------- fin ultrasonido 2

const int Trigger = 4;   //Pin digital 2 para el Trigger del sensor
const int Echo = 11;   //Pin digital 3 para el Echo del sensor
const int servo=10; //pin del servo
int distancia;
int distancia_detener=9;
int distancia_libre=8;

int carstop= A0;

int red;
int green;
int blue;

//variables para controlar

int derecha_adelante = 3;
int derecha_atras = 5;
int izquierda_adelante = 9;
int izquierda_atras = 6;
//variables para detectar

int amarillo_input = 2;
const int sensorPin_laser = 8;

//banderas de libertad
int libre_izquierda=0;
int libre_derecha=0;
int aux1=0;
int randomNumber;


void setup(){
  Serial.begin(9600);

  //-----------Bt------------------
  EEBlue.begin(38400);  //Default Baud for comm, it may be different for your Module. 
  Serial.println("The bluetooth gates are open");
  int comandos=0; //carro no arranca aún
  //--------------fin Bt----------

//ultrasonido
   pinMode(Trigger, OUTPUT); //pin como salida
   pinMode(Echo, INPUT);  //pin como entrada
  digitalWrite(Trigger, LOW);//Inicializamos el pin con 0

  //----------- ultrasonido 2
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
    pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  //-------------ultrasonido 2

// car stop;  
  analogWrite(carstop,0);

// varibles de colores

 red=0;
 green=0;
 blue=0;
 grados=180;
  
//sensores de proximidad
   pinMode(sensorPin_laser , INPUT);  //definir pin como entrada
   pinMode(amarillo_input , INPUT);

   boolean lbandera=0;
   i=0;
   // ------------------cámara------------------
        
  Serial.begin(9600);
  Serial.println("Start InitOV7670 test program");
 // digitalWrite(8,HIGH);delayMicroseconds(SIO_CLOCK_DELAY);   
 // digitalWrite(8,LOW);delayMicroseconds(SIO_CLOCK_DELAY);   
 // digitalWrite(8,HIGH);delayMicroseconds(SIO_CLOCK_DELAY);   
 
  if(InitOV7670())    
    Serial.println("InitOV7670 OK");
  else
    Serial.println("InitOV7670 NG");
   // -----------------fin cámara-------------

}

void loop(){
                //---------Bt-----------------
                  if (EEBlue.available()){
                    Serial.write(comandos);  }
                  
                // -----------fin Bt-------------

                if(lectura()>5 && lectura_2()<5){
                  corregir_derecha();
                  delay(700);
                  }
                if(lectura()>5 && lectura_2()>7){
                  corregir_izquierda();
                  delay(700);
                  }
                  
                if(lectura()<=5 && lectura_2()<=5){
                    analogWrite(carstop,675);
                    Serial.print("distancia: ");
                    parar();
                    colores();
                    delay(800);                 
                    delay(800);    
                    randomNumber=0;    
                    Serial.print(i);
                    delay(3400);
                    analogWrite(carstop,0);
                  girar_derecha();//
                  }
                  
                if(lectura()<=5 && lectura_2()>5){
                    analogWrite(carstop,675);
                    Serial.print("distancia: ");
                    parar();
                    colores();
                    delay(800);                 
                    delay(800);    
                    randomNumber=0;    
                    Serial.print(i);
                    delay(3400);
                    analogWrite(carstop,0);                
                  girar_izquierda();
                }
                    
                aux1=lectura();
                
                //------- inicia detención y toma de datos
                 if(aux1<=distancia_detener ){

                    }
                // ------termina función ------
}


//-------------------------------métodos para la navegación y para el envio de comandos------------------


//métodos cámara
void InitSCCB(void) //SCCB Initialization (Pines)
{
  pinMode(SIO_C,OUTPUT); //Configura pines de arduino (Para caso particular ) 20 SCA y 21 SCL
  pinMode(SIO_D,OUTPUT);
  
  digitalWrite(SIO_C,HIGH);
  digitalWrite(SIO_D,HIGH);
  
  Serial.println("InitSCCB - Port Direction Set & Set High OK");
}
void StartSCCB(void) //SCCB Start (Inicia la comuncacion)
{
  Serial.println("StartSCCB");
 
  digitalWrite(SIO_D,HIGH);
  delayMicroseconds(SIO_CLOCK_DELAY);
  
  digitalWrite(SIO_C,HIGH);
  delayMicroseconds(SIO_CLOCK_DELAY);
  
  digitalWrite(SIO_D,LOW);
  delayMicroseconds(SIO_CLOCK_DELAY);
  
  digitalWrite(SIO_C,LOW);
  delayMicroseconds(SIO_CLOCK_DELAY);
}
 
void StopSCCB(void) //SCCB Stop (Termina la comunicacion)
{
  //Serial.println("StopSCCB");
 
  digitalWrite(SIO_D,LOW);
  delayMicroseconds(SIO_CLOCK_DELAY);
  
  digitalWrite(SIO_C,HIGH);
  delayMicroseconds(SIO_CLOCK_DELAY);
  
  digitalWrite(SIO_D,HIGH);
  delayMicroseconds(SIO_CLOCK_DELAY);
}
 
bool SCCBWrite(byte m_data)
{
  unsigned char j;
  bool success;

  for ( j = 0; j < 8; j++ ) //Loop transmit data 8 times
  {
    if( (m_data<<j) & 0x80 )
      digitalWrite(SIO_D,HIGH);
    else
      digitalWrite(SIO_D,LOW);
  
    delayMicroseconds(SIO_CLOCK_DELAY);
    
  
  digitalWrite(SIO_C,HIGH);
  delayMicroseconds(SIO_CLOCK_DELAY);
    
  digitalWrite(SIO_C,LOW);
    delayMicroseconds(SIO_CLOCK_DELAY);
  }
 

 // digitalWrite(8,LOW); //debug
  
  pinMode(SIO_D,INPUT); // I pass a bus of SIO_D to slave (OV7670)
  digitalWrite(SIO_D,LOW); // Pull-up prevention  --this line is not present in embedded programmer lib
  delayMicroseconds(SIO_CLOCK_DELAY);
 
  // digitalWrite(8,HIGH); //debug
  
  digitalWrite(SIO_C,HIGH);
  delayMicroseconds(SIO_CLOCK_DELAY);
 
 // digitalWrite(8,LOW); //debug
  
  if(digitalRead(SIO_D)==HIGH)
    success= false;
  else
    success= true; 

  digitalWrite(SIO_C,LOW);
  delayMicroseconds(SIO_CLOCK_DELAY); 
  
  pinMode(SIO_D,OUTPUT); //Return the bus of SIO_D to master (Arduino)
  
  //delayMicroseconds(SIO_CLOCK_DELAY); 
  //digitalWrite(SIO_D,LOW);
  //delayMicroseconds(SIO_CLOCK_DELAY); 
 
  //pinMode(SIO_C,OUTPUT); //Return the bus of SIO_C to master (Arduino)
 
  return success;  
}
 
bool InitOV7670(void) //Reseteo básico del com14
{
  char temp = 0x80;

  InitSCCB();
  
  if( ! WriteOV7670(0x12, temp) ) //Reset SCCB
  {
    Serial.println("Resetting SCCB Failed");
    return false;
  }

  return true; 
}  
 
////////////////////////////
//To write to the OV7660 register: 
// function Return value: Success = 1 failure = 0
bool WriteOV7670(char regID, char regDat)
{
  StartSCCB();
  if( ! SCCBWrite(0x42) )
  {
        Serial.println(" Write Error 0x42");
      StopSCCB();
    return false;
  }
  
  delayMicroseconds(SIO_CLOCK_DELAY);

    if( ! SCCBWrite(regID) )
  {
    StopSCCB();
    return false;
  }
  delayMicroseconds(SIO_CLOCK_DELAY);
    if( ! SCCBWrite(regDat) )
  {
    StopSCCB();
    return false;
  }
  
    StopSCCB();
    Serial.println("Send");
    return true;
}

//métodos de control de llantas
void andar(){
  analogWrite(derecha_adelante,125);
  analogWrite(izquierda_adelante,125);
  comandos = orientacion(grados);
  }

void corregir_derecha(){
  analogWrite(derecha_adelante,60);
  analogWrite(izquierda_adelante,125);
  Serial.println("corregir_derecha ");
  delay(100);  
  }

  void corregir_izquierda(){
  analogWrite(derecha_adelante,125);
  analogWrite(izquierda_adelante,60);
  Serial.println("corregir_izquierda ");
  delay(100); 
  }

  void parar(){
  analogWrite(derecha_adelante,0);
  analogWrite(izquierda_adelante,0);
  analogWrite(derecha_atras,0);
  analogWrite(izquierda_atras,0); 
  comandos=0; 
  Serial.println("parar");
  }

    void girar_derecha(){
  analogWrite(derecha_adelante,0);
  analogWrite(izquierda_adelante,125);
  analogWrite(derecha_atras,125);
  analogWrite(izquierda_atras,0); 
  delay(800);  
  analogWrite(derecha_adelante,0);
  analogWrite(izquierda_adelante,0);
  analogWrite(derecha_atras,0);
  analogWrite(izquierda_atras,0);
  delay(100);
  Serial.println("giro derecha");
  grados=navegacion(90);

  }

  void girar_izquierda(){
  analogWrite(derecha_adelante,125);
  analogWrite(izquierda_adelante,0);
  analogWrite(derecha_atras,0);
  analogWrite(izquierda_atras,125); 
  delay(800);
  analogWrite(derecha_adelante,0);
  analogWrite(izquierda_adelante,0);
  analogWrite(derecha_atras,0);
  analogWrite(izquierda_atras,0);
  delay(700);  
  Serial.println("giro izquierda");
  grados=navegacion(-90);
  }

  


//funcion de ultrasonido

int lectura(){
  long t; //timepo que demora en llegar el eco
  long d; //distancia en centimetros
  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);          //Enviamos un pulso de 10us
  digitalWrite(Trigger, LOW);  
  t = pulseIn(Echo, HIGH); //obtenemos el ancho del pulso
  d = t/59;             //escalamos el tiempo a una distancia en cm  
  //Serial.print(d);      //Enviamos serialmente el valor de la distancia  
  
  return d;
  }

//--------------ultrasonido 2
int lectura_2(){
  
      long duration; // variable for the duration of sound wave travel
      int distance; // variable for the distance measurement
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      duration = pulseIn(echoPin, HIGH);
      distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
      return distance;
      
   }

void colores(){
  if(analogRead(A1)>=500){
    red=red+1;
    comandos=5;
    }
  if(analogRead(A2)>=500){
    green=green+1;
    comandos=6;
    }
  if(analogRead(A3)>=500){
    blue=blue+1;
    comandos=7;
    }  
  }

void camara_loop(){
  
  if(!bad_write){
  bad_write=true;
  InitOV7670(); //Reseteo básico del com
  //Configuracion de imagenes
   
  i=WriteOV7670(0x0C, 0x08); //Com3
  if(!i){bad_write=false;}

  i=WriteOV7670(0x12, 0x0C); //Com7
  if(!i){bad_write=false;}

  i=WriteOV7670(0x40, 0xD0); //Com15
  if(!i){bad_write=false;}

  i=WriteOV7670(0x3E, 0x00); //Com14
  if(!i){bad_write=false;}

  i=WriteOV7670(0x11, 0xC0); //Clkrc
  if(!i){bad_write=false;}

    //Registros Magicos
  i=WriteOV7670(0x3A, 0x04); // TSLB: habilita una secuencia de salida que se usa en el COM13
  if(!i){bad_write=false;}
  i=WriteOV7670(0x14, 0x18); // COM09: control de ganancia
  if(!i){bad_write=false;}
  i=WriteOV7670(0x3D, 0xC0); // C0M13:activa la correccion gamma y el ajuste automatico del nivel de saturacion UV
  if(!i){bad_write=false;}
  i=WriteOV7670(0x17, 0x14); // HSTART:cambia hstart 
  if(!i){bad_write=false;}
  i=WriteOV7670(0x18, 0x02); // HSTOP:cambia hstop
  if(!i){bad_write=false;}
  i=WriteOV7670(0x32, 0x80); // HREF:href deja el valor qye viene por default 
  if(!i){bad_write=false;}
  i=WriteOV7670(0x19, 0x03); // VSTRT :vref start
  if(!i){bad_write=false;}
  i=WriteOV7670(0x1A, 0x7B); // VSTOP:vref stop
  if(!i){bad_write=false;}
  i=WriteOV7670(0x03, 0x0A); // VREF: cambia el control vertical de referencia(vref)
  if(!i){bad_write=false;}
  i=WriteOV7670(0x0F, 0x41); // COM06:no reinicia todos los tiempos cuando el formato cambia 
  if(!i){bad_write=false;}
  i=WriteOV7670(0x1E, 0x00); // MVFP: no hace nada se cambian valores reservados 
  if(!i){bad_write=false;}
  i=WriteOV7670(0x33, 0x0B); // CHLF: no hace nada se cambian valores reservados 
  if(!i){bad_write=false;}
  i=WriteOV7670(0x74, 0x00); // REG74:esta por defecto el valor de ganancia es digital es decir esta inhabilitada 
  if(!i){bad_write=false;}
  i=WriteOV7670(0xB1, 0x0C); // ABLCl:Habilita el ABLC, es decir la calibracion automatica del nivel de negro
  if(!i){bad_write=false;}
  i=WriteOV7670(0xB2, 0x0E); // RSVD :se cambian valores reservados
  if(!i){bad_write=false;}
  i=WriteOV7670(0xB3, 0x80); // THL_ST:Valor par defecto que configura el target del ABLC
  if(!i){bad_write=false;}
  Serial.print("Bad_write ");
  Serial.println(!bad_write);
  }
  }

  int navegacion(int rumbo){
     grados=grados+rumbo;
     if(grados==360){
      grados==0;
     }
     if(grados==-90){
      grados==270;
     } 
     return grados;
  }

  
  int orientacion(int grados){
      int respuesta;
     if(grados==0){
      respuesta=2;
      }
     if(grados==90){
      respuesta=3;
      }
     if(grados==180){
      respuesta=1;
      }
     if(grados==270){
      respuesta=4;
      }
     return respuesta;    
 }

    
  

  
