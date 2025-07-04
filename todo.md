# 🚀 Feuille de Route du Projet Mots Croisés 🚀

Cette feuille de route détaille les objectifs, les priorités et les améliorations planifiées pour l'algorithme de résolution et de génération de mots croisés.

---

## I. 💡 Optimisation du Traitement des Mots

### **1. Gestion des Arbres de Mots (Tries)**

* **Priorité 1 : Affinage de la Logique de Sélection d'Arbre**
    * **Objectif :** Optimiser la fonction `findWordsByPattern()` en garantissant le **choix le plus efficace** entre l'arbre normal et l'arbre inversé pour chaque recherche.
    * **Justification :** Cette fonction est cruciale pour la performance globale de l'algorithme de résolution.
    * *Considération : Analyser les motifs de recherche (e.g., présence de caractères connus en début/fin de mot) pour diriger le choix de l'arbre.*

* **Priorité 2 : Outil d'Évaluation des Performances de Recherche**
    * **Objectif :** Développer un mécanisme pour **évaluer et comparer précisément** les performances des recherches en fonction du type d'arbre utilisé.
    * *Résultat attendu : Des métriques claires pour valider les gains d'optimisation.*

* **Priorité 2 : Vérification du Gain de Performance de l'Arbre Inversé**
    * **Objectif :** Confirmer **empiriquement** que l'utilisation de l'arbre inversé apporte un **bénéfice significatif** en performance dans les scénarios prévus.
    * *Méthode : Mener des benchmarks ciblés pour quantifier l'impact de cette optimisation.*

---

## II. 🧩 Amélioration de l'Algorithme de Génération de Grilles

### **1. Forward Checking et Vérification des Contraintes**

* **Priorité 3 : Optimisation des Cellules (`QChar` et `undo()` )**
    * **Objectif :** Surcharger l'opérateur `=` de la cellule pour ne copier que le `QChar` et implémenter une méthode `undo()` pour éviter les copies coûteuses de grilles entières lors du backtracking.
    * *Impact : Réduction significative de l'overhead lié aux opérations de copie/restauration d'état.*

* **Priorité 1 : Évaluation du "Forward Checking"**
    * **Objectif :** Examiner le comportement actuel du "forward checking" et déterminer la nécessité de modifications additionnelles (par exemple, évaluer la viabilité globale de la grille ou le nombre de mots possibles restants pour les cases non encore remplies).
    * **Décision :** Si l'approche actuelle est jugée suffisante et ne présente pas de goulots d'étranglement, aucune modification ne sera apportée.

### **2. Heuristiques de Sélection de Mots**

* **Priorité 2 : Développement de Nouvelles Heuristiques**
    * **Objectif :** Créer des heuristiques avancées pour la sélection des mots, visant un équilibre entre :
        * Le choix du mot qui réduit le plus de possibilités pour les variables restantes (**"Least Remaining Values"**).
        * Le choix du mot qui a le plus d'intersections non résolues (**"Most Constraining Variable"**), afin de maximiser les contraintes tôt dans l'algorithme.

* **Priorité 2 : Système d'Évaluation des Heuristiques**
    * **Objectif :** Mettre en place un cadre pour évaluer comparativement ces heuristiques afin de déterminer laquelle offre les **meilleurs résultats** en termes de temps de génération et de qualité des grilles produites.

### **3. Algorithmes de Résolution**

* **Priorité 2 : Exploration d'Algorithmes Alternatifs**
    * **Objectif :** Rechercher et implémenter des approches alternatives ou combinées pour la résolution et la génération de mots croisés.
    * *Exemples à tester :*
        * **Algorithme DLX (Dancing Links X)** pour la résolution exacte (e.g., problème de couverture exacte).
        * **Arc Consistency** (AC-3, AC-4, etc.) pour la propagation de contraintes.
        * **Least Constraining Value (LCV)** : Choisir la valeur qui élimine le moins d'options pour les variables restantes.
        * **Backjumping / Conflict-Directed Backjumping** pour optimiser le retour arrière lors de l'échec.
        * Combinaison de **recuit simulé et de backtracking**.

* **Priorité 2 : Optimisation des Possibilités de Mots**
    * **Objectif :** Limiter le nombre d'essais pour les mots possibles dans la fonction `possibleWord`.
    * **Objectif :** Limiter le nombre de mots récupérés dans `fillAllWordToFind()`.
    * *Impact : Réduire l'espace de recherche et accélérer la convergence de l'algorithme.*

### **4. Validation des Positions des Cellules**

* **Priorité 2 : Ajustement de `isCrosswordCellPosValid()`**
    * **Objectif :** Modifier la fonction `isCrosswordCellPosValid()` pour permettre une distance de 0 entre certaines cellules.
    * **Cas d'usage :** Utile pour la coupure de mots aux coordonnées `x=1` et `y=1` (par exemple, pour des mots en coin ou des grilles non standards).

---

## III. 🖥️ Interface Utilisateur (IHM)

### **1. IHM de Génération de Grilles**

* **Priorité 3 : Création de l'Interface Dédiée**
    * **Objectif :** Développer une interface utilisateur intuitive spécifiquement pour le processus de génération des grilles.

* **Priorité 3 : Intégration des Fonctionnalités de Contrôle**
    * **Objectif :** Ajouter des fonctionnalités permettant aux utilisateurs de lancer, d'arrêter et de suivre la progression en temps réel de la génération des grilles.

* **Priorité 3 : Options de Configuration de Génération**
    * **Objectif :** Inclure des options pour configurer les paramètres de génération, tels que la taille de la grille, le temps alloué, l'algorithme et l'heuristique à utiliser.

### **2. IHM de Jeu**

* **Priorité 2 : Développement de l'Interface de Jeu**
    * **Objectif :** Concevoir et implémenter l'interface utilisateur qui permettra aux utilisateurs de jouer aux mots croisés.

* **Priorité 1 : Implémentation des Fonctionnalités de Jeu de Base**
    * **Objectif :** Mettre en œuvre les interactions fondamentales avec la grille de jeu (saisie de lettres, vérification de mots, etc.).

* **Priorité 2 : Intégration du Chargement de Grilles JSON**
    * **Objectif :** Intégrer une fonction `fromJson()` pour charger des grilles à partir de fichiers JSON et permettre à l'utilisateur de sélectionner ces fichiers via l'interface.

---

## IV. 🛠️ Autres Améliorations

### **1. Architecture Générale**

* **Priorité 4 : Refactoring de `GeneratedGridData`**
    * **Objectif :** Refactoriser la classe `GeneratedGridData` pour éliminer les données non utilisées et simplifier sa structure.

### **2. Gestion des Workers**

* **Priorité 1 : Signal d'Arrêt des Workers**
    * **Objectif :** Modifier la fonction `stopAllActiveWorkers()` dans `GridGenerator` pour qu'elle envoie un signal (un "flag") aux workers actifs, leur permettant d'arrêter proprement leur algorithme en cours.
    * **Mise en œuvre :** Adapter les algorithmes de résolution pour qu'ils surveillent et respectent ce flag d'arrêt.

### **3. Sauvegarde des Grilles**

* **Priorité 3 : Système de Sauvegarde Structuré**
    * **Objectif :** Mettre en place un système robuste pour sauvegarder les grilles générées, en les organisant de manière logique dans des dossiers structurés (par exemple : `grids/8x8/threadId_gridIndex.json`).

### **4. Outils et Maintenance**

* **Priorité 2 : Audits Réguliers de Performance et Fuites Mémoire**
    * **Objectif :** Effectuer des vérifications régulières des performances avec des outils tels que `perf record` ou `callgrind`, et rechercher activement les fuites de mémoire avec `memcheck`.

### **5. Dictionnaire**

* **Priorité 2 : Amélioration du Dictionnaire**
    * **Objectif :** Rechercher ou créer un dictionnaire de meilleure qualité pour les mots croisés.
    * *Pistes :*
        * Explorer des options de dictionnaires payants.
        * Utiliser des APIs externes (comme Gemini, si applicable et pertinent) pour obtenir des définitions et des indices.
        * Filtrer les mots peu courants en fonction de leur fréquence d'apparition dans de grands corpus de textes.

* **Priorité 2 : Adaptation des Scripts Python**
    * **Objectif :** Adapter les scripts Python existants pour l'analyse des mots, des définitions et l'association des indices avec le nouveau dictionnaire.

### **6. Tests et Documentation**

* **Priorité 1 : Implémentation de Tests Unitaires**
    * **Objectif :** Mettre en place des tests unitaires robustes pour les composants clés du projet.

* **Priorité 1 : Commentaire de Code par IA**
    * **Objectif :** Utiliser Gemini pour générer des commentaires de code explicatifs, améliorant ainsi la compréhension et la maintenabilité du codebase.