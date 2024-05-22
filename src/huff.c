#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "huff.h"
#include "debug.h"
#include "helper.h" // helper function header file

#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

/**
 * @brief Emits a description of the Huffman tree used to compress the current block.
 * @details This function emits, to the standard output, a description of the
 * Huffman tree used to compress the current block.
 */
void emit_huffman_tree() {
    // To be implemented.
    // Num nodes portion of emit_huffman_tree
    fputc((num_nodes >> 8) & 0xFF , stdout);
    fputc(num_nodes & 0xFF, stdout);
    int seven = 7;
    unsigned char buffer = 0;
    int *count = &seven;
    unsigned char *buffer_ptr = &buffer;
    trace_back();
    post_order_traversal(nodes, buffer_ptr, count);
    if(*count != 7){
        fputc(buffer, stdout);
    }
    dfs(nodes);
}

/**
 * @brief Reads a description of a Huffman tree and reconstructs the tree from
 * the description.
 * @details  This function reads, from the standard input, the description of a
 * Huffman tree in the format produced by emit_huffman_tree(), and it reconstructs
 * the tree from the description.
 *
 * @return 0 if the tree is read and reconstructed without error, 1 if EOF is
 * encountered at the start of a block, otherwise -1 if an error occurs.
 */
int read_huffman_tree() {
    NODE *stack_ptr = nodes;
    unsigned char start = fgetc(stdin);
    if(start == EOF){
        return 1;
    }
    else{
        unsigned char byte2 = fgetc(stdin);
        if(feof(stdin)){
            return -1;
        }
        unsigned int combined = (start << 8) | byte2;
        num_nodes = combined;
        NODE *high_end_ptr = nodes + num_nodes;
        int node_count = 0;
        // Extract bits
        while(node_count != num_nodes){
            unsigned char byte = fgetc(stdin);
            if(byte == EOF){
                return -1;
            }
            for (int i = 7; i >= 0; i--) {
                unsigned char bit = (byte >> i) & 1;
                // Push
                if(bit == 0){
                    NODE new_node = {NULL, NULL, NULL, 1, 0};
                    *(stack_ptr) = new_node;
                    stack_ptr++;
                    node_count++;
                }
                // Pop
                else{
                    stack_ptr--;
                    NODE R = *stack_ptr;
                    *high_end_ptr = R;
                    NODE *r_ptr = high_end_ptr;
                    high_end_ptr--;
                    stack_ptr--;
                    NODE L = *stack_ptr;
                    *high_end_ptr = L;
                    NODE *l_ptr = high_end_ptr;
                    high_end_ptr--;
                    NODE P = {l_ptr, r_ptr, NULL, 1, 300};
                    *stack_ptr = P;
                    stack_ptr++;
                    node_count++;
                }
                if(node_count == num_nodes){
                    break;
                }
            }
        }

        for (int i = 0; i < num_nodes; i++) {
             if((*(nodes + i)).left && (*(nodes + i)).right){
                (*(nodes + i)).left -> parent = nodes + i;
                (*(nodes + i)).right -> parent = nodes + i;
             }
        }
        if (dfs_assign_value(nodes) == -1){
            return -1;
        }
    }
    return 0;
}

/**
 * @brief Reads one block of data from standard input and emits corresponding
 * compressed data to standard output.
 * @details This function reads raw binary data bytes from the standard input
 * until the specified block size has been read or until EOF is reached.
 * It then applies a data compression algorithm to the block and outputs the
 * compressed block to the standard output.  The block size parameter is
 * obtained from the global_options variable.
 *
 * @return 0 if compression completes without error, -1 if an error occurs.
 */
int compress_block() {
    int seven = 7;
    unsigned char buffer = 0;
    int *count = &seven;
    unsigned char *buffer_ptr = &buffer;
    if(get_frequencies() == -1){
        return -1;
    }
    int weight = build_tree();
    emit_huffman_tree();
    encode(buffer_ptr, count, weight);
    if(*count != 7){
        fputc(buffer, stdout);
    }

    return 0;
}

/**
 * @brief Reads one block of compressed data from standard input and writes
 * the corresponding uncompressed data to standard output.
 * @details This function reads one block of compressed data from the standard
 * input, it decompresses the block, and it outputs the uncompressed data to
 * the standard output.  The input data blocks are assumed to be in the format
 * produced by compress().  If EOF is encountered before a complete block has
 * been read, it is an error.
 *
 * @return 0 if decompression completes without error, -1 if an error occurs.
 */
int decompress_block() {
    int code = read_huffman_tree();
    if (code == -1){
        return -1;
    }
    else if(code == 1) {
        return 0;
    }
    // Decode
    NODE *search_ptr = nodes;
    while(search_ptr -> symbol != 400){
        unsigned char byte = fgetc(stdin);
        if(feof(stdin)){
            return -1;
        }
        // printf("%x\n", byte);
        for (int i = 7; i >= 0; i--) {
            unsigned char bit = (byte >> i) & 1;
            if(bit == 0){
                    search_ptr = search_ptr -> left;
                    if((search_ptr -> symbol) == 400){
                        break;
                    }
                    else if((search_ptr -> symbol != 300)){
                        fputc(search_ptr->symbol,stdout);
                        search_ptr = nodes;
                    }
            }
            else{
                    search_ptr = search_ptr -> right;
                    if((search_ptr -> symbol) == 400){
                        break;
                    }
                    else if((search_ptr -> symbol != 300)){
                        fputc(search_ptr->symbol,stdout);
                        search_ptr = nodes;
                    }
            }
        }
        if((search_ptr -> symbol) == 400){
                break;
        }
    }
    return 0;
}

/**
 * @brief Reads raw data from standard input, writes compressed data to
 * standard output.
 * @details This function reads raw binary data bytes from the standard input in
 * blocks of up to a specified maximum number of bytes or until EOF is reached,
 * it applies a data compression algorithm to each block, and it outputs the
 * compressed blocks to standard output.  The block size parameter is obtained
 * from the global_options variable.
 *
 * @return 0 if compression completes without error, -1 if an error occurs.
 */
int compress() {
    // To be implemented.
    while (!feof(stdin)) {
        if (compress_block() == -1){
            return -1;
        }
    }
    fflush(stdout);
    return 0;
}

/**
 * @brief Reads compressed data from standard input, writes uncompressed
 * data to standard output.
 * @details This function reads blocks of compressed data from the standard
 * input until EOF is reached, it decompresses each block, and it outputs
 * the uncompressed data to the standard output.  The input data blocks
 * are assumed to be in the format produced by compress().
 *
 * @return 0 if decompression completes without error, -1 if an error occurs.
 */
int decompress() {
    while (!feof(stdin)) {
        int code = decompress_block();
        if (code == -1){
            return -1;
        }
        else if (code == 1){
            return 0;
        }
        else{
            continue;
        }
    }
    fflush(stdout);
    return 0;
}

/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the
 * program, returning 0 if validation succeeds and -1 if validation fails.
 * Upon successful return, the selected program options will be set in the
 * global variable "global_options", where they will be accessible
 * elsewhere in the program.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @return 0 if validation succeeds and -1 if validation fails.
 * @modifies global variable "global_options" to contain a bitmap representing
 * the selected options.
 */
int validargs(int argc, char **argv) {
    // To be implemented.
    int position = 1; // Keeps track of position of argument
    int d_count = 0; // Keeps track of d flags
    int c_count = 0; // Keeps track of c flags
    int b_count = 0; // Keeps track of b flags
    int d_position = 0; // Makes sure d is in the right position
    int c_position = 0; // Makes sure c is in the right position
    int b_position = 0; // Makes sure b is in the right position
    int block_size = 0; // Holds the blocksize argument if present
    // Too few arguments
    if (argc < 2){
        fprintf(stderr, "Error: Too few arguments\n");
        return -1;
    }
    for (char **p = argv + 1; *p != NULL; p++) {
        // h flag handling
        if (string_compare(*p, "-h") == 0 && position == 1) {
            global_options = 0x1;
            return 0;
        }
        // No h flag, check to see if # of args is too many
        if (argc > 4) {
            fprintf(stderr, "Error: Too many arguments\n");
            return -1;
        }
        // Checks # of -d, if greater than 1 validation fails
        if (string_compare(*p, "-d") == 0) {
            // Check if d is first
            if (string_compare(*p, "-d") == 0 && position == 1) {
                d_position = position;
            }
            d_count++;
            if (d_count > 1 || c_count > 0) {
                fprintf(stderr, "Error: Number of d or c flags invalid\n");
                return -1;
            }
        }
        // Checks # of -c, if greater than 1 validation fails
        if (string_compare(*p, "-c") == 0) {
            // Checks if c is first
            if (string_compare(*p, "-c") == 0 && position == 1) {
                c_position = position;
            }
            c_count++;
            if (c_count > 1 || d_count > 0) {
                fprintf(stderr, "Error: Number of d or c flags invalid\n");
                return -1;
            }
        }
        // Checks # of -b, if greater than 1 validation fails
        if (string_compare(*p, "-b") == 0) {
            // Checks if b is the second argument
            if (string_compare(*p, "-b") == 0 && position == 2) {
                b_position = position;
            }
            b_count++;
            if (b_count > 1 || !(c_count == 1)) {
                fprintf(stderr, "Error: Number or ordering of b or c flags invalid\n");
                return -1;
            }
        }
        // Handling a given blocksize
        if (position == 3){
            block_size = string_to_int(*p);
            if (block_size == -1){
                fprintf(stderr, "Error: Invalid character found\n");
                return -1;
            }
            else if (block_size < 1024 || block_size > 65536){
                fprintf(stderr, "Error: Invalid block size\n");
                return -1;
            }
        }

        position++;
    }

    // Handles operations after argv is parsed
    if (d_position == 1 && argc == 2){
        int default_block_size = 65536 - 1;
        int block_size_converted = (default_block_size << 16);
        block_size_converted |= (1<<2);
        global_options = block_size_converted;

        return 0;
    } else if (c_position == 1 && block_size == 0 && argc == 2){
        int default_block_size = 65536 - 1;
        int block_size_converted = (default_block_size << 16);
        block_size_converted |= (1<<1);
        global_options = block_size_converted;

        return 0;
    } else if (c_position == 1 && !(block_size == 0) && b_position == 2){
        int block_size_converted = ((block_size - 1) << 16);
        block_size_converted |= (1<<1);
        global_options = block_size_converted;

        return 0;
    }
    fprintf(stderr, "Error: Invalid combination of arguments\n");
    return -1;
}

// Function to compare strings
int string_compare(char *s1, char *s2){
    while(*s1 && *s2 && *s1 == *s2){
        s1++;
        s2++;
    }
    if (*s1 == *s2) {
        return 0;
    } else {
        return -1;
    }
}

// Function to convert a string to an integer
int string_to_int(char *str){
    int result = 0;
    // Check if all characters are digits
    while (*str) {
        if (*str < '0' || *str > '9') {
            return -1; // Not a digit, break out of the loop
        }
        result = result * 10 + (*str - '0'); // Get the numeric value from ASCII
        str++;
    }

    return result;
}

// Function to convert an integer to a string
void int_to_string(int num, char *str) {
    if (num < 0) {
        *str++ = '-';
        num = -num;
    }
    if (num == 0) {
        *str++ = '0';
        *str = '\0';
        return;
    }
    char *end = str;
    while (num > 0) {
        *end++ = '0' + (num % 10);
        num /= 10;
    }
    *end = '\0';
    char *start = str;
    while (start < end) {
        char temp = *start;
        *start++ = *--end;
        *end = temp;
    }
}

// Function to get the frequencies of a symbols and add nodes to the node array
int get_frequencies(){
    int c; // Variable to store the character read from stdin
    NODE *node_ptr = nodes; // Pointer to node array
    int block_count = 0;
    int block_count_converted;
    num_nodes = 0; // Initialize number of nodes to 0
    // Initialize Node pointer array to NULL
    for(int i = 0; i < (MAX_SYMBOLS); i++){
        (*(node_for_symbol + i)) = NULL;
    }
    // Read characters from stdin until EOF or end of block is encountered
    while (block_count_converted != global_options && (c = fgetc(stdin)) != EOF) {
        if (*(node_for_symbol + c) == NULL){
            NODE new_node = {NULL, NULL, NULL, 1 ,c};
            *node_ptr = new_node;
            *(node_for_symbol + c) = node_ptr;
            (*(current_block + block_count)) = c;
            node_ptr++;
            num_nodes++; // Increment number of nodes variable
        }
        else{
            (*(node_for_symbol + c)) -> weight = ((*(node_for_symbol + c)) -> weight + 1);
            (*(current_block + block_count)) = c;
        }
        block_count++;
        block_count_converted = ((block_count-1)<<16);
        block_count_converted |= (1<<1);
    }
    if(c == EOF && block_count == 0){
        return -1;
    }
    // Create an END node and add it to the end of the nodes array
    NODE end = {NULL, NULL, NULL, 0, 400};
    *node_ptr = end;
    num_nodes++; // Increment number of nodes variable
    return 0;

}

// Displays array of nodes
void display_nodes(){
    int i = 0;
    while(i < num_nodes){
        if((*(nodes + i)).left && (*(nodes+i)).right && (*(nodes + i)).parent){
            printf("Index: %d Weight: %d, Symbol: %d left: %p right: %p parent: %p\n", i,
                (*(nodes + i)).weight, (*(nodes + i)).symbol, (*(nodes + i)).left , (*(nodes + i)).right, (*(nodes + i)).parent);
            i++;
        }
        else if((*(nodes + i)).left && (*(nodes+i)).right){
            printf("Index: %d Weight: %d, Symbol: %p left: %p right: %p\n", i, (*(nodes + i)).weight, (nodes + i), (*(nodes + i)).left, (*(nodes + i)).right);
            i++;
        }
        else if((*(nodes + i)).parent){
            printf("Index: %d Weight: %d, Symbol: %d parent: %p\n", i, (*(nodes + i)).weight, (*(nodes + i)).symbol, (*(nodes + i)).parent);
            i++;
        }
        else{
            printf("Index: %d Weight: %d, Symbol: %d parent: %p\n", i, (*(nodes + i)).weight, (*(nodes + i)).symbol, (*(nodes + i)).parent);
            i++;
        }
    }
}

//Displays array of node pointers
void display_node_pointers(){
    for(int i = 0; i < (MAX_SYMBOLS); i++){
        if((*(node_for_symbol + i))){
        printf("Index: %d Weight: %d, Symbol: %d\n", i, (*(*(node_for_symbol + i))).weight, (*(*(node_for_symbol + i))).symbol);
        }
    }
}

// Builds huffman tree
int build_tree(){
    NODE *node_to_combine1; // Pointer to the current minimum weight node
    NODE *node_to_combine2; // Pointer to another current minimum weight node
    int sum; // Store sum of minimum weights
    int index;
    int index2;
    int remaining_nodes = num_nodes; // Variable to track how many nodes are left so the loop can end
    int node_index; // Variable to save the index to put the combined parent node
    int remove_index; // Variable to save the index to remove a node
    while(remaining_nodes != 1){
        if ( ( (*nodes).weight && (*nodes).weight != 0) || (*nodes).symbol == 400){
            node_to_combine1 = nodes;
            index = 0;
        }
        if (((*(nodes + 1)).weight && (*(nodes + 1)).weight != 0) || (nodes + 1)->symbol == 400){
            node_to_combine2 = nodes + 1;
            index2 = 1;
        }
        // Loop through nodes array and get minimum nodes
        if (node_to_combine1 && node_to_combine2){
            for(int i = 2; i < remaining_nodes; i++){
                if((*(nodes+i)).symbol == 400 || (*(nodes+i)).weight != 0){
                  int weight = (*(nodes+i)).weight;
                      if(weight < (*node_to_combine1).weight  || weight < (*node_to_combine2).weight){
                            if((*node_to_combine1).weight > (*node_to_combine2).weight){
                                node_to_combine1 = nodes + i;
                                index = i;
                            }
                            else if ((*node_to_combine2).weight > (*node_to_combine1).weight){
                                node_to_combine2 = nodes + i;
                                index2 = i;
                            }
                            else{
                                node_to_combine1 = nodes + i;
                                index = i;
                        }
                    }
                }
            }
            remaining_nodes--;
        }

        if (index < index2){
            node_index = index;
            remove_index = index2;
        }
        else{
            node_index = index2;
            remove_index = index;
        }

        // Create new nodes to move content
        NODE new_node1 = {NULL, NULL, NULL, node_to_combine1->weight, node_to_combine1->symbol};
        NODE new_node2 = {NULL, NULL, NULL, node_to_combine2->weight, node_to_combine2->symbol};

        // Add minimum weights
        sum = new_node1.weight + new_node2.weight;

        // Create combined node
        NODE combined_node = {NULL, NULL, NULL, sum, 300}; // Make symbol 300 to distinguish from default 256 ASCII characters

        // Assign parent node
        *(nodes + node_index) = combined_node;

        // Shift the nodes to maintain a contiguous sequence
        for (int i = remove_index; i < num_nodes; i++ ){
            *(nodes + i) = *(nodes + i + 1);

        }
        // Reposition nodes
        if ( new_node1.weight < new_node2.weight){
            *(nodes + num_nodes) = new_node2;
            *(nodes + num_nodes - 1) = new_node1;
        }
        else{
            *(nodes + num_nodes) = new_node1;
            *(nodes + num_nodes - 1) = new_node2;
        }
        num_nodes++;
    }
    // Assign pointers to the nodes in nodes array

    // Assign children to root node
    nodes->left = (nodes + num_nodes - 2);
    nodes->right = (nodes + num_nodes - 1);

    // Start at the 2nd element (not root)
    NODE *nodes_loop_ptr1 = nodes + 1; // Slow pointer to mark the children
    NODE *nodes_loop_ptr2 = nodes + 1; // Pointer to search for 0 symbols (parent)
    while(nodes_loop_ptr2->weight != 0 || nodes_loop_ptr2->symbol == 400){
        if(nodes_loop_ptr2->symbol == 300){
            nodes_loop_ptr2 -> left = nodes_loop_ptr1;
            nodes_loop_ptr1 -> parent = nodes_loop_ptr2;
            nodes_loop_ptr1++;
            nodes_loop_ptr2 -> right = nodes_loop_ptr1;
            nodes_loop_ptr1 -> parent = nodes_loop_ptr2;
            nodes_loop_ptr1++;
            nodes_loop_ptr2++;
        }
        else{
            nodes_loop_ptr2++;
        }

    }
    // Assign parent to last two nodes
    (nodes + num_nodes - 2) -> parent = nodes;
    (nodes + num_nodes - 1) -> parent = nodes;

    // Fix node pointer array
    for(int i = 0; i < num_nodes; i++){
        if( (nodes + i) -> symbol != 300 && (nodes + i) -> symbol !=400){
            *(node_for_symbol + ((nodes + i) -> symbol)) = nodes + i;
        }
    }
    return nodes -> weight;
}

// Assigns 0 or 1 to weights of nodes
void trace_back(){
    NODE *trace_back_ptr;
    for(int i = 0; i < MAX_SYMBOLS; i++){
        if ( (*(node_for_symbol + i)) ){
            trace_back_ptr = *(node_for_symbol + i);
            trace_back_ptr -> weight = 0;
            while (trace_back_ptr -> parent != NULL){
                trace_back_ptr = trace_back_ptr -> parent;
                if (trace_back_ptr -> symbol == 300){
                    trace_back_ptr -> weight = 1;
                }
            }
        }

    }
}

// Prints a post order traversal of the huffman tree
void post_order_traversal(NODE *root, unsigned char *buffer, int* count){
    if (root != NULL) {
        post_order_traversal(root->left, buffer, count);
        post_order_traversal(root->right, buffer, count);
        emitBit(root -> weight, buffer, count);
    }
}

void emitBit(int bit, unsigned char *buffer, int *count) {
    // Add the bit to the buffer
    *buffer |= (bit & 0x01) << *count;

    // Increment the counter
    (*count)--;

    // If the buffer is full, emit the byte and reset the buffer and counter
    if (*count == -1) {
        fputc(*buffer, stdout);
        *buffer = 0;
        *count = 7;
    }
}

void dfs(NODE *root){
    if (root == NULL) {
        return;
    }

    // If its the END symbol
    if (root->left == NULL && root->right == NULL && root->symbol == 400){
        fputc(255, stdout);
        fputc(0, stdout);
    }
    else if (root->left == NULL && root->right == NULL) {
        fputc(root->symbol, stdout);
    }

    // Recursively find leaf nodes in the left subtree
    dfs(root->left);

    // Recursively find leaf nodes in the right subtree
    dfs(root->right);
}

void dfs_clear(NODE *root){
    if (root == NULL) {
        return;
    }

    root -> weight = 300;

    // Recursively find leaf nodes in the left subtree
    dfs_clear(root->left);

    // Recursively find leaf nodes in the right subtree
    dfs_clear(root->right);
}

int dfs_assign_value(NODE *root){
    if (root == NULL) {
        return -1;
    }
    if(feof(stdin)){
        return -1;
    }

    if (root -> left == NULL && root -> right == NULL){
        unsigned char c = fgetc(stdin);
        if (c != 255){
            root -> symbol = c;
        }
        else{
            if(fgetc(stdin) == 0){
                root -> symbol = 400;
            }
            else{
                root -> symbol = c;
            }
        }
    }
    // Recursively find leaf nodes in the left subtree
    dfs_assign_value(root->left);

    // Recursively find leaf nodes in the right subtree
    dfs_assign_value(root->right);

    return 0;
}

void encode(unsigned char *buffer, int *count, int weight){
    NODE *code_ptr = nodes;
    int i = 0;
    while((weight) != i){
        trace_back_encode((*(current_block + i)));
        while(code_ptr-> symbol != (*(current_block + i))){
            if((*(code_ptr -> left)).weight == (*(current_block + i))){
                emitBit(0, buffer, count);
                if( code_ptr -> left){
                    code_ptr = code_ptr -> left;
                }
            }
            else{
                emitBit(1, buffer, count);
                if( code_ptr -> right){
                    code_ptr = code_ptr -> right;
                }
            }
        }
        i++;
        code_ptr = nodes;
        dfs_clear(nodes);
    }
    NODE *trace_back_ptr = nodes + 1;
    while (trace_back_ptr -> parent != NULL){
        trace_back_ptr -> weight = 400;
        if( trace_back_ptr -> parent){
            trace_back_ptr = trace_back_ptr -> parent;
        }
    }
    code_ptr = nodes;
    while(code_ptr-> symbol != 400){
            if((*(code_ptr -> left)).weight == 400){
                emitBit(0, buffer, count);
                if( code_ptr -> left){
                    code_ptr = code_ptr -> left;
                }
            }
            else{
                emitBit(1, buffer, count);
                if( code_ptr -> right){
                    code_ptr = code_ptr -> right;
                }
            }
        }
}

// Assigns come from instructions to weights of nodes
void trace_back_encode(unsigned char c){
    NODE *trace_back_ptr;
    trace_back_ptr = (*(node_for_symbol + c));
    unsigned char symbol = (*(node_for_symbol + c))->symbol;
    while (trace_back_ptr -> parent != NULL){
        trace_back_ptr -> weight = symbol;
        if( trace_back_ptr -> parent){
            trace_back_ptr = trace_back_ptr -> parent;
        }
    }
}
