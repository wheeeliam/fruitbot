// fruit_bot.c
// Assignment 3, COMP1511 18s1: Fruit Bot
//
// This program by WILLIAM CHEN (z5165219) on 16-05-18
//
// Version 1.0.0: Assignment released.
// Version 1.0.1: minor bug in main fixed

#include <stdio.h>
#include <assert.h>

#include "fruit_bot.h"

void print_player_name(void);
void print_move(struct bot *b);
void run_unit_tests(void);

// ADD PROTOTYPES FOR YOUR FUNCTIONS HERE

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
    // THE LINES BELOW IMPLEMENT A SIMPLE (USELESS) STRATEGY
    // REPLACE THEN WITH YOUR CODE

    // int silly_choice = b->turns_left % 3;
    // if (silly_choice == 0) {
    //     printf("Move 1\n");
    // } else if (silly_choice == 1) {
    //     printf("Sell 1\n");
    // } else {
    //     printf("Buy 1\n");
    // }
    // if (fruit_unsellable(b) == 1) {
    //     dump_fruit(b) ;
    // }
    
    struct location *current_loc = b->location;
    struct location buy_loc;
    struct location sell_loc;

    int quantity = 0;
    int distance = 0;
    
    int tripTurnsRequired = 0;
    get_highest_net_pair (b, &buyLocation, &sellLocation);
    tripTurnsRequired = RETURN_TRIP * get_n_moves_req (b, &buy_loc, &sell_loc) + BUY_SELL_TURNS;

    get_highest_margin() 
    // Look for the best fruit to buy
    // memcpy location to buy_loc and sell_loc

    // Buy options:
    // - Refuel if required
    // - Move to buy_loc
    // - Buy when at buy_loc
    
    if ((b->fruit == NULL || b->quantity == 0) && b->turns_left >= tripTurnsRequired) {
        // Buy when at buy location
        if (strcmp (b->location->fruit, buy_loc.fruit &&) == 0 &&
            strcmp (b->fruit, b->location->fruit) == 0 && ) {
            quantity = get_max_quantity (b, &buy_loc, &sell_loc);
            printf ("Buy %d", quantity);
            return;
        // Refuels if required
        } else if (move_or_refuel (b, b->location, &buy_loc) == 1) {
            get_fuel(b);
            return;
        // Move to buy location
        } else {
            distance = get_distance(b, b->location, &buy_loc);
            printf("Move %d", distance);
            return;
        }

    // Sell options:
    // - Refuel if required
    // - Move to sell_loc
    // - Sell when at sell_loc
    // - If sell_loc is exhausted, move to new sell_loc
    // - Sell when at new sell_loc

    } else if (b->fruit != NULL) {
        // Sell when at sellLocation
        if (strcmp (b->location->fruit, sell_loc.fruit) == 0 && 
            strcmp (b->fruit, b->location->fruit) == 0) {
            quantity = get_max_quantity (b, &buyLocation, &sellLocation);
            printf ("Sell %d", quantity);
            return;
        // Refuels if required
        } else if (move_or_refuel (b, b->location, &sell_loc) == 1) {
            get_fuel (b);
            return;
        // Move to sellLocation
        } else {
            distance = get_distance(b, b->location, &sell_loc);
            printf("Move %d", distance);
            return;
        }
    // No options left:
    // - Keep moving to use all fuel up

    } else {
        printf("Move %d", b->maximum_move);
        return;
    }
}

}


// ADD A COMMENT HERE EXPLAINING YOUR OVERALL TESTING STRATEGY

void run_unit_tests(void) {
    // PUT YOUR UNIT TESTS HERE
    // This is a difficult assignment to write unit tests for,
    // but make sure you describe your testing strategy above.
}


// ADD YOUR FUNCTIONS HERE


// How am I going to make money?

// net profit margin = ((buy and sell difference) * max quantity - 2 * average fuel cost) / no. of moves
// This calculates the net margin per turn
// highest net profit margin would be the commodity you would want to buy
// net_profit_margin() = gross_margin * get_max_quantity() - 2 * average_fuel_cost() / n_moves_req()

int fruit_unsellable (struct bot *b) {
    int dump = 1;
    struct location *current_loc = b->location;

    if (b->fruit == NULL || b->fruit_kg == NULL) {
        return 0;
    }

    while (b->location != current_loc) {
        // Check whether fruit is exhausted
        if (b->location->fruit != NULL && b->location->price > 0 &&
        strcmp (b->fruit, b->location->fruit) == 0 && b->location->quantity > 0) {
            dump = 0;
            break;
        }
        b->location = b->location->next;
    }

    return dump;
}

int dump_fruit(struct bot *b) {
    struct location *current_loc = b->location;

    while (b->location != current_loc) {
        if (b->location->fruit == "Anything") {
            printf("Sell %d", b->quantity);
            return;
        }
        b->location = b->location->next;
    }
}

// ========== FUEL FUNCTIONS ==========


// Determines whether bot should move to prospective location or refuel
int move_or_refuel (struct bot *b, struct location *start, struct location *prospective) {
    int prospectiveMoves = 0;

    // Calculates moves to prospective location then to nearest fuel station
    prospectiveMoves += abs (get_distance (b, start, prospective));
    prospectiveMoves += abs (get_distance_to_fuel (b, prospective));

    // Move if fuel is sufficient to cover prospective moves
    if (b->fuel < prospectiveMoves) {
        return 1;
    } else {
        return 0;
    }
}


// Moves bot to fuel station and refuels
void get_fuel (struct bot *b) {
    int n_bots_at_location = 0;
    int distance = 0;
    n_bots_at_location = get_n_bots_at_location (b, b->location);

    // Buy fuel if at fuel station
    if (strcmp(b->location->fruit, "Electricity") && b->location->quantity > n_bots_at_location) {
        printf("Buy %d", b->battery_capacity - b->battery_level);
        return;
    // Move to fuel station
    } else {
        distance = get_distance_to_fuel (b, b->location);
        printf("Move %d", distance);
        return;
    }
}


// Finds closest operating fuel station
int get_distance_to_fuel (struct bot *b, struct location *start) {
    int nearestE = 0, nearestW = 0;
    int nBotsAtELocation = 0, nBotsAtWLocation = 0;
    int operatingFuelStation = 0;
    struct location *currentLocation = b->location;

    // Count nearest operating fuel station in positive direction
    b->location = start;
    do {
        // Stop loop if operating fuel station is found
        nBotsAtPLocation = get_n_bots_at_location (b, b->location);
        if (strcmp(b->location->fruit, "Electricity") &&
        b->location->quantity > nBotsAtELocation) {
            operatingFuelStation = 1;
            break;
        }
        b->location = b->location->east;
        nearestE++;
    } while (strcmp (b->location->name, start->name) != 0);

    // Count nearest operating fuel station in negative direction
    b->location = start;
    do {
        // Stop loop if operating fuel station is found
        nBotsAtNLocation = get_n_bots_at_location (b, b->location);
        if (strcmp(b->location->fruit, "Electricity") &&
        b->location->quantity > nBotsAtWLocation) {
            operatingFuelStation = 1;
            break;
        }

        b->location = b->location->west;
        nearestW--;
    } while (strcmp (b->location->name, start->name) != 0);
    b->location = currentLocation;

    // Check and return closest direction operating fuel station 
    if (operatingFuelStation == 0) {
        return 0;
    } else if (nearestE <= abs (nearestW)) {
        return nearestE;
    } else {
        return nearestW;
    }
}

double net_profit_margin (struct bot *b, struct location *buy_loc, struct location *sell_loc) {
    int gross_margin = 0;
    int max_quantity = 0; 
    double total_moves = 0;
    double fuel_cost = 0;
    double net_margin = 0;

    gross_margin = sell_loc->price - buy_loc->price;
    max_quantity = get_max_quantity (b, buy_loc, sell_loc);
    fuel_cost = get_average_battery_cost (b, buy_loc, sell_loc);
    total_moves = RETURN_TRIP * n_moves_req (b, buy_loc, sell_loc) + BUY_SELL_TURNS;

    net_margin = (max_quantity * gross_margin - RETURN_TRIP * average_fuel_cost) / total_moves;

    return net_margin;
}

void get_highest_profit_margin (struct bot *b, struct location *buy_loc, struct loation *sell_loc) {
    struct location *current_loc = b->location;
    double highest_margin = 0;
    double curr_margin = 0;
    int exit_condition = 0;
    int flag = 0;

    while (b->location != current_loc || exit_condition == 0) {
        exit_condition = 1;
        if (b->location->price < 0) {
            struct location *curr_seller = b->location;
            while (b->location != curr_seller || flag == 0) {
                if (b->location->price > 0 && strcmp(b->location->fruit, curr_seller->fruit) == 0) {
                    curr_margin = net_profit_margin(b, curr_seller, b->location);
                    if (curr_margin > highest_margin) {
                        highest_margin = curr_margin;
                        memcpy (buy_loc, curr_seller, sizeof (struct location));
                        memcpy (sell_loc, b->location, sizeof(struct location));
                    }
                }
                b->location = b->location->east;
            }
        }     
        b->location = b->location->east;
    }
    
}

int get_max_quantity (struct bot *b, struct location *buy_loc, struct location *sell_loc) {
    int max_quant = 0;
    int current_quant = b->fruit_kg;

    if (strcmp (b->location->fruit, buy_loc->fruit) == 0) {
        if (buy_loc->quantity <= sell_loc->quantity) {
            max_quant = buy_loc->quantity;
        } else {
            max_quant = sell_loc->quantity;
        }
    } else if (strcmp (b->location->fruit, sell_loc->fruit) == 0) {
        if (current_quant < sell_loc->quantity) {
            max_quant = current_quant;
        } else {
            max_quant = sell_loc->quantity;
        }
    }
    return max_quant;
} 

double get_average_battery_cost (struct bot *b, struct location *buy_loc, struct location *sell_loc) {
    int battery_price_sum = 0;
    double average_battery_price = 0;
    double average_battery_cost = 0;
    int distance = 0;
    int location_count = 0;
    struct location *first_loc = b->location;
    int flag = 0;

    while (b->location != first_loc || flag == 0) {
        flag = 1;
        if (strcmp (b->location->fruit, "Electricity") == 0) {
            battery_price_sum += b->location->price;
            location_count++;
        } 
        b->location = b->location->east;
    }
    
    distance = abs (get_distance (b, buy_loc, sell_loc));
    average_battery_cost = battery_price_sum / location_count;
    average_battery_price = distance * average_battery_cost;

    return average_battery_cost;
}

int get_n_moves_req (struct bot *b, struct location *buy_loc, struct location *sell_loc) {
    struct location *current_loc = b->location;
    int distance = 0
    int max_move = 0
    int curr_dist = 0;
    int n_moves_count = 1;

    distance = abs (get_distance (b, buy_loc, sell_loc));
    max_move = b->maximum_move;

    while ((curr_dist + max_move) < distance) {
        curr_dist += max_move;
        n_moves_count++;
    }

    return n_moves_count;
}

int get_distance (struct bot *b, struct location *buy_loc, struct location *sell_loc) {
    struct location *current_loc = b->location;
    int east_dist = 0
    int west_dist = 0;

    b->location = buy_loc;
    while (strcmp (b->location->name, sell_loc->name) != 0) {
        b->location = b->location->east;
        east_dist++;       
    }

    b->location = buy_loc;
    while (strcmp (b->location->name, sell_loc->name) != 0) {
        b->location = b->location->west;
        west_dist--;       
    }

    if (east_dist <= abs (west_dist)) {
        return east_dist;
    } else {
        return west_dist;
    }
}

// Determine number of bots at a specified location l
int get_n_bots_at_location (struct bot *b, struct location *loc) {
    // return 0 if no bot_list
    if (loc->bots == NULL) {
        return 0;
    }

    int n_bots_at_location = 0;
    struct bot_list *first_bot = loc->bots;
    struct bot_list *bots_at_loc = loc->bots;
    while (bots_at_loc != NULL) {
        n_bots_at_location++;
        bots_at_loc = bots_at_loc->next
    }
    l->bots = first_bot;

    return n_bots_at_location;
}