IP=10.42.0.42

#lxi scpi --ip $IP "display:data?"

lxi scpi --ip $IP ":autoscale"
lxi scpi --ip $IP ":stop"
lxi scpi --ip $IP ":run"
lxi scpi --ip $IP ":clear"
lxi scpi --ip $IP ":tforce"
lxi scpi --ip $IP ":single"

lxi scpi --ip $IP ":CHANnel1:DISPlay on"
lxi scpi --ip $IP ":CHANnel2:DISPlay on"
lxi scpi --ip $IP ":CHANnel3:DISPlay on"
lxi scpi --ip $IP ":CHANnel4:DISPlay on"

lxi scpi --ip $IP ":CHANnel1:DISPlay off"
lxi scpi --ip $IP ":CHANnel2:DISPlay off"
lxi scpi --ip $IP ":CHANnel3:DISPlay off"
lxi scpi --ip $IP ":CHANnel4:DISPlay off"
