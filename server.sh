#!/bin/bash
App="/home/wumh/myProject/brks/src/brks"
 
echo $1
echo $App
 
function killProsess() {
	NAME=$1
	echo $NAME
        
        // 获取进程 PID
	PID=$(ps -e | grep $NAME | awk '{print $1}') 
	
	echo "PID: $PID"
        // 杀死进程
	kill -9 $PID
}
 
function start() {
	echo "start"
	echo "----------------"
	$App
}
 
function stop() {
	echo "stop"
	echo "----------------"
	killProsess "brks"
}
 
function restart() {
	echo "restart"
	echo "----------------"
	stop
	start
}
 
case "$1" in
	start )
		start
		;;
	stop )
		stop
		;;
	restart )
		restart
		;;
	* )
		echo "****************"
		echo "no command"
		echo "****************"
		;;
esac