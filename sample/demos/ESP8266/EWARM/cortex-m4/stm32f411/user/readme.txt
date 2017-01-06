1.Connect the STM32  Nucleo board to a PC with a USB cable Type-A to Mini-B through USB connector CN1 to power the  board.



2. model is esp8266 made by AI-Cloud,Connect as this     

   serial 1(communcation)£º        PA9-----------URXD
                                   PA10----------UTXD
   esp8266:			               PA0-----------CH_PD
				                   PA1-----------RST
				                   3.3v----------VCC
				                   GND-----------GND
   Bound£º                         115200
		 
   serial2(debug)£º                USB        
			   
   esp8266(right side up)          GND  GPIO2  GPIO0  URXD
                                   UTXD CH_PD  RST    VCC


 3.Make the project and then download and Run.
  
  
 4.Open one serial port assistant , Observe the buffer  received and Led on board.
  
  
 5.Any questions or suggestions, please contact us.