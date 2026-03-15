// ============================================================================
// FICHIER DE TEST — Complexity Analyzer Extension
// Une fonction par niveau de complexité (couleur)
//
// Format des commentaires :
//   Complexité réelle : la vraie complexité algorithmique
//   Résultat attendu  : ce que l'extension DEVRAIT afficher (Phase 1)
//   Couleur attendue   : la couleur de fond de la ligne de signature
// ============================================================================
 
#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>
 
 
// ---------------------------------------------------------------------------
// 🟢 EXCELLENT — O(1) — Vert (#31B44C)
//
// Complexité réelle  : O(1) — accès direct, aucune boucle
// Résultat attendu   : O(1)
// Couleur attendue   : Vert (Excellent)
// ---------------------------------------------------------------------------
int constantAccess(std::vector<int>& arr) {
    if (arr.empty()) return -1;
    int first = arr[0];
    int last = arr[arr.size() - 1];
    int mid = arr[arr.size() / 2];
    return first + last + mid;
}
 
 
// ---------------------------------------------------------------------------
// 🟢 EXCELLENT — O(log n) — Vert (#31B44C)
//
// Complexité réelle  : O(log n) — division par 2 à chaque itération
// Résultat attendu   : O(log n)
// Couleur attendue   : Vert (Excellent)
// Note               : L'extension détecte le pattern "/ 2" dans la boucle
// ---------------------------------------------------------------------------
int binarySearch(std::vector<int>& arr, int target) {
    int low = 0;
    int high = arr.size() - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (arr[mid] == target) {
            return mid;
        } else if (arr[mid] < target) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return -1;
}
 
 
// ---------------------------------------------------------------------------
// 🟡 GOOD — O(n) — Jaune-vert (#C4D400)
//
// Complexité réelle  : O(n) — une seule boucle parcourant tout le tableau
// Résultat attendu   : O(n)
// Couleur attendue   : Jaune-vert (Good)
// ---------------------------------------------------------------------------
int findMax(std::vector<int>& arr) {
    if (arr.empty()) return -1;
    int maximum = arr[0];
    for (int i = 1; i < static_cast<int>(arr.size()); i++) {
        if (arr[i] > maximum) {
            maximum = arr[i];
        }
    }
    return maximum;
}
 
 
// ---------------------------------------------------------------------------
// 🟡 GOOD — O(n) — Jaune-vert (#C4D400)
// VERSION RANGE-BASED FOR LOOP
//
// Complexité réelle  : O(n) — range-based for loop
// Résultat attendu   : O(n)
// Couleur attendue   : Jaune-vert (Good)
// Note               : Vérifie que l'extension détecte les for_range_loop
// ---------------------------------------------------------------------------
long long sumAll(std::vector<int>& arr) {
    long long total = 0;
    for (const auto& x : arr) {
        total += x;
    }
    return total;
}
 
 
// ---------------------------------------------------------------------------
// 🟠 FAIR — O(n log n) — Orange (#FFC800)
//
// Complexité réelle  : O(n log n) — boucle externe divise par 2, interne O(n)
// Résultat attendu   : O(n log n)
// Couleur attendue   : Orange (Fair)
// Note               : Shell sort avec gap /= 2 explicite
// ---------------------------------------------------------------------------
void shellSort(std::vector<int>& arr) {
    int n = arr.size();
    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; i++) {
            int temp = arr[i];
            int j = i;
            while (j >= gap && arr[j - gap] > temp) {
                arr[j] = arr[j - gap];
                j -= gap;
            }
            arr[j] = temp;
        }
    }
}
// L'extension voit : for (gap) → for (i) → while (j) = 3 boucles imbriquées
// AVEC le pattern "/= 2" et "/ 2" détecté.
// RÉSULTAT PROBABLE Phase 1 : O(n²) (si 3 boucles + halving → n² log n simplifié)
//                              ou O(n³) (si halving non appliqué aux 3 boucles)
// RÉSULTAT IDÉAL            : O(n log n)
// C'est un bon cas limite pour tester la détection.
 
 
// ---------------------------------------------------------------------------
// 🔴 BAD — O(n²) — Rouge-orange (#FF6A52)
//
// Complexité réelle  : O(n²) — deux boucles imbriquées classiques
// Résultat attendu   : O(n²)
// Couleur attendue   : Rouge-orange (Bad)
// ---------------------------------------------------------------------------
void selectionSort(std::vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n; i++) {
        int minIdx = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[minIdx]) {
                minIdx = j;
            }
        }
        std::swap(arr[i], arr[minIdx]);
    }
}
 
 
// ---------------------------------------------------------------------------
// 🔴 BAD — O(n²) — Rouge-orange (#FF6A52)
// VERSION DO-WHILE
//
// Complexité réelle  : O(n²) — do-while avec boucle for interne
// Résultat attendu   : O(n²)
// Couleur attendue   : Rouge-orange (Bad)
// Note               : Vérifie que l'extension détecte les do_statement
// ---------------------------------------------------------------------------
void cocktailSort(std::vector<int>& arr) {
    int n = arr.size();
    bool swapped = true;
    int start = 0;
    int end = n - 1;
    do {
        swapped = false;
        for (int i = start; i < end; i++) {
            if (arr[i] > arr[i + 1]) {
                std::swap(arr[i], arr[i + 1]);
                swapped = true;
            }
        }
        end--;
        for (int i = end; i > start; i--) {
            if (arr[i] < arr[i - 1]) {
                std::swap(arr[i], arr[i - 1]);
                swapped = true;
            }
        }
        start++;
    } while (swapped);
}
 
 
// ---------------------------------------------------------------------------
// 🔴 HORRIBLE — O(n³) — Rouge (#E63946)
//
// Complexité réelle  : O(n³) — trois boucles imbriquées
// Résultat attendu   : O(n³)
// Couleur attendue   : Rouge (Horrible)
// Note               : Multiplication de matrices naïve
// ---------------------------------------------------------------------------
std::vector<std::vector<int>> matrixMultiply(
    std::vector<std::vector<int>>& a,
    std::vector<std::vector<int>>& b
) {
    int n = a.size();
    std::vector<std::vector<int>> result(n, std::vector<int>(n, 0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    return result;
}
 
 
// ---------------------------------------------------------------------------
// 🔴 HORRIBLE — O(2ⁿ) — Rouge (#E63946)
//
// Complexité réelle  : O(2ⁿ) — récursion double (arbre binaire d'appels)
// Résultat attendu   : ⚠️  Phase 1 affichera O(1) car aucune boucle
//                       La récursion n'est pas détectée en Phase 1.
// Couleur attendue   : Vert (Excellent) ← FAUX mais limite connue
// ---------------------------------------------------------------------------
int fibonacciRecursive(int n) {
    if (n <= 1) return n;
    return fibonacciRecursive(n - 1) + fibonacciRecursive(n - 2);
}
 
 
// ---------------------------------------------------------------------------
// 🔴 HORRIBLE — O(n!) — Rouge (#E63946)
//
// Complexité réelle  : O(n!) — génération de toutes les permutations
// Résultat attendu   : ⚠️  Phase 1 affichera O(n) car une seule boucle
//                       visible (le for). La récursion n'est pas comptée.
// Couleur attendue   : Jaune-vert (Good) ← FAUX mais limite connue
// ---------------------------------------------------------------------------
void generatePermutations(
    std::vector<int>& arr,
    int start,
    std::vector<std::vector<int>>& result
) {
    if (start == static_cast<int>(arr.size()) - 1) {
        result.push_back(arr);
        return;
    }
    for (int i = start; i < static_cast<int>(arr.size()); i++) {
        std::swap(arr[start], arr[i]);
        generatePermutations(arr, start + 1, result);
        std::swap(arr[start], arr[i]);
    }
}
 
 
// ============================================================================
// RÉSUMÉ — Ce que l'extension Phase 1 devrait afficher
//
// Fonction                  | Réel      | Phase 1 attendu | Correct ?
// --------------------------|-----------|-----------------|----------
// constantAccess            | O(1)      | O(1)            | ✅
// binarySearch              | O(log n)  | O(log n)        | ✅
// findMax                   | O(n)      | O(n)            | ✅
// sumAll (range-based for)  | O(n)      | O(n)            | ✅
// shellSort                 | O(n log n)| O(n²) ou O(n³)  | ❌ (limite)
// selectionSort             | O(n²)     | O(n²)           | ✅
// cocktailSort (do-while)   | O(n²)     | O(n²)           | ✅
// matrixMultiply            | O(n³)     | O(n³)           | ✅
// fibonacciRecursive        | O(2ⁿ)    | O(1)            | ❌ (récursion)
// generatePermutations      | O(n!)     | O(n)            | ❌ (récursion)
//
// Score Phase 1 : 7/10 corrects pour ce fichier de test
// Les 3 erreurs sont des limites connues (récursion + n log n)
//
// BONUS — Types de boucles C++ testés :
//   ✅ for classique (selectionSort, matrixMultiply)
//   ✅ while (binarySearch)
//   ✅ do-while (cocktailSort)
//   ✅ range-based for (sumAll)
// ============================================================================