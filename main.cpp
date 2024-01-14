#include "Grove_LCD_RGB_Backlight.h"
#include "mbed.h"

#define WAIT_TIME_MS 5

//Definicion de los perifericos
DigitalOut led1(LED1); //variable led1 en pin de la placa
DigitalIn boton(D2);
DigitalOut rele(D3);
AnalogIn pot(A0);
Grove_LCD_RGB_Backlight rgbLCD(PB_9, PB_8);

Timer temporizador; //

float V0g = 1;
float V200g = 2;

enum estados {
  Reposo,
  Pulsado,
  Llenado,
  soltando,
  Calibracion0g,
  Calibracion0gPresionado,
  Calibracion200g,
  Calibracion200gPresionado
} estado;

float peso;
char m1[16];
char m2[16];

void CalcularPeso() { peso = 200 * ((pot * 3.33) - V0g) / (V200g - V0g); }

void estadoReposo() {
  if (boton == 1) {
    estado = Pulsado;
    temporizador.reset();
    temporizador.start();
    rgbLCD.locate(0, 0);
    rgbLCD.print("Esperando Soltar:");
  }
}
void estadoPulsado() {

  if (boton == 0) {
    if (temporizador.read() < 2) {
      estado = Llenado;
      led1 = 1;
    } else {
      estado = Calibracion0g;
      rgbLCD.clear();
      rgbLCD.locate(0, 0);
      rgbLCD.print("Quita Peso y");
      rgbLCD.locate(0, 1);
      rgbLCD.print("Pulsa boton");
    }
  }
}

void estadoLenando() {
  led1 = 1;
  rgbLCD.clear();
  rgbLCD.locate(0, 0);
  rgbLCD.print("Pesando:");
  rgbLCD.locate(0, 1);
  sprintf(m1, "%f", peso);
  rgbLCD.print(m1);
  if (peso >= 100) {
    estado = soltando;
    led1 = 0;
    rele = 1;
    temporizador.reset();
    temporizador.start();
    rgbLCD.clear();
    rgbLCD.locate(0, 0);
    rgbLCD.print("Esperando soltar");
  }
}

void estadoSoltando() {
  if ((peso <= 50) || (temporizador.read() >= 2)) {
    rele = 0;
    estado = Reposo;
    rgbLCD.clear();
    rgbLCD.locate(0, 0);
    rgbLCD.print("Reposo:");
  }
}

void estadoCalibracion0g() {
  if (boton == 1) {
    estado = Calibracion0gPresionado;
    rgbLCD.clear();
    rgbLCD.locate(0, 0);
    rgbLCD.print("Suelta Boton");
    V0g = pot * 3.3;
  }
}

void estadoCalibracion0gPresionado() {
  if (boton == 0) {
    estado = Calibracion200g;
    rgbLCD.clear();
    rgbLCD.locate(0, 0);
    rgbLCD.print("Pon 200g");
    rgbLCD.locate(0, 1);
    rgbLCD.print("Pulsa boton");
  }
}

void estadoCalibracion200g() {
  if (boton == 1) {
    estado = Calibracion200gPresionado;
    rgbLCD.clear();
    rgbLCD.locate(0, 0);
    rgbLCD.print("Suelta Boton");
    V200g = pot * 3.3;
  }
}

void estadoCalibracion200gPresionado() {
  if (boton == 0) {
    estado = Reposo;
    rgbLCD.clear();
    rgbLCD.locate(0, 0);
    rgbLCD.print("Reposo:");
  }
}

int main() {
  led1 = 0;
  rele = 0;

  temporizador.reset();
  rgbLCD.setRGB(0xff, 0xff, 0xff);
  estado = Reposo;
  rgbLCD.locate(0, 0);
  rgbLCD.print("Reposo:");

  while (true) {
    CalcularPeso();
    switch (estado) {
    case Reposo:
      estadoReposo();
      break;
    case Pulsado:
      estadoPulsado();
      break;
    case Llenado:
      estadoLenando();
      break;
    case soltando:
      estadoSoltando();
      break;
    case Calibracion0g:
      estadoCalibracion0g();
      break;
    case Calibracion0gPresionado:
      estadoCalibracion0gPresionado();
      break;
    case Calibracion200g:
      estadoCalibracion200g();
      break;
    case Calibracion200gPresionado:
      estadoCalibracion200gPresionado();
      break;
    }

    thread_sleep_for(WAIT_TIME_MS);
  }
}