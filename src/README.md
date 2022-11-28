# Code source

## Dépendances
- cmake
- opencv
- make

## Instructions

Pour compiler le projet lancer les commandes suivantes

```
mkdir build
cd build
cmake ..
make -j
```

Il y aura alors 3 exécutables qui sont les outils d'obscuration développés `build/Blackhead/Blackhead`, `build/Blur/Blur` et `build/Pixel/Pixel`. Chacun de ces exécutables prends un argument qui doit être une image.

Pour la détection de visages, nous utilisons la base de données [LFW (Labeled faces in the wild)](https://vis-www.cs.umass.edu/lfw/) téléchargeable comme ceci:

```
wget https://vis-www.cs.umass.edu/lfw/lfw.tgz
tar -xzf lfw.tgz -C ../
```

Il y a aussi divers outils pour manipuler cette base de données (Annotate, Faceloc, GenDataSet,...)

Pour la génération d'un modèle de réseau siamois, le notebook `Classification/Classification.ipynb` est disponible ainsi qu'une version script.
