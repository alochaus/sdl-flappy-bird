#include<stdio.h>
#include<SDL2/SDL.h>

typedef struct {
    SDL_Rect data;
    int counted;

    struct LinkedListNode* next;
} LinkedListNode;

typedef struct {
    int length;

    LinkedListNode* head;
    LinkedListNode* tail;
} LinkedList;

LinkedListNode* LinkedListNode_init() {
    LinkedListNode* node = malloc(sizeof(LinkedListNode));
    node->next = NULL;
    node->counted = 0;

    node->data.x = 0;
    node->data.y = 0;
    node->data.w = 0;
    node->data.h = 0;

    return node;
}

LinkedList* LinkedList_init() {
    LinkedList* ll = malloc(sizeof(LinkedList));

    ll->length = 0;
    ll->head = NULL;
    ll->tail = NULL;

    return ll;
}

void LinkedList_insert(SDL_Rect rect, LinkedList* le) {
    LinkedListNode* new_node = LinkedListNode_init();

    if(!le->head) {
        le->head = new_node;
        le->tail = new_node;
    }

    le->tail->next = new_node;
    le->tail = new_node;

    new_node->data = rect;

    le->length++;
}

void LinkedList_delete(int index, LinkedList* le) {
    printf("DELETANDO %d\n", index);

    LinkedListNode* head = le->head;

    // if(index >= le->length) {
    //     return;
    // }

    if(index == 0) {
        le->head = le->head->next;
        free(head);
        le->length--;
        return;
    }

    // PERCORRA A LISTA INTEIRA
    for(int i = 1; i < le->length; i++) {
        // ATÉ ENCONTRAR O ÍNDICE ANTERIOR
        if (i == index-1) break;
        head = head->next;
    }

    LinkedListNode* node_that_will_be_deleted = head->next;

    head->next = node_that_will_be_deleted->next;
    free(node_that_will_be_deleted);
    le->length--;
}

LinkedListNode* LinkedList_search(LinkedList* ll, int index) {
    LinkedListNode* head = ll->head;

    // CASO DE ERRO
    if (head == NULL) {
        printf("[]\n");
        return;
    }

    for(int i=0; i<index; i++) {
        //printf("%i, ", cabeca->data);
        head = head->next;
    }

    return head;
}

void LinkedList_pop(LinkedList* ll) {
    LinkedList_delete(0, ll);
}

void print(LinkedList* le) {
    LinkedListNode* cabeca = le->head;

    // CASO DE ERRO
    if (cabeca == NULL) {
        printf("[]\n");
        return;
    }

    printf("[");
    while(cabeca) {
        printf("%i, ", cabeca->data);
        cabeca = cabeca->next;
    }
    printf("\b\b]\n");
}