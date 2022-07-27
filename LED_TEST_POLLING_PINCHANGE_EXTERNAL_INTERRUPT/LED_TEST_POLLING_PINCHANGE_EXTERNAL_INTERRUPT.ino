/* A test program using the ATMEGA328p which tests 3 LEDs connected to the 
 * ATMEGA328p via 3 separate push-buttons. Each color button lights up it's 
 * corresponding LED when pressed. This program demonstrates use of External 
 * Interrupts, Pin Change Interrupts and Polling methods. 
*/

/**************************** VARIABLE DECLARATIONS *************************/

const int PORTB_OFFSET = 8;                   //Port B Pin 0 is located on 
                                              //Digital 8 on the Arduino.

const int blue_btn_pin = 3;                   //DDD5 - Port D, Pin 3 (Digital 3)
const int green_btn_pin = (10 - PORTB_OFFSET);//DDB2 - Port B, Pin 2 (Digital 10)
const int red_btn_pin = 6;                    //DDD6 - Port D, Pin 6 (Digital 6)

const int blue_led_pin = (13 - PORTB_OFFSET); //DDB5 - Port B, Pin 5 (Digital 13)
const int green_led_pin = 4;                  //DDD4 - Port D, Pin 4 (Digital 4)
const int red_led_pin = (12 - PORTB_OFFSET);  //DDB4 - Port B, Pin 4 (Digital 12)

//read the current state of bit 6 from the PIND register and store this in 
//the variable red_btn.   
volatile bool red_btn = (PIND & (1 << red_btn_pin)) >> red_btn_pin;
//read the current state of bit 3 from the PIND register and store this in 
//the variable blue_btn.   
volatile bool blue_btn = (PIND & (1 << blue_btn_pin)) >> blue_btn_pin;

/**************************** FUNCTION PROTOTYPES ***************************/
void button_press_polling(int btn_pin, int led_pin);
void configure();

int main()
{
  configure();
  
  while(1)
  { 
  //read the current state of bit 2 from the PINB register and store this in 
  //the variable green_btn. 
    button_press_polling(green_btn_pin, green_led_pin);   
  }// end while
  
  return 0;
}// end main()

// Set up Pin Change Interrupt and External Interrupt Registers.
// Port configuration for LEDs and push-buttons.
void configure()
{
  /************************ Interrupt register setup *************************/  
  // Clear Interrupts during Interrupt register setup.
  cli();

  // Pin Change Interrupts - Enable PCIE2 Bit2 = 1 (Port D)
  PCICR |= B00000100;
  // Set PCINT22 - Port D Bit6 = 1 (Digital 6, where the red button is connected)
  // Note: Pin change interrupts will occur when both pressing AND releasing the
  // red button.
  PCMSK2 |= B01000000;  

  // External Interrupt Control Register A (EICRA) - Interrupt sense control.
  EICRA &= ~(1 << ISC11); // Set Bit 3 of EICRA register to logic level low
  EICRA |= (1<< ISC10);   // And Bit 2 of EICRA rgister to logic level high.
  // EICRA register set to [ - - - - 0 1 x x ] Both Rising and Falling edge 
  // logic signals trigeer an interrupt request on INT1 (Digital 3 Pin).
  EIMSK |= (1 << INT1); // Enable External Interuupt Pin for INT1 only.    

  // Enable Global Interrupts.
  sei();

  /*************************** push button I/O setup ************************/
  DDRD &= ~(1 << blue_btn_pin); //Set Pin 3 of Port D as an input. 
  PORTD |= (1 << blue_btn_pin); //Enable the internal pull-up resistor for Pin 5
                                //of Port D 
  DDRD &= ~(1 << red_btn_pin);  //Set Pin 6 of Port D as an input. 
  PORTD |= (1 << red_btn_pin);  //Enable the internal pull-up resistor for Pin 6
                                //of Port D                              
  DDRB &= ~(1 << green_btn_pin);//Set Pin 2 of Port B as an input.
  PORTB |= (1 << green_btn_pin);//Enable the internal pull-up resistor for Pin 2
                                //of Port B

  /******************************* LED I/O setup ****************************/
  DDRB |= (1 << blue_led_pin); //Set Pin 5 of Port B as an output.
  DDRB |= (1 << red_led_pin);  //Set Pin 4 of Port B as an output.
  DDRD |= (1 << green_led_pin);//Set Pin 4 of Port D as an output.  
}// end configure()

//Polling method - needs to be checked as often as possible! Least efficient
//method. Uses the green button and LED.
void button_press_polling(int btn_pin, int led_pin)
{
  //read the state of btn_pin (HIGH/LOW) and store in btn_state.
  bool btn_state = (PINB & (1 << btn_pin)) >> btn_pin;              

    if(btn_state == LOW) //Button is pressed
    {
      // Set output of the green LED to logic high - turn green LED on.
      PORTD |= (1 << led_pin);    
    }
      
    else //Button has been released / no longer depressed.
    {
      // Set output of the LED to logic low - turn green LED off.
      PORTD &= ~(1 << led_pin);
    }
}//end button_press_polling() function.

//Pin Change Interrupt routine for detecting presses and releases of the red
//button.
ISR(PCINT2_vect)
{
  // Interrupt for Port D - read the state of the red button
  red_btn = (PIND & (1 << red_btn_pin)) >> red_btn_pin;
  
  if(red_btn == LOW) //red button has been pressed.
  {   
    // Set output of the red led to logic high - turn it on.    
    PORTB |= (1 << red_led_pin);
  }
  else //red button has been released.
  {
    // Set output of the red led to logic low - turn it off.
    PORTB &= ~(1 << red_led_pin);
  }
}//end ISR - Pin Change Interrupt

//External Interrupt routine for detecting presses and releases of the blue
//button.
ISR(INT1_vect)
{
  // External Interrupt (INT1) for Port D - read the state of the blue button
  blue_btn = (PIND & (1 << blue_btn_pin)) >> blue_btn_pin;

  if(blue_btn == LOW) //blue button has been pressed.
  {   
    // Set output of the blue led to logic high - turn it on.    
    PORTB |= (1 << blue_led_pin);
  }
  else //blue button has been released.
  {
    // Set output of the red led to logic low - turn it off.
    PORTB &= ~(1 << blue_led_pin);
  }
}//end ISR - External Interrupt

