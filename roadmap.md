# Feuille de route GPU — ncorps

Principe : une version fonctionnelle d'abord, les performances ensuite. Aucune étape n'est validée sans comparaison à la référence de l'étape 0.

---

## Étape 0 — Rendre le projet vérifiable

- [x] Ajouter un paramètre `seed` au constructeur de `Bodies`
- [x] Lire N, le nombre de pas et G en ligne de commande
- [x] Passer à G = 1 par défaut
- [ ] Corriger l'énergie cinétique pour utiliser `m_m[i]` au lieu de 1
- [ ] Ajouter l'énergie potentielle adoucie : −G·mᵢ·mⱼ / √(r² + ε²)
- [ ] Afficher la dérive de l'énergie totale (cinétique + potentielle)
- [ ] Écrire les positions finales dans un fichier de référence (petit N, peu de pas)
- [ ] Ajouter un mode de comparaison à la référence avec tolérance relative (~1e-10 en `double`)

**Critère de fin**
- [ ] Deux exécutions avec la même graine donnent des résultats identiques
- [ ] La dérive d'énergie du schéma semi-implicite est nettement plus faible que celle de l'Euler explicite

---

## Étape 1 — Paramétrer la précision

- [ ] Rendre le type flottant paramétrable (`template <typename Real>` ou alias `real_t`)
- [ ] Adapter `Bodies`, `Forces`, les noyaux et les concepts
- [ ] Compiler et exécuter en `float` et en `double`

**Critère de fin**
- [ ] Écart de résultats `float` / `double` mesuré
- [ ] Dérive d'énergie `float` / `double` mesurée

---

## Étape 2 — Kokkos sur CPU (backend OpenMP)

- [ ] Intégrer Kokkos au `CMakeLists.txt` (backend OpenMP)
- [ ] Initialiser Kokkos avec `Kokkos::ScopeGuard` dans `main`
- [ ] Remplacer les `std::vector` de `Bodies` et `Forces` par des `Kokkos::View<Real*>`
- [ ] Initialiser les positions dans un mirror hôte (`create_mirror_view`), puis `deep_copy`
- [ ] Transformer les boucles externes en `Kokkos::parallel_for` (`RangePolicy`)
- [ ] Transformer `get_system_energy` en `Kokkos::parallel_reduce`
- [ ] Adapter les concepts aux nouvelles signatures
- [ ] Vérifier la vectorisation de la boucle interne (`-fopt-info-vec`)

**Critère de fin**
- [ ] Résultats dans la tolérance de l'étape 0
- [ ] Performances proches de la version OpenMP actuelle

---

## Étape 3 — Kokkos sur GPU (backend CUDA), version naïve

- [ ] Compiler Kokkos avec `Kokkos_ENABLE_CUDA=ON` et `Kokkos_ARCH_ADA89=ON`
- [ ] Garder les données sur le GPU pendant toute la boucle en temps
- [ ] Calculer l'énergie sur le GPU, sans rapatrier les tableaux
- [ ] Copier `g_` et `eps2_` dans des variables locales `constexpr` avant les lambdas device
- [ ] Ajouter `Kokkos::fence()` avant l'arrêt de chaque chronomètre
- [ ] Rapatrier les positions uniquement pour la validation

**Critère de fin**
- [ ] Résultats dans la tolérance de l'étape 0
- [ ] Exécution fonctionnelle jusqu'à N ≥ 10⁵

---

## Étape 4 — Mesurer et comprendre

- [ ] Balayer N de 10³ à 2·10⁵
- [ ] Tracer le débit en interactions par seconde (et en GFLOP/s, 20 flops par interaction)
- [ ] Profiler avec Nsight Systems : lancement des noyaux vs durée des noyaux
- [ ] Profiler le noyau de forces avec Nsight Compute (section Speed Of Light)
- [ ] Comparer le débit mesuré aux plafonds théoriques FP64 et FP32 du GPU

**Critère de fin**
- [ ] Facteur limitant du noyau identifié et expliqué

---

## Étape 5 — Passer en `float`

- [ ] Compiler et mesurer la version GPU en `float`
- [ ] Comparer la dérive d'énergie à celle de la version `double`
- [ ] Décider de la précision par défaut

**Critère de fin**
- [ ] Comparatif `float` / `double` : performances et précision

---

## Étape 6 — Version en tuiles (`TeamPolicy`)

- [ ] Implémenter le noyau en tuiles avec scratch memory de niveau 0
- [ ] Placer les deux `team_barrier()` (après le chargement, après le calcul)
- [ ] Remplir la dernière tuile avec des corps de masse nulle (padding)
- [ ] Ne faire aucun `return` anticipé dans le noyau
- [ ] Variante : stocker les corps en `{x, y, z, m}` compact dans la tuile
- [ ] Variante : dérouler la boucle sur `k`
- [ ] Tester plusieurs tailles de tuile (128, 256, 512)
- [ ] Vérifier l'absence de conflits de banc dans Nsight Compute

**Critère de fin**
- [ ] Comparatif naïf / tuiles pour plusieurs N, expliqué avec Nsight Compute

---

## Étape 7 — CUDA natif

- [ ] Créer une cible séparée (`.cu`) avec la même disposition des données
- [ ] Réutiliser le même format de validation
- [ ] Implémenter la version naïve puis la version en tuiles
- [ ] Comparer au noyau Kokkos en tuiles
- [ ] Comparer à l'exemple `nbody` des CUDA Samples
- [ ] Explorer `rsqrtf` et `--use_fast_math`
- [ ] Explorer `__launch_bounds__` (registres, occupation)
- [ ] Explorer le calcul de plusieurs corps i par thread (2 à 4)

**Critère de fin**
- [ ] Origine de l'écart entre CUDA natif et Kokkos identifiée

---

## Étape 8 (optionnelle) — Aller plus loin

- [ ] Plusieurs GPU avec MPI : répartir les corps i entre rangs, `MPI_Allgatherv` des positions à chaque pas
- [ ] Barnes-Hut (O(N log N)) — à traiter comme un nouveau projet

---

## Pièges à garder en tête

- [ ] Toujours faire `Kokkos::fence()` avant d'arrêter un chronomètre
- [ ] Ne jamais copier les données entre hôte et GPU à chaque pas de temps
- [ ] Comparer CPU et GPU avec une tolérance, jamais au bit près
- [ ] Valider sur des simulations courtes (le système est chaotique)
- [ ] Mesurer sur des N assez grands pour que le GPU soit rentable
