# ncorps

Simulation gravitationnelle à N corps en C++20, parallélisée sur CPU avec OpenMP (threads et vectorisation SIMD).

Le programme calcule, à chaque pas de temps, l'interaction gravitationnelle de chaque corps avec tous les autres (méthode directe « toutes paires », en O(N²)), puis fait avancer positions et vitesses avec un schéma d'intégration au choix. Il compare les temps d'exécution d'une version mono-thread et d'une version multi-thread, ainsi que la dérive de l'énergie cinétique.

## Sommaire

- [Modèle physique](#modèle-physique)
- [Architecture du code](#architecture-du-code)
- [Parallélisation](#parallélisation)
- [Compilation](#compilation)
- [Exécution](#exécution)
- [Ajouter un modèle ou un intégrateur](#ajouter-un-modèle-ou-un-intégrateur)
- [Limites connues](#limites-connues)

## Modèle physique

### Force gravitationnelle

L'accélération subie par le corps *i* est la somme des contributions de tous les corps *j* :

```
aᵢ = Σⱼ G · mⱼ · (rⱼ − rᵢ) / (|rⱼ − rᵢ|² + ε²)^(3/2)
```

avec `G = 6.674e-11`.

Le terme ε² est un **adoucissement de Plummer** (*softening*), fixé à `ε² = 1e-9`. Il a deux rôles :

- **supprimer le cas particulier i = j** : la distance à soi-même vaut ε au lieu de 0, le coefficient reste fini et la contribution vaut exactement zéro (elle est multipliée par un déplacement nul). La boucle interne n'a donc pas de branchement, ce qui facilite sa vectorisation ;
- **éviter la divergence** de la force lorsque deux corps sont très proches.

Pour des distances grandes devant ε, on retrouve la force de Newton : l'erreur relative est de l'ordre de ε²/r², négligeable à l'échelle des positions initiales (entre 1 et 1000).

> Le modèle de forces ne multiplie pas par la masse du corps *i* : les valeurs stockées dans `Forces` sont en réalité des **accélérations**.

### Intégration en temps

Deux schémas sont disponibles, avec un pas de temps fixe `dt` :

| Schéma | Classe | Mise à jour |
|---|---|---|
| Euler explicite | `EulerExplicit` | position avec l'ancienne vitesse, puis vitesse |
| Euler semi-implicite (symplectique) | `EulerSemiImplicit` | vitesse, puis position avec la nouvelle vitesse |

Le schéma semi-implicite conserve beaucoup mieux l'énergie sur le long terme, pour un coût identique.

### Conditions initiales

Les positions sont tirées uniformément dans le cube [1, 1000]³, les vitesses sont nulles et toutes les masses valent 1.

## Architecture du code

```
src/
├── main.cpp                   # boucle de simulation, benchmarks, affichage
├── api/
│   └── concepts.hpp           # concepts C++20 : ForceModelC, TimeIntegrator
├── datatype/
│   ├── bodies.hpp             # état des corps (positions, vitesses, masses)
│   └── forces.hpp             # accélérations calculées à chaque pas
└── impl/
    ├── force_model.hpp        # ForceModel : gravité avec adoucissement
    └── time_integration.hpp   # EulerExplicit, EulerSemiImplicit
```

### Disposition des données

Les données sont organisées en **structure de tableaux** (*Structure of Arrays*, SoA) : un `std::vector<double>` par composante (`m_rx`, `m_ry`, `m_rz`, `m_vx`…), plutôt qu'un tableau de structures `{x, y, z, vx, …}`.

Des corps consécutifs ont ainsi des composantes contiguës en mémoire, ce qui permet au compilateur de charger plusieurs corps dans un même registre vectoriel.

### Découpage en noyaux

Chaque composant expose une fonction statique `step_all` qui traite **tous les corps en une fois** :

```cpp
ForceModel::step_all(const Bodies& b, Forces& f);             // calcule toutes les accélérations
EulerSemiImplicit::step_all(Bodies& b, const Forces& f, dt);  // avance tous les corps d'un pas
```

La boucle de simulation se réduit alors à :

```cpp
for (int t = 0; t < nb_iter; t++) {   // séquentielle : le pas t+1 dépend du pas t
    FM::step_all(b, f);
    TI::step_all(b, f, dt);
}
```

Le modèle de forces et l'intégrateur sont des paramètres de template, contraints par les concepts `ForceModelC` et `TimeIntegrator` (voir `api/concepts.hpp`).

## Parallélisation

La boucle en temps reste séquentielle. Le parallélisme est exprimé **à l'intérieur de chaque noyau**, à deux niveaux :

| Niveau | Où | Directive |
|---|---|---|
| Threads | boucle sur les corps *i* du modèle de forces | `#pragma omp parallel for` |
| SIMD | boucle interne sur les corps *j* (réduction) | `#pragma omp simd reduction(+ : ax, ay, az)` |
| Threads et SIMD | boucle des intégrateurs | `#pragma omp parallel for simd` |

Quelques points de conception :

- **Réduction explicite** : la boucle interne accumule les contributions dans `ax`, `ay`, `az`. Sans la clause `reduction`, le compilateur refuse de la vectoriser, car cela change l'ordre des additions flottantes. La clause l'autorise pour ces seules variables, sans recourir à `-ffast-math`.
- **`__restrict`** : les noyaux travaillent sur des pointeurs locaux marqués `__restrict`. Le compilateur n'a ainsi pas à envisager que deux tableaux se chevauchent, ce qui supprime les tests d'aliasing à l'exécution.
- **Même code en séquentiel et en parallèle** : le nombre de threads est fixé de l'extérieur (`omp_set_num_threads` dans `main.cpp`, ou la variable `OMP_NUM_THREADS`). La version « séquentielle » est mono-thread mais reste vectorisée.
- **Résultats indépendants du nombre de threads** : avec `schedule(static)`, chaque corps *i* est calculé intégralement par un seul thread, avec la même séquence d'opérations quel que soit le nombre de threads.

## Compilation

### Prérequis

- CMake ≥ 3.23
- un compilateur C++20 (GCC ou Clang récent)
- OpenMP

### Construction

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Le type de build `Release` active `-O3`.

### Cibler le processeur

Sans option `-march`, le compilateur produit du code pour le x86-64 de base (SSE2 uniquement) : pas d'AVX ni de FMA. Pour exploiter le processeur de la machine :

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-march=native"
```

Un binaire compilé avec `-march=native` peut ne pas fonctionner sur une autre machine (erreur `Illegal instruction`).

### Vérifier la vectorisation

Avec GCC, le rapport de vectorisation indique quelles boucles ont été vectorisées :

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-march=native -fopt-info-vec-optimized"
cmake --build build
```

Les deux noyaux doivent apparaître avec la mention `loop vectorized`.

## Exécution

```bash
./build/nbody
```

Le nombre de threads de la version parallèle se règle avec `OMP_NUM_THREADS` :

```bash
OMP_NUM_THREADS=8 ./build/nbody
```

Les paramètres de simulation sont des constantes en tête de `src/main.cpp` :

| Constante | Valeur | Rôle |
|---|---|---|
| `N` | 1000 | nombre de corps |
| `NB_ITER` | 100 | nombre de pas de temps |
| `DT` | 0.001 | pas de temps |

### Sortie

Le programme exécute trois simulations, chacune sur un jeu de corps tiré aléatoirement :

| Nom | Intégrateur | Threads |
|---|---|---|
| `SEQ/EX` | Euler explicite | 1 |
| `SEQ/ESI` | Euler semi-implicite | 1 |
| `OMP/ESI` | Euler semi-implicite | tous |

Pour chacune, il affiche le temps d'exécution et la variation d'énergie cinétique entre le début et la fin de la simulation.

## Ajouter un modèle ou un intégrateur

Il suffit d'écrire une classe qui satisfait le concept correspondant, puis de la passer en paramètre de `run`.

Un intégrateur doit fournir :

```cpp
static void step_all(Bodies& b, const Forces& f, double dt);
```

Un modèle de forces doit fournir :

```cpp
static void step_all(const Bodies& b, Forces& f);
```

Par exemple :

```cpp
run<ForceModel, MonIntegrateur>(bodies, DT, NB_ITER);
```

Si la classe ne respecte pas la signature attendue, l'erreur de compilation indique quel concept n'est pas satisfait et quelle expression est invalide.

## Limites connues

- **Mouvement quasi nul** : avec `G = 6.674e-11`, des masses unitaires et des distances de l'ordre de 500, les accélérations sont d'environ 10⁻¹³. Les corps ne bougent presque pas en 100 pas, et la variation d'énergie mesurée ne permet pas de valider les intégrateurs. Pour un test significatif, utiliser `G = 1` ou des masses plus grandes.
- **Énergie incomplète** : `get_system_energy` ne calcule que l'énergie cinétique, avec une masse fixée à 1 au lieu de `m_m[i]`. L'énergie conservée par le système inclut aussi l'énergie potentielle gravitationnelle (somme sur les paires de −G·mᵢ·mⱼ / √(r² + ε²)).
- **Conditions initiales non reproductibles** : la graine du générateur aléatoire provient de `std::random_device`, donc chaque exécution, et chaque simulation d'une même exécution, part de positions différentes. Une graine fixe est nécessaire pour comparer des variantes du code sur les mêmes données.
- **Précision numérique** : selon les options de compilation (présence ou non de FMA, compilateur utilisé), les résultats peuvent différer au dernier bit. Le système étant chaotique, ces écarts s'amplifient au fil des pas de temps.
- **Petite taille par défaut** : avec N = 1000, le coût de création des régions parallèles devient visible, en particulier pour l'intégrateur, qui fait peu de calcul par corps. La mise à l'échelle en nombre de threads est meilleure pour N plus grand.
