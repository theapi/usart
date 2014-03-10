//#define F_CPU 16000000L

#include <avr/io.h>
#include <avr/interrupt.h>

#define FOSC 16000000 // Clock Speed
//#define BAUD 19200     // datasheet p203 shows error rate of 0.2%
//#define BAUD 38400
#define BAUD 57600 // datasheet p203 shows error rate of 2.1% (cannot currently get higher rates to work)


#define UBRR (FOSC/16/BAUD-1)

#define PIN_LED    PB0 // The led to blink
#define COMPARE_REG 249 // OCR0A when to interupt (datasheet: 14.9.4)
#define T1 1000 // timeout value for the blink (mSec)

/********************************************************************************
Function Prototypes
********************************************************************************/

void initTimer(void);
void USART_Init();
void USART_Transmit( unsigned char data );

/********************************************************************************
Global Variables
********************************************************************************/

volatile unsigned char flag_usart_rx;
volatile unsigned char received_byte;
volatile unsigned int time1;


/********************************************************************************
Interupt Routines
********************************************************************************/

//timer 0 compare ISR
ISR(TIMER0_COMPA_vect)
{
    if (time1 > 0)  --time1;
}

/**
 * Interrupt when the USART receives a byte.
 */
ISR(USART_RX_vect)
{
   // Fetch the received byte value, which also clears the interrupt flag.
   received_byte = UDR0;

   // @todo buffer bytes to be processed later

   // Flag that a byte has been received.
   //flag_usart_rx = 1;

   UDR0 = received_byte;
}

/********************************************************************************
Main
********************************************************************************/
int main (void)
{
	DDRB = 0xFF; // set all to output
	PORTB = 0; // all off

	USART_Init();
	initTimer();

	// crank up the ISRs
	sei();

	// main loop
    while(1) {

        // Check if there is any serial data to process.
        if (flag_usart_rx) {
            flag_usart_rx = 0;
            // Send the usart received byte back where it came from.
            //USART_Transmit('-');
            //USART_Transmit(received_byte);

            //@todo: send buffered bytes
        }

        if (time1 == 0) {
            // reset the timer
            time1 = T1;
            // toggle the led
            PORTB ^= (1 << PB0);
            USART_Transmit('-');
            USART_Transmit(0x0a); // new line (to keep my python script happy)
        }

    }
}


/********************************************************************************
Functions
********************************************************************************/
void USART_Init(void)
{
    UBRR0H = (UBRR >> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register
    UBRR0L = UBRR;        // Load lower 8-bits of the baud rate value into the low byte of the UBRR register

    // Use 8-bit character sizes & 1 stop bit
    UCSR0C = (0 << USBS0) | (1 << UCSZ00) | (1 << UCSZ01);

    // Enable receiver, transmitter, interrupt on receive.
    UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
}

void USART_Transmit( unsigned char data )
{
    // Wait for empty transmit buffer
    while ( !( UCSR0A & (1 << UDRE0)) )
        ;
    // Put data into buffer, sends the data
    UDR0 = data;
}

void initTimer(void)
{
	// set up timer 0 for 1 mSec ticks (timer 0 is an 8 bit timer)

	// Interupt mask register - to enable the interupt (datasheet: 14.9.6)
	// (Bit 1 – OCIE0A: Timer/Counter0 Output Compare Match A Interrupt Enable)
	TIMSK0 = (1 << OCIE0A); // (2) turn on timer 0 cmp match ISR

	// Compare register - when to interupt (datasheet: 14.9.4)
	// OCR0A = 249; // set the compare reg to 250 time ticks = 1ms
	//OCR0A = 124; // 0.5 ms
	//OCR0A = 50; // 1/5 of 1ms = 0.0002s
	//OCR0A = 30; // 0.00012s = 0.12ms
	//OCR0A = 25; // 0.0001s = 0.1ms
	OCR0A = COMPARE_REG;

    // Timer mode (datasheet: 14.9.1)
	TCCR0A = (1 << WGM01); // (0b00000010) turn on clear-on-match

	// Prescaler (datasheet: 14.9.2)
	// 16MHz/64=250kHz so precision of 0.000004 = 4us
	// calculation to show why 64 is required the prescaler:
	// 1 / (16000000 / 64 / 250) = 0.001 = 1ms
	TCCR0B = ((1 << CS10) | (1 << CS11)); // (0b00000011)(3) clock prescalar to 64

	// Timer initialization
    time1 = T1;
}
