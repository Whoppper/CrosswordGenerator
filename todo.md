# 🚀 Feuille de Route du Projet Mots Croisés 🚀

Cette feuille de route détaille les objectifs, les priorités et les améliorations planifiées pour l'algorithme de résolution et de génération de mots croisés, ainsi que l'interface utilisateur et la maintenance du projet.

---

## I. 💡 Optimisation du Traitement des Mots

### 1. Gestion des Arbres de Mots (Tries)

* **P1 : Affinage de la Logique de Sélection d'Arbre**
    * **Objectif :** Optimiser la fonction `findWordsByPattern()` en garantissant le choix le plus efficace entre l'arbre normal et l'arbre inversé pour chaque recherche.
    * **Justification :** Cette fonction est cruciale pour la performance globale de l'algorithme de résolution.
    * **Considération :** Analyser les motifs de recherche (ex: présence de caractères connus en début/fin de mot) pour diriger le choix de l'arbre.
* **P1 : Affinage des fonctions de l'Arbre**
    * **Objectif :** Limiter les appels à `findAnyWord()` et aux fonctions d'arbre fréquemment utilisées.
    * **Justification :** Améliorer les performances globales.
    * **Exemple :** le pattern contient "_____a_", un mot possible existe probablement sans avoir besoin de verifier.
* **P2 : Outil d'Évaluation des Performances de Recherche**
    * **Objectif :** Développer un mécanisme pour évaluer et comparer précisément les performances des recherches en fonction du type d'arbre utilisé.
    * **Résultat attendu :** Des métriques claires pour valider les gains d'optimisation.
* **P2 : Vérification du Gain de Performance de l'Arbre Inversé**
    * **Objectif :** Confirmer empiriquement que l'utilisation de l'arbre inversé apporte un bénéfice significatif en performance dans les scénarios prévus.
    * **Méthode :** Mener des benchmarks ciblés pour quantifier l'impact de cette optimisation.

---

## II. 🧩 Amélioration de l'Algorithme de Génération de Grilles

### 1. Forward Checking et Vérification des Contraintes

* **P1 : Évaluation du "Forward Checking"**
    * **Objectif :** Examiner le comportement actuel du "forward checking" et déterminer la nécessité de modifications additionnelles (par exemple, évaluer la viabilité globale de la grille ou le nombre de mots possibles restants pour les cases non encore remplies).
    * **Décision :** Si l'approche actuelle est jugée suffisante, aucune modification ne sera apportée.
* **P3 : Optimisation des Cellules (`QChar` et `undo()`)**
    * **Objectif :** Surcharger l'opérateur `=` de la cellule pour ne copier que le `QChar` et implémenter une méthode `undo()` pour éviter les copies coûteuses de grilles entières lors du backtracking.
    * **Impact :** Réduction significative de la surcharge liée aux opérations de copie/restauration d'état.

### 2. Heuristiques de Sélection de Mots

* **P2 : Développement de Nouvelles Heuristiques**
    * **Objectif :** Créer des heuristiques avancées pour la sélection des mots, visant un équilibre entre :
        * Le choix du mot qui réduit le plus de possibilités pour les variables restantes (**"Least Remaining Values"**).
        * Le choix du mot qui a le plus d'intersections non résolues (**"Most Constraining Variable"**), afin de maximiser les contraintes tôt dans l'algorithme.
* **P2 : Système d'Évaluation des Heuristiques**
    * **Objectif :** Mettre en place un cadre pour évaluer comparativement ces heuristiques afin de déterminer laquelle offre les meilleurs résultats en termes de temps de génération et de qualité des grilles produites.

### 3. Algorithmes de Résolution

* **P2 : Exploration d'Algorithmes Alternatifs**
    * **Objectif :** Rechercher et implémenter des approches alternatives ou combinées pour la résolution et la génération de mots croisés.
    * **Exemples à tester :** **Algorithme DLX (Dancing Links X)** pour la résolution exacte (par exemple, problème de couverture exacte), **Arc Consistency** (AC-3, AC-4, etc.) pour la propagation de contraintes, **Backjumping / Conflict-Directed Backjumping** pour optimiser le retour arrière lors de l'échec, Combinaison de **recuit simulé et de backtracking**.
* **P2 : Optimisation des Possibilités de Mots**
    * **Objectif :** Limiter le nombre d'essais pour les mots possibles dans la fonction `possibleWord`.
    * **Objectif :** Limiter le nombre de mots récupérés dans `fillAllWordToFind()`.
    * **Impact :** Réduire l'espace de recherche et accélérer la convergence de l'algorithme.

### 4. Validation des Positions des Cellules

* **P2 : Ajustement de `isCrosswordCellPosValid()`**
    * **Objectif :** Modifier la fonction `isCrosswordCellPosValid()` pour permettre une distance de 0 entre certaines cellules.
    * **Cas d'usage :** Utile pour la coupure de mots aux coordonnées `x=1` et `y=1`.

---

## III. 🖥️ Interface Utilisateur (IHM)

### 1. IHM Principale et Configuration

* **P3 : Ajouter une Fonctionnalité de Génération de Base de Données dans l'IHM Principale**
    * **Objectif :** Intégrer une option dans l'interface principale permettant de générer la base de données (choix du fichier d'entrée, du fichier de sortie, et du script de parsing à utiliser).
    * **Action :** Supprimer le chemin de la base de données (`dbpath`) du fichier de configuration.

### 2. IHM de Jeu

* **P1 : Implémentation des Fonctionnalités de Jeu de Base**
    * **Objectif :** Mettre en œuvre les interactions fondamentales avec la grille de jeu (saisie de lettres, vérification de mots, etc.).
* **P2 : Développement de l'Interface de Jeu**
    * **Objectif :** Concevoir et implémenter l'interface utilisateur qui permettra aux utilisateurs de jouer aux mots croisés.
* **P2 : Intégration du Chargement de Grilles JSON**
    * **Objectif :** Intégrer une fonction `fromJson()` pour charger des grilles à partir de fichiers JSON et permettre à l'utilisateur de sélectionner ces fichiers via l'interface.

---

## IV. 🛠️ Autres Améliorations

### 1. Outils et Maintenance

* **P1 : Gestion des Plantages lors de la Suppression de la DB**
    * **Objectif :** Gérer les plantages lorsque la base de données est supprimée en cours d'analyse.
* **P2 : Audits Réguliers de Performance et Fuites Mémoire**
    * **Objectif :** Effectuer des vérifications régulières des performances avec des outils tels que `perf record` ou `callgrind`, et rechercher activement les fuites de mémoire avec `memcheck`.

### 2. Dictionnaire

* **P2 : Amélioration du Dictionnaire**
    * **Objectif :** Rechercher ou créer un dictionnaire de meilleure qualité pour les mots croisés.
    * **Pistes :** Explorer des options de dictionnaires payants, utiliser des APIs externes (comme Gemini, si applicable et pertinent) pour obtenir des définitions et des indices, filtrer les mots peu courants en fonction de leur fréquence d'apparition.
* **P2 : Adaptation des Scripts Python**
    * **Objectif :** Adapter les scripts Python existants pour la génération du dictionnaire avec les indices et les definitions en plus.

### 3. Tests et Documentation

* **P1 : Implémentation de Tests Unitaires**
    * **Objectif :** Mettre en place des tests unitaires robustes pour les composants clés du projet.
* **P1 : Commentaire de Code par IA**
    * **Objectif :** Utiliser Gemini pour générer des commentaires de code explicatifs, améliorant ainsi la compréhension et la maintenabilité du codebase.

### 4. Codebase

* **P1 : Renommage et Refactorisation**
    * **Objectif :** Renommer les fichiers selon le format **CamelCase** et trouver de meilleurs noms pour certaines variables afin d'améliorer la lisibilité du code.
* **P1 : Ajout de la Traduction**
    * **Objectif :** Intégrer des fonctionnalités de traduction pour l'interface utilisateur.

* **P1 : Refonte du Fichier `style.css`**
    * **Objectif :** Améliorer l'esthétique de l'application en créant un fichier `style.css` plus moderne et agréable visuellement.
    * **Pistes :** Explorer des palettes de couleurs harmonieuses, des typographies lisibles et des éléments d'interface cohérents pour une meilleure expérience utilisateur.

    ya des fonctions dans les hpp les mettre dans le cpp
    changer les tojson et fromjson, sont pas ouf
    mettre la grille dans le json, si je rajoute des sauvegardes de grille en cours
    virer les friend de crossword manager