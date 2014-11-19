INTRODUCTION
------------
Ez az alkalmazas ket dobokockarol keszult kepen meghatarozza a felso lapok
pont erteket.
A program forraskodja elerheto a githubon: https://github.com/stampho/dice


REQUIREMENTS
------------
A program elinditasahoz szukseges, hogy az alabbi fuggveny konyvtarak telepitve
legyenek:
* Qt 5.3.2 (http://www.qt.io/download-open-source/)
* OpenCV 2.4.9 (http://opencv.org/downloads.html)


INSTALLATION
------------
Az alkalmazas elinditasahoz eloszor annak forrasat le kell forditani.
A forditas menetet a **dice.pro** project fileban lehet konfiguralni.

Ha az OpenCV nem rendszerszinten lett telepitve, akkor a **dice.pro** project
filet modositani kell! A *LIBS* es az *INCLUDEPATH* valtozok erteket az egyedileg
telepitett OpenCV konyvtarai szerint kell beallitani!
Peldaul:
```
LIBS += -L/usr/lib/ -lopencv_core -lopencv_highgui -lopencv_imgproc
INCLUDEPATH += -I/usr/include/opencv
```

Ha az OpenCV rendszerszinten telepitve lett (linux) akkor a fenti lepesre nincs
szukseg. A program telepitesere 2 fele lehetoseg van:

* Az egyszerubbik, hogy ha a Qt-val telepitett Qt Creator IDE-t elinditjuk
a qtcreator paranccsal. Ezutan megnyitjuk vele a **dice.pro** project
filet, majd a Run gomb megnyomasaval (Ctrl+r) forditjuk es futtatjuk a
programot.
* A masik lehetoseg, hogy a project gyoker konyvtaraban qmake paranccsal
legeneraljuk a Makefile-t es make paranccsal buildeljuk:
```
$ qmake
$ make
```


EXECUTION
---------
A programot futtathatjuk vagy a fent emlitett modon Qt Creator-bol vagy pedig
terminalbol kiadott ./dice paranccsal.


FILE LIST
---------

-------------------------------------------------------------------------
dice.pro                 |  qmake project file
dicewindow.(cpp|h|ui)    |  GUI forras fajlok
imageexplorer.(cpp|h)    |  kep bongeszest megvalosito kod
imageloader.(cpp|h)      |  kep betoltes forrasa
imagestack.(cpp|h)       |  detektalas lepeseit megvalosito kod, minden lepes kepe tarolva van (stack)
README                   |  ez a file
API/core.h               |  altalanosan hasznlat tipusok es makrok
API/cube.(cpp|h)         |  kockan vegrehajthato muveletek kodja
API/dimage.(cpp|h)       |  sajat kep tipus, a Qt-ban nincs hasznalva
API/face.(cpp|h)         |  kocka oldalain vegrehajthato muveletke kodja
examples/cvdemo/main.cpp |  opencv GUI-t hasznalo demo kodja (deprecated)
examples/cvdemo/Makefile |  cvdemo project file
samples/*.(jpg|png)      |  minta kepek

