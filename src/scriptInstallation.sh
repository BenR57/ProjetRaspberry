#!/bin/bash

#Téléchargement des librairies python
pip install RPi.GPio
pip install Serial

#Mise en place du système GIT
git clone git@github.com:BenR57/ProjetRaspberry.git /home/pi/Applications/detectionEvents/ProjetRaspberry
git config --global user.name 'Team Raspberry'
git config --global user.email '<>'

#Mise en place du système de démarrage du système de détection
sudo cp /home/pi/Applications/fichierInstallation/detection.service /usr/lib/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable detection.service
sudo systemctl start detection.service

#Modification Crontab
cp /var/spool/cron/crontabs/pi .
echo "0 * * * * /bin/bash /home/pi/Applications/detectionEvents/gitManager.sh" >> pi
mv pi /var/spool/cron/crontabs/
sudo service cron reload



