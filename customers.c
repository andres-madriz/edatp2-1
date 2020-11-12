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
#include "odbc.h"
#include "customers.h"

#define BufferLength 512

/* Declaration of static functions */
static int ShowCustomersSubMenu();
static void FindCustomers(SQLHDBC dbc, int ret);
static void ListProductsCustomers(SQLHDBC dbc, int ret);
static void BalanceCustomers(SQLHDBC dbc, int ret);

void ShowCustomersMenu(SQLHDBC dbc, int ret){

    int nChoice = 0;

    do {
        nChoice = ShowCustomersSubMenu();
        switch (nChoice) {

            case 1: {
                FindCustomers(dbc, ret);
            }
                break;
            
            case 2: {
                ListProductsCustomers(dbc, ret);
            }
                break;

            case 3: {
                BalanceCustomers(dbc, ret);
            }
                break;

            case 4: {
                printf("Going back!\n\n");
            }
                break;
        }
    } while (nChoice != 4);

}

static int ShowCustomersSubMenu(){

    int nSelected = 0;
    char buf[16];

    do {
        printf(" (1) Find\n"
               " (2) List Products\n"
               " (3) Balance\n"
               " (4) Back\n\n");
        printf("Enter a number that corresponds to your choice > ");

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

static void FindCustomers(SQLHDBC dbc, int ret){

    SQLHSTMT stmt = NULL;
    char x1[BufferLength] = "\0";
    char x2[BufferLength] = "\0";
    char cname[BufferLength] = "\0";
    char cfirstname[BufferLength] = "\0";
    char clastname[BufferLength] = "\0";
    int ret2, ret3, i, cnumber = 0;

    ret2 = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    /* The query is prepared */
    ret2 = SQLPrepare(stmt, (SQLCHAR*) "select customername, contactfirstname, contactlastname, customernumber from customers where ((contactfirstname like ?) or (contactlastname like ?)) order by customernumber", SQL_NTS);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return;
    }

    /* The input data is asked */
    printf("Enter customer name > ");
    (void) fflush(stdout);
    ret3 = scanf("%s", x1);
    if(ret3 == 0){
        printf("No input\n");
        return;
    }

    x2[0] = '%';
    for(i = 1; x1[i - 1] != '\0'; i++){
        x2[i] = x1[i - 1];
    }
    x2[i] = '%';

    (void) SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, x2, 0, NULL);
    (void) SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, x2, 0, NULL);

    (void) SQLExecute(stmt);

    (void) SQLBindCol(stmt, 1, SQL_C_CHAR, (SQLVARCHAR *) cname, BufferLength, NULL);
    (void) SQLBindCol(stmt, 2, SQL_C_CHAR, (SQLVARCHAR *) cfirstname, BufferLength, NULL);
    (void) SQLBindCol(stmt, 3, SQL_C_CHAR, (SQLVARCHAR *) clastname, BufferLength, NULL);
    (void) SQLBindCol(stmt, 4, SQL_C_SLONG, &cnumber, 0, NULL);

    /* Loop through the rows in the result-set */
    while (SQL_SUCCEEDED(ret2 = SQLFetch(stmt))) {
        printf("%d %s %s %s\n",cnumber, cname, cfirstname, clastname);
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

static void ListProductsCustomers(SQLHDBC dbc, int ret){

    SQLHSTMT stmt = NULL;
    char pname[BufferLength] = "\0";
    int ret2, ret3, cnumber, sum = 0;

    ret2 = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    /* The query is prepared */
    ret2 = SQLPrepare(stmt, (SQLCHAR*) "select p.productname, sum(od.quantityordered)from customers as c join orders as o on o.customernumber = c.customernumber join orderdetails as od on od.ordernumber = o.ordernumber join products as p on od.productcode = p.productcode where c.customernumber = ? group by p.productcode;", SQL_NTS);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return;
    }

    /* The input data is asked */
    printf("Enter customer number > ");
    (void) fflush(stdout);
    ret3 = scanf("%d", &cnumber);
    if(ret3 == 0){
        printf("No input\n");
        return;
    }

    (void) SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &cnumber, 0, NULL);

    (void) SQLExecute(stmt);

    (void) SQLBindCol(stmt, 1, SQL_C_CHAR, (SQLVARCHAR *) pname, BufferLength, NULL);
    (void) SQLBindCol(stmt, 2, SQL_C_SLONG, &sum, 0, NULL);

    /* Loop through the rows in the result-set */
    while (SQL_SUCCEEDED(ret2 = SQLFetch(stmt))) {
        printf("%s %d\n", pname, sum);
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

static void BalanceCustomers(SQLHDBC dbc, int ret){

    SQLHSTMT stmt = NULL;
    int ret2, ret3, cnumber;
    double balance = 0;

    ret2 = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    /* The query is prepared */
    ret2 = SQLPrepare(stmt, (SQLCHAR*) "select t.total - v.total as balance from (select sum(p.amount) as total from customers as c join payments as p on p.customernumber = c.customernumber where c.customernumber = ? group by c.customernumber) as t, (select sum(od.quantityordered * od.priceeach) as total from customers as c join orders as o on o.customernumber = c.customernumber join orderdetails as od on od.ordernumber = o.ordernumber join products as p on od.productcode = p.productcode where c.customernumber = ?) as v;", SQL_NTS);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return;
    }

    /* The input data is asked */
    printf("Enter customer number > ");
    (void) fflush(stdout);
    ret3 = scanf("%d", &cnumber);
    if(ret3 == 0){
        printf("No input\n");
        return;
    }

    (void) SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &cnumber, 0, NULL);
    (void) SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &cnumber, 0, NULL);

    (void) SQLExecute(stmt);

    (void) SQLBindCol(stmt, 1, SQL_C_DOUBLE, &balance, BufferLength, NULL);

    /* Loop through the rows in the result-set */
    while (SQL_SUCCEEDED(ret2 = SQLFetch(stmt))) {
        printf("Balance = %.2f\n", balance);
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
