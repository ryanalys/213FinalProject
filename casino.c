#include "casino.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct time {
  int startTime;
  int closingTime;
  int hour;
  int minute;
} ourtime_t;

ourtime_t ourClock;

typedef struct blackJackData {
  int count;
  pthread_mutex_t bjLock;
} bjdata_t;

typedef struct Node {
  int suit;
  int value;
  struct Node* next;
} node_t;

//char cardValues[13] = {'2', '3', '4', '5', '6', '7', '8', '9', '1', 'K', 'J', 'Q', 'A'};

typedef struct Deck {
  pthread_mutex_t lock;
  int cardsLeft;
  struct Node* first;
} deck_t;

deck_t blackJackDeck;

void initDeck(deck_t* deck) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 13; j++) {
      node_t newNode;
      newNode.suit = i;
      newNode.value = j;
      newNode.next = deck->first;
      deck->first = &newNode;
    }
  }
  pthread_mutex_init(&(blackJackDeck.lock), NULL);
}

// 0 if you're still playing, 1 if you decide to quit totally
int quitForever;

// 0 if you're still playing, 1 if you decide to quit for tonight
int quitTonight;

void dayOne() {
  printf("Hello newcomer! Welcome to Stardrop Casino.\n");
  printf("To start off, we request all new patrons enter $100 to place their first bets.\n");
  printf("....There. You can always get more money by betting!\n Have a plesant day.\n\n\n");
}

int options() {
  int choice = 0;
  int attempts = 1;
  char* readBuffer;
  size_t charSize = 0;
  printf("Enter the integer corresponding to one of the following choices\n");
  printf("     1. TicTacToe\n"
           "     2. BlackJack\n"
           "     3. Leave for the day\n"
           "     4. Leave forever     ");
  getline(&readBuffer, &charSize, stdin);
  choice = atoi(readBuffer);
  if (choice <= 0) {
    choice = 9;
  }
  while ((choice > 4) && (attempts < 6)) {
    printf("That is not a valid choice.\n");
    attempts++;
    printf("Enter the integer corresponding to one of the following choices\n");
    printf("     1. TicTacToe\n"
           "     2. BlackJack\n"
           "     3. Leave for the day\n"
           "     4. Leave forever \n");
    getline(&readBuffer, &charSize, stdin);
    choice = atoi(readBuffer);
  }
  if (attempts == 5) {
    printf("Please take this seriously. It is a casino. It is not time for fun and goofing around.\n");
    printf("You will be escorted out. You are not welcome back\n");
    choice = 0;
    quitForever = 1;
  }
  return choice;
}

void ticTacToe() {
  printf("We have successfully reached ticTacToe!\n");
  //Stub
}

void printCards(node_t first, node_t second) {
  char cardValues[] = {'2', '3', '4', '5', '6', '7', '8', '9', '1', 'K', 'J', 'Q', 'A'};
  char firstVal = cardValues[first.value];
  char secondVal = cardValues[second.value];

  if (first.suit == 0) {
    printf("First card is the %c of Clubs\n\n", firstVal);
  } else if (first.suit == 1) {
    printf("First card is the %c of Diamonds\n\n", firstVal);
  } else if (first.suit == 2) {
    printf("First card is the %c of Spades\n\n", firstVal);
  } else {
    printf("First card is the %c of Hearts\n\n", firstVal);
  }
  printf("%d first %d second", first.value, second.value);

  if (second.suit == 0) {
    printf("Second card is the %c of Clubs\n", secondVal);
  } else if (second.suit == 1) {
    printf("Second card is the %c of Diamonds\n", secondVal);
  } else if (second.suit == 2) {
    printf("Second card is the %c of Spades\n", secondVal);
  } else {
    printf("Second card is the %c of Hearts\n", secondVal);
  }
}

void* npcBlackJack(void* args) {
  bjdata_t *data = (bjdata_t *)args;
  pthread_mutex_lock(&blackJackDeck.lock);
  node_t card1 = *(blackJackDeck.first);
  blackJackDeck.first = (blackJackDeck.first)->next;
  node_t card2 = *(blackJackDeck.first);
  blackJackDeck.first = (blackJackDeck.first)->next;
  pthread_mutex_unlock(&blackJackDeck.lock);
  int count = (data->count);
  pthread_mutex_lock(&(data->bjLock));
  //Stub until I actually make blackjack
  if (count == 0) {
    printf("Player 1: Aw, man.\n");
  } else if (count == 1) {
    printf("Player 2: Oh my god! This never happens\n");
  } else if (count == 2) {
    printf("Player 3: Oh no, what will my girlfriend think.\n");
  } else {
    printf("Player 4: I need a drink.\n");
  }
  count++;
  pthread_mutex_unlock(&(data->bjLock));

  printCards(card1, card2);
  return NULL;
}

void blackJack() {
  printf("\nWe're just watching for now!\n");
  printf("Remember that a 1 is actually a 10. The 0 rubbed off (oops)\n");
  pthread_t threads[4];

  bjdata_t countstruct;
  countstruct.count = 1;
  pthread_mutex_init(&(countstruct.bjLock), NULL);

  for (int i = 0; i < 4; i++) {
    pthread_create(&threads[i], NULL, npcBlackJack, &countstruct);
    countstruct.count = countstruct.count + 1;
    pthread_join(threads[i], NULL);
  }
/** 
  for (size_t i = 0; i < 4; i++) {
    if (pthread_join(threads[i], NULL)) {
      perror("pthread_join failed");
      exit(2);
    }
  }*/
}

void newDay() {
  printf("<You enter the casino floor>\n What do you want to play first?\n");
  ourClock.hour = 12;
  ourClock.minute = 0;
  quitTonight = 0;
}

int main(void) {
  ourClock.startTime = 12;
  ourClock.closingTime = 23;
  dayOne();
  initDeck(&blackJackDeck);

  while(quitForever == 0) {
    newDay();
    while(quitTonight == 0) {
      int choice = options();
      if(choice == 1) {
        ourClock.hour = ourClock.hour + 1;
        pid_t p = fork();
        if (p < 0) {
          perror("fork fail");
          exit(1);
        } else if (p == 0) {
          ticTacToe();
        } else {
          wait(NULL);
        }
      } else if (choice == 2) {
        pid_t p = fork();
        if (p < 0) {
          ourClock.hour = ourClock.hour + 23;
          perror("fork fail");
          exit(1);
        } else if (p == 0) {
          blackJack();
        } else{
          wait(NULL);
        }
      } else if (choice == 3) {
        quitTonight = 1;
        printf("See you tomorrow!\n");
      } else if (choice == 4) {
        quitForever = 1;
        printf("Congratulations on finally making it out of here!\n");
        return;
      }

      if(ourClock.hour >= 23) {
        printf("The casino is now closing. See you tommorrow.\n");
        quitTonight = 1;
      }
    }
  }
}
