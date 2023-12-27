#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>

/*
	CRUD Exercise: This program is a C and SQLite exercise. 
	It's run select, insert, delete and update operations.
*/

static int show_results(
			void *not_used, // sqlite standard param, empty pointer not used here.
			int number_of_columns_in_result_row, 
			char **value_of_columns, 
			char **column_name){
    /*
     *  show_results: print returned data from fetch db table
     */
    
    for (int i = 0; i < number_of_columns_in_result_row; i++){
        printf("%s: %s\n", column_name[i], value_of_columns[i] ? value_of_columns[i] : "NULL");
    }
	printf("---------------------------------------------------\n");
    
    return 0;
}


static int count_affected_rows(sqlite3 *db, char *sql_count){
	/*
     *  count_affected_rows: count rows from a SELECT COUNT() query
	 *	sql_count: query like SELECT COUNT ([column_name]) FROM table_name
	 *  return: integer
     */
	sqlite3_stmt *stmt;
	sqlite3_prepare_v2(db, sql_count, -1, &stmt, NULL);
	
	while (sqlite3_step(stmt) != SQLITE_DONE) {
		return sqlite3_column_int(stmt, 0);
	}
}

static void clear (void){
    /*
	 * clear: remove characteres eventually in stdin    
	 */
    int c = 0;
    while ((c = getchar()) != '\n' && c != EOF);
}


int main (void)
{

    //db pointer
	sqlite3 *db;

    //conn: store the db connection code
    int conn = sqlite3_open("data.db", &db);
	
    // check if the db connection was succedded or not
	if (conn != SQLITE_OK) {
		printf("Could not open data.db: Error %d - %s\n", conn, sqlite3_errmsg(db));
		exit(1);
	}

	// hold error messages from ran sql query 
    char *error_messages = 0;
	char sql[800];
	char sql_count_rows[200];
    
    // copy query string to sql var
    strcpy(sql, "CREATE TABLE IF NOT EXISTS customers(\
            id      		integer     NOT NULL PRIMARY KEY AUTOINCREMENT,\
            name    		char(100)   NOT NULL,\
            email   		char(100)   NOT NULL,\
			last_modified	datetime 	NOT NULL \
        );\
		INSERT INTO customers(name, email, last_modified) \
		VALUES \
		('John Lennon', 'john@gmail.com', DATETIME()), \
		('Barack Obama', 'barack@gmail.com', DATETIME()), \
		('Lionel Messi', 'lionel@gmail.com', DATETIME()), \
		('Cristiano Ronaldo', 'cristiano@gmail.com', DATETIME()), \
		('Bruno Mars', 'bruno@gmail.com', DATETIME());\
		");
	
	// is_succedded: used to test if query execution failed or was successed 
	int is_succedded = 0;

    // the sqlite3_exec return non 0 is an error result
    is_succedded = sqlite3_exec(db, sql, NULL, 0, &error_messages);
    if (is_succedded){
        printf("Create table error: %d\n", is_succedded);
        exit (1);
    }
    
	int quit = 1;
	int option;
	int customer_id;
	int is_digit = 0;
	int rows_affected = 0;
	char y = 'y';
	char name[200];
	char email[100];

	

	while (quit){
        while(is_digit != 1){
    
			printf("\n\n");
			printf("---------------------------------------------------\n");
			printf("-------------  CUSTOMERS MANAGEMENT  --------------\n");
			printf("---------------------------------------------------\n");
            printf("\nChoose an option:\n\n");
            printf("1) All customers\n");
            printf("2) Find customer by ID\n");
            printf("3) New customer \n");
            printf("4) Delete customer\n");
            printf("5) Update customer\n");
            printf("6) Exit\n\n");
            printf(">> ");
	
			if(scanf(" %d", &option) == 1){
				//printf("It's a number\n");
				is_digit = 1;
			} else {
				printf("Only digits are accepted.\n");
			}
			clear();
		}
		is_digit = 0; // reset var
		
		switch (option){
			case 1:

				printf("\n\n");
				printf("---------------------------------------------------\n");
				printf("-----------------  ALL CUSTOMERS  -----------------\n");
				printf("---------------------------------------------------\n\n");

				strcpy(sql, "SELECT id, name, email, datetime(last_modified, 'localtime') AS \"last modified\" FROM customers;");

				is_succedded = 0; //reset var
				is_succedded = sqlite3_exec(db, sql, show_results, 0, &error_messages);
				
				printf("Total of customers: %d\n",
					count_affected_rows(db, "SELECT COUNT(*) AS customers FROM customers;"));
				
				if (is_succedded){
					printf("List customers error: %d\n", is_succedded);
					exit (1);
				}

				break;

			case 2: //SELECT ONE

				do{
		
					printf("Type a customer id: ");
					if(scanf(" %d", &customer_id) == 1){
						// when increment is_digit is reseted in next loop lap, 
                        // but if decremented works fine 
				        is_digit--;	
						
					} else {
						printf("Only digits are accepted.\n");
					}
					clear();
					
				}while(is_digit == 0);

				sprintf(sql_count_rows, "SELECT COUNT(*) FROM customers WHERE id=%d;", customer_id);
				
				// check if customer_id exists
				if(count_affected_rows(db, sql_count_rows) == 0){
					printf("No customer found by id: %d\n",customer_id);
				}else {

					printf("\n\n");
					printf("---------------------------------------------------\n");
					printf("-------------------  CUSTOMER  --------------------\n");
					printf("---------------------------------------------------\n\n");


					sprintf(sql, "SELECT * FROM customers WHERE id=%d;", customer_id);

					is_succedded = 0; //reset var
					is_succedded = sqlite3_exec(db, sql, show_results, 0, &error_messages);
					if (is_succedded){
						printf("List customer by id error: %d\n", is_succedded);
						exit (1);
					}
				}
				
				break;

			case 3: // INSERT
					
				do{ //Check empty input 
					printf("Name: ");
					fgets(name, sizeof(name), stdin);
					name[strcspn(name, "\n")] = 0;
					
					if(strlen(name)== 0){
						printf("Name can not be empty.\n");
					}
					
				}while(strlen(name) == 0);

				do{ //Check empty input
					printf("E-mail: ");
					fgets(email, sizeof(email), stdin);
					email[strcspn(email, "\n")] = 0;
					
					if(strlen(email)== 0){
						printf("E-mail can not be empty.\n");
					}
					
				}while(strlen(email)== 0);
                
				sprintf(sql, "INSERT INTO customers (name, email)\
					VALUES (\'%s\', \'%s\', DATETIME())", name, email);

				is_succedded = 0; //reset var
				is_succedded = sqlite3_exec(db, sql, NULL, 0, &error_messages);
				if (is_succedded){
					printf("Insert new customer error: %d\n", is_succedded);
					exit (1);
				}else{
					printf("%s was inserted successfully\n", name);
				}

				break;

			case 4: // DELETE

				// Check by inexistent customer
				customer_id = 0;
				do{
					printf("Type the customer id: ");

					if(scanf(" %d", &customer_id) == 1){
						is_digit--; 
						
					} else {
						printf("Only digits are accepted.\n");
					}
				
				}while(is_digit == 0);

				clear();

				sprintf(sql_count_rows, "SELECT COUNT(*) FROM customers WHERE id=%d;", customer_id);
				
				if(count_affected_rows(db, sql_count_rows) == 0){
					printf("No customer found by id: %d\n",customer_id);
				}else {

					printf("\n\n");
					printf("---------------------------------------------------\n");
					printf("------------------  CUSTOMER  ---------------------\n");
					printf("---------------------------------------------------\n\n");


					sprintf(sql, "SELECT * FROM customers WHERE id=%d;", customer_id);

					is_succedded = 0; //reset var
					is_succedded = sqlite3_exec(db, sql, show_results, 0, &error_messages);
					if (is_succedded){
						printf("There are no customer with the id: %d\n", is_succedded);
						exit (1);
					}

					printf("Do you confirm delete operation? [y/n]: ");

					char confirm_delete;
	
					scanf(" %c", &confirm_delete);
					clear();

					if (confirm_delete != y){
						printf("Operation cancelled\n");
					}else{
			
						sprintf(sql, "DELETE FROM customers WHERE id=%d", customer_id);

						is_succedded = 0; //reset var
						is_succedded = sqlite3_exec(db, sql, NULL, 0, &error_messages);
						if (is_succedded){
							printf("delete error: %d\n", is_succedded);
							exit (1);
						}else{
							printf("Customer deleted successfully\n");
						}
					}
				}

				break;

			case 5: //UPDATE

				customer_id = 0;
				do{
					printf("Type the customer id: ");

					if(scanf(" %d", &customer_id) == 1){
						is_digit--; 
						
					} else {
						printf("Only digits are accepted.\n");
					}
				
				}while(is_digit == 0);

				clear();

				sprintf(sql_count_rows, "SELECT COUNT(*) FROM customers WHERE id=%d;", customer_id);
				
				if(count_affected_rows(db, sql_count_rows) == 0){
					printf("No customer found by id: %d\n",customer_id);
				}else {

					printf("\n\n");
					printf("---------------------------------------------------\n");
					printf("------------------  CUSTOMER  ---------------------\n");
					printf("---------------------------------------------------\n\n");


					sprintf(sql, "SELECT * FROM customers WHERE id=%d;", customer_id);

					is_succedded = 0; //reset var
					is_succedded = sqlite3_exec(db, sql, show_results, 0, &error_messages);
									
					if (is_succedded){
						printf("List customer by id error: %d\n", is_succedded);
						exit (1);
					}

					char change_name;
					char change_email;

					printf("Update name? [y/n]: ");
					scanf(" %c", &change_name);
					clear();
					
					if (change_name == y){
						printf("New name: ");
						fgets(name, sizeof(name), stdin);
						name[strcspn(name, "\n")] = 0;
					}

					printf("Update e-mail? [y/n]: ");
					scanf(" %c", &change_email);
					clear();

					if (change_email == y){

						printf("E-mail: ");
						fgets(email, sizeof(email), stdin);
						email[strcspn(email, "\n")] = 0;
					}

					if (change_email != y && change_name != y){
						printf("Operation cancelled\n");
					}else{
						
						if (change_name == y )
						{
							sprintf(sql, "UPDATE customers SET name=\'%s\', last_modified=DATETIME() \
								WHERE id=%d", name, customer_id);
						}
						if (change_email == y)
						{
							sprintf(sql, "UPDATE customers SET email=\'%s\', last_modified=DATETIME() \
								WHERE id=%d", email, customer_id);
						}
						if (change_name == y  && change_email == y)
						{
							sprintf(sql, "UPDATE customers SET name=\'%s\', email=\'%s\', last_modified=DATETIME() \
								WHERE id=%d", name, email, customer_id);
						}
						
						is_succedded = 0; //reset var
						is_succedded = sqlite3_exec(db, sql, NULL, 0, &error_messages);
						if (is_succedded){
							printf("update error: %d\n", is_succedded);
							exit (1);
						}else{
							printf("Customer updated successfully\n");
						}
					}
				}
				break;

			case 6:
				printf("Program ended\n");
				quit=0;
				break;
			default:
				printf("Invalid option\n");
				break;
		}	
	}

	return 0;
}


