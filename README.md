# volvoextradisp
Extra display to my volvo

In my 2013 Volvo I do miss some information in the original instrument cluster. Such as temperature of Engine.
Reading in VIDA in says that display will show me if the temperature is to high.
So the value itself has to be on the CANbus, so start to digging into the cantraffic on the 
midspeed network gave me some information. I just hijacked the the CAN network behind the midcenter placed display. (It just have 4 wires, 12V, GND, CANL, CANH, so the "key on" or 15 Signal has to be on the network)

This is just a very rough estimation. May not be correct.

ID 0x155, DL8, data d5 d6 - RPM 16bit unsigned.

ID 0x155, DL8, D1 - Status (bit 3 - Ignition on?) 	

ID 0x155, DL8, D2 - Status (bit 6 - Engine startup)?

ID 0x155, DL8, D2 - Status (bit 3 - Engine running)?

ID 0x145, DL8, data d7 d8 - Speed 16bit unsigned. (Speed * 100) (Gets deci speed) (Backup Speed on D5 D6)

ID 0x195 D5 D6, D7 D8

0x145 Speed, 2 different

ID 0x4C5	Bit 6, CLT		- Coolant in C degrees, offset 60, so 80 is 20 degree C.

ID 0x035 D1: On? 0x04 = Ignition ON? (Not really sure here)

Structure used to get data from canbus and convert to a real unit:

volvo_id_s  rpm {.adress=0x155, .data_offset=4, .data_length=2, .multiplier=1.0, .value_offset=0};    
//Just raw value of RPM, same as the real RPM.

volvo_id_s  speed {.adress=0x145, .data_offset=6, .data_length=2, .multiplier=0.1, .value_offset=0};  
//There is two speed signals in same can frame, one from gearbox and the second from ABS system. Values in deci Km/h.

volvo_id_s  clt {.adress=0x4C5, .data_offset=5, .data_length=1, .multiplier=1.0, .value_offset=60};  
//Kind of rough values, precision of 1,5 degree. 


ID active in car:

0x035			Key ON?  Counter on byte 3 to 5, unknown
0x076
0x0E5
0x0F5
0x105
0x115
0x125
0x135
0x145			Dual Speed,  byte0 = ?, byte1-2 = synctime?, byte3 = unused, byte4-5 Speed1 byte6-7 Speed2
0x155			RPM
0x175 			All zero - All time
0x185
0x190			All zero - All time
0x195				Byte 4-5 - Some kind of level
0x1C0			
0x1F5
0x205
0x215
0x220			All zero - All time
0x235
0x245
0x265
0x27F			Byte5  = System ON (Not sleeping?)
0x285
0x28A			All zero - All time
0x295				
0x2B5	
0x2C5
0x2C6			All zero - All time
0x2D5			
0x2E5			
0x2F5			
0x305	
0x315		
0x325			All zero - All time
0x335			
0x345
0x355
0x365		
0x375
0x385			
0x395

0x3A5			CAN-TP?

0x3C0
0x3D5		
0x3E5
0x3F5		

0x400
0x405
0x415
0x425
0x435
0x445
0x455
0x465
0x475
0x485
0x495
0x4A5
0x4B5
0x4C5
0x4E5
0x4F1
0x4F5
0x501
0x503
0x504
0x505
0x509
0x510
0x511
0x547
0x554		8	01	12	00	10	54	00	00	00		Fixed all time
0x581		8	00	00	FF	FF	FF	FF	FF	FF		Fixed all time
























sort.sh:

Will be used to sort data from .trc files.

Usage:
./sort input.file canid byteinframe output.file 

This will also plot files with gnuplot, settings in gnuplotconf

This sort functions will only look at 1 byte.
To be able to grab two bytes, use sort16.sh, otherwise the same as sort.sh


Added arduino code for Teensy with CAN shield:

Just some example code, not nearly finished yet! Havent even tried it in the car yet.

2016-06-15	-	 Update

Now working code for temp reading.
Code still rough but working. 
Next step is to find Key ON signal, to be able to shut display down when car is not running.

Also add a button to be able to shift between, CLT, RPM, SPD

