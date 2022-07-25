/* Oscilloscope
*  =============
*
*  Function:               Utilises timer based interrupts to sample an analogue signal and plots its trace on the N5110 LCD screen.
*                        
*                          !!WARNING!!
*                          Ensure that the supplied signal doesnt not exceed ranges of 1-10Hz with an amplitude of no more than 3.3V
*                          with an Offset of 1V is applied.
*                        
*  Circuit Schematic No.:  9     : https://github.com/ELECXJEL2645/Circuit_Schematics
*  Required Libraries:     N5110 : https://github.com/ELECXJEL2645/N5110 
*
*  Authored by:            Andrew Knowles
*  Date:                   07/2022
*  Collaberators:          Dr Craig Evans
*                          Dr Alexander Valavanis
*  Version:                1.0
*  Revision Date:          07/2022 
*  MBED Studio Version:    1.4.1
*  MBED OS Version:        6.14.0
*  Board:	               NUCLEO L476RG */

#include "mbed.h"
#include "N5110.h"

#define xaxisStart 5
#define xaxisFinish 69
#define yaxisStart 5
#define yaxisFinish 42

//Pin assignment format:  lcd(IO, Ser_TX, Ser_RX, MOSI, SCLK, PWM)  
N5110 lcd(PC_7, PA_9, PB_10, PB_5, PB_3, PA_10);
AnalogIn probe(PA_0);   //scope probe attached to pin PA_0

Ticker scopeTimer;

void scopeTimer_isr();
void drawbackGround();
void drawTrace(int xPosOld, int xPosNew, int yPosOld, int yPosNew);

volatile bool g_scopeTimerTrigger;
int previousYposition = yaxisFinish - (probe.read() * 37);
int currentYposition;

int main(){
    lcd.init(LPH7366_1);
    scopeTimer.attach(&scopeTimer_isr, 1ms);

    while (1) {
        drawbackGround();

        //ensure that the waveforms trace remains within the X Axis bounds
        for (int x = xaxisStart; x < xaxisFinish; x++) {

            //Check if the IRS flag has been triggered
            if (g_scopeTimerTrigger) {

                //clear ISR flag
                g_scopeTimerTrigger = false;

                /*  -Analogue readings from 0.0 to 1.0 
                *   -Multiplied with the height of Y Axis (yaxisStart - yaxisFinish = 37) to scale the reading
                *   -Subtract scaled reading from the end point of the Y Axis to determine Y Coord. */
                currentYposition = yaxisFinish - (probe.read() * 37);

                //Updates the trace with regards to the new probe reading
                drawTrace(x, x + 1, previousYposition, currentYposition);

                //Updates the previous trace position
                previousYposition = currentYposition;
            }
            sleep();
        }
    }
}

void scopeTimer_isr() {
    g_scopeTimerTrigger = true;
}

void drawbackGround(){
    lcd.clear();
    lcd.drawRect(0, 0, 84, 48, FILL_TRANSPARENT);                           //draws screen boarder

    //               x0   ,      y0    ,     x1    ,     y1
    lcd.drawLine(xaxisStart, yaxisStart, xaxisStart, yaxisFinish, 1);       //draws Y-Axis
    lcd.drawLine(xaxisStart, yaxisFinish, xaxisFinish, yaxisFinish, 1);     //draws X-Axis
    lcd.drawLine(xaxisStart, 31, xaxisStart - 2, 31, 1);                    //draws 1V level indicator
    lcd.drawLine(xaxisStart, 19, xaxisStart - 2, 19, 1);                    //draws 2V level indicator
    lcd.drawLine(xaxisStart, 8, xaxisStart - 2, 8, 1);                      //draws 3V level indicator
    lcd.drawLine(37, yaxisFinish, 37, yaxisFinish + 2, 1);                  
    lcd.drawLine(69, yaxisFinish, 69, yaxisFinish + 2, 1);                 
    
    lcd.refresh();
}

void drawTrace(int xPosOld, int xPosNew, int yPosOld, int yPosNew) {
    lcd.drawLine(xPosOld, yPosOld, xPosNew, yPosNew, 1);
    lcd.refresh();
}
