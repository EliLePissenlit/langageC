# Projet Tic-Tac-Toe en C avec SDL2

## Architecture du Projet

Le projet est structuré en plusieurs modules distincts, chacun ayant une responsabilité spécifique:

### 1. Structure des Fichiers

```
tomarts01-c_propre_copie/
├── README.md           - Documentation du projet
├── CMakeLists.txt     - Configuration de compilation
├── ai.c/h             - Intelligence artificielle
├── game.h             - Structures et constantes principales
├── logic.c/h          - Logique du jeu
├── main.c             - Point d'entrée et boucle principale
├── menu.c/h           - Gestion des menus
└── rendering.c/h      - Rendu graphique
```

### 2. Description des Modules

#### a. Module Principal (game.h)
- Définit les constantes globales du jeu (taille de la grille, dimensions de l'écran)
- Contient la structure principale `game_t` qui maintient l'état du jeu
- Définit les états possibles du jeu (en cours, victoire, égalité)

#### b. Module Logique (logic.c/h)
- Gère les règles du jeu et la mécanique de base
- Vérifie les conditions de victoire
- Gère les tours des joueurs
- Maintient l'état du plateau de jeu

#### c. Module IA (ai.c/h)
- Implémente trois niveaux de difficulté:
  - Facile: Coups aléatoires
  - Moyen: Combine stratégie simple et aléatoire
  - Difficile: Utilise l'algorithme Minimax
- Gère la prise de décision de l'ordinateur

#### d. Module Menu (menu.c/h)
- Gère l'interface utilisateur des menus
- Permet la sélection du mode de jeu (JcJ ou JcIA)
- Gère la sélection de la difficulté de l'IA
- Implémente le menu de fin de partie

#### e. Module Rendu (rendering.c/h)
- Gère tout l'affichage graphique avec SDL2
- Dessine la grille, les X et les O
- Affiche les menus et les écrans de victoire
- Gère les animations et les transitions

### 3. Flux de Jeu

1. **Démarrage**
   - Initialisation de SDL2 et des composants
   - Création de la fenêtre et du renderer
   - Chargement des polices

2. **Menu Principal**
   - Choix entre Joueur vs Joueur ou Joueur vs IA
   - Si IA sélectionnée, choix de la difficulté

3. **Partie**
   - Alternance des tours entre les joueurs
   - Vérification des conditions de victoire après chaque coup
   - En mode IA, calcul et exécution automatique des coups de l'ordinateur

4. **Fin de Partie**
   - Affichage de l'état final pendant 2 secondes
   - Transition vers le menu de victoire
   - Options pour rejouer ou retourner au menu principal

### 4. Fonctionnalités Principales

#### Intelligence Artificielle
- **Mode Facile**: Place les pions aléatoirement
- **Mode Moyen**: 
  1. Cherche d'abord une victoire immédiate
  2. Bloque les victoires imminentes de l'adversaire
  3. Joue aléatoirement si aucune stratégie n'est applicable
- **Mode Difficile**:
  - Utilise l'algorithme Minimax pour des coups optimaux
  - Évalue toutes les possibilités jusqu'à la fin de partie
  - Choix du meilleur coup possible

#### Interface Graphique
- Menus intuitifs avec boutons
- Animation de transition pour la fin de partie
- Affichage clair de l'état du jeu
- Design épuré et responsive

### 5. Compilation et Exécution

```bash
# Création du dossier de build
mkdir build
cd build

# Configuration avec CMake
cmake ..

# Compilation
make

# Exécution
./procedural
```

### 6. Dépendances

- SDL2
- SDL2_gfx
- SDL2_ttf

### 7. Points Techniques Notables

1. **Gestion de la Mémoire**
   - Utilisation appropriée des constructeurs/destructeurs
   - Libération systématique des ressources SDL
   - Pas de fuites mémoire

2. **Architecture Modulaire**
   - Séparation claire des responsabilités
   - Interfaces bien définies entre les modules
   - Code facilement extensible

3. **Robustesse**
   - Gestion des erreurs à chaque étape
   - Vérifications des valeurs de retour
   - Messages de debug détaillés

### 8. Extensions Possibles

1. **Améliorations Gameplay**
   - Ajout d'un système de score
   - Mode tournoi
   - Replay des parties

2. **Améliorations Techniques**
   - Sauvegarde/chargement de parties
   - Configuration personnalisable
   - Support réseau pour jeu en ligne

3. **Améliorations IA**
   - Ajout d'algorithmes d'apprentissage
   - Personnalités d'IA différentes
   - Niveaux de difficulté supplémentaires