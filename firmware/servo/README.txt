Ce projet est un firmware FPGA de contolleur de servo de telecommande.
Une description de ce genre de servos peut être trouve a:
http://fribotte.free.fr/bdtech/pic/pic_et_servo.html
Il est inspire du projet "Atmel AVR 2004 Design Contest Entry Project Number A3722"
de Circuit Cellar par Eric Gagnon :
http://www.circuitcellar.com/avr2004/HA3722.html
Ce controleur de servo permet de controler jusqu'a 24 servos.
Il est facilement extensible a plus.

L'utilisation d'un FPGA est justifie car avec les sorties classiques d'un microcontroleur,
meme avec l'utilisation d'un timer hardware et d'une programmation en assembleur,
le repect precis des largeurs des impulsions PWM est quasi impossible surtout avec
un nombre eleve de servos a commander.
Un FPGA permet de realiser une concurence vraie dans la generation des signaux.
D'ailleurs avec ce controleur, il n'y a pas de fretillement des servos quelque
soient leur position les uns par rapport aux autres, ce qui n'est pas le cas
dans une solution classique a microcontoleur.

1 - CREATION D'UN NOUVEAU PROJET DANS XILINX ISE
Version Xilinx ISE 9.2.04i
Creer le nouveau projet:
File->New Project
Selectionner Top Level Source: HDL (Harware Description Language)
Pour le FPGA de la carte Armadeus choisir:
Family: Spartan3
Device: XC3S200
Package: TQ144
Speed: -4
Top Level Source: HDL
Synthesis Tool: XST
Simulator: ISE Simulator (VHDL/Verilog)
Preferred Language: VHDL

Importer les fichiers sources dans le projet ISE:
Projet->Add Source
Selectionner les fichiers:
SERVO_top.vhd
CLK_GENERATOR.vhd
pwm_module.vhd
servo.ucf

2 - SYNTHESE - CREATION DU FICHIER FIRMWARE  SERVO_top.bit
Generate Programming File->clic droit->Run

3 - CHARGEMENT DU FIRMWARE SERVO_top.bit DANS L'APF9328
Se placer dans le repertoire qui contient le fichier SERVO_top.bit

Etablir une connexion RS232 avec l'APF9328 comme le decrit:
http://www.armadeus.com/wiki/index.php?title=Connection_with_U-Boot_on_Linux

Mettre sous tension l'APF9328
Au message Hit any key to stop autoboot:
Appuyer sur une touche pour obtenir le prompt:
BIOS> 

Choisir l'emplacement memoire de l'upload:
loadb 08000000

Le message:
## Ready for binary (kermit) download to 0x08000000 at 115200 bps...
apparait.

Passer en mode commande Kermit:
Ctrl+Altgr+\+c 

Upload du fichier:
send SERVO_top.bit

Kermit upload le fichier.
Taper c pour reconnecter le terminal.

Flasher le firmware:
run flash_firmware

Charger le firmware dans le FPGA:
fpga load 0 ${firmware_addr} ${firmware_len}

Pour un chargement automatique au boot:
setenv firmware_autoload 1
saveenv

Le firmware est maintenant charge et utilisable.

4 - UTILISATION DU FIRMWARE
Actuellement seul le firmware est ecrit. 
Les servos sont a brancher sur les ports de la DevLight suivants via un buffer
74HCT244:
L24N_3/P87 -> servo 00
L24P_3/P86 -> servo 01
L23N_3/P85 -> servo 02
L40P_2/P92 -> servo 03
L40N_3/P90 -> servo 04
L40P_3/P89 -> servo 05
L24N_2/P96 -> servo 06
L24P_2/P95 -> servo 07
L40N_2/P93 -> servo 08
L22P_2/P99 -> servo 09
L23N_2/P98 -> servo 10
L23P_2/P97 -> servo 11
L21N_2/P103 -> servo 12
L21P_2/P102 -> servo 13
L22N_2/P100 -> servo 14
L01P_2/P107 -> servo 15
L20N_2/P105 -> servo 16
L20P_2/P104 -> servo 17
L32N_0/P128 -> servo 18
L32P_0/P127 -> servo 19
L01N_2/P108 -> servo 20
L31P_0/P129 -> servo 21
L31N_0/P130 -> servo 22
L32N_1/P125 -> servo 23

Commande BIOS pour le controle des servos:
Au chargement du firmware, tous les servos sont desactives.

Activation des servos 0-15:
mw.w 12000040 FFFF

Activation des servos 16-23:
mw.w 12000042 00FF

Desactivation des servos 0-15:
mw.w 12000040 0000

Desactivation des servos 16-23:
mw.w 12000042 0000

Au chargement du firmware, tous les servos sont en position mediane.
Positionnement du servo 0 a la position maximale anti-horaire:
mw.w 12000000 0000
Positionnement du servo 0 a la position maximale horaire:
mw.w 12000000 0FFF
Positionnement du servo 0 a la position mediane:
mw.w 12000000 0800

Voici les adresses a utiliser pour les autres servos:
0x12000000 -> servo 0
0x12000002 -> servo 1
0x12000004 -> servo 2
0x12000006 -> servo 3
0x12000008 -> servo 4
0x1200000A -> servo 5
0x1200000C -> servo 6
0x1200000E -> servo 7
0x12000010 -> servo 8
0x12000012 -> servo 9
0x12000014 -> servo 10
0x12000016 -> servo 11
0x12000018 -> servo 12
0x1200001A -> servo 13
0x1200001C -> servo 14
0x1200001E -> servo 15
0x12000020 -> servo 16
0x12000022 -> servo 17
0x12000024 -> servo 18
0x12000026 -> servo 19
0x12000028 -> servo 20
0x1200002A -> servo 21
0x1200002C -> servo 22
0x1200002E -> servo 23

5 - ROAD MAP
De futurs developpements sont en cours:
	- Driver Linux pour le firmware FPGA,
	- Daemon de commande des servos via le driver Linux,
	- Client reseau pour daemon de commande...
