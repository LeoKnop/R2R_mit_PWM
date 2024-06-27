#include "msp430F5529.h"
#include "lib/init.h"

#define pwm_aufloesung 8
#define filter_aufloesung 8
#define r2r_aufloesung 8

// Globale Variablen Deklaration
unsigned int conversioncounter = 0;     // Zeigt an wie viele Werte der ADC schon übertragen hat
unsigned int pwm_on = 0;                // Pulsbreitenverhältniss: pwm_on/2^pwm_aufloesung
unsigned int pwm_anzahl = (1 << pwm_aufloesung); // Auflösung PWM (einstellbar durch k)

unsigned int r2r_ansteuerung = 0;       // legt fest welche Bits des R2R Netzwerkes gesetzt werden
unsigned int r2r_anzahl = (1 << r2r_aufloesung);

unsigned int mittelwert = 0;
unsigned int mittelwert_anzahl = (1 << filter_aufloesung);
long sum;                               // Summe zum berechnen des Mittelwerts ( muss double sein, da 8Bit * 16 Bit = 28 BIT Zahlen Adiert werden)

int SPI_recieve(void);
void UART_send(int two_bytes);

void main(void)
{
  // Initialisierungen
  
  
  while(1)
  {
    P4OUT |= BIT7;                      // Grüne LED leuchtet wenn das Programm bereit zum Starten ist
    P1OUT &= ~BIT0;                     // RoteLED aus
    while(0 == P1IFG);                  // Wenn S2 gedrückt wird, startet das Prtogramm
    P1OUT |= BIT0;                      // Rote LED leuchtet wenn das Programm läuft
    P4OUT &= ~BIT7;                     // Grüne LED aus
    
    r2r_ansteuerung = 0;                // Zum Start alle "Bits" des R2R auf 0
    while(r2r_ansteuerung < r2r_aufloesung)                                      // Schleife steuert jede Biteinstellung des R2R-Netzwerkes an
    {
      P6OUT = r2r_ansteuerung;
      pwm_on = 0;
      while(pwm_on < pwm_anzahl)                                                // Schleife steuert jedes Pulsbreitenverhältniss an
      {
        sum = 0;
        conversioncounter = 0;
        while(conversioncounter < mittelwert_anzahl)                            // Schleife bildet Mittelwert
        {
          sum = sum + SPI_recieve();    // Messung durch ADC
          ++conversioncounter;
        }
        mittelwert = sum >> 8;          // Teilen der Summe durch 256
        UART_send(mittelwert);           // Über UART in csv
        ++pwm_on;                       // erhöht Pulsbreite und fungiert als counter
      }
      ++r2r_ansteuerung;
    } 
  }
}


int SPI_recieve(void)
{
  return 0;
}

void UART_send(int two_bytes)
{
  char lsb_byte = two_bytes & 0x0F;
  char msb_byte = two_bytes >> 8;
    
  UCA1TXBUF = lsb_byte;
  while((UCTXIFG & UCA1IFG) == 0);
  UCA1TXBUF = msb_byte;
  while((UCTXIFG & UCA1IFG) == 0);
}