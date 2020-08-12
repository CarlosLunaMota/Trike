/** *********************************************************************** **/
/**                                                                         **/
/** # CLM_LIBS                                                              **/
/**                                                                         **/
/** **CONTENT:** A trimmed version of the CLM_LIBS code-generating macros   **/
/**                                                                         **/
/** **AUTHOR:**  Carlos Luna-Mota                                           **/
/**                                                                         **/
/** **SOURCE:**  <https://github.com/CarlosLunaMota/CLM_LIBS>               **/
/**                                                                         **/
/** *********************************************************************** **/

#include <assert.h>     /* Include assertions unless NDEBUG is defined.      */
#include <stdbool.h>    /* Bool type and {true, false} values.               */
#include <stdint.h>     /* Fixed width integer types.                        */
#include <stdio.h>      /* Input and output functions.                       */
#include <stdlib.h>     /* Memory allocation and random functions.           */

#ifndef CLM_LIBS


  #define CLM_LIBS 20200702


  #define IMPORT_CLM_RAND(prefix)                                               \
                                                                                \
    static inline size_t prefix##rand_size_t(const size_t n) {                  \
                                                                                \
        /* Preconditions */                                                     \
        assert(n > 0);                                                          \
        assert(RAND_MAX >= n);                                                  \
                                                                                \
        /* Monte-Carlo uniformly random generator */                            \
        size_t r, range = RAND_MAX - (RAND_MAX % n);                            \
        do { r = rand(); } while (r >= range);                                  \
        return r % n;                                                           \
    }                                                                           \
                                                                                \

  #define IMPORT_CLM_STREE(type, less, prefix)                                  \
                                                                                \
    typedef struct prefix##stree_s {                                            \
        struct prefix##stree_s *left;                                           \
        struct prefix##stree_s *right;                                          \
        type            data;                                                   \
    } prefix##stree_s, *prefix##stree;                                          \
                                                                                \
    static inline type prefix##stree_root(prefix##stree *tree) {                \
                                                                                \
        /* Precondition */                                                      \
        assert(*tree != NULL);                                                  \
                                                                                \
        return (*tree)->data;                                                   \
    }                                                                           \
                                                                                \
    static inline type prefix##stree_min(prefix##stree *tree) {                 \
                                                                                \
        /* Precondition */                                                      \
        assert(*tree != NULL);                                                  \
                                                                                \
        prefix##stree_s dummy, *right, *temp, *root = *tree;                    \
                                                                                \
        /* Top Down Splay-Min */                                                \
        dummy.left = NULL;                                                      \
        right      = &dummy;                                                    \
        for (;;) {                                                              \
                                                                                \
            /* Rotate Right */                                                  \
            if (root->left == NULL) { break; }                                  \
            temp        = root->left;                                           \
            root->left  = temp->right;                                          \
            temp->right = root;                                                 \
            root        = temp;                                                 \
                                                                                \
            /* Link Right */                                                    \
            if (root->left == NULL) { break; }                                  \
            right->left = root;                                                 \
            right       = root;                                                 \
            root        = root->left;                                           \
        }                                                                       \
                                                                                \
        /* Final assemble & return */                                           \
        right->left = root->right;                                              \
        root->right = dummy.left;                                               \
        *tree       = root;                                                     \
        return root->data;                                                      \
    }                                                                           \
                                                                                \
    static inline type prefix##stree_max(prefix##stree *tree) {                 \
                                                                                \
        /* Precondition */                                                      \
        assert(*tree != NULL);                                                  \
                                                                                \
        prefix##stree_s dummy, *left, *temp, *root = *tree;                     \
                                                                                \
        /* Top Down Splay-Max */                                                \
        dummy.right = NULL;                                                     \
        left        = &dummy;                                                   \
        for (;;) {                                                              \
                                                                                \
            /* Rotate Left */                                                   \
            if (root->right == NULL) { break; }                                 \
            temp        = root->right;                                          \
            root->right = temp->left;                                           \
            temp->left  = root;                                                 \
            root        = temp;                                                 \
                                                                                \
            /* Link Left */                                                     \
            if (root->right == NULL) { break; }                                 \
            left->right = root;                                                 \
            left        = root;                                                 \
            root        = root->right;                                          \
        }                                                                       \
                                                                                \
        /* Final assemble & return */                                           \
        left->right = root->left;                                               \
        root->left  = dummy.right;                                              \
        *tree       = root;                                                     \
        return root->data;                                                      \
    }                                                                           \
                                                                                \
    static inline type prefix##stree_pop(prefix##stree *tree) {                 \
                                                                                \
        /* Precondition */                                                      \
        assert(*tree != NULL);                                                  \
                                                                                \
        prefix##stree_s dummy, *left, *right, *temp, *root, *old_root = *tree;  \
        type            data;                                                   \
                                                                                \
        /* Store data */                                                        \
        data = old_root->data;                                                  \
                                                                                \
        /* Particular case */                                                   \
        if (old_root->right == NULL && old_root->left == NULL) {                \
           *tree = NULL;                                                        \
            free(old_root);                                                     \
            return data;                                                        \
        }                                                                       \
                                                                                \
        /* General Case */                                                      \
        if (old_root->right == NULL) {                                          \
                                                                                \
            /* Top-down simple splay-max the old_root->left; */                 \
            root        = old_root->left;                                       \
            dummy.right = NULL;                                                 \
            left        = &dummy;                                               \
            for (;;) {                                                          \
                                                                                \
                /* Rotate Left */                                               \
                if (root->right == NULL) { break; }                             \
                temp        = root->right;                                      \
                root->right = temp->left;                                       \
                temp->left  = root;                                             \
                root        = temp;                                             \
                                                                                \
                /* Link Left */                                                 \
                if (root->right == NULL) { break; }                             \
                left->right = root;                                             \
                left        = root;                                             \
                root        = root->right;                                      \
            }                                                                   \
                                                                                \
            /* Final assemble */                                                \
            left->right = root->left;                                           \
            root->left  = dummy.right;                                          \
                                                                                \
        } else {                                                                \
                                                                                \
            /* Top-down simple splay-min the old_root->right; */                \
            root       = old_root->right;                                       \
            dummy.left = NULL;                                                  \
            right      = &dummy;                                                \
            for (;;) {                                                          \
                                                                                \
                /* Rotate Right */                                              \
                if (root->left == NULL) { break; }                              \
                temp        = root->left;                                       \
                root->left  = temp->right;                                      \
                temp->right = root;                                             \
                root        = temp;                                             \
                                                                                \
                /* Link Right */                                                \
                if (root->left == NULL) { break; }                              \
                right->left = root;                                             \
                right       = root;                                             \
                root        = root->left;                                       \
            }                                                                   \
                                                                                \
            /* Final assemble */                                                \
            right->left = root->right;                                          \
            root->right = dummy.left;                                           \
            root->left  = old_root->left;                                       \
        }                                                                       \
                                                                                \
        /* Store the new root, free the old root and return its content. */     \
        *tree = root;                                                           \
        free(old_root);                                                         \
        return data;                                                            \
    }                                                                           \
                                                                                \
    static inline bool prefix##stree_next(prefix##stree *tree) {                \
                                                                                \
        prefix##stree_s dummy, *right, *temp, *root, *old_root = *tree;         \
                                                                                \
        /* Trivial case 1: Empty tree */                                        \
        if (old_root == NULL) { return false; }                                 \
                                                                                \
        /* Trivial case 2: There is not a bigger element */                     \
        if (old_root->right == NULL) { return false; }                          \
                                                                                \
        /* General case: Top-down simple splay-min of the old_root->right */    \
        root            = old_root->right;                                      \
        old_root->right = NULL;                                                 \
        dummy.left      = NULL;                                                 \
        right           = &dummy;                                               \
        for (;;) {                                                              \
                                                                                \
            /* Rotate Right */                                                  \
            if (root->left == NULL) { break; }                                  \
            temp        = root->left;                                           \
            root->left  = temp->right;                                          \
            temp->right = root;                                                 \
            root        = temp;                                                 \
                                                                                \
            /* Link Right */                                                    \
            if (root->left == NULL) { break; }                                  \
            right->left = root;                                                 \
            right       = root;                                                 \
            root        = root->left;                                           \
        }                                                                       \
                                                                                \
        /* Final assemble & return */                                           \
        right->left = root->right;                                              \
        root->right = dummy.left;                                               \
        root->left  = old_root;                                                 \
        *tree       = root;                                                     \
        return true;                                                            \
    }                                                                           \
                                                                                \
    static inline bool prefix##stree_prev(prefix##stree *tree) {                \
                                                                                \
        prefix##stree_s dummy, *left, *temp, *root, *old_root = *tree;          \
                                                                                \
        /* Trivial case 1: Empty tree */                                        \
        if (old_root == NULL) { return false; }                                 \
                                                                                \
        /* Trivial case 2: There is not a smaller element */                    \
        if (old_root->left == NULL) { return false; }                           \
                                                                                \
        /* General case: Top-down splay-max of the root->left */                \
        root           = old_root->left;                                        \
        old_root->left = NULL;                                                  \
        dummy.right    = NULL;                                                  \
        left           = &dummy;                                                \
        for (;;) {                                                              \
                                                                                \
            /* Rotate Left */                                                   \
            if (root->right == NULL) { break; }                                 \
            temp        = root->right;                                          \
            root->right = temp->left;                                           \
            temp->left  = root;                                                 \
            root        = temp;                                                 \
                                                                                \
            /* Link Left */                                                     \
            if (root->right == NULL) { break; }                                 \
            left->right = root;                                                 \
            left        = root;                                                 \
            root        = root->right;                                          \
        }                                                                       \
                                                                                \
        /* Final assemble & return */                                           \
        left->right = root->left;                                               \
        root->left  = dummy.right;                                              \
        root->right = old_root;                                                 \
        *tree       = root;                                                     \
        return true;                                                            \
    }                                                                           \
                                                                                \
    static inline bool prefix##stree_find(prefix##stree *tree,                  \
                                          const type data) {                    \
                                                                                \
        prefix##stree_s dummy, *left, *right, *temp, *root = *tree;             \
        bool found = false;                                                     \
                                                                                \
        /* Trivial case: Empty tree */                                          \
        if (root == NULL) { return false; }                                     \
                                                                                \
        /* General case */                                                      \
        dummy.left = dummy.right = NULL;                                        \
        left       = right       = &dummy;                                      \
        for (;;) {                                                              \
                                                                                \
            /* Case 1: data < root->data */                                     \
            if (less(data, root->data)) {                                       \
                                                                                \
                /* Rotate Right */                                              \
                if (root->left == NULL) { break; }                              \
                if (less(data, root->left->data)) {                             \
                    temp        = root->left;                                   \
                    root->left  = temp->right;                                  \
                    temp->right = root;                                         \
                    root        = temp;                                         \
                    if (root->left == NULL) { break; }                          \
                }                                                               \
                                                                                \
                /* Link Right */                                                \
                right->left = root;                                             \
                right       = root;                                             \
                root        = root->left;                                       \
            }                                                                   \
                                                                                \
            /* Case 2: data > root->data */                                     \
            else if (less(root->data, data)) {                                  \
                                                                                \
                /* Rotate Left */                                               \
                if (root->right == NULL) { break; }                             \
                if (less(root->right->data, data)) {                            \
                    temp        = root->right;                                  \
                    root->right = temp->left;                                   \
                    temp->left  = root;                                         \
                    root        = temp;                                         \
                    if (root->right == NULL) { break; }                         \
                }                                                               \
                                                                                \
                /* Link Left */                                                 \
                left->right = root;                                             \
                left        = root;                                             \
                root        = root->right;                                      \
            }                                                                   \
                                                                                \
            /* Case 3: data == root->data */                                    \
            else { found = true; break; }                                       \
        }                                                                       \
                                                                                \
        /* Final assemble & return */                                           \
        left->right = root->left;                                               \
        right->left = root->right;                                              \
        root->left  = dummy.right;                                              \
        root->right = dummy.left;                                               \
        *tree       = root;                                                     \
        return found;                                                           \
    }                                                                           \
                                                                                \
    static inline bool prefix##stree_insert(prefix##stree *tree,                \
                                            const type data) {                  \
                                                                                \
        prefix##stree_s dummy, *left, *right, *temp, *new_root, *root = *tree;  \
        bool found = false;                                                     \
                                                                                \
        /* Splay data to the root of the tree */                                \
        if (root != NULL) {                                                     \
            dummy.left = dummy.right = NULL;                                    \
            left       = right       = &dummy;                                  \
            for (;;) {                                                          \
                                                                                \
                /* Case 1: data < root->data */                                 \
                if (less(data, root->data)) {                                   \
                                                                                \
                    /* Rotate Right */                                          \
                    if (root->left == NULL) { break; }                          \
                    if (less(data, root->left->data)) {                         \
                        temp        = root->left;                               \
                        root->left  = temp->right;                              \
                        temp->right = root;                                     \
                        root        = temp;                                     \
                        if (root->left == NULL) { break; }                      \
                    }                                                           \
                                                                                \
                    /* Link Right */                                            \
                    right->left = root;                                         \
                    right       = root;                                         \
                    root        = root->left;                                   \
                }                                                               \
                                                                                \
                /* Case 2: data > root->data */                                 \
                else if (less(root->data, data)) {                              \
                                                                                \
                    /* Rotate Left */                                           \
                    if (root->right == NULL) { break; }                         \
                    if (less(root->right->data, data)) {                        \
                        temp        = root->right;                              \
                        root->right = temp->left;                               \
                        temp->left  = root;                                     \
                        root        = temp;                                     \
                        if (root->right == NULL) { break; }                     \
                    }                                                           \
                                                                                \
                    /* Link Left */                                             \
                    left->right = root;                                         \
                    left        = root;                                         \
                    root        = root->right;                                  \
                }                                                               \
                                                                                \
                /* Case 3: data == root->data */                                \
                else { found = true; break; }                                   \
            }                                                                   \
                                                                                \
            /* Final assemble & return */                                       \
            left->right = root->left;                                           \
            right->left = root->right;                                          \
            root->left  = dummy.right;                                          \
            root->right = dummy.left;                                           \
        }                                                                       \
                                                                                \
        /* Trivial case 1: Overwrite data */                                    \
        if (found) {                                                            \
            root->data = data;                                                  \
            *tree      = root;                                                  \
            return true;                                                        \
        }                                                                       \
                                                                                \
        /* Allocate a new root node */                                          \
        new_root = (prefix##stree) malloc(sizeof(prefix##stree_s));             \
        if (new_root == NULL) {                                                 \
            fprintf(stderr, "ERROR: Unable to insert data into stree\n");       \
            *tree = root;                                                       \
            return false;                                                       \
        }                                                                       \
                                                                                \
        /* Trivial case 2: Empty tree */                                        \
        if (root == NULL) { new_root->left = new_root->right = NULL; }          \
                                                                                \
        /* General case */                                                      \
        else if (less(data, root->data)) {                                      \
            new_root->right = root;                                             \
            new_root->left  = root->left;                                       \
            root->left      = NULL;                                             \
        } else {                                                                \
            new_root->left  = root;                                             \
            new_root->right = root->right;                                      \
            root->right = NULL;                                                 \
        }                                                                       \
                                                                                \
        /* Store data and return */                                             \
        new_root->data = data;                                                  \
        *tree          = new_root;                                              \
        return true;                                                            \
    }                                                                           \
                                                                                \

  #define IMPORT_CLM_WTREE(type, less, prefix)                                  \
                                                                                \
    typedef struct prefix##wtree_s {                                            \
      struct prefix##wtree_s *left;                                             \
      struct prefix##wtree_s *right;                                            \
      size_t          size;                                                     \
      type            data;                                                     \
    } prefix##wtree_s, *prefix##wtree;                                          \
                                                                                \
    static inline size_t prefix##wtree_size(const prefix##wtree *tree) {        \
                                                                                \
        return ((*tree) ? ((*tree)->size) : (0));                               \
    }                                                                           \
                                                                                \
    static inline size_t prefix##wtree_find(const prefix##wtree *tree,          \
                                            const type data) {                  \
                                                                                \
        prefix##wtree node = *tree;                                             \
        size_t        rank = 1;                                                 \
                                                                                \
        /* Find data in the tree */                                             \
        while (node) {                                                          \
            if (less(data, node->data)) { node = node->left; }                  \
            else {                                                              \
                if (node->left)             { rank += node->left->size;    }    \
                if (less(node->data, data)) { node  = node->right; rank++; }    \
                else                        { return rank;                 }    \
            }                                                                   \
        }                                                                       \
                                                                                \
        /* Data not found */                                                    \
        return 0;                                                               \
    }                                                                           \
                                                                                \
    static inline size_t prefix##wtree_insert(prefix##wtree *tree,              \
                                              const type data) {                \
                                                                                \
        /* Rebalancing constants (DELTA, GAMMA)                          */     \
        /* (5/2, 3/2) -> Worst case = O(2.06 log(size)) ~ Red-Black tree */     \
        /* (3/1, 2/1) -> Worst case = O(2.41 log(size))                  */     \
        /* (1/0, 1/0) -> Worst case = O(size) ~ Standrad Binary Tree     */     \
        const size_t DELTA_NUM = 5;                                             \
        const size_t DELTA_DEN = 2;                                             \
        const size_t GAMMA_NUM = 3;                                             \
        const size_t GAMMA_DEN = 2;                                             \
                                                                                \
        /* Variables */                                                         \
        prefix##wtree left, right, node = *tree;                                \
        size_t        rank, l_weight, r_weight;                                 \
        bool          is_right = false;                                         \
                                                                                \
        /* Trivial case: Insert data into empty tree */                         \
        if (node == NULL) {                                                     \
            node = (prefix##wtree) malloc(sizeof(prefix##wtree_s));             \
            if (node == NULL) {                                                 \
                fprintf(stderr, "ERROR: Unable to insert data into wtree.\n");  \
                return 0;                                                       \
            } else {                                                            \
                node->size = 1;                                                 \
                node->data = data;                                              \
                node->left = node->right = NULL;                                \
                *tree      = node;                                              \
                return 1;                                                       \
            }                                                                   \
        }                                                                       \
                                                                                \
        /* Case 1: Insert data into node->left */                               \
        if (less(data, node->data)) {                                           \
            rank = prefix##wtree_insert(&(node->left), data);                   \
        }                                                                       \
                                                                                \
        /* Case 2: Insert data into node->right */                              \
        else if (less(node->data, data)) {                                      \
            rank = prefix##wtree_insert(&(node->right), data);                  \
            is_right = true;                                                    \
        }                                                                       \
                                                                                \
        /* Case 3: Overwrite data */                                            \
        else {                                                                  \
            node->data = data;                                                  \
            return (node->left) ? (node->left->size + 1) : (1);                 \
        }                                                                       \
                                                                                \
        /*** REBALANCE (if something changed) ********************************/ \
        if (rank) {                                                             \
                                                                                \
            /* Update node->size */                                             \
            l_weight   = (node->left)  ? (node->left->size  + 1) : (1);         \
            r_weight   = (node->right) ? (node->right->size + 1) : (1);         \
            node->size = l_weight + r_weight - 1;                               \
                                                                                \
            /* Update rank */                                                   \
            if (is_right) { rank += l_weight; }                                 \
                                                                                \
            /* If node->left has become too big after insertion */              \
            if (DELTA_NUM*r_weight < DELTA_DEN*l_weight) {                      \
                                                                                \
                left     = node->left;                                          \
                l_weight = (left->left)  ? (left->left->size  + 1) : (1);       \
                r_weight = (left->right) ? (left->right->size + 1) : (1);       \
                                                                                \
                /* If node->left->right is small: make a single rotation */     \
                if (GAMMA_DEN*r_weight < GAMMA_NUM*l_weight) {                  \
                    node->left  = left->right;                                  \
                    left->right = node;                                         \
                    left->size  = node->size;                                   \
                    node->size  = 1;                                            \
                    node->size += (node->left)  ? (node->left->size)  : (0);    \
                    node->size += (node->right) ? (node->right->size) : (0);    \
                    *tree       = left;                                         \
                }                                                               \
                                                                                \
                /* If node->left->right is big: make a double rotation */       \
                else {                                                          \
                    right        = left->right;                                 \
                    left->right  = right->left;                                 \
                    right->left  = left;                                        \
                    node->left   = right->right;                                \
                    right->right = node;                                        \
                    right->size  = node->size;                                  \
                    node->size   = 1;                                           \
                    node->size  += (node->left)  ? (node->left->size)  : (0);   \
                    node->size  += (node->right) ? (node->right->size) : (0);   \
                    left->size   = 1;                                           \
                    left->size  += (left->left)  ? (left->left->size)  : (0);   \
                    left->size  += (left->right) ? (left->right->size) : (0);   \
                    *tree        = right;                                       \
                }                                                               \
            }                                                                   \
                                                                                \
            /* If node->right has become too big after insertion */             \
            else if (DELTA_NUM*l_weight < DELTA_DEN*r_weight) {                 \
                                                                                \
                right    = node->right;                                         \
                l_weight = (right->left)  ? (right->left->size  + 1) : (1);     \
                r_weight = (right->right) ? (right->right->size + 1) : (1);     \
                                                                                \
                /* If node->right->left is small: make a single rotation */     \
                if (GAMMA_DEN*l_weight < GAMMA_NUM*r_weight) {                  \
                    node->right = right->left;                                  \
                    right->left = node;                                         \
                    right->size = node->size;                                   \
                    node->size  = 1;                                            \
                    node->size += (node->left)  ? (node->left->size)  : (0);    \
                    node->size += (node->right) ? (node->right->size) : (0);    \
                    *tree       = right;                                        \
                }                                                               \
                                                                                \
                /* If node->right->left is big: make a double rotation */       \
                else {                                                          \
                    left         = right->left;                                 \
                    right->left  = left->right;                                 \
                    left->right  = right;                                       \
                    node->right  = left->left;                                  \
                    left->left   = node;                                        \
                    left->size   = node->size;                                  \
                    node->size   = 1;                                           \
                    node->size  += (node->left)  ? (node->left->size)  : (0);   \
                    node->size  += (node->right) ? (node->right->size) : (0);   \
                    right->size  = 1;                                           \
                    right->size += (right->left)  ? (right->left->size)  : (0); \
                    right->size += (right->right) ? (right->right->size) : (0); \
                    *tree        = left;                                        \
                }                                                               \
            }                                                                   \
        }                                                                       \
        /*** End of REBALANCE ************************************************/ \
                                                                                \
        /* Report rank of the inserted element */                               \
        return rank;                                                            \
    }                                                                           \
                                                                                \
    static inline type prefix##wtree_select(const prefix##wtree *tree,          \
                                            const size_t rank) {                \
                                                                                \
        prefix##wtree node   = *tree;                                           \
        size_t        target =  rank;                                           \
        size_t        size   = (node) ? (node->size) : (0);                     \
                                                                                \
        /* Preconditions */                                                     \
        assert(1 <= rank && rank <= size);                                      \
                                                                                \
        /* Particular case: Get Min */                                          \
        if (rank == 1) { while (node->left) { node = node->left; } }            \
                                                                                \
        /* Particular case: Get Max */                                          \
        else if (rank == size) { while (node->right) { node = node->right; } }  \
                                                                                \
        /* General case: Perform a O(lg(size)) search */                        \
        else {                                                                  \
            for (;;) {                                                          \
                size = (node->left) ? (node->left->size + 1) : (1);             \
                if (target == size) { break; }                                  \
                else if (target < size) { node = node->left; }                  \
                else { target -= size; node = node->right; }                    \
            }                                                                   \
        }                                                                       \
                                                                                \
        return node->data;                                                      \
    }                                                                           \
                                                                                \
    static inline type prefix##wtree_remove(prefix##wtree *tree,                \
                                            const size_t rank) {                \
                                                                                \
        /* Preconditions */                                                     \
        assert( *tree        != NULL);                                          \
        assert((*tree)->size >= rank);                                          \
        assert(            1 <= rank);                                          \
                                                                                \
        /* Rebalancing constants (DELTA, GAMMA)                          */     \
        /* (5/2, 3/2) -> Worst case = O(2.06 log(size)) ~ Red-Black tree */     \
        /* (3/1, 2/1) -> Worst case = O(2.41 log(size))                  */     \
        /* (1/0, 1/0) -> Worst case = O(size) ~ Standrad Binary Tree     */     \
        const size_t DELTA_NUM = 5;                                             \
        const size_t DELTA_DEN = 2;                                             \
        const size_t GAMMA_NUM = 3;                                             \
        const size_t GAMMA_DEN = 2;                                             \
                                                                                \
        /* Variables */                                                         \
        prefix##wtree next, left, right, node = *tree;                          \
        size_t r_weight, l_weight = (node->left) ? (node->left->size+1) : (1);  \
        type   data;                                                            \
                                                                                \
        /* Case 1: remove from node->left */                                    \
        if (l_weight > rank) {                                                  \
            data = prefix##wtree_remove(&(node->left), rank);                   \
        }                                                                       \
                                                                                \
        /* Case 2: remove from node->right */                                   \
        else if (l_weight < rank) {                                             \
            data = prefix##wtree_remove(&(node->right), rank-l_weight);         \
        }                                                                       \
                                                                                \
        /* Case 3: swap data with min(node->right) and then remove it */        \
        else if (node->left && node->right) {                                   \
            next = node->right;                                                 \
            while (next->left) { next = next->left; }                           \
            data       = node->data;                                            \
            node->data = next->data;                                            \
            next->data = data;                                                  \
            data       = prefix##wtree_remove(&(node->right), 1);               \
        }                                                                       \
                                                                                \
        /* Case 4: remove leaf */                                               \
        else {                                                                  \
            if (node->left) { *tree = node->left;  }                            \
            else            { *tree = node->right; }                            \
            data = node->data;                                                  \
            free(node);                                                         \
            return data;                                                        \
        }                                                                       \
                                                                                \
        /* Update node->size */                                                 \
        l_weight   = (node->left)  ? (node->left->size  + 1) : (1);             \
        r_weight   = (node->right) ? (node->right->size + 1) : (1);             \
        node->size = l_weight + r_weight - 1;                                   \
                                                                                \
        /*** REBALANCE *******************************************************/ \
                                                                                \
        /* If node->left has become too big */                                  \
        if (DELTA_NUM*r_weight < DELTA_DEN*l_weight) {                          \
                                                                                \
            left     = node->left;                                              \
            l_weight = (left->left)  ? (left->left->size  + 1) : (1);           \
            r_weight = (left->right) ? (left->right->size + 1) : (1);           \
                                                                                \
            /* If node->left->right is small: make a single rotation */         \
            if (GAMMA_DEN*r_weight < GAMMA_NUM*l_weight) {                      \
                node->left  = left->right;                                      \
                left->right = node;                                             \
                left->size  = node->size;                                       \
                node->size  = 1;                                                \
                node->size += (node->left)  ? (node->left->size)  : (0);        \
                node->size += (node->right) ? (node->right->size) : (0);        \
                *tree       = left;                                             \
            }                                                                   \
                                                                                \
            /* If node->left->right is big: make a double rotation */           \
            else {                                                              \
                right        = left->right;                                     \
                left->right  = right->left;                                     \
                right->left  = left;                                            \
                node->left   = right->right;                                    \
                right->right = node;                                            \
                right->size  = node->size;                                      \
                node->size   = 1;                                               \
                node->size  += (node->left)  ? (node->left->size)  : (0);       \
                node->size  += (node->right) ? (node->right->size) : (0);       \
                left->size   = 1;                                               \
                left->size  += (left->left)  ? (left->left->size)  : (0);       \
                left->size  += (left->right) ? (left->right->size) : (0);       \
                *tree        = right;                                           \
            }                                                                   \
        }                                                                       \
                                                                                \
        /* If node->left has become too big */                                  \
        else if (DELTA_NUM*l_weight < DELTA_DEN*r_weight) {                     \
                                                                                \
            right    = node->right;                                             \
            l_weight = (right->left)  ? (right->left->size  + 1) : (1);         \
            r_weight = (right->right) ? (right->right->size + 1) : (1);         \
                                                                                \
            /* If node->right->left is small: make a single rotation */         \
            if (GAMMA_DEN*l_weight < GAMMA_NUM*r_weight) {                      \
                node->right = right->left;                                      \
                right->left = node;                                             \
                right->size = node->size;                                       \
                node->size  = 1;                                                \
                node->size += (node->left)  ? (node->left->size)  : (0);        \
                node->size += (node->right) ? (node->right->size) : (0);        \
                *tree       = right;                                            \
            }                                                                   \
                                                                                \
            /* If node->right->left is big: make a double rotation */           \
            else {                                                              \
                left         = right->left;                                     \
                right->left  = left->right;                                     \
                left->right  = right;                                           \
                node->right  = left->left;                                      \
                left->left   = node;                                            \
                left->size   = node->size;                                      \
                node->size   = 1;                                               \
                node->size  += (node->left)  ? (node->left->size)  : (0);       \
                node->size  += (node->right) ? (node->right->size) : (0);       \
                right->size  = 1;                                               \
                right->size += (right->left)  ? (right->left->size)  : (0);     \
                right->size += (right->right) ? (right->right->size) : (0);     \
                *tree        = left;                                            \
            }                                                                   \
        }                                                                       \
        /*** End of REBALANCE ************************************************/ \
                                                                                \
        /* Return the data removed from the tree */                             \
        return data;                                                            \
    }                                                                           \
                                                                                \

#endif
