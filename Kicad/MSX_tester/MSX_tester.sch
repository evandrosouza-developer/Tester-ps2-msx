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
L BlueBlackPill:BluePill U?
U 1 1 616EB669
P 5550 3850
F 0 "U?" H 5550 2519 50  0000 C CNN
F 1 "BluePill" H 5550 2428 50  0000 C CNN
F 2 "" V 5475 4800 50  0001 C CNN
F 3 "" V 5475 4800 50  0001 C CNN
	1    5550 3850
	1    0    0    -1  
$EndComp
Text Label 4600 3350 0    50   ~ 0
X7
Wire Wire Line
	4750 3350 4450 3350
Text Label 4600 3250 0    50   ~ 0
X6
Wire Wire Line
	4750 3250 4450 3250
Text Label 4600 3150 0    50   ~ 0
X5
Wire Wire Line
	4750 3150 4450 3150
Text Label 4600 3050 0    50   ~ 0
X4
Wire Wire Line
	4750 3050 4450 3050
Text Label 6500 3150 0    50   ~ 0
X3
Wire Wire Line
	6650 3150 6350 3150
Text Label 6500 3250 0    50   ~ 0
X2
Wire Wire Line
	6650 3250 6350 3250
Text Label 4600 4650 0    50   ~ 0
X1
Wire Wire Line
	4750 4650 4450 4650
Text Label 4600 4550 0    50   ~ 0
X0
Wire Wire Line
	4750 4550 4450 4550
Text Label 6500 3550 0    50   ~ 0
Y3
Wire Wire Line
	6650 3550 6350 3550
Text Label 6500 3650 0    50   ~ 0
Y2
Wire Wire Line
	6650 3650 6350 3650
Text Label 6500 3750 0    50   ~ 0
Y1
Wire Wire Line
	6650 3750 6350 3750
Text Label 6500 3850 0    50   ~ 0
Y0
Wire Wire Line
	6650 3850 6350 3850
Text Label 4500 4250 0    50   ~ 0
CAPS
Wire Wire Line
	4750 4250 4250 4250
Text Label 4500 4350 0    50   ~ 0
KANA
Wire Wire Line
	4750 4350 4250 4350
Entry Wire Line
	6650 3550 6750 3650
Entry Wire Line
	6650 3650 6750 3750
Entry Wire Line
	6650 3750 6750 3850
Entry Wire Line
	6650 3850 6750 3950
$Comp
L power:GNDD #PWR?
U 1 1 61708957
P 5450 5050
F 0 "#PWR?" H 5450 4800 50  0001 C CNN
F 1 "GNDD" H 5454 4895 50  0000 C CNN
F 2 "" H 5450 5050 50  0001 C CNN
F 3 "" H 5450 5050 50  0001 C CNN
	1    5450 5050
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 4950 5350 5000
Wire Wire Line
	5350 5000 5450 5000
Wire Wire Line
	5450 5000 5450 5050
Wire Wire Line
	5450 4950 5450 5000
Connection ~ 5450 5000
Wire Wire Line
	5550 4950 5550 5000
Wire Wire Line
	5550 5000 5450 5000
$Comp
L power:+3.3V #PWR?
U 1 1 6170A0D7
P 5600 2650
F 0 "#PWR?" H 5600 2500 50  0001 C CNN
F 1 "+3.3V" H 5615 2823 50  0000 C CNN
F 2 "" H 5600 2650 50  0001 C CNN
F 3 "" H 5600 2650 50  0001 C CNN
	1    5600 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	5550 2750 5550 2700
Wire Wire Line
	5550 2700 5600 2700
Wire Wire Line
	5600 2700 5600 2650
Wire Wire Line
	5650 2750 5650 2700
Wire Wire Line
	5650 2700 5600 2700
Connection ~ 5600 2700
Wire Bus Line
	6750 2150 4350 2150
Entry Wire Line
	4350 3150 4450 3050
Entry Wire Line
	4350 3250 4450 3150
Entry Wire Line
	4350 3350 4450 3250
Entry Wire Line
	4350 3450 4450 3350
Entry Wire Line
	4350 4650 4450 4550
Entry Wire Line
	4350 4750 4450 4650
Entry Wire Line
	6650 3250 6750 3150
Entry Wire Line
	6650 3150 6750 3050
Wire Wire Line
	6350 3350 7000 3350
Wire Wire Line
	6350 3450 7000 3450
Text Label 6800 3350 0    50   ~ 0
INT_TIM2
Text Label 6850 3450 0    50   ~ 0
SYSTICK
Wire Wire Line
	6350 4150 7000 4150
Text Label 6550 4150 0    50   ~ 0
Xint
Wire Wire Line
	6350 4250 7250 4250
Text Label 5450 2150 0    50   ~ 0
X_BUS
Text Label 6750 3800 0    50   ~ 0
Y_BUS
Wire Wire Line
	6350 4050 7250 4050
Text GLabel 6850 3950 2    50   Input ~ 0
Serial_RX
Text GLabel 7250 4050 0    50   Input ~ 0
Serial_TX
Text GLabel 7250 4250 0    50   Input ~ 0
Y_Port_Sync
Text Notes 7050 7050 0    50   ~ 0
MSX Sub system keyboard Emulator\nTo be controlled by serial console\nThe Y_Port_Sync is to be used on oscilloscopes as external sync.\nIt has some test capabilities increased over a real MSX.\n\nNo PCB will be done for this one.
Text Notes 8300 7650 0    50   ~ 0
October, 19th, 2021
Text Notes 7750 7500 0    50   ~ 0
PS/2 - MSX Tester
Wire Wire Line
	6350 3950 6850 3950
Wire Bus Line
	6750 2150 6750 3400
Wire Bus Line
	6750 3550 6750 4000
Wire Bus Line
	4350 2150 4350 4800
$EndSCHEMATC
