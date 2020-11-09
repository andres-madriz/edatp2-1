export PGDATABASE:=classicmodels
export PGUSER :=alumnodb
export PGPASSWORD :=alumnodb
export PGCLIENTENCODING:=LATIN9
export PGHOST:=localhost

# database creation
DBNAME =$(PGDATABASE)
PSQL = psql
CREATEDB = createdb
DROPDB = dropdb --if-exists
PG_DUMP = pg_dump
PG_RESTORE = pg_restore

# compiling macros
CC = gcc -g
CFLAGS = -Wall -Wextra -pedantic -ansi
LDLIBS = -lodbc

# recompile if this header changes
HEADERS = odbc.h products.h

EXE = menu
P = products
ODBC = odbc
DEPS = $(P).o $(ODBC).o
OBJ = $(EXE).o

all: dropdb createdb restore shell $(EXE) log
createdb:
	@echo Creando BBDD
	@$(CREATEDB)
dropdb:
	@echo Eliminando BBDD
	@$(DROPDB) $(DBNAME)
	rm -f *.log
dump:
	@echo creando dumpfile
	@$(PG_DUMP) > $(DBNAME).sql
restore:
	@echo restore data base
	@cat $(DBNAME).sql | $(PSQL)  
psql: shell
shell:
	@echo create psql shell
	@$(PSQL)

$(P).o: $(P).h
	@echo Compiling $(P)
	$(CC) $(CFLAGS) -c $(P).c

$(ODBC).o: $(ODBC).h
	@echo Compiling $(ODBC)
	$(CC) $(CFLAGS) -c $(ODBC).c

$(EXE): $(DEPS)
	@echo Compiling $(EXE)
	$(CC) $(CFLAGS) -c $(EXE).c
	$(CC) $(CFLAGS) $(DEPS) $(EXE).o $(LDLIBS) -o $(EXE)

log:
	@echo "Running splint command..." | tee splint.log
	splint -nullpass *.c *.h | tee -a splint.log

clean :
	rm -f *.o core $(EXE)
	rm -f *.o core $(P)
	rm -f *.o core $(ODBC)
	rm -f *.log
