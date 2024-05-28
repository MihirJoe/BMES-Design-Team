///////////////////////////////////////
// Rotary encoder sensor YT Tutorial //
//          ArduinoCraft             //
///////////////////////////////////////
// Initialization of the required variables
int counter = 0;
boolean direction;
int Pin_clk_Last;
int Pin_clk_Aktuell;
 
// Definition of the input pins
int pin_clk = 3;
int pin_dt = 4;
int button_pin = 5;
  
  
void setup ()
{
   // input pins are initialized ...
   pinMode (pin_clk, INPUT);
   pinMode (pin_dt, INPUT);
   pinMode (button_pin, INPUT);
    
   // ... and their pull-up resistors activated
   digitalWrite (pin_clk, true);
   digitalWrite (pin_dt, true);
   digitalWrite (button_pin, true);
    
   // Initial reading of the Pin_CLK
   Pin_clk_Last = digitalRead (pin_clk);
   Serial.begin (115200);
 }
 
 
void loop ()
{
   // Read out the current status
   Pin_clk_Aktuell = digitalRead (pin_clk);
    
   // Check for change
   if (Pin_clk_Aktuell != Pin_clk_Last)
   {
          
        if (digitalRead(pin_dt)!= Pin_clk_Aktuell)
        {
            // Pin_CLK changed first
            counter ++;
            direction = true;
        }
          
        else
        {// Otherwise, Pin_DT changed first
            direction = false;
            counter--;
        }
        Serial.println ("Rotation detected:");
        Serial.print ("Direction of rotation:");
         
        if (direction)
        {
           Serial.println ("clockwise");
        }
        else
        {
           Serial.println ("counterclockwise");
        }
         
        Serial.print ("Current position:");
        Serial.println (counter);
        Serial.println ("------------------------------");
          
   }
    
   // Preparation for the next run:
   // The value of the current run is the previous value for the next run
   Pin_clk_Last = Pin_clk_Aktuell;
    
   // Reset function to save the current position
   if (! digitalRead (button_pin) && counter != 0)
     {
       counter = 0;
       Serial.println ("position reset");
     }
      
 }