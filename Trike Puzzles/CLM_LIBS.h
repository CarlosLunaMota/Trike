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
#include <time.h>       /* Time related functions.                           */ 

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
                                                                                
#endif
