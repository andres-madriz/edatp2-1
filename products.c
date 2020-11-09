
/**
 *
 * Descripcion: Implementation of product's menu functions
 *
 * Fichero: products.c
 * Autor: Ángela Valderrama & Andrés Navarro
 * Fecha: 8-11-2020
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"
#include "products.h"

#define BufferLength 512

/* Declaration of static functions */
static int ShowProductsSubMenu();
static void StockProducts(SQLHDBC dbc, int ret);
static void FindProducts(SQLHDBC dbc, int ret);



/***************************************************/
/* Function: ShowProductsMenu                      */
/*                                                 */
/* This function lets the user choose the          */
/* desired option from the products menu           */
/***************************************************/
void ShowProductsMenu(SQLHDBC dbc, int ret){
    int nChoice = 0;

    do {
        nChoice = ShowProductsSubMenu();
        switch (nChoice) {

            case 1: {
                StockProducts(dbc, ret);
            }
                break;
            
            case 2: {
                FindProducts(dbc, ret);
            }
                break;

            case 3: {
                printf("Going back!\n\n");
            }
                break; 
        }
    } while (nChoice != 3);
}

/***************************************************/
/* Function: ShowProductsSubMenu                   */
/*                                                 */
/* This function lets the user choose the          */
/* desired option from the products sub-menu       */
/***************************************************/
int ShowProductsSubMenu(){

    int nSelected = 0;
    char buf[16];

    do {
        printf(" (1) Stock\n"
               " (2) Find\n"
               " (3) Back\n\n");
        printf("Enter a number that corresponds to your choice > ");

        if ((scanf("%s", buf)) == 0)
            /* reading input failed, give up: */
            nSelected = 0;
        else
            /* have some input, convert it to integer: */
            nSelected = atoi(buf);
        printf("\n");

        if ((nSelected < 1) || (nSelected > 3)) {
            printf("You have entered an invalid choice. Please try again\n\n\n");
        }

    } while ((nSelected < 1) || (nSelected > 3));

    return nSelected;

}

/***************************************************/
/* Function: StockProducts                         */
/*                                                 */
/* The user will provide a ProductCode and the     */
/* function will return the Quantity in Stock      */
/* of such product                                 */
/***************************************************/
void StockProducts(SQLHDBC dbc, int ret){

    SQLHSTMT stmt = NULL;
    char x[BufferLength] = "\0";
    int ret2, ret3;
    int y = 0;

    ret2 = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    /* The query is prepared */
    ret2 = SQLPrepare(stmt, (SQLCHAR*) "select quantityinstock from products where productcode = ?", SQL_NTS);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return;
    }

    /* The input data is asked */
    printf("Enter productcode > ");
    (void) fflush(stdout);
    ret3 = scanf("%s", x);
    if(ret3 == 0){
        printf("No input\n");
        return;
    }
    
    (void) SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, x, 0, NULL);

    (void) SQLExecute(stmt);

    (void) SQLBindCol(stmt, 1, SQL_C_SLONG, &y, 0, NULL);

    /* Loop through the rows in the result-set */
    while (SQL_SUCCEEDED(ret2 = SQLFetch(stmt))) {
        printf("%d\n", y);
    }
    
    (void) SQLCloseCursor(stmt);
    (void) fflush(stdout);
    printf("\n");
    
    /* free up statement handle */
    ret = SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        return;
    }

}

/***************************************************/
/* Function: FindProducts                          */
/*                                                 */
/* The user will provide a string and the function */
/* will return all the ProductNames and            */
/* ProductCodes containing such string             */
/***************************************************/
void FindProducts(SQLHDBC dbc, int ret){

    SQLHSTMT stmt = NULL;
    char y[BufferLength] = "\0";
    char z[BufferLength] = "\0";
    char x1[BufferLength] = "\0";
    char x2[BufferLength] = "\0";
    int ret2, ret3, i;


    /* Allocate a statement handle */
    ret2 = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    ret2 = SQLPrepare(stmt, (SQLCHAR*) "select productcode, productname from products where productname like ? order by productcode", SQL_NTS);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return;
    }

    printf("Enter productname > ");
    (void) fflush(stdout);
    ret3 = scanf("%s", x1);
    if(ret3 == 0){
        printf("No input\n");
        return;
    }

    x2[0] = '%';
    for(i = 1; x1[i] != '\0'; i++){
        x2[i] = x1[i - 1];
    }
    x2[i] = '%';


    (void) SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, x2, 0, NULL);

    (void) SQLExecute(stmt);

    (void) SQLBindCol(stmt, 1, SQL_C_CHAR, (SQLVARCHAR *) y, BufferLength, NULL);
    (void) SQLBindCol(stmt, 2, SQL_C_CHAR, (SQLVARCHAR *) z, BufferLength, NULL);

    /* Loop through the rows in the result-set */
    while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        printf("%s, %s\n", y, z);
    }

    (void) SQLCloseCursor(stmt);
    (void) fflush(stdout);

    printf("\n");
    
    /* free up statement handle */
    ret = SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        return;
    }

    return;
}