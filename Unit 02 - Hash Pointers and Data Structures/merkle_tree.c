#include "merkle_tree.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if defined(__APPLE__)
#include <CommonCrypto/CommonDigest.h>
#define SHA256_DIGEST_LENGTH CC_SHA256_DIGEST_LENGTH
#define SHA256(data, len, hash) CC_SHA256(data, len, hash)
#else
#include <openssl/sha.h>
#endif

#define MAX_HASH_LEN 65
#define HASH_SIZE SHA256_DIGEST_LENGTH
#define MAX_HASH_HEX_LENGTH (HASH_SIZE * 2 + 1)

typedef struct Node {
    unsigned char hash[HASH_SIZE];
    struct Node *left;
    struct Node *right;
    struct Node *parent;
} Node;

struct MerkleTree {
    Node *root;
    Node **leaves;
    int leaf_count;
    int capacity;
};

struct MerkleProof {
    unsigned char **siblings;
    int *directions;
    int count;
};

void *sha256_hash(const void *input, size_t length, unsigned char *output) {
    SHA256((const unsigned char *)input, length, output);
}

char* hash_to_string(const unsigned char *hash) {
    char *output = malloc(MAX_HASH_HEX_LENGTH);
    for(int i = 0; i < HASH_SIZE; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[MAX_HASH_HEX_LENGTH - 1] = 0;
    return output;
}

Node* create_node(const unsigned char *hash) {
    Node *node = malloc(sizeof(Node));
    memcpy(node->hash, hash, HASH_SIZE);
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    return node;
}

MerkleTree* create_merkle_tree() {
    MerkleTree *tree = malloc(sizeof(MerkleTree));
    tree->root = NULL;
    tree->leaves = NULL;
    tree->leaf_count = 0;
    tree->capacity = 0;
    return tree;
}

void add_element(MerkleTree *tree, const char *data) {
    unsigned char hash[HASH_SIZE];
    sha256_hash(data, strlen(data), hash);
    Node *new_node = create_node(hash);

    if (tree->leaf_count == tree->capacity) {
        tree->capacity = tree->capacity == 0 ? 1 : tree->capacity * 2;
        tree->leaves = realloc(tree->leaves, tree->capacity * sizeof(Node*));
    }
    tree->leaves[tree->leaf_count++] = new_node;

    // Rebuild the tree
    int n = tree->leaf_count;
    Node **current_level = tree->leaves;
    while (n > 1) {
        int next_level_size = (n + 1) / 2;
        Node **next_level = malloc(next_level_size * sizeof(Node*));
        
        for (int i = 0; i < n - 1; i += 2) {
            Node *left = current_level[i];
            Node *right = current_level[i + 1];
            
            unsigned char combined[HASH_SIZE * 2];
            memcpy(combined, left->hash, HASH_SIZE);
            memcpy(combined + HASH_SIZE, right->hash, HASH_SIZE);
            
            unsigned char parent_hash[HASH_SIZE];
            sha256_hash(combined, HASH_SIZE * 2, parent_hash);
            Node *parent = create_node(parent_hash);
            
            parent->left = left;
            parent->right = right;
            left->parent = parent;
            right->parent = parent;
            
            next_level[i / 2] = parent;
        }
        
        // If there's an odd number of nodes, promote the last node to the next level
        if (n % 2 != 0) {
            next_level[next_level_size - 1] = current_level[n - 1];
        }
        
        if (current_level != tree->leaves) {
            free(current_level);
        }
        current_level = next_level;
        n = next_level_size;
    }
    
    tree->root = current_level[0];
    if (current_level != tree->leaves) {
        free(current_level);
    }
}

bool find_element(MerkleTree *tree, const char *data) {
    unsigned char hash[HASH_SIZE];
    sha256_hash(data, strlen(data), hash);
    for (int i = 0; i < tree->leaf_count; i++) {
        if (memcmp(tree->leaves[i]->hash, hash, HASH_SIZE) == 0) {
            return true;
        }
    }
    return false;
}

void remove_element(MerkleTree *tree, const char *data) {
    unsigned char hash[HASH_SIZE];
    sha256_hash(data, strlen(data), hash);
    for (int i = 0; i < tree->leaf_count; i++) {
        if (memcmp(tree->leaves[i]->hash, hash, HASH_SIZE) == 0) {
            free(tree->leaves[i]);
            for (int j = i; j < tree->leaf_count - 1; j++) {
                tree->leaves[j] = tree->leaves[j + 1];
            }
            tree->leaf_count--;
            // Rebuild the tree
            MerkleTree *new_tree = create_merkle_tree();
            for (int k = 0; k < tree->leaf_count; k++) {
                add_element(new_tree, hash_to_string(tree->leaves[k]->hash));
            }
            free_merkle_tree(tree);
            *tree = *new_tree;
            free(new_tree);
            return;
        }
    }
}



bool verify_proof(const unsigned char *root_hash, const char *data, MerkleProof *proof) {
    unsigned char current_hash[HASH_SIZE];
    sha256_hash(data, strlen(data), current_hash);
    
    for (int i = 0; i < proof->count; i++) {
        unsigned char combined[HASH_SIZE * 2];
        if (proof->directions[i] == 1) {
            memcpy(combined, current_hash, HASH_SIZE);
            memcpy(combined + HASH_SIZE, proof->siblings[i], HASH_SIZE);
        } else {
            memcpy(combined, proof->siblings[i], HASH_SIZE);
            memcpy(combined + HASH_SIZE, current_hash, HASH_SIZE);
        }
        sha256_hash(combined, HASH_SIZE * 2, current_hash);
    }

    return memcmp(current_hash, root_hash, HASH_SIZE) == 0;
}

void print_node(Node *node, int depth) {
    if (node == NULL) return;
    
    for (int i = 0; i < depth; i++) printf("  ");
    char *hash_str = hash_to_string(node->hash);
    printf("%s\n", hash_str);
    free(hash_str);
    
    print_node(node->left, depth + 1);
    print_node(node->right, depth + 1);
}

void print_tree(MerkleTree *tree) {
    print_node(tree->root, 0);
}

void print_leaf_hashes(MerkleTree *tree) {
    for (int i = 0; i < tree->leaf_count; i++) {
        char *hash_str = hash_to_string(tree->leaves[i]->hash);
        printf("%s\n", hash_str);
        free(hash_str);
    }
}

void free_node(Node *node) {
    if (node == NULL) return;
    free_node(node->left);
    free_node(node->right);
    free(node);
}

void free_merkle_tree(MerkleTree *tree) {
    free_node(tree->root);
    free(tree->leaves);
    free(tree);
}

void free_merkle_proof(MerkleProof *proof) {
    for (int i = 0; i < proof->count; i++) {
        free(proof->siblings[i]);
    }
    free(proof->siblings);
    free(proof->directions);
    free(proof);
}

const unsigned char* get_root_hash(MerkleTree *tree) {
    if (tree && tree->root) {
        return tree->root->hash;
    }
    return NULL;
}

int get_proof_count(MerkleProof *proof) {
    if (proof) {
        return proof->count;
    }
    return 0;
}

const unsigned char* get_proof_sibling(MerkleProof *proof, int index) {
    if (proof && index >= 0 && index < proof->count) {
        return proof->siblings[index];
    }
    return NULL;
}
MerkleProof* generate_proof(MerkleTree *tree, const char *data) {
    unsigned char hash[HASH_SIZE];
    sha256_hash(data, strlen(data), hash);

    // Căutăm frunza care conține hash-ul datelor
    Node *leaf = NULL;
    int leaf_index = -1;
    for (int i = 0; i < tree->leaf_count; i++) {
        if (memcmp(tree->leaves[i]->hash, hash, HASH_SIZE) == 0) {
            leaf = tree->leaves[i];
            leaf_index = i;
            break;
        }
    }

    if (leaf == NULL) {
        printf("Elementul nu a fost gasit în arbore.\n");
        return NULL;
    }

    // Inițializăm structura MerkleProof
    MerkleProof *proof = malloc(sizeof(MerkleProof));
    proof->count = 0;
    proof->siblings = NULL;
    proof->directions = NULL;

    Node *current_node = leaf;

    // Urcăm în arbore până la rădăcină și colectăm hash-urile fraților
    while (current_node->parent != NULL) {
        Node *parent = current_node->parent;
        unsigned char *sibling_hash = NULL;

        if (parent->left == current_node) {
            sibling_hash = parent->right ? parent->right->hash : parent->left->hash;
            proof->directions = realloc(proof->directions, (proof->count + 1) * sizeof(int));
            proof->directions[proof->count] = 1; // Direcția stânga (1)
        } else {
            sibling_hash = parent->left ? parent->left->hash : parent->right->hash;
            proof->directions = realloc(proof->directions, (proof->count + 1) * sizeof(int));
            proof->directions[proof->count] = 0; // Direcția dreapta (0)
        }

        proof->siblings = realloc(proof->siblings, (proof->count + 1) * sizeof(unsigned char *));
        proof->siblings[proof->count] = malloc(HASH_SIZE);
        memcpy(proof->siblings[proof->count], sibling_hash, HASH_SIZE);
        proof->count++;

        current_node = parent; // Urcăm la următorul nod
    }

    return proof;
}
void read_students_from_file(MerkleTree *tree, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Eroare la deschiderea fișierului");
        return;
    }

    char name[100]; 
    while (fgets(name, sizeof(name), file)) {
        // Elimină caracterul de nouă linie
        name[strcspn(name, "\n")] = 0;

        char *start = strchr(name, '"');
        char *end = strrchr(name, '"');
        if (start && end && start != end) {
            // Muta pointerele pentru a sări peste ghilimele
            start++;
            *end = '\0'; // Transformă ultimul ghilimele în terminator de șir
            add_element(tree, start);
        }
    }

    fclose(file);
}
int main() {
    MerkleTree *tree = create_merkle_tree();

    // Lista studenților - exemplu simplu (înlocuiește cu lista reală)
    // const char *students[] = {
    //     "Alice", "Bob", "Charlie", "David", "Eve"
    // };

    // // Adaugă fiecare student în arbore
    // for (int i = 0; i < 5; i++) {
    //     add_element(tree, students[i]);
    // }

    read_students_from_file(tree, "students.txt");
    
    // Afișează hash-ul rădăcinii arborelui Merkle
    const unsigned char *root_hash = get_root_hash(tree);
    char *root_hash_str = hash_to_string(root_hash);
    printf("Hash-ul radacinii Merkle: %s\n", root_hash_str);
    free(root_hash_str);

    // Generarea unui proof pentru un student specificat (ex: "Charlie")
    const char *student_name = "Horatiu Lazea";
    MerkleProof *proof = generate_proof(tree, student_name);

    // Afișează proof-ul generat
    if (proof != NULL) {
        printf("Proof pentru %s:\n", student_name);
        for (int i = 0; i < proof->count; i++) {
            char *sibling_hash_str = hash_to_string(proof->siblings[i]);
            printf("Sibling %d (%s): %s\n", i, proof->directions[i] == 1 ? "stanga" : "dreapta", sibling_hash_str);
            free(sibling_hash_str);
        }

        // Verifică proof-ul
        if (verify_proof(root_hash, student_name, proof)) {
            printf("Proof valid pentru %s!\n", student_name);
        } else {
            printf("Proof invalid pentru %s!\n", student_name);
        }

        free_merkle_proof(proof);
    }

    free_merkle_tree(tree);
    return 0;
}
