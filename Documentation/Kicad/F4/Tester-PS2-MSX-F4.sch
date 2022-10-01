EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Tester-PS2-MSX-F4-rescue:GNDD-power-PS2-MSX-F4-rescue-PS2-MSX-F4-rescue #DGND0101
U 1 1 60C49885
P 4913 4618
F 0 "#DGND0101" H 4913 4368 50  0001 C CNN
F 1 "GNDD-power" H 4913 4468 50  0000 C CNN
F 2 "" H 4913 4618 50  0000 C CNN
F 3 "" H 4913 4618 50  0000 C CNN
	1    4913 4618
	1    0    0    -1  
$EndComp
Text Label 4113 2768 0    50   ~ 0
X2
Text Label 4113 2868 0    50   ~ 0
X4
Text Label 6013 2668 0    50   ~ 0
X1
Text Label 6013 2968 0    50   ~ 0
X7
Wire Wire Line
	5015 4568 4915 4568
Text Notes 8250 7650 0    50   ~ 0
September, 28th 2022
Text Notes 7400 7500 0    50   ~ 0
PS/2 Adapter for MSX keyboard Project using WeAct Studio STM32F401CCU6 V2.0+ (Black Pill)
NoConn ~ 4815 2368
NoConn ~ 5015 2368
NoConn ~ 5815 3768
NoConn ~ 5815 3968
NoConn ~ 5815 4068
NoConn ~ 5815 4168
Wire Wire Line
	4913 4618 4915 4568
Text Notes 7350 6950 0    71   ~ 0
Designed by Evandro Souza\nFree to use as is.\nFor more details, see the technical docs.
Wire Wire Line
	6038 5168 6038 3068
Wire Wire Line
	5815 3168 6113 3168
Text Label 4088 3068 0    50   ~ 0
Y0
Text Label 5863 3068 0    50   ~ 0
Y1
Text Label 5863 3168 0    50   ~ 0
Y2
Text Label 5863 3268 0    50   ~ 0
Y3
Text Label 6015 2868 0    50   ~ 0
X5
Text Label 3713 3968 0    50   ~ 0
KanaLed
Text Label 3713 3768 0    50   ~ 0
CapsLed
NoConn ~ -2150 2900
Text Label 4113 2968 0    50   ~ 0
X6
$Comp
L Tester-PS2-MSX-F4-rescue:+5VD-power-PS2-MSX-F4-rescue-PS2-MSX-F4-rescue #PWR0101
U 1 1 610F50E6
P 4913 2328
F 0 "#PWR0101" H 4913 2178 50  0001 C CNN
F 1 "+5VD-power" H 4638 2403 50  0000 C CNN
F 2 "" H 4913 2328 50  0001 C CNN
F 3 "" H 4913 2328 50  0001 C CNN
	1    4913 2328
	1    0    0    -1  
$EndComp
Wire Wire Line
	5815 3068 6038 3068
$Comp
L Tester-PS2-MSX-F4-rescue:BlackPill-minif4_board-1-YAAJ_BluePill-PS2-MSX-F4-rescue-PS2-MSX-F4-rescue U1
U 1 1 60FE37A6
P 5015 3468
F 0 "U1" H 4415 2518 50  0000 C CNN
F 1 "WeAct Studio STM32F401CEU6 V2.0+ (Black Pill)" H 5090 2068 50  0000 C CNN
F 2 "BlueBlackPill:BlackPill-minif4_board-1" V 4940 4418 50  0001 C CNN
F 3 "" V 4940 4418 50  0001 C CNN
	1    5015 3468
	1    0    0    -1  
$EndComp
Text Label 3928 3668 0    50   ~ 0
X3
Wire Wire Line
	2883 3968 4215 3968
Wire Wire Line
	2883 3768 4215 3768
Wire Wire Line
	6113 3168 6113 5168
Wire Wire Line
	4215 3068 4088 3068
NoConn ~ 4715 2368
NoConn ~ 5815 3868
NoConn ~ 4215 3368
NoConn ~ 4215 3468
NoConn ~ 4215 4068
NoConn ~ 4215 4268
NoConn ~ 4215 4168
Wire Wire Line
	6188 3268 6188 5168
Wire Wire Line
	5815 3268 6188 3268
NoConn ~ 5815 3568
NoConn ~ 5815 3468
Wire Wire Line
	2668 3268 4215 3268
NoConn ~ 5815 2768
Wire Wire Line
	4913 2328 4915 2368
Text Label 2813 5268 0    50   ~ 0
Y_BUS
Text Label 2833 2168 0    50   ~ 0
X_BUS
Wire Wire Line
	4088 5168 4088 3068
Text GLabel 2654 3170 0    50   Output ~ 0
TX
Text GLabel 2668 3268 0    50   Input ~ 0
RX
Text GLabel 2883 3768 0    50   Output ~ 0
CapsLed
Text GLabel 2883 3968 0    50   Output ~ 0
KanaLed
Text GLabel 2663 2268 0    50   Input ~ 0
X7
Text GLabel 2663 2368 0    50   Input ~ 0
X6
Text GLabel 2663 2468 0    50   Input ~ 0
X5
Text GLabel 2663 2568 0    50   Input ~ 0
X4
Text GLabel 2663 2668 0    50   Input ~ 0
X3
Text GLabel 2663 2768 0    50   Input ~ 0
X2
Text GLabel 2663 2868 0    50   Input ~ 0
X1
Text GLabel 2663 2968 0    50   Input ~ 0
X0
Entry Wire Line
	3813 2168 3913 2268
Entry Wire Line
	3913 2168 4013 2268
Entry Wire Line
	3713 2168 3813 2268
Entry Wire Line
	3613 2168 3713 2268
Entry Wire Line
	3513 2168 3613 2268
Entry Wire Line
	2709 2268 2809 2168
Entry Wire Line
	2813 2268 2913 2168
Entry Wire Line
	2913 2268 3013 2168
Entry Wire Line
	3013 2268 3113 2168
Entry Wire Line
	3113 2268 3213 2168
Entry Wire Line
	3213 2268 3313 2168
Entry Wire Line
	3313 2268 3413 2168
Wire Wire Line
	3713 2768 4215 2768
Wire Wire Line
	4013 2968 4215 2968
Wire Wire Line
	4215 3668 3813 3668
Text Label 4113 2668 0    50   ~ 0
X0
Wire Wire Line
	3613 2668 4215 2668
Wire Wire Line
	2663 2368 2813 2368
Wire Wire Line
	2663 2468 2913 2468
Wire Wire Line
	2663 2568 3013 2568
Wire Wire Line
	2663 2668 3113 2668
Wire Wire Line
	2663 2768 3213 2768
Entry Wire Line
	3413 2268 3513 2168
Wire Wire Line
	2709 2268 2663 2268
Text GLabel 6264 3668 2    50   Output ~ 0
Read_X_Scan
Wire Wire Line
	5815 3668 6264 3668
Text GLabel 6251 3369 2    50   Output ~ 0
Y_Begin_Mark
Wire Wire Line
	5815 3368 6251 3369
Text GLabel 2663 4718 0    50   Output ~ 0
Y2
Text GLabel 2662 4818 0    50   Output ~ 0
Y1
Text GLabel 2663 4918 0    50   Output ~ 0
Y0
Wire Wire Line
	2662 4818 3263 4818
Entry Wire Line
	3563 5268 3463 5168
Entry Wire Line
	3463 5268 3363 5168
Entry Wire Line
	3363 5268 3263 5168
Entry Wire Line
	3263 5268 3163 5168
Wire Wire Line
	2654 3170 4215 3168
Text GLabel 2663 4618 0    50   Output ~ 0
Y3
Wire Wire Line
	2663 4918 3163 4918
Wire Wire Line
	3163 4918 3163 5168
Wire Wire Line
	3263 5168 3263 4818
Wire Wire Line
	2663 4718 3363 4718
Wire Wire Line
	3363 4718 3363 5168
Wire Wire Line
	3463 5168 3463 4618
Wire Wire Line
	3463 4618 2663 4618
Entry Wire Line
	6013 5268 6113 5168
Entry Wire Line
	6088 5268 6188 5168
Entry Wire Line
	5938 5268 6038 5168
Entry Wire Line
	3988 5268 4088 5168
Connection ~ 4915 4568
Wire Wire Line
	6413 2668 6413 2268
Entry Wire Line
	6313 2168 6413 2268
Wire Wire Line
	5815 2668 6413 2668
Wire Wire Line
	3613 2268 3613 2668
Wire Wire Line
	3713 2768 3713 2268
Wire Wire Line
	4013 2968 4013 2268
Wire Wire Line
	3913 2268 3913 2868
Wire Wire Line
	3913 2868 4215 2868
Wire Wire Line
	3813 3668 3813 2268
Entry Wire Line
	6513 2168 6613 2268
Entry Wire Line
	6413 2168 6513 2268
Wire Wire Line
	5815 2968 6613 2968
Wire Wire Line
	5815 2868 6513 2868
Wire Wire Line
	6513 2868 6513 2268
Wire Wire Line
	6613 2968 6613 2268
Wire Wire Line
	2813 2268 2813 2368
Wire Wire Line
	2913 2268 2913 2468
Wire Wire Line
	3013 2268 3013 2568
Wire Wire Line
	3113 2268 3113 2668
Wire Wire Line
	3213 2268 3213 2768
Wire Wire Line
	3313 2268 3313 2868
Wire Wire Line
	3313 2868 2663 2868
Wire Wire Line
	3413 2268 3413 2968
Wire Wire Line
	3413 2968 2663 2968
Text Notes 6938 3501 0    50   ~ 0
Y_Begin_Mark pin is only low signaling that these Y3:Y0 bits\n(Y_Scan) are  the first colunm to scan, as defined by user on\nconsole by Scan -> Begin menu options. Its defaut state is high.\n\nCan be used to trigger an external sampling asset, like, for example,\nan oscilloscope.
Text Notes 6946 4657 0    50   ~ 0
Read_X_Scan pin is low signaling that these Y3:Y0 bits (Y_Scan)\nare updated and return high when reading  X7:X0 (response from\nPS/2 to MSX Keyboard Converter - or a decoded MSX keyboard itself).\n\nIt marks a time window to the converter has to answer.\nThis time is also defined by user on console by tunning its time using\n"<" and ">".\n\nThis information can also be used to trigger an external\nsampling asset, like, an oscilloscope or logic analyzer.\n\nDetails can be found on the pdf file Tester Technical and \nPerformance Manual, on project Github Documentation folder.
Text Notes 10636 7634 0    50   ~ 0
1.0
Wire Bus Line
	2763 5268 6469 5268
Wire Bus Line
	2760 2168 6675 2168
$EndSCHEMATC
