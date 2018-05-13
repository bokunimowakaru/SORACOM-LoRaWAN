1 cls:?"LoRaWAN for IchigoJam
100 'Init
110 uart 3:clt:led 0
120 ?"AT":wait 30
130 if inkey()<>asc("O") goto 130
140 if inkey()<>asc("K") goto 140
150 if tick()>300 end
160 ?"ATZ":wait 30
200 'Join
210 ?"AT+JOIN":wait 10
220 if inkey() goto 220
230 ?"AT+NJS=?":wait 10
240 I=inkey():if I=asc("1") goto 300
250 if !I wait 250:goto 220
260 if inkey()<>13 goto 260
270 goto 240
300 'Send
310 if inkey() goto 310
320 led 1
330 B=btn()
340 ?"AT+SEND=1:";B
350 led 0
360 wait 300
370 goto 300
