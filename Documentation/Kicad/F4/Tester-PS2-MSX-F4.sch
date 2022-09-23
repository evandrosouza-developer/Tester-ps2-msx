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
P 6150 4700
F 0 "#DGND0101" H 6150 4450 50  0001 C CNN
F 1 "GNDD-power" H 6150 4550 50  0000 C CNN
F 2 "" H 6150 4700 50  0000 C CNN
F 3 "" H 6150 4700 50  0000 C CNN
	1    6150 4700
	1    0    0    -1  
$EndComp
Text Label 5175 2750 0    50   ~ 0
X0'
Text Label 5175 2850 0    50   ~ 0
X2'
Text Label 5175 2950 0    50   ~ 0
X4'
Text Label 7250 2750 0    50   ~ 0
X1'
Text Label 7250 3050 0    50   ~ 0
X7'
Wire Wire Line
	6250 4650 6150 4650
Text Notes 8250 7650 0    50   ~ 0
October, 31th 2021
Text Notes 7400 7500 0    50   ~ 0
PS/2 Adapter for MSX keyboard Project using WeAct Studio STM32F401CCU6 V2.0+ (Black Pill)
NoConn ~ 6050 2450
NoConn ~ 6250 2450
NoConn ~ 7050 3850
NoConn ~ 7050 4050
NoConn ~ 7050 4150
NoConn ~ 7050 4250
Wire Wire Line
	6150 4700 6150 4650
Text Notes 7350 6950 0    71   ~ 0
Designed by Evandro Souza\nFree to use as is.\nFor more details, see the technical docs.
Wire Wire Line
	7050 2750 7650 2750
Wire Wire Line
	7275 5250 7275 3150
Wire Wire Line
	7050 3250 7350 3250
Wire Wire Line
	5325 5250 5325 3150
Text Label 5325 3150 0    50   ~ 0
Y0
Text Label 7100 3150 0    50   ~ 0
Y1
Text Label 7100 3250 0    50   ~ 0
Y2
Text Label 7100 3350 0    50   ~ 0
Y3
Wire Wire Line
	7950 3050 7050 3050
Text Label 7250 2950 0    50   ~ 0
X5'
Text Label 4950 4050 0    50   ~ 0
KanaLed
Text Label 4950 3850 0    50   ~ 0
CapsLed
NoConn ~ -2150 2900
Wire Wire Line
	4930 3050 5450 3050
Wire Wire Line
	4660 2850 5450 2850
Wire Wire Line
	4570 2750 4570 5050
Wire Wire Line
	4570 2750 5450 2750
Text Label 5175 3050 0    50   ~ 0
X6'
Wire Wire Line
	4660 5050 4660 2850
$Comp
L Tester-PS2-MSX-F4-rescue:+5VD-power-PS2-MSX-F4-rescue-PS2-MSX-F4-rescue #PWR0101
U 1 1 610F50E6
P 6150 2075
F 0 "#PWR0101" H 6150 1925 50  0001 C CNN
F 1 "+5VD-power" H 5875 2150 50  0000 C CNN
F 2 "" H 6150 2075 50  0001 C CNN
F 3 "" H 6150 2075 50  0001 C CNN
	1    6150 2075
	1    0    0    -1  
$EndComp
Wire Wire Line
	7050 2950 7850 2950
Wire Wire Line
	7050 3150 7275 3150
Wire Wire Line
	7650 2750 7650 5050
$Comp
L Tester-PS2-MSX-F4-rescue:BlackPill-minif4_board-1-YAAJ_BluePill-PS2-MSX-F4-rescue-PS2-MSX-F4-rescue U1
U 1 1 60FE37A6
P 6250 3550
F 0 "U1" H 5650 2600 50  0000 C CNN
F 1 "WeAct Studio STM32F401CEU6 V3.0+ (Black Pill)" H 6325 2150 50  0000 C CNN
F 2 "BlueBlackPill:BlackPill-minif4_board-1" V 6175 4500 50  0001 C CNN
F 3 "" V 6175 4500 50  0001 C CNN
	1    6250 3550
	1    0    0    -1  
$EndComp
Text Label 5165 3750 0    50   ~ 0
X3'
Wire Wire Line
	4830 2950 5450 2950
Wire Wire Line
	4300 4050 5450 4050
Wire Wire Line
	4000 3850 5450 3850
Wire Wire Line
	7350 3250 7350 5250
Wire Wire Line
	5450 3150 5325 3150
NoConn ~ 5950 2450
NoConn ~ 7050 3950
NoConn ~ 5450 3450
NoConn ~ 5450 3550
NoConn ~ 7050 3750
NoConn ~ 5450 4150
NoConn ~ 5450 4350
NoConn ~ 5450 4250
Wire Wire Line
	4830 2950 4830 5050
Wire Wire Line
	4925 5050 4930 3050
Wire Wire Line
	7425 3350 7425 5250
Wire Wire Line
	7050 3350 7425 3350
Wire Wire Line
	8575 3450 8625 3450
$Comp
L Tester-PS2-MSX-F4-rescue:GNDD-power-PS2-MSX-F4-rescue-PS2-MSX-F4-rescue #PWR0106
U 1 1 60D1B504
P 8925 3675
F 0 "#PWR0106" H 8925 3425 50  0001 C CNN
F 1 "GNDD-power" H 8925 3525 50  0001 C CNN
F 2 "" H 8925 3675 50  0000 C CNN
F 3 "" H 8925 3675 50  0000 C CNN
	1    8925 3675
	1    0    0    -1  
$EndComp
Wire Wire Line
	8925 3650 8925 3675
Wire Wire Line
	8925 3050 8925 3250
$Comp
L Tester-PS2-MSX-F4-rescue:R-Device-PS2-MSX-F4-rescue-PS2-MSX-F4-rescue R10
U 1 1 60D17220
P 8425 3450
F 0 "R10" V 8425 3350 50  0000 L CNN
F 1 "33K" V 8325 3350 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 8355 3450 50  0001 C CNN
F 3 "~" H 8425 3450 50  0001 C CNN
	1    8425 3450
	0    1    -1   0   
$EndComp
NoConn ~ 7050 3650
NoConn ~ 7050 3550
Text Label 3945 3250 0    50   ~ 0
3.3V_Serial_TX
Text Label 3945 3350 0    50   ~ 0
3.3V_Serial_RX
Wire Wire Line
	3600 3350 5450 3350
Wire Wire Line
	3600 3250 5450 3250
Wire Wire Line
	7050 3450 8275 3450
$Comp
L Tester-PS2-MSX-F4-rescue:2SC1815-Transistor_BJT-PS2-MSX-F4-rescue-PS2-MSX-F4-rescue Q1
U 1 1 611133C5
P 8825 3450
F 0 "Q1" H 9015 3496 50  0000 L CNN
F 1 "2SC1815" H 9015 3405 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 9025 3375 50  0001 L CIN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Toshiba%20PDFs/2SC1815.pdf" H 8825 3450 50  0001 L CNN
	1    8825 3450
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR0105
U 1 1 61305367
P 6350 2400
F 0 "#PWR0105" H 6350 2250 50  0001 C CNN
F 1 "+3.3V" H 6365 2573 50  0000 C CNN
F 2 "" H 6350 2400 50  0001 C CNN
F 3 "" H 6350 2400 50  0001 C CNN
	1    6350 2400
	1    0    0    -1  
$EndComp
Wire Wire Line
	6350 2450 6350 2400
Wire Wire Line
	4745 3750 4745 5050
Wire Wire Line
	5450 3750 4745 3750
NoConn ~ 7050 2850
Wire Wire Line
	6150 2075 6150 2450
Entry Bus Bus
	7550 5150 7650 5050
Entry Bus Bus
	7750 5150 7850 5050
Entry Bus Bus
	7850 5150 7950 5050
Wire Wire Line
	7850 2950 7850 5050
Wire Wire Line
	7950 5050 7950 3050
Entry Bus Bus
	4470 5150 4570 5050
Entry Bus Bus
	4560 5150 4660 5050
Entry Bus Bus
	4645 5150 4745 5050
Entry Bus Bus
	4730 5150 4830 5050
Entry Bus Bus
	4825 5150 4925 5050
Entry Bus Bus
	5225 5350 5325 5250
Entry Bus Bus
	7175 5350 7275 5250
Entry Bus Bus
	7250 5350 7350 5250
Entry Bus Bus
	7325 5350 7425 5250
Wire Bus Line
	4000 5350 8320 5350
Wire Bus Line
	4000 5150 8315 5150
Text Label 4050 5150 0    50   ~ 0
X_BUS
Text Label 4050 5350 0    50   ~ 0
Y_BUS
$EndSCHEMATC
