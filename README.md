# Choco

## Simple frontend for Chocolate Doom on Raspberry Pi

![Screenshot](screenshot.png)

### Quick start

```
git clone https://www.github.com/deshrike/choco
cd choco
make
```

Copy choco, choco.ini and play to the folder that has your WAD files (for example /home/pi/wad).

```
cp choco /home/pi/wad
cp choco.ini /home/pi/wad
cp play /home/pi/wad
```

Uses these WAD files:

- doom.wad
- doom2.wad
- heretic.wad
- hexen.wad
- STRIFE1.WAD
- VOICES.WAD
- plutonia.wad
- tnt.wad

You can disable certain games in choco.ini

Start the frontend:

```
cd /home/pi/wad
./play
```

