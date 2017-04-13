#!/bin/sh
CONEXION="./Plantillas/centralpub.conf"
EX_IFACE="wlan0"
IN_IFACE="eth0"
INTRANET="192.168.2.0/24"
GATEWAYIP="192.168.1.1"
ANYWHERE="any/0"

#Comprueba conexion y dns con un ping ...
ping -c 3 8.8.8.8
ping -c 3 8.8.8.8 & wait $!
if [ $? != 0 ]; then

killall wpa_supplicant
#airmon-ng check kill
ifconfig $EX_IFACE down
	
	macchanger -r  $EX_IFACE
        # 80:65:6D:14:92:A1
	iw reg set BO
	iwconfig $EX_IFACE txpower 30
	
ifconfig $EX_IFACE up
iwconfig $EX_IFACE power off
iwconfig $EX_IFACE rate  54M
ifconfig $EX_IFACE 192.168.1.29 netmask 255.255.255.0
ifconfig $IN_IFACE 192.168.2.1 netmask 255.255.255.0

. ./cortafuegos

# echo 1 > /proc/sys/net/ipv4/ip_forward 
# /etc/sysctl.conf, asegurándonos de que exista una línea como: net.ipv4.ip_forward=1
#MASQUERADE podría funcionar también si la dirección IP de eth0 fuese estática, pero recomienda utilizar SNAT.
#iptables -t nat -A POSTROUTING -s 192.168.3.0/24 -o eth0 -j SNAT --to 80.58.1.14
iptables -t nat -A POSTROUTING -o $EX_IFACE -j MASQUERADE
route add default gw $GATEWAYIP $EX_IFACE
# Con la linea anterior ya establecemos la ruta por defecto, pero en ocasiones me ha fallado ¿? 
# Es redundante para establecer la ruta por defecto ...
ip route replace default via $GATEWAYIP

iptables -t nat -A PREROUTING -i eth0 -p udp --dport 53 -j REDIRECT --to-ports 53

#iptables -t nat -A PREROUTING -i eth0 -p udp --dport 53 -j DNAT --to 192.168.1.1:53
#iptables -t nat -A OUTPUT -d 192.168.1.1 -p udp --dport 53 -j REDIRECT --to-ports 53

wpa_supplicant -B -i $EX_IFACE -c $CONEXION

# sleep 3
service dnscrypt-proxy restart
service tor restart
service dnsmasq restart

#ntpdate -v hora.roa.es
info

else
	echo "------------------------------ Ya estoy conectado ..........................."
        
       fi
# watch -n 5 info
# watch -n 2 cat /proc/net/wireless
info

