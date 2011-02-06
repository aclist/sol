#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include "stack.h"

static bool maneuvre_stack(struct stack *stack) {
  return(stack->card->frame->start_y >= MANEUVRE_STACKS_STARTING_Y);
}

static bool waste_pile_stack(struct stack *stack) {
  return((stack->card->frame->start_x == WASTE_PILE_STARTING_X) &&
         (stack->card->frame->start_y == WASTE_PILE_STARTING_Y));
}

static void refresh_card_coordinates(struct stack *origin, struct stack *destination) {
  origin->card->frame->start_x = destination->card->frame->start_x;
  origin->card->frame->start_y = destination->card->frame->start_y;
  if (!empty(destination) && maneuvre_stack(destination)) {
    origin->card->frame->start_y++;
  }

  return;
}

void allocate_stack(struct stack **stack) {
  if (!(*stack = malloc(sizeof(**stack)))) {
    fprintf(stderr, "%s: %s (%s:%d)\n", program_name, strerror(errno), __FILE__, __LINE__ - 1);
    exit(errno);
  }

  allocate_card(&((*stack)->card));

  return;
}

void initialize_stack(struct stack *stack) {
  initialize_card(stack->card);
  stack->next = NULL;

  return;
}

void delete_stack(struct stack *stack) {
  delete_card(stack->card);
  free(stack);

  return;
}

bool empty(struct stack *stack) {
  return(stack->card->value == NO_VALUE);
}

int length(struct stack *stack) {
  struct stack *iterator = stack;
  int length = 0;

  if (!empty(stack)) {
    length = 1;
    while (iterator->next != NULL) {
      length++;
      iterator = iterator->next;
    }
  }

  return(length);
}

void push(struct stack **stack, struct card *card) {
  struct stack *new_stack = NULL;

  if (empty(*stack)) {
    (*stack)->card = card;
  } else {
    allocate_stack(&new_stack);
    new_stack->card = card;
    new_stack->next = (*stack);
    *stack = new_stack;
  }
}

/* FIXME: hack hack hack to get the old coordinates after clearing the structure */
struct stack *pop(struct stack **stack) {
  struct stack *popped_entry = NULL;

  if(!empty(*stack)) {
    popped_entry = *stack;
    if (length(*stack) == 1) {
      int start_y, start_x;
      start_y = (*stack)->card->frame->start_y;
      start_x = (*stack)->card->frame->start_x;
      allocate_stack(stack);
      initialize_stack(*stack);
      set_frame((*stack)->card->frame, start_y, start_x);
    } else {
      *stack = (*stack)->next;
    }
    popped_entry->next = NULL;
  }

  return(popped_entry);
}

void move_card(struct stack **origin, struct stack **destination) {
  struct stack *stack = NULL;

  refresh_card_coordinates(*origin, *destination);
  stack = pop(origin);
  push(destination, stack->card);

  return;
}
