#!/bin/sh

cd ..
mkdir -p /usr/share/games
chmod 755 /usr/share/games
cp -rdf moon-lander /usr/share/games/
chmod 755 /usr/share/games/moon-lander/images
chmod 644 /usr/share/games/moon-lander/images/*
chmod 755 /usr/share/games/moon-lander/images/backgrounds
chmod 644 /usr/share/games/moon-lander/images/backgrounds/*
chmod 755 /usr/share/games/moon-lander/images/kablam
chmod 644 /usr/share/games/moon-lander/images/kablam/*
chmod 755 /usr/share/games/moon-lander/sounds
chmod 644 /usr/share/games/moon-lander/sounds/*
chmod 755 /usr/share/games/moon-lander/fonts
chmod 644 /usr/share/games/moon-lander/fonts/*
cp moon-lander/moon-lander /usr/bin/
