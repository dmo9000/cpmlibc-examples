� "n|� �'}�"��
}�1��
�	�  � �	� ��* ~���C�~���C��	�  � ��~ʹ��#�����q��~~��ʹÎ�^#V��͖Î�#~+�ʹ�~#�ʹ���#�������|Ϳ�}Ϳ�|�ʹ�}ô����Ϳ������O !!	~������������h���*p#~T]��"p!!��#����1	� 
M� :N����y:n�"�2:n�1�O�:/O�! �/}��"lA ���:/2!��">	22� }��"lQ ���� "j0 ���P	� :/�A2�m	� o	� �
� :�� ���¢:��1ڢ�:Ң�0����!�"p:n�1���2?2@!0�P!'"p!0��\�!w"p*0�o:2�l:3�l:4�l*5�o*7�o:9�l::�l*;�o*=�o:n�1�X:?�l:@�lɯ�l��l��/>*�& o  �w ����/��ͯ*p�>0!�͔�+Ì͢�~�0�w����!��
�#���!�s#r#q#p�!�
�w#��x���y���z���{�����!����!����!����!��������#�����կ�#��33y�~�������G�,> �G�5>0�"p�*p#~T]��"p!���#����!d"p:2!� ��v)=�q|�:3���ʉ	=	�G|�x��a��l:3�G*5��=?�����G:4���a��l�*5 	� O:2��G�y��)� �O�u�*7 	� O�u*7|�g}o|�g}oT]*;|��|��� �R���a�* �?�u�*��/>+:n�1�^:?!� ��A)=�<�:@����Q	=�L	|����a��o�!� �o�!�"p*9�)� �2��l:2!� �ʋ)=|2�O :�!  �ʣ	=��o� �<���u���?�u�!�"p*5:�=O �B��o�GO:3<=�����R�=�����w���L���w���_�w�!]"p� *5	�O���o��:3<=��.��R�=�%���w���L�w��B�B}�o|�gy�O��R�R}�o|�gy�Ox�G�x�yG|O}g> o�:/�A2����a�eͻ��	� ��!2"p!Q�P�!�"p��P!2"p!Q��\�!�"p��\�!�"p!A�P!"p!A��\��:n�1�:n�"��:/�A2�͖�	� ��:/�A2���	� �� �A�A�AyO�|�g}oyO���!z 	~���	G~������!z 	~���	G~O�wy���ɀ@ �*u#"u���*}#"}��¦�*y#"y���*{#"{���*#"��:/�A2�!qT] 6 ��*5�0#"q��* +��R���	� �*qDM!�T]6 ��*9"�  )���"s\ >?� ��ʪo& )))))� ~�� �*5|��p���i��W�fo& �0�a͂�iͰ��Ý��^#V{�ʕ*5��Rڒ��0�a͂ÕͰ�##�Ý\ �� �$� � 2� � \ >?� ���`	o& )))))� ~���R	~���R	� ~����#���`	 �*5|��(	���!	��	�	o& �0�a͙�!	Ͱ���R	��^#V{��M	*5��R�J	��0�a͙�M	Ͱ�##�\ >�� ��:�_ � *qDM*5����}	+��x��t	#"w!'"p�*5 �  �u!S"p*s�o!}"p*u�o!�"p*y�o!Q"p*{�o!{"p*}�o!�"p*w�o!%"p*s�O�*u��*y��*w�O�u!�"p*�o�	� �:/�A2��	� :/�A2�	� :/2k&	� l	� �
� :�����u
��N
:��:�N
:��aڄ
�{�N
���A��N
2/� :/_� �¤
��:k2/_� ����	� ��:\ =����
� 2/� :/_� ���s�1����
�{� � ͊͒͢������
O !		~#fo�p�� 
0
...@
�                                                                                                                                     FATAL ERROR - Unable to select drive$MUST USE CP/M 2.2 OR CP/M 3.1$FATAL ERROR - Unable to find BIOS$        0123456789ABCDEF
Hit <RETURN> to continue $  
























$MML:DISKSTAT         DRIVE A: CHARACTERISTICS 










                   1    Display DPB statistics 
                   2    Display DPH statistics 
                   3    Display disk ALLOCATION 


                   9    Select new disk 



$     Enter your choice, or <RETURN>   $   			 6e� 
��ig�����`y���������������������        0123456789+*                           DRIVE A: DISC PARAMETER BLOCK

            0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16 
DPB:       00h 00h 00h 00h 00h 00h 00h 00h 00h 00h 00h 00h 00h 00h 00h 00h 00h

              SPT  BSH BLM EXM    DSM     DRM  AL0-AL1    CKS     OFF  PSH PHM
DPB(HEX):    0000h 00h 00h 00h   0000h   0000h 00h 00h   0000h   0000h 00h 00h
DPB(DEC):   65535  255 255 255  65535   65535  255 255  65535   65535  255 255

          BLOCK       EXTENT      MAX DISK    DIRECTORY   CHECK SUM   SECTOR
          SIZE (K)    FOLDS       SIZE (K)    ENTRIES     ENTRIES     SIZE
DPB(DEC):  16K          15        1048576      65536       65536      32768
                                                                            

ALLOCATION OF DISK BLOCKS
                             Data       1K    128 byte  
                            Blocks    Blocks   Records    Capacity

              Directory      65535  16777215  16777215    16777215 Entries
              Data           65535  16777215  16777215  4294967296 Bytes
                          --------  --------  --------
                             65000  16777215  16777215
$                     DRIVE A: DISK PARAMETER HEADER

            0   1   2   3   4   5   6   7   8   9  10  11  
DPH(HEX):  00h 00h 00h 00h 00h 00h 00h 00h 00h 00h 00h 00h 

           12  13  14  15  16  17  18  19  20  21  22  23  24  
DPH(HEX):  00h 00h 00h 00h 00h 00h 00h 00h 00h 00h 00h 00h 00h 

           XLT     -0- -0- -0- -0- -0- -0- -0- -0- -0- MF  
DPH(HEX):  0000h   00h 00h 00h 00h 00h 00h 00h 00h 00h 00h 

           DPB     CSV     ALV     DIRBCB  DTABCB  HASH    HBANK
DPH(HEX):  0000h   0000h   0000h   0000h   0000h   0000h   00h 








$                     DRIVE A: DISK PARAMETER HEADER

            0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  
DPH(HEX):  00h 00h 00h 00h 00h 00h 00h 00h 00h 00h 00h 00h 00h 00h 00h 00h 

           XLT     -0- -0- -0- -0- -0- -0- DIRBCB  DPB     CSV     ALV     
DPH(HEX):  0000h   00h 00h 00h 00h 00h 00h 0000h   0000h   0000h   0000h   














$                 
TPA too small for allocation vector$                     DRIVE A: DISK ALLOCATION

                            DATA BLOCKS 

       TOTAL (DRM+1)              65536 

       Directory                     16 
       Data                       65535 
       Erased and recoverable     65535 
       Unused                     65535 
                                  ----- 
                                  65536 

       Erased and reused          65535 
       Duplicated data            65535 
       Blocks outside range       65535 






$                     DRIVE A: FILE ALLOCATION





$                     DRIVE A: DIRECTORY ALLOCATION





$



















                     SELECT NEW DISK





$ Enter drive name (A:, B:, etc ) ?    $    
--- Unable to select new disk ---




$