import processing.serial.*;

String Listapuertos[];
int datos;
Serial myPort;    
String myString;

int segundos=0;
int minutos=0;
int horas=0;

int ancho=25;
int mina=30;

int x=1000;
int y=700;

int lado;
int alto;

int puntos_r;
int puntos_g;
int puntos_b;

color r = color(255, 0, 0);  
color v = color(0, 255, 0);  
color a = color(0, 0, 255); 
color b = color(255, 255, 255); 
color n = color(0, 0, 0);  

void setup() {  
  
  size(2000,1000);
  background(0);
  printArray(Serial.list());
  Listapuertos=Serial.list();
  println(Listapuertos[0]);
  myPort = new Serial(this, "COM4",38400);
  println("puerto");
  println(myPort);
  
 puntos_r=0;
 puntos_g=0;
 puntos_b=0;
}

void draw() {
  datos=myPort.read();
  println("datos: "+datos);
  texto();
  mapa();
  graficador(datos);  
}

void texto(){ 
  segundos=(segundos+1);
  if(segundos==60){
  minutos=minutos+1;
  segundos=0;
  }
  if(minutos==60){
  horas=horas+1;
  minutos=0;
  }
  
  fill(n);  // 
  noStroke();    
  textSize(32);
  rect(400, 50, 250, 100);
  rect(1500, 50, 450, 200);
  
  fill(b);  // 
  text("tiempo: "+horas+" : "+minutos+" : "+segundos,300,100);
  fill(r);
  text("Puntuación Roja: "+puntos_r,1500,100);
  fill(v);
  text("Puntuación Verde: "+puntos_g,1500,150);
  fill(a);
  text("Puntuación Azul: "+puntos_b,1500,200); 

}

void mapa(){
  x=x+lado;
  y=y+alto;
  fill(b);  // 
  noStroke();    
  textSize(32);
  rect(x, y, ancho, ancho); 

}

void graficador(int datos){
  if (datos == 1){
    lado=-1;
    alto=0;
  }  
  if (datos == 2){
    lado=1;
    alto=0;
  }
  if (datos == 3){
    lado=0;
    alto=-1;
  }
  if (datos == 4){
    lado=0;
    alto=1;
  }  
  if (datos == 0){
    lado=0;
    alto=0;
  }  
  if (datos == 5){
    puntos_r=puntos_r+1;    
    fill(r);
    ellipse(x, y, mina, mina);    
  }
    if (datos == 6){
    puntos_g=puntos_g+1;
    fill(v);
    ellipse(x, y, mina, mina);   
  }
    if (datos == 7){
    puntos_b=puntos_b+1;
    fill(a);
    ellipse(x, y, mina, mina);   
  }  
}


 
