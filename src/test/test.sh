IP=10.42.0.42

#lxi scpi --address $IP "display:data?"

lxi scpi --address $IP ":autoscale"
lxi scpi --address $IP ":stop"
lxi scpi --address $IP ":run"
lxi scpi --address $IP ":clear"
lxi scpi --address $IP ":tforce"
lxi scpi --address $IP ":single"

lxi scpi --address $IP ":CHANnel1:DISPlay on"
lxi scpi --address $IP ":CHANnel2:DISPlay on"
lxi scpi --address $IP ":CHANnel3:DISPlay on"
lxi scpi --address $IP ":CHANnel4:DISPlay on"

lxi scpi --address $IP ":CHANnel1:DISPlay off"
lxi scpi --address $IP ":CHANnel2:DISPlay off"
lxi scpi --address $IP ":CHANnel3:DISPlay off"
lxi scpi --address $IP ":CHANnel4:DISPlay off"
