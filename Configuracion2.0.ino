#define SIO_C 21
#define SIO_D 20
#define SIO_CLOCK_DELAY 100

bool i=false;
bool bad_write =false; 
 
void setup()
{
  //pinMode(8,OUTPUT);
  
//  while(1)
//  {
//    digitalWrite(8,HIGH);
//    delayMicroseconds(SIO_CLOCK_DELAY);    
//    digitalWrite(8,LOW);
//    delayMicroseconds(SIO_CLOCK_DELAY);    
//  }
    
  Serial.begin(9600);
  Serial.println("Start InitOV7670 test program");
 // digitalWrite(8,HIGH);delayMicroseconds(SIO_CLOCK_DELAY);   
 // digitalWrite(8,LOW);delayMicroseconds(SIO_CLOCK_DELAY);   
 // digitalWrite(8,HIGH);delayMicroseconds(SIO_CLOCK_DELAY);   
 
  if(InitOV7670())    
    Serial.println("InitOV7670 OK");
  else
    Serial.println("InitOV7670 NG");
  
  
}
 
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
void loop()
{
  if(!bad_write){
  bad_write=true;
  InitOV7670(); //Reseteo básico del com14
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
