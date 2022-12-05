# Code source

## Dépendances
- cmake
- opencv
- make
- libeigen3-dev
- nlohmann-json3-dev

## Instructions

Pour compiler le projet lancer les commandes suivantes

```
mkdir build
cd build
cmake ..
make -j
```

Pour utiliser le modèle avec le système de GUI, il est nécessaire de le générer auparavent à l'aide du notebook, puis de le convertir à l'aide des scripts pythons téléchargés en dépendences pour placer le json à côté de l'exécutable.

Il y aura alors 3 exécutables qui sont les outils d'obscuration développés `build/Blackhead/Blackhead`, `build/Blur/Blur` et `build/Pixel/Pixel`. Chacun de ces exécutables prends un argument qui doit être une image.

Pour la détection de visages, nous utilisons la base de données [LFW (Labeled faces in the wild)](https://vis-www.cs.umass.edu/lfw/) téléchargeable comme ceci:

```
wget https://vis-www.cs.umass.edu/lfw/lfw.tgz
tar -xzf lfw.tgz -C ../
```

Il y a aussi divers outils pour manipuler cette base de données (Annotate, Faceloc, GenDataSet,...)

Pour la génération d'un modèle de réseau siamois, le notebook [`Classification/Classification.ipynb`](https://github.com/AurelienBesnier/Projet-HAI927/blob/main/src/Classification/Classification.ipynb) est disponible ainsi qu'une version script.
