INTRODUCTION
------------
Ez az alkalmazas ket dobokockarol keszult kepen meghatarozza a felso lapok
pont erteket.
A program forraskodja elerheto a githubon: https://github.com/stampho/dice


REQUIREMENTS
------------
A program forditasahoz es inditasahoz szukseges, hogy az alabbi fuggveny
konyvtarak telepitve legyenek:
* Qt 5.3.2 (http://www.qt.io/download-open-source/)
* OpenCV 2.4.9 (http://opencv.org/downloads.html)


INSTALLATION
------------
Az alkalmazas elinditasahoz eloszor annak forrasat le kell forditani.
A forditas menetet a **dice.pro** project fileban lehet konfiguralni.

Ha az OpenCV nem rendszerszinten lett telepitve, akkor a **dice.pro** project
filet modositani kell! A *LIBS* es az *INCLUDEPATH* valtozok erteket az egyedileg
telepitett OpenCV konyvtarai szerint kell beallitani.
Peldaul:
```
LIBS += -L/usr/lib/ -lopencv_core -lopencv_highgui -lopencv_imgproc
INCLUDEPATH += -I/usr/include/opencv
```

Ha az OpenCV rendszerszinten telepitve lett (linux) akkor a fenti lepesre nincs
szukseg. A program telepitesere 2 fele lehetoseg van:

1. Inditsuk el a Qt Creator IDE-t a *qtcreator* paranccsal.
Ezutan nyissuk meg vele a **dice.pro** project
filet, majd a Run gomb megnyomasaval (Ctrl+r) forditjuk es futtatjuk a
programot.
2. A masik lehetoseg, hogy a project gyoker konyvtaraban qmake paranccsal
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

File                     |  Description
------------------------ | ----------------------------------------------------
dice.pro                 |  qmake project file
dicewindow.(cpp h ui)    |  GUI implementacio
imageexplorer.(cpp h)    |  kep bongeszes implementacioja
imageloader.(cpp h)      |  kep betoltes implementacioja
imagestack.(cpp h)       |  detektalas lepeseit megvalosito kod, minden
                         |  lepes kepe tarolva van (stack)
README.md                |  ez a file
API/core.h               |  altalanosan hasznalt tipusok es makrok definicioi
API/cube.(cpp h)         |  kockan vegrehajthato muveletek kodja
API/dimage.(cpp h)       |  sajat kep tipus (Qt-ban nincs hasznalva)
API/face.(cpp h)         |  kocka oldalain vegrehajthato muveletek kodja
examples/cvdemo/main.cpp |  opencv GUI-t hasznalo demo kodja (deprecated)
examples/cvdemo/Makefile |  cvdemo project file
samples/*.(jpg png)      |  minta kepek

