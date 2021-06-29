# Online Banking Management System.

## About the Project.
* This is a server-client based Banking System. All accounts, transactional, customer information, and admin details are stored in files.
* Account-holders have to pass through a login system to enter their accounts.
* The application is password-protected administrative access, thus preventing the whole management system from unauthorized access.
* Two types of login are there (a) Admin and (b) Customer. There are 2 types of the customer account (i) Normal User and, (ii) Joint User.
* Once we log in as an administrator, we can add, delete, modify, and search for specific account details or customer detail.
* If we login as a joint account holder, proper file locking has been implemented to maintain consistency of Database.
* All these things were mostly done using system calls.


## Files in the Project.

* `init.sh` : It creates the db(Database) folder which contains files like `Account.txt, Admin.txt, Customer.txt, and AdminLogs.txt` and a folder named `Transaction` which contain the Transaction file related to each account. It also runs init.c file which creates 2 admin for the system.

* `init.c` : This creates 2 admin for the Banking system
 
* `server.c` : This is a Banking server.

* `client.c` : This is Admin and Customer client.

* `operation.c` : This file contains all the Admin and Customer operation.

* `structures.h` : It contains all the necessary structures like Customer, Transaction, Admin, Account, and AdminLogs which are used in code.
  
* `readDb.c` : This is the code that helps us to understand what exactly happening inside the database when the server and client communicate with each other.


## How to run?

1. Run `init.sh` by following command in terminal typeing 
   ```bash
   ./init.sh
   ```

2. Run `server.c` by following command in terminal typeing 
   ```bash
   cc server.c -o server
   ./server
   ```
3. Run `client.c` by following command in terminal typeing 
   ```bash
   cc client.c -o client
   ./client
   ```
4. Now Banking System is live. 
5. To see what Database Inserting, Deleting, and Updating when server and client interact with each other. Run `readDb.c` by following command in terminal typing 
   ```bash
   cc readDb.c -o readDb
   ./readDb
   ```


   

## Created By Parth Patel (MT2020057)


