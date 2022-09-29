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
L MSX_tester-rescue:BluePill-BlueBlackPill U1
U 1 1 616EB669
P 4430 3618
F 0 "U1" H 4430 3623 50  0000 C CNN
F 1 "BluePill" H 4430 2196 50  0000 C CNN
F 2 "" V 4355 4568 50  0001 C CNN
F 3 "" V 4355 4568 50  0001 C CNN
	1    4430 3618
	1    0    0    -1  
$EndComp
Text Label 3480 3118 0    50   ~ 0
X7
Wire Wire Line
	3630 3118 3330 3118
Text Label 3480 3018 0    50   ~ 0
X6
Wire Wire Line
	3630 3018 3330 3018
Text Label 3480 2918 0    50   ~ 0
X5
Wire Wire Line
	3630 2918 3330 2918
Text Label 3480 2818 0    50   ~ 0
X4
Wire Wire Line
	3630 2818 3330 2818
Text Label 3480 4218 0    50   ~ 0
X1
Wire Wire Line
	3630 4218 3330 4218
Text Label 3480 4118 0    50   ~ 0
X0
Wire Wire Line
	3630 4118 3330 4118
Text Label 5380 3318 0    50   ~ 0
Y3
Wire Wire Line
	6030 3318 5230 3318
Text Label 5380 3418 0    50   ~ 0
Y2
Wire Wire Line
	6030 3418 5230 3418
Text Label 5380 3518 0    50   ~ 0
Y1
Wire Wire Line
	6030 3518 5230 3518
Text Label 5380 3618 0    50   ~ 0
Y0
Wire Wire Line
	6030 3618 5230 3618
Text Label 3380 3918 0    50   ~ 0
CAPS
Text Label 3380 4018 0    50   ~ 0
KANA
Wire Wire Line
	3630 4018 3130 4018
$Comp
L power:GNDD #PWR?
U 1 1 61708957
P 4330 4818
F 0 "#PWR?" H 4330 4568 50  0001 C CNN
F 1 "GNDD" H 4334 4663 50  0000 C CNN
F 2 "" H 4330 4818 50  0001 C CNN
F 3 "" H 4330 4818 50  0001 C CNN
	1    4330 4818
	1    0    0    -1  
$EndComp
Wire Wire Line
	4230 4718 4230 4768
Wire Wire Line
	4230 4768 4330 4768
Wire Wire Line
	4330 4768 4330 4818
Wire Wire Line
	4330 4718 4330 4768
Connection ~ 4330 4768
Wire Wire Line
	4430 4718 4430 4768
Wire Wire Line
	4430 4768 4330 4768
$Comp
L power:+3.3V #PWR?
U 1 1 6170A0D7
P 4480 2418
F 0 "#PWR?" H 4480 2268 50  0001 C CNN
F 1 "+3.3V" H 4495 2591 50  0000 C CNN
F 2 "" H 4480 2418 50  0001 C CNN
F 3 "" H 4480 2418 50  0001 C CNN
	1    4480 2418
	1    0    0    -1  
$EndComp
Wire Wire Line
	4430 2518 4430 2468
Wire Wire Line
	4430 2468 4480 2468
Wire Wire Line
	4480 2468 4480 2418
Wire Wire Line
	4530 2518 4530 2468
Wire Wire Line
	4530 2468 4480 2468
Connection ~ 4480 2468
Entry Wire Line
	3230 2918 3330 2818
Entry Wire Line
	3230 3018 3330 2918
Entry Wire Line
	3230 3118 3330 3018
Entry Wire Line
	3230 3218 3330 3118
Entry Wire Line
	3230 4218 3330 4118
Entry Wire Line
	3230 4318 3330 4218
Text Label 5340 3118 0    50   ~ 0
INT_TIM2
Text Label 5360 3218 0    50   ~ 0
SYSTICK
Wire Wire Line
	5230 3918 5630 3918
Text Label 5430 3918 0    50   ~ 0
Xint
Wire Wire Line
	5230 4018 5620 4019
Text Label 3230 3588 0    50   ~ 0
X_BUS
Wire Wire Line
	5230 3818 5798 3818
Text GLabel 5798 3718 2    50   Input ~ 0
UART_RX
Text GLabel 5798 3818 2    50   Output ~ 0
UART_TX
Text GLabel 5620 4019 2    50   Output ~ 0
Y_Begin_Mark
Text Notes 7050 7050 0    50   ~ 0
MSX Sub system keyboard Emulator\nTo be controlled by serial console\nThe Y_Port_Sync is to be used on oscilloscopes as external sync.\nIt has some test capabilities increased over a real MSX.\n\nNo PCB will be done for this one.
Text Notes 8300 7650 0    50   ~ 0
September, 29th, 2022
Text Notes 7750 7500 0    50   ~ 0
PS/2 - MSX Tester
Text Label 3480 4418 0    50   ~ 0
X3
Wire Wire Line
	3630 4418 3330 4418
Text Label 3480 4318 0    50   ~ 0
X2
Wire Wire Line
	3630 4318 3330 4318
Entry Wire Line
	3230 4418 3330 4318
Entry Wire Line
	3230 4518 3330 4418
Wire Wire Line
	5230 3118 5675 3118
Wire Wire Line
	5230 3218 5675 3218
Wire Wire Line
	3130 3918 3630 3918
Text GLabel 5630 3918 2    50   Output ~ 0
Read_X_Scan
Text GLabel 6030 3318 2    50   Output ~ 0
Y3
Text GLabel 6030 3418 2    50   Output ~ 0
Y2
Text GLabel 6030 3518 2    50   Output ~ 0
Y1
Text GLabel 6030 3618 2    50   Output ~ 0
Y0
Wire Wire Line
	5230 3718 5798 3718
Text GLabel 3130 3918 0    50   Output ~ 0
CapsLed
Text GLabel 3130 4018 0    50   Output ~ 0
KanaLed
Entry Wire Line
	3130 2818 3230 2918
Entry Wire Line
	3130 2918 3230 3018
Entry Wire Line
	3130 3018 3230 3118
Entry Wire Line
	3130 3118 3230 3218
Entry Wire Line
	3130 3218 3230 3318
Entry Wire Line
	3130 3318 3230 3418
Entry Wire Line
	3130 3418 3230 3518
Entry Wire Line
	3130 3518 3230 3618
Text GLabel 2880 2818 0    50   Input ~ 0
X7
Text GLabel 2880 2918 0    50   Input ~ 0
X6
Text GLabel 2880 3018 0    50   Input ~ 0
X5
Text GLabel 2880 3118 0    50   Input ~ 0
X4
Text GLabel 2880 3218 0    50   Input ~ 0
X3
Text GLabel 2880 3318 0    50   Input ~ 0
X2
Text GLabel 2880 3418 0    50   Input ~ 0
X1
Text GLabel 2880 3518 0    50   Input ~ 0
X0
Wire Wire Line
	2880 2818 3130 2818
Wire Wire Line
	3130 2918 2880 2918
Wire Wire Line
	3130 3018 2880 3018
Wire Wire Line
	2880 3118 3130 3118
Wire Wire Line
	2880 3218 3130 3218
Wire Wire Line
	2880 3318 3130 3318
Wire Wire Line
	2880 3418 3130 3418
Wire Wire Line
	2880 3518 3130 3518
Text Notes 6540 4897 0    50   ~ 0
Y_Begin_Mark pin is only low signaling that these Y3:Y0 bits\n(Y_Scan) are  the first colunm to scan, as defined by user on\nconsole by Scan -> Begin menu options. Its defaut state is high.\nCan be used to trigger an external sampling asset, like, for example,\nan oscilloscope. Details can be found on Tester Technical and\nPerformance Manual.pdf, on project Github Documentation folder.\nCan be used to trigger an external sampling asset, like, for example,\nan oscilloscope. Details can be found on Tester Technical and\nPerformance Manual.pdf, on project Github Documentation folder.
Text Notes 6535 3875 0    50   ~ 0
Read_X_Scan pin is low signaling that these Y3:Y0 bits (Y_Scan)\nare updated and return high when reading  X7:X0 (response from\nPS/2 to MSX Keyboard Converter - or a decoded MSX keyboard itself).\nIt marks a time window to the converter has to answer.\nThis time is also defined by user on console by tunning its time using\n"<" and ">".\nCan be used to trigger an external sampling asset, like, for example,\nan oscilloscope. Details can be found on Tester Technical and\nPerformance Manual.pdf, on project Github Documentation folder.\nThis information can also be used to trigger an external sampling\nasset, like, an oscilloscope or logic analyzer.
Text Notes 10746 7635 0    50   ~ 0
1.4
Wire Bus Line
	3230 2618 3230 4568
$EndSCHEMATC
