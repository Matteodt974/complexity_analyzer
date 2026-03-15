# ============================================================================
# FICHIER DE TEST — Complexity Analyzer Extension
# Une fonction par niveau de complexité (couleur)
#
# Format des commentaires :
#   Complexité réelle : la vraie complexité algorithmique
#   Résultat attendu  : ce que l'extension DEVRAIT afficher (Phase 1)
#   Couleur attendue   : la couleur de fond de la ligne de signature
# ============================================================================
 
 
# ---------------------------------------------------------------------------
# 🟢 EXCELLENT — O(1) — Vert (#31B44C)
#
# Complexité réelle  : O(1) — accès direct, aucune boucle
# Résultat attendu   : O(1)
# Couleur attendue   : Vert (Excellent)
# ---------------------------------------------------------------------------
def constant_access(arr):
    if len(arr) == 0:
        return None
    first = arr[0]
    last = arr[-1]
    mid = arr[len(arr) // 2]
    return first + last + mid
 
 
# ---------------------------------------------------------------------------
# 🟢 EXCELLENT — O(log n) — Vert (#31B44C)
#
# Complexité réelle  : O(log n) — division par 2 à chaque itération
# Résultat attendu   : O(log n)
# Couleur attendue   : Vert (Excellent)
# Note               : L'extension détecte le pattern "/ 2" dans la boucle
# ---------------------------------------------------------------------------
def binary_search(arr, target):
    low = 0
    high = len(arr) - 1
    while low <= high:
        mid = (low + high) // 2
        if arr[mid] == target:
            return mid
        elif arr[mid] < target:
            low = mid + 1
        else:
            high = mid - 1
    return -1
 
 
# ---------------------------------------------------------------------------
# 🟡 GOOD — O(n) — Jaune-vert (#C4D400)
#
# Complexité réelle  : O(n) — une seule boucle parcourant tout le tableau
# Résultat attendu   : O(n)
# Couleur attendue   : Jaune-vert (Good)
# ---------------------------------------------------------------------------
def find_max(arr):
    if len(arr) == 0:
        return None
    maximum = arr[0]
    for i in range(1, len(arr)):
        if arr[i] > maximum:
            maximum = arr[i]
    return maximum
 
 
# ---------------------------------------------------------------------------
# 🟠 FAIR — O(n log n) — Orange (#FFC800)
#
# Complexité réelle  : O(n log n) — merge sort classique
# Résultat attendu   : ⚠️  ATTENTION — Phase 1 affichera probablement O(n)
#                       car merge_sort n'a qu'une boucle visible (la boucle
#                       merge) et la récursion n'est pas détectée.
#                       Phase 2 (graphe d'appels) pourrait mieux faire mais
#                       la récursion reste une limitation.
#                       Pour obtenir O(n log n) en Phase 1, on utilise plutôt
#                       une version itérative ci-dessous.
# Couleur attendue   : Orange (Fair)
# ---------------------------------------------------------------------------
def merge_sort_iterative(arr):
    n = len(arr)
    size = 1
    while size < n:
        for start in range(0, n, size * 2):
            mid = min(start + size, n)
            end = min(start + size * 2, n)
            left = arr[start:mid]
            right = arr[mid:end]
            i = j = 0
            k = start
            while i < len(left) and j < len(right):
                if left[i] <= right[j]:
                    arr[k] = left[i]
                    i += 1
                else:
                    arr[k] = right[j]
                    j += 1
                k += 1
            while i < len(left):
                arr[k] = left[i]
                i += 1
                k += 1
            while j < len(right):
                arr[k] = right[j]
                j += 1
                k += 1
        size = size * 2
    return arr
# NOTE : Cette fonction a une boucle while externe qui double `size` (halving
# pattern inversé : size * 2 → log n itérations) et une boucle for interne
# qui parcourt n éléments. L'extension devrait détecter 2 boucles imbriquées
# + le pattern "* 2". Le résultat dépendra de l'implémentation exacte de
# la détection de halving. Si elle ne détecte que "/ 2" et pas "* 2",
# le résultat sera O(n²) au lieu de O(n log n).
#
# RÉSULTAT PROBABLE Phase 1 : O(n²) (2 boucles imbriquées, halving non détecté)
# RÉSULTAT IDÉAL            : O(n log n)
 
 
# ---------------------------------------------------------------------------
# 🟠 FAIR — O(n log n) — Orange (#FFC800)
# VERSION ALTERNATIVE avec pattern / 2 explicite
#
# Complexité réelle  : O(n log n) — boucle externe divise par 2, interne est O(n)
# Résultat attendu   : O(n log n)
# Couleur attendue   : Orange (Fair)
# Note               : Cette version utilise "gap // 2" pour que l'extension
#                       détecte le halving pattern avec 2 boucles imbriquées
# ---------------------------------------------------------------------------
def shell_sort(arr):
    n = len(arr)
    gap = n // 2
    while gap > 0:
        for i in range(gap, n):
            temp = arr[i]
            j = i
            while j >= gap and arr[j - gap] > temp:
                arr[j] = arr[j - gap]
                j -= gap
            arr[j] = temp
        gap = gap // 2
    return arr
# L'extension voit : while (gap > 0) → for → while = 3 boucles imbriquées
# MAIS avec le pattern "// 2" détecté.
# RÉSULTAT PROBABLE Phase 1 : O(n³) ou O(n²) selon la profondeur comptée
# RÉSULTAT IDÉAL            : O(n log n) (mais difficile à détecter statiquement)
#
# C'est un bon cas pour voir les limites de l'analyse statique.
 
 
# ---------------------------------------------------------------------------
# 🔴 BAD — O(n²) — Rouge-orange (#FF6A52)
#
# Complexité réelle  : O(n²) — deux boucles imbriquées classiques
# Résultat attendu   : O(n²)
# Couleur attendue   : Rouge-orange (Bad)
# ---------------------------------------------------------------------------
def selection_sort(arr):
    n = len(arr)
    for i in range(n):
        min_idx = i
        for j in range(i + 1, n):
            if arr[j] < arr[min_idx]:
                min_idx = j
        arr[i], arr[min_idx] = arr[min_idx], arr[i]
    return arr
 
 
# ---------------------------------------------------------------------------
# 🔴 HORRIBLE — O(n³) — Rouge (#E63946)
#
# Complexité réelle  : O(n³) — trois boucles imbriquées
# Résultat attendu   : O(n³)
# Couleur attendue   : Rouge (Horrible)
# Note               : Multiplication de matrices naïve
# ---------------------------------------------------------------------------
def matrix_multiply(a, b):
    n = len(a)
    result = [[0] * n for _ in range(n)]
    for i in range(n):
        for j in range(n):
            for k in range(n):
                result[i][j] += a[i][k] * b[k][j]
    return result
 
 
# ---------------------------------------------------------------------------
# 🔴 HORRIBLE — O(2ⁿ) — Rouge (#E63946)
#
# Complexité réelle  : O(2ⁿ) — récursion double (arbre binaire d'appels)
# Résultat attendu   : ⚠️  Phase 1 affichera O(1) car aucune boucle
#                       La récursion n'est pas détectée en Phase 1.
#                       Phase 2 détectera la récursion mais ne calculera pas
#                       la complexité exponentielle.
# Couleur attendue   : Vert (Excellent) ← FAUX mais c'est une limite connue
# ---------------------------------------------------------------------------
def fibonacci_recursive(n):
    if n <= 1:
        return n
    return fibonacci_recursive(n - 1) + fibonacci_recursive(n - 2)
 
 
# ---------------------------------------------------------------------------
# 🔴 HORRIBLE — O(n!) — Rouge (#E63946)
#
# Complexité réelle  : O(n!) — génération de toutes les permutations
# Résultat attendu   : ⚠️  Phase 1 affichera O(n) car une seule boucle
#                       visible (le for). La récursion qui multiplie n'est
#                       pas comptée.
# Couleur attendue   : Jaune-vert (Good) ← FAUX mais c'est une limite connue
# ---------------------------------------------------------------------------
def generate_permutations(arr, start=0):
    if start == len(arr) - 1:
        return [arr[:]]
    result = []
    for i in range(start, len(arr)):
        arr[start], arr[i] = arr[i], arr[start]
        result.extend(generate_permutations(arr, start + 1))
        arr[start], arr[i] = arr[i], arr[start]
    return result
 
 
# ============================================================================
# RÉSUMÉ — Ce que l'extension Phase 1 devrait afficher
#
# Fonction                  | Réel      | Phase 1 attendu | Correct ?
# --------------------------|-----------|-----------------|----------
# constant_access           | O(1)      | O(1)            | ✅
# binary_search             | O(log n)  | O(log n)        | ✅
# find_max                  | O(n)      | O(n)            | ✅
# merge_sort_iterative      | O(n log n)| O(n²)           | ❌ (limite)
# shell_sort                | O(n log n)| O(n²) ou O(n³)  | ❌ (limite)
# selection_sort            | O(n²)     | O(n²)           | ✅
# matrix_multiply           | O(n³)     | O(n³)           | ✅
# fibonacci_recursive       | O(2ⁿ)    | O(1)            | ❌ (récursion)
# generate_permutations     | O(n!)     | O(n)            | ❌ (récursion)
#
# Score Phase 1 : 5/9 corrects pour ce fichier de test
# Les 4 erreurs sont des limites connues (récursion + n log n)
# ============================================================================