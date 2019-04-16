// fruit_bot.c
// Assignment 3, COMP1511 18s1: Fruit Bot
//
// This program by WILLIAM CHEN (z5165219) on 16-05-18
//
// Version 1.0.0: Assignment released.
// Version 1.0.1: minor bug in main fixed

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "fruit_bot.h"

#define RETURN_TURN    2
#define BUY_SELL_TURNS 2
#define LOWEST_MARGIN  -2147483647

void print_player_name(void);
void print_move(struct bot *b);
void run_unit_tests(void);

// ADD PROTOTYPES FOR YOUR FUNCTIONS HERE

int fruit_unsellable (struct bot *b);
void dump_fruit(struct bot *b);
int get_distance_to_dump (struct bot *b);
int move_or_recharge (struct bot *, struct location *, struct location *);
void get_battery (struct bot *);
int get_distance_to_recharge (struct bot *);
double net_profit_margin (struct bot *, struct location *, struct location *);
void get_highest_profit_margin (struct bot *, struct location **, struct location **);
int get_max_quantity (struct bot *, struct location *, struct location *);
double get_average_battery_cost (struct bot *, struct location *, struct location *);
int get_n_moves_req (struct bot *, struct location *, struct location *);
int get_distance (struct bot *, struct location *, struct location *);
int get_n_bots_at_location (struct bot *, struct location *);
struct location * create_location (char *, char *, int, int);
struct bot *create_bot (char *, struct location *, int, int, char *, int, int, int, int, int);
void test_one(void);
void test_two(void);
void test_three(void);

// YOU SHOULD NOT NEED TO CHANGE THIS MAIN FUNCTION

int main(int argc, char *argv[]) {

    if (argc > 1) {
        // supply any command-line argument to run unit tests
        run_unit_tests();
        return 0;
    }

    struct bot *me = fruit_bot_input(stdin);
    if (me == NULL) {
        print_player_name();
    } else {
        print_move(me);
    }

    return 0;
}

void print_player_name(void) {
    // CHANGE THIS PRINTF TO YOUR DESIRED PLAYER NAME
    printf("Electric Fruitbat");

}

// print_move - should print a single line indicating
//              the move your bot wishes to make
//
// This line should contain only the word Move, Sell or Buy
// followed by a single integer

void print_move(struct bot *b) {
    struct location *current_loc = b->location;
    struct location *buy_loc = b->location;
    struct location *sell_loc = b->location;
    int moves_required = 0;
    int quantity = 0;
    int distance = 0;

    // Finds most optimal buy and sell location, and as well as fruit for profit
    get_highest_profit_margin (b, &buy_loc, &sell_loc);
    moves_required = get_n_moves_req (b, buy_loc, sell_loc) + BUY_SELL_TURNS;

    // If sellers for fruit held run out 
    if (fruit_unsellable (b) == 1) {
        dump_fruit(b);
        return;
    } else if ((b->fruit == NULL || b->fruit_kg == 0) && b->turns_left >= moves_required) {
        // Buy when at buy location
        if (strcmp(current_loc->name, buy_loc->name) == 0 ) {
            quantity = get_max_quantity (b, buy_loc, sell_loc);
            printf("Buy %d\n", quantity);
            return;
        // Recharge if required
        } else if (move_or_recharge (b, current_loc, buy_loc) == 1) {
            get_battery(b);
            return;
        // Move to buy location
        } else {
            distance = get_distance (b, current_loc, buy_loc);
            printf("Move %d\n", distance);
            return;
        }
    // Conditions to sell fruit 
    } else if (b->fruit != NULL) {
        // Sell when at sell location
        if (strcmp(current_loc->name, sell_loc->name) == 0 &&
            strcmp(b->fruit, sell_loc->fruit) == 0) { 
            quantity = get_max_quantity (b, buy_loc, sell_loc);
            printf("Sell %d\n", quantity);
            return;
        // Recharge if required
        } else if (move_or_recharge (b, current_loc, sell_loc) == 1) {
            get_battery (b);
            return;
        // Move to sell location
        } else {
            distance = get_distance (b, current_loc, sell_loc);
            printf("Move %d\n", distance);
            return;
        }
    // If no options left, keep moving to use all battery up
    } else {
        // printf("No options left\n");
        printf("Move %d\n", b->maximum_move);
        return;
    }
}


// ADD A COMMENT HERE EXPLAINING YOUR OVERALL TESTING STRATEGY

// My overall testing strategy is to check that the output of my functions allign to
// the numbers and decisions that I would take if I were in the position of the bot.
// This basically means checking different situations where I thought that the bot
// would run into difficulty or would not know what to compute, and as well as 
// ensuring that the basic situations the bot would compute properly.

// While all the functions do work properly in the single bot world simulation, there
// are definitely certain limitations to the multi bot world simulations.

struct location *create_location (char *name, char *fruit, int price, int quantity) {
    struct location *new_loc = malloc(sizeof(struct location));
    new_loc->name = strdup(name);
    new_loc->fruit = strdup(fruit);
    new_loc->price = price;
    new_loc->quantity = quantity;
    return new_loc;
}

struct bot *create_bot (char *name, struct location *loc, int cash, int battery_level, char *fruit,
    int fruit_kg, int turns_left, int battery_capacity, int maximum_move, int maximum_fruit_kg) {
    struct bot *new_bot = malloc(sizeof(struct bot));
    new_bot->name = strdup(name);
    new_bot->location = loc;
    new_bot->cash = cash;
    new_bot->battery_level = battery_level;
    new_bot->fruit = strdup(fruit);
    new_bot->fruit_kg = fruit_kg;
    new_bot->turns_left = turns_left;
    new_bot->battery_capacity = battery_capacity;
    new_bot->maximum_move = maximum_move;
    new_bot->maximum_fruit_kg = maximum_fruit_kg;
    return new_bot;
}

void run_unit_tests(void) {
    // PUT YOUR UNIT TESTS HERE
    // This is a difficult assignment to write unit tests for,
    // but make sure you describe your testing strategy above.
    
    // To test the functions that I have created, I have created my own map in fruit_bot.c
    // Below is "world0.txt"

    // Buy & sell test
    test_one();

    // Move or recharge test
    test_two();

    // Dump fruit test
    test_three();

}

void test_one(void) {
    struct location *A = create_location ("CSE", "Nothing", 0, 0);
    struct location *B = create_location ("Mathews A", "Apples", 51, 10);
    struct location *C = create_location ("Quadrangle", "Nothing", 0, 0);
    struct location *D = create_location ("Campus Charging", "Electricity", -4, 100);
    struct location *E = create_location ("Kensington Apple Farm", "Apples", -18, 3);
    struct location *F = create_location ("Campus Compost Heap", "Anything", 1, 1000);
    struct location *G = create_location ("CLB 7", "Apples", 43, 3);
    struct bot *b = create_bot ("Botty McBotbot", A, 117, 74, "Nothing", 0, 19, 74, 7, 21);
    struct location *buy_loc = b->location;
    struct location *sell_loc = b->location;
    struct location *current_loc = b->location;

    A->east = B;
    B->east = C;
    C->east = D;
    D->east = E;
    E->east = F;
    F->east = G;
    G->east = A;

    A->west = G;
    B->west = A;
    C->west = B;
    D->west = C;
    E->west = D;
    F->west = E;
    G->west = F;

    // Use this function to see what is the best buy option
    get_highest_profit_margin (b, &buy_loc, &sell_loc); 

    // Using this function, finds the most profitable buy and sell location
    // Changes the addresses of previously initialised buy and sell locations
    assert(strcmp(buy_loc->name, "Kensington Apple Farm") == 0);
    assert(strcmp(sell_loc->name, "Mathews A") == 0);

    // Currently contains no fruit, thus has no option to dump fruit
    assert(fruit_unsellable (b) == 0);

    // Distance between buy and sell locations is to "Move -3"
    assert(get_distance (b, buy_loc, sell_loc) == -3);

    // First turn, the bot will "Move -3" to get to buy location
    assert(get_distance (b, current_loc, buy_loc) == -3);
    
    // If the bot was one dest. closer, it would only have to "Move -2"
    current_loc = current_loc->west;
    assert(get_distance (b, current_loc, buy_loc) == -2);


    // Bot knows that the maximum quantity will be to buy 3, as the qty at sell location is 3
    assert(get_max_quantity (b, buy_loc, sell_loc) == 3);
    assert(get_n_moves_req (b, buy_loc, sell_loc) == 1);

    // Part of the formula that I had used to figure out the highest profit margins
    // Cost is positive, as I later put a '-' sign in front in the function net_profit_margin
    // I did not test the other 'double' type functions as I would not correctly assert them 
    assert(get_average_battery_cost (b, buy_loc, sell_loc) == 12);

    // In this test, this showed that the bot needed to move three steps in the western
    // direction. Furthermore, the bot would then have bought max quantity to follow
    // up with this move - which would have been 3 apples.
}

void test_two(void) {
    struct location *A = create_location("CSE", "Nothing", 0, 0);
    struct location *B = create_location("Mathews A", "Apples", 51, 10);
    struct location *C = create_location("Quadrangle", "Nothing", 0, 0);
    struct location *D = create_location("Campus Charging", "Electricity", -4, 100);
    struct location *E = create_location("Kensington Apple Farm", "Apples", -18, 3);
    struct location *F = create_location("Campus Compost Heap", "Anything", 1, 1000);
    struct location *G = create_location("CLB 7", "Apples", 43, 3);
    struct bot *b = create_bot("Botty McBotbot", C, 117, 74, "Nothing", 0, 19, 2, 7, 21);
    struct location *buy_loc = b->location;
    struct location *sell_loc = b->location;
    struct location *current_loc = b->location;

    A->east = B;
    B->east = C;
    C->east = D;
    D->east = E;
    E->east = F;
    F->east = G;
    G->east = A;

    A->west = G;
    B->west = A;
    C->west = B;
    D->west = C;
    E->west = D;
    F->west = E;
    G->west = F;

    // Use this function to see what is the best buy option
    get_highest_profit_margin (b, &buy_loc, &sell_loc); 

    // Evidently as battery is at 0, the most suitable buy location would be at 
    // the closest charging location - Campus Charging
    // However, since get_highest_profit_margin function does not account for
    // electricity, Campus Charging is not classifed as the buy location
    assert(strcmp(buy_loc->name, "Campus Charging") != 0);

    // It would be the best decision to recharge, and hence returns 0
    assert(move_or_recharge (b, buy_loc, sell_loc) == 0);

    // The distance needed to get to Campus Charging is to "Move 1"
    // Since this returns 1, then "get_battery" function also works (void function)
    assert(get_distance_to_recharge (b) == 1);
    
    // Following this example of how the bot would move to the closest charging 
    // station, the bot would then buy electricity - at the quantity of: 
    // "b->battery_capacity - b->battery_level" to fill up the capacity.
}

void test_three(void) {
    struct location *A = create_location("CSE", "Nothing", 0, 0);
    struct location *B = create_location("Mathews A", "Apples", 51, 0);
    struct location *C = create_location("Quadrangle", "Nothing", 0, 0);
    struct location *D = create_location("Campus Charging", "Electricity", -4, 100);
    struct location *E = create_location("Kensington Apple Farm", "Apples", -18, 5);
    struct location *F = create_location("Campus Compost Heap", "Anything", 1, 1000);
    struct location *G = create_location("CLB 7", "Apples", 43, 0);
    struct bot *b = create_bot("Botty McBotbot", B, 117, 74, "Apples", 10, 15, 50, 7, 21);
    struct location *buy_loc = b->location;
    struct location *sell_loc = b->location;
    struct location *current_loc = b->location;

    A->east = B;
    B->east = C;
    C->east = D;
    D->east = E;
    E->east = F;
    F->east = G;
    G->east = A;

    A->west = G;
    B->west = A;
    C->west = B;
    D->west = C;
    E->west = D;
    F->west = E;
    G->west = F;

    // Use this function to see what is the best buy option
    get_highest_profit_margin (b, &buy_loc, &sell_loc); 

    // Checks whether fruit is unsellable or not
    // Thus fruit is unsellable and must be sold to Campus Compost Heap to maximise profit
    assert(fruit_unsellable (b) == 1);

    // As expected, fruit is unsellable as, while there may be seller for the same fruit,
    // there are no more buyers of the fruit
    assert(B->quantity == 0);
    assert(G->quantity == 0);
    assert(E->quantity > 0);

    // While the expected sell location is Campus Compost Heap, get_highest_profit_margin 
    // function does not account for locations which buy "Anything"
    // However this does not influence decision
    assert(strcmp(sell_loc->name, "Campus Compost Heap") != 0);EN

    // Distance to closed dump is "Move -3" which the void function "dump_fruit"
    // prints out to succeed the decision
    assert(get_distance_to_dump (b) == -3);
}

// ADD YOUR FUNCTIONS HERE

// No more buyers for fruit means no profit to be made, hence deems fruit as unsellable
int fruit_unsellable (struct bot *b) {
    struct location *current_loc = b->location;
    int dump = 1;
    int flag = 0;

    if (b->fruit_kg == 0 || b->fruit == NULL) {
        return 0;
    }

    while (current_loc != b->location || flag == 0) {
        flag = 1;
        if (current_loc->fruit != NULL && strcmp(b->fruit, current_loc->fruit) == 0 &&
            current_loc->price > 0 && current_loc->quantity > 0) {
            dump = 0;
            break;
        }
        current_loc = current_loc->east;
    }

    return dump;
}

// If fruit unsellable, bot makes a decision to get closer to dumping fruit
void dump_fruit (struct bot *b) {
    int exit_condition = 0;
    int distance = 0;

    if (strcmp(b->location->fruit, "Anything") == 0) {
        printf("Sell %d\n", b->fruit_kg);
    } else {
        distance = get_distance_to_dump (b);
        printf("Move %d\n", distance);
    }

}

// Finds closest compost to dump fruit
int get_distance_to_dump (struct bot *b) {
    struct location *current_loc = b->location;
    int east_dist = 0;
    int west_dist = 0;
    int valid_dump = 0;
    int flag = 0;

    while (current_loc != b->location || flag == 0) {
        flag = 1;
        if (strcmp(current_loc->fruit, "Anything") == 0) {
            valid_dump = 1;
            break;
        }
        current_loc = current_loc->east;
        east_dist++;
    }
    
    flag = 0;
    current_loc = b->location;
    while (current_loc != b->location || flag == 0) {
        flag = 1;
        if (strcmp(current_loc->fruit, "Anything") == 0) {
            valid_dump = 1;
            break;
        }
        current_loc = current_loc->west;
        west_dist--;
    }

    if (valid_dump == 0) {
        return 0;
    } else if (east_dist <= abs (west_dist)) {
        return east_dist;
    } else {
        return west_dist;
    }
}

// Bot considers whether it has enough battery to move or if requires more battery
int move_or_recharge (struct bot *b, struct location *start_loc, struct location *end_loc) {
    int required_moves = 0;
    int buy_sell_dist = get_distance (b, start_loc, end_loc);
    int recharge_dist = get_distance_to_recharge (b);

    required_moves = abs(buy_sell_dist) + abs(recharge_dist);

    if (b->battery_level < required_moves) {
        return 1;
    } else {
        return 0;
    }
}

// Decisions to get to closest recharge station or recharge
void get_battery (struct bot *b) {
    int n_bots_at_location = 0;
    int distance = 0;
    n_bots_at_location = get_n_bots_at_location (b, b->location);

    if (strcmp(b->location->fruit, "Electricity") == 0 && b->location->quantity > n_bots_at_location) {
        printf("Buy %d\n", b->battery_capacity - b->battery_level);
    } else {
        distance = get_distance_to_recharge (b);
        printf("Move %d\n", distance);
    }
}

// Finds closest destination to recharge
int get_distance_to_recharge (struct bot *b) {
    struct location *current_loc = b->location;
    int east_dist = 0;
    int west_dist = 0;
    int valid_charging_station = 0;
    int flag = 0;

    while (current_loc != b->location || flag == 0) {
        flag = 1;
        if (strcmp(current_loc->fruit, "Electricity") == 0) {
            valid_charging_station = 1;
            break;
        }
        current_loc = current_loc->east;
        east_dist++;
    }

    flag = 0;
    current_loc = b->location;
    while (current_loc != b->location || flag == 0) {
        flag = 1;
        if (strcmp(current_loc->fruit, "Electricity") == 0) {
            valid_charging_station = 1;
            break;
        }
        current_loc = current_loc->west;
        west_dist--;
    }

    if (valid_charging_station == 0) {
        return 0;
    } else if (east_dist <= abs (west_dist)) {
        return east_dist;
    } else {
        return west_dist;
    }
}

// Finds all recharge locations and gets average battery cost
double get_average_battery_cost (struct bot *b, struct location *buy_loc, struct location *sell_loc) {
    struct location *current_loc = b->location;
    int battery_price_sum = 0;
    double average_battery_price = 0;
    double average_battery_cost = 0;
    int distance = 0;
    int location_count = 0;
    int flag = 0;

    while (current_loc != b->location || flag == 0) {
        flag = 1;
        if (strcmp(current_loc->fruit, "Electricity") == 0) {
            battery_price_sum += current_loc->price;
            location_count++;
        } 
        current_loc = current_loc->east;
    }

    distance = abs (get_distance (b, buy_loc, sell_loc));
    average_battery_price = battery_price_sum / location_count;
    average_battery_cost = fabs (distance * average_battery_price);

    return average_battery_cost;
}

// Calculate net margin, which figures out the most suitable fruit to buy and sell
double net_profit_margin (struct bot *b, struct location *buy_loc, struct location *sell_loc) {
    int gross_margin = 0;
    int max_quantity = 0; 
    int total_moves = 0;
    double battery_cost = 0;
    double net_margin = 0;

    gross_margin = sell_loc->price - buy_loc->price;
    max_quantity = get_max_quantity (b, buy_loc, sell_loc);
    battery_cost = get_average_battery_cost (b, buy_loc, sell_loc);
    total_moves = RETURN_TURN * get_n_moves_req (b, buy_loc, sell_loc) + BUY_SELL_TURNS;
    net_margin = (max_quantity * gross_margin - RETURN_TURN * battery_cost) / (double) total_moves;

    return net_margin;
}

// Creates pointers to optimal buy and sell locations
void get_highest_profit_margin (struct bot *b, struct location **buy_loc, struct location **sell_loc) {
    struct location *current_loc = b->location;
    double highest_margin = LOWEST_MARGIN;
    double curr_margin = 0;
    int exit_condition = 0;
    int flag = 0;

    while (current_loc != b->location || exit_condition == 0) {
        exit_condition = 1;
        
        if (current_loc->price < 0) {
            struct location *curr_seller = current_loc;           
            while (current_loc != curr_seller || flag == 0) {
                flag = 1;
                if (current_loc->price > 0 && strcmp(curr_seller->fruit, current_loc->fruit) == 0) {
                    curr_margin = net_profit_margin(b, curr_seller, current_loc);
                    if (curr_margin >= highest_margin) {
                        highest_margin = curr_margin;
                        *buy_loc = curr_seller;
                        *sell_loc = current_loc;
                    }
                }
                current_loc = current_loc->east;
            }
            flag = 0;
        }     
        current_loc = current_loc->east;
    }   
}

// Gets quantity for bot to buy
int get_max_quantity (struct bot *b, struct location *buy_loc, struct location *sell_loc) {
    struct location *current_loc = b->location;
    int max_quant = 0;
    int current_quant = b->fruit_kg;

    if (strcmp(sell_loc->fruit, buy_loc->fruit) == 0) {
        if (buy_loc->quantity <= sell_loc->quantity) {
            max_quant = buy_loc->quantity;
        } else {
            max_quant = sell_loc->quantity;
        }
    } 

    return max_quant;
} 

// Gets number of moves required for bot to get from start to finish
int get_n_moves_req (struct bot *b, struct location *buy_loc, struct location *sell_loc) {
    struct location *current_loc = b->location;
    int distance = 0;
    int max_move = 0;
    int curr_dist = 0;
    int n_moves_count = 1;

    distance = abs(get_distance (b, buy_loc, sell_loc));
    max_move = b->maximum_move;

    while ((curr_dist + max_move) < distance) {
        curr_dist += max_move;
        n_moves_count++;
    }

    return n_moves_count;
}

// Calculates closest distance from one destination to another
int get_distance (struct bot *b, struct location *buy_loc, struct location *sell_loc) {    
    struct location *current_loc = b->location;
    int east_dist = 0;
    int west_dist = 0;

    current_loc = buy_loc;
    while (strcmp(current_loc->name, sell_loc->name) != 0) {
        east_dist++;            
        current_loc = current_loc->east;
    }

    current_loc = buy_loc;
    while (strcmp(current_loc->name, sell_loc->name) != 0) {
        west_dist--;       
        current_loc = current_loc->west;
    }
    current_loc = b->location;

    if (east_dist <= abs (west_dist)) {
        return east_dist;
    } else {
        return west_dist;
    }
}

// Figures out how many bots at a location
int get_n_bots_at_location (struct bot *b, struct location *loc) {
    if (loc->bots == NULL) {
        return 0;
    }

    int n_bots_at_location = 0;
    struct bot_list *first_bot = loc->bots;
    struct bot_list *bots_at_loc = loc->bots;
    
    while (bots_at_loc != NULL) {
        n_bots_at_location++;
        bots_at_loc = bots_at_loc->next;
    }
    loc->bots = first_bot;

    return n_bots_at_location;
}