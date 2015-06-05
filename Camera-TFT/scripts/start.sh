#!/bin/bash

sudo /etc/init.d/fbcp start
sudo /etc/init.d/servoctrl start
#screen -d -m -S camctrl sudo python /usr/local/bin/camctrl.py
screen -d -m -S camctrl python /usr/local/bin/camctrl.py

exit 1
