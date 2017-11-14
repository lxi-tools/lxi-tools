IP=$1
SCPI="lxi scpi --address $IP"

$SCPI "*IDN?"
$SCPI ":autoscale"
$SCPI ":stop"
$SCPI ":run"
$SCPI ":clear"
$SCPI ":tforce"
$SCPI ":single"

$SCPI ":CHANnel1:DISPlay on"
$SCPI ":CHANnel2:DISPlay on"
$SCPI ":CHANnel3:DISPlay on"
$SCPI ":CHANnel4:DISPlay on"

$SCPI ":CHANnel1:DISPlay off"
$SCPI ":CHANnel2:DISPlay off"
$SCPI ":CHANnel3:DISPlay off"
$SCPI ":CHANnel4:DISPlay off"
