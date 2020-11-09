/**
 *
 * Descripcion: Implementation of the menu and its functions
 *
 * Fichero: menu.c
 * Autor: Ángela Valderrama & Andrés Navarro
 * Fecha: 8-11-2020
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"
#include "products.h"


static int ShowMainMenu();

int main(void) {
    
    SQLHENV env = NULL;
    SQLHDBC dbc = NULL;
    int ret;
    int nChoice = 0;

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
      fprintf(stderr, "Failed to connect\n");
      odbc_extract_error("SQLDriverConnect", dbc, SQL_HANDLE_DBC);
      return EXIT_FAILURE;
    }

    do {
        nChoice = ShowMainMenu();
        switch (nChoice) {
            case 1: {
                ShowProductsMenu(dbc, ret);
            }
                break;

            case 2: {
                printf("Orders menu not ready yet\n");
            }
                break;

            case 3: {
                printf("Customers menu not ready yet\n");
            }
                break;
            case 4: {
                printf("Bye!\n\n");
            }
                break;
        }
    } while (nChoice != 4);

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        fprintf(stderr,"Error when disconnecting\n");
        return EXIT_FAILURE;
    }

    return 0;
}

/***************************************************/
/* Function: ShowMainMenu                          */
/*                                                 */
/* This function shows the main interface/menu     */
/* from it we can access the other sections        */
/***************************************************/
int ShowMainMenu() {
    int nSelected = 0;
    char buf[16]; /* buffer for reading user input
                     in theory we only need 1 char
                     but a large buffer will handle
                     more gracefully wrong inputs
                     as 'qwerty' */

    do {
        printf("This is a classic cars program\n");
        printf("You can choose to display three different sections: products, orders or customers\n\n");

        printf(" (1) Products\n"
               " (2) Orders\n"
               " (3) Customers\n"
               " (4) Quit\n\n"
               "Enter a number that corresponds to your choice > ");
        if ((scanf("%s", buf)) == 0)
            /* reading input failed, give up: */
            nSelected = 0;
        else
            /* have some input, convert it to integer: */
            nSelected = atoi(buf);
        printf("\n");

        if ((nSelected < 1) || (nSelected > 4)) {
            printf("You have entered an invalid choice. Please try again\n\n\n");
        }
    } while ((nSelected < 1) || (nSelected > 4));

    return nSelected;
}