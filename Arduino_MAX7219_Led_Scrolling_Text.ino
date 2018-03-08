/* Arduino Scrolling Text MAX7219 LED Dot Matrix Display */
/* Create for QSY.BY by EU4DGC / Pavel Hladkikh  */
/* http://qsy.by/begushhaya-stroka-arduino-max7219-i-para-provodov/ */

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>


int pinCS = 10; // Подключаем CS к 10-му пину, DIN к MOSI и CLK к SCK 
int numberOfHorizontalDisplays = 1; // Количество модулей по горизонтали
int numberOfVerticalDisplays = 8; // Количество модулей по вертикали
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

String tape = "";
int wait = 10; // Скорость прокрутки в миллисикундах
int spacer = 1; // Промежуток между символами (кол-во точек)
int width = 5 + spacer; // Ширина символа 

/* Перекодировка русского шрифта из UTF-8 в Windows-1251 */

String utf8rus(String source)
{
  int i,k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };

  k = source.length(); i = 0;
  while (i < k) {
    n = source[i]; i++;
    if (n >= 0xC0) {
      switch (n) {
        case 0xD0: {
          n = source[i]; i++;
          if (n == 0x81) { n = 0xA8; break; }
          if (n >= 0x90 && n <= 0xBF) n = n + 0x2F;
          break;
        }
        case 0xD1: {
          n = source[i]; i++;
          if (n == 0x91) { n = 0xB7; break; }
          if (n >= 0x80 && n <= 0x8F) n = n + 0x6F;
          break;
        }
      }
    }
    m[0] = n; target = target + String(m);
  }
return target;
}
/* Код для работы с com-портом */
String Serial_Read() {
  unsigned char c; // переменная для чтения сериал порта
  String Serial_string = ""; // Формируемая из символов строка
  while (Serial.available() > 0) { // Если в сериал порту есть символы
    c = Serial.read(); // Читаем символ
    //Serial.print(c,HEX); Serial.print(" "); Serial.print(c);
    if (c == '\n') {  // Если это конец строки
          return Serial_string; // Возвращаем строку 
    }
    if (c == 0xB8) c = c - 0x01;  // Коррекция кодов символа под таблицу ???? так как русские символы в таблице сдвинуты относительно стандартной кодировки utf на 1 символ
    if (c >= 0xBF && c <= 0xFF) c = c - 0x01;
    Serial_string = Serial_string + String(char(c)); //Добавить символ в строку
  }
  return Serial_string; 
}



void setup() {
  Serial.begin(9600);
  tape = utf8rus("QSY.BY Amateur Radio WorkShop"); // Этот текст выводиться при включении или если в com-порт не пришла информация 
  matrix.setIntensity(3); // Яркость от 0 до 15
  matrix.setRotation(matrix.getRotation()+3); //1 - 90  2 - 180   3 - 270

}
void loop() {

  if (Serial.available()){
    tape=Serial_Read();
  } 
  for ( int i = 0 ; i < width * tape.length() + matrix.width() - 1 - spacer; i++ ) 
       {
    matrix.fillScreen(LOW);

    int letter = i / width; // Номер символа выводимого на матрицу 
    
    int x = (matrix.width() - 1) - i % width;  
    int y = (matrix.height() - 8) / 2; // Центрируем текст по вертикали

    while ( x + width - spacer >= 0 && letter >= 0 ) {
      if ( letter < tape.length() ) {
        matrix.drawChar(x, y, tape[letter], HIGH, LOW,1);
      }
      letter--;
      x -= width;
    }
    matrix.write(); // Вывод сообщения на экран
    delay(wait);
  }
}

