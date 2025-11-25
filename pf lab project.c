#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int id;
    char title[50];
    char author[50];
    int totalquantity;
    int availablequantity;
} Book;

typedef struct {
    int id;
    char name[100];
    float penalty;
} Member;

typedef struct {
    char username[50];
    char password[50];
} User;

typedef struct {
    int bookID;
    int memberID;
    char issueDate[20];
    char dueDate[20];
    int returned;
} IssueRecord;

void adminMenu();
void userMenu(char username[]);
void addBook();
void addMember();
void issueBook();
void returnBook();
void searchBook();
void viewAvailableBook();
void viewIssuedBook();
void registerUser();
int loginUser(char username[]);
void viewMembers();
void getDate(char *buffer);
void addDays(char *date , int days , char *newdate);
int daysBetween(const char *date1, const char *date2);
void readstring(char *buffer, int size);
int getIntInput(char message[]);
int getIntInput(char message[]) {
    char buffer[100];
    int value;
    char *end;

    while (1) {
        printf("%s", message);
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            continue;
        }

        value = strtol(buffer, &end, 10); 

        if (end == buffer || (*end != '\n' && *end != '\0')) {
            printf("Invalid input! Please enter a number.\n");
        } else {
            return value;
        }
    }
}

void readstring(char *buffer, int size) {
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
}

void registerUser() {
    User u;
    FILE *fp = fopen("users.txt", "a");

    printf("\nEnter New Username: ");
    readstring(u.username, 50);

    printf("Enter New Password: ");
    readstring(u.password, 50);

    fprintf(fp, "%s %s\n", u.username, u.password);
    fclose(fp);

    printf("Registration Successful!\n");
}

int loginUser(char username[]) {
    User u;
    char pass[50];
    FILE *fp = fopen("users.txt", "r");
    if (!fp) return 0;
    printf("\nEnter Username: ");
    readstring(username, 50);
    printf("Enter Password: ");
    readstring(pass, 50);
    while (fscanf(fp, "%49s %49s", u.username, u.password) != EOF) {
        if (strcmp(u.username, username) == 0 && strcmp(u.password, pass) == 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

void adminMenu() {
    int choice;
    do {
        printf("\n===== ADMIN MENU =====\n");
        printf("1. Add Book\n");
        printf("2. Add Member\n");
        printf("3. View Available Books\n");
        printf("4. View Issued Books\n");
        printf("5. View Members And Penalties\n");
        printf("6. Logout\n");

        choice = getIntInput("Enter Choice: ");

        switch (choice) {
            case 1: addBook(); break;
            case 2: addMember(); break;
            case 3: viewAvailableBook(); break;
            case 4: viewIssuedBook(); break;
            case 5: viewMembers(); break;
        }
    } while (choice != 6);
}

void viewMembers() {
    Member m;
    FILE *fp = fopen("members.txt", "r");

    if (!fp) {
        printf("No members found!\n");
        return;
    }
    printf("\n---- MEMBER LIST ----\n");

    while (fscanf(fp, "%d|%[^|]|%f\n", &m.id, m.name, &m.penalty) != EOF) {
        printf("ID: %d | Name: %s | Penalty: %.2f\n", m.id, m.name, m.penalty);
    }

    fclose(fp);
}

void userMenu(char username[]) {
    int choice;
    do {
        printf("\n===== USER MENU (%s) =====\n", username);
        printf("1. Search Book\n");
        printf("2. Issue Book\n");
        printf("3. Return Book\n");
        printf("4. Logout\n");

        choice = getIntInput("Enter Choice: ");

        switch (choice) {
            case 1: searchBook(); break;
            case 2: issueBook(); break;
            case 3: returnBook(); break;
        }
    } while (choice != 4);
}

void addBook() {
    Book b;
    FILE *fp = fopen("books.txt", "a");

    b.id = getIntInput("\nEnter Book ID: ");
    printf("Enter Title: ");
    readstring(b.title, 50);
    printf("Enter Author: ");
    readstring(b.author, 50);
    b.totalquantity = getIntInput("Enter Quantity: ");

    b.availablequantity = b.totalquantity;

    fprintf(fp, "%d|%s|%s|%d|%d\n", b.id, b.title, b.author, b.totalquantity, b.availablequantity);
    fclose(fp);
    printf("Book Added Successfully!\n");
}

void addMember() {
    Member m;
    FILE *fp = fopen("members.txt", "a");

    m.id = getIntInput("\nEnter Member ID: ");
    printf("Enter Member Name: ");
    readstring(m.name, 50);
    m.penalty = 0;
    fprintf(fp, "%d|%s|%.2f\n", m.id, m.name, m.penalty);
    fclose(fp);
    printf("Member Added Successfully!\n");
}

void searchBook() {
    Book b;
    int id, found = 0;

    id = getIntInput("\nEnter Book ID: ");

    FILE *fp = fopen("books.txt", "r");

    while (fscanf(fp, "%d|%[^|]|%[^|]|%d|%d\n", &b.id, b.title, b.author,&b.totalquantity, &b.availablequantity) != EOF) {
        if (b.id == id) {
            printf("Title: %s\nAuthor: %s\nAvailable: %d\n",b.title, b.author, b.availablequantity);
            found = 1;
            break;
        }
    }
    fclose(fp);

    if (!found) printf("Book Not Found!\n");
}

void issueBook() {
    Book b;
    Member m;
    IssueRecord r;

    int bookID, memberID;
    int foundBook = 0, foundMember = 0;
    bookID = getIntInput("\nEnter Book ID: ");
    memberID = getIntInput("Enter Member ID: ");

    FILE *fm = fopen("members.txt", "r");
    while (fscanf(fm, "%d|%[^|]|%f\n", &m.id, m.name, &m.penalty) != EOF) {
        if (m.id == memberID) foundMember = 1;
    }
    fclose(fm);

    if (!foundMember) {
        printf("Member Not Found!\n");
        return;
    }
    FILE *fb = fopen("books.txt", "r");
    FILE *temp = fopen("temp_books.txt", "w");

    while (fscanf(fb, "%d|%[^|]|%[^|]|%d|%d\n",&b.id, b.title, b.author,&b.totalquantity, &b.availablequantity) != EOF) {

        if (b.id == bookID) {
            foundBook = 1;

            if (b.availablequantity == 0) {
                printf("No copies available!\n");
                fclose(fb); fclose(temp);
                remove("temp_books.txt");
                return;
            }

            b.availablequantity--;
        }

        fprintf(temp, "%d|%s|%s|%d|%d\n",b.id, b.title, b.author, b.totalquantity, b.availablequantity);
    }

    fclose(fb);
    fclose(temp);

    remove("books.txt");
    rename("temp_books.txt", "books.txt");

    if (!foundBook) {
        printf("Book Not Found!\n");
        return;
    }
    FILE *fi = fopen("issue.txt", "a");
    getDate(r.issueDate);       
    addDays(r.issueDate, 7, r.dueDate); 

    r.bookID = bookID;
    r.memberID = memberID;
    r.returned = 0;

    fprintf(fi, "%d|%d|%s|%s|%d\n",r.bookID, r.memberID, r.issueDate, r.dueDate, r.returned);

    fclose(fi);

    printf("Book issued successfully! Due date is %s\n", r.dueDate);
}

void returnBook() {
    Book b;
    IssueRecord r;
    Member m;

    int bookID, foundIssue = 0;
    char returnDate[20];

    bookID = getIntInput("\nEnter Book ID to return: ");

    printf("Enter return date (dd/mm/yyyy): ");
    readstring(returnDate, 20);

    FILE *fi = fopen("issue.txt", "r");
    FILE *tempI = fopen("temp_issue.txt", "w");

    int lateDays = 0;
    int memberID = -1;
    char dueDate[20];

    if (!fi) {
        printf("No issued records found!\n");
        return;
    }

    while (fscanf(fi, "%d|%d|%[^|]|%[^|]|%d\n", &r.bookID, &r.memberID, r.issueDate, r.dueDate, &r.returned) != EOF) {

        if (r.bookID == bookID && r.returned == 0 && !foundIssue) {
            foundIssue = 1;
            r.returned = 1;
            memberID = r.memberID;
            strcpy(dueDate, r.dueDate);

            lateDays = daysBetween(dueDate, returnDate);
            if (lateDays < 0) lateDays = 0;
        }

        fprintf(tempI, "%d|%d|%s|%s|%d\n",r.bookID, r.memberID, r.issueDate, r.dueDate, r.returned);
    }

    fclose(fi);
    fclose(tempI);

    if (!foundIssue) {
        printf("No record found for this book ID or book already returned!\n");
        remove("temp_issue.txt"); 
        return;
    }

    remove("issue.txt");
    rename("temp_issue.txt", "issue.txt");

    if (lateDays > 0) {
        FILE *fm = fopen("members.txt", "r");
        FILE *tempM = fopen("temp_members.txt", "w");

        while (fscanf(fm, "%d|%[^|]|%f\n", &m.id, m.name, &m.penalty) != EOF) {
            if (m.id == memberID) {
                m.penalty += lateDays * 5;
                printf("Late return! %s is late by %d days. Penalty added: %d\n",m.name, lateDays, lateDays * 5);
            }
            fprintf(tempM, "%d|%s|%.2f\n", m.id, m.name, m.penalty);
        }

        fclose(fm);
        fclose(tempM);
        remove("members.txt");
        rename("temp_members.txt", "members.txt");
    }

    FILE *fb = fopen("books.txt", "r");
    FILE *tempB = fopen("temp_books.txt", "w");

    while (fscanf(fb, "%d|%[^|]|%[^|]|%d|%d\n",&b.id, b.title, b.author, &b.totalquantity, &b.availablequantity) != EOF) {
        if (b.id == bookID)
            b.availablequantity++;
        fprintf(tempB, "%d|%s|%s|%d|%d\n", b.id, b.title, b.author, b.totalquantity, b.availablequantity);
    }

    fclose(fb);
    fclose(tempB);

    remove("books.txt");
    rename("temp_books.txt", "books.txt");

    printf("Book Returned Successfully!\n");
}

void viewAvailableBook() {
    Book b;
    FILE *fp = fopen("books.txt", "r");

    printf("\nAvailable Books:\n");

    while (fscanf(fp, "%d|%[^|]|%[^|]|%d|%d\n",&b.id, b.title, b.author,&b.totalquantity, &b.availablequantity) != EOF) {

        if (b.availablequantity > 0) {
            printf("ID: %d | %s | %s | Available: %d\n",b.id, b.title, b.author, b.availablequantity);
        }
    }
    fclose(fp);
}

void viewIssuedBook() {
    IssueRecord r;
    FILE *fp = fopen("issue.txt", "r");

    printf("\nIssued Books:\n");

    while (fscanf(fp, "%d|%d|%[^|]|%[^|]|%d\n",&r.bookID, &r.memberID,r.issueDate, r.dueDate, &r.returned) != EOF) {

        if (r.returned == 0)
            printf("BookID: %d | Member: %d | Due: %s\n", r.bookID, r.memberID, r.dueDate);
    }
    fclose(fp);
}

void getDate(char *buffer) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    sprintf(buffer, "%02d/%02d/%04d",tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900);
}

void addDays(char *date, int days, char *newDate) {
    int d, m, y;
    sscanf(date, "%d/%d/%d", &d, &m, &y);

    struct tm t = {0};
    t.tm_mday = d + days;
    t.tm_mon = m - 1;
    t.tm_year = y - 1900;
    mktime(&t);

    sprintf(newDate, "%02d/%02d/%04d",
            t.tm_mday, t.tm_mon + 1, t.tm_year + 1900);
}

int daysBetween(const char *d1, const char *d2){
    int d,m,y;
    struct tm t1={0}, t2={0};

    sscanf(d1, "%d/%d/%d", &d,&m,&y);
    t1.tm_mday = d; t1.tm_mon = m-1; t1.tm_year = y-1900;

    sscanf(d2, "%d/%d/%d", &d,&m,&y);
    t2.tm_mday = d; t2.tm_mon = m-1; t2.tm_year = y-1900;

    time_t time1 = mktime(&t1);
    time_t time2 = mktime(&t2);

    return (int)((time2 - time1) / (60*60*24));
}

int main() {
    int choice;
    char username[50], password[50];
    while (1) {
        printf("\n====== LIBRARY LOGIN ======\n");
        printf("1. Admin Login\n");
        printf("2. User Login\n");
        printf("3. User Registration\n");
        printf("4. Exit\n");

        choice = getIntInput("Enter choice: ");

        if (choice == 1) {
            printf("\nEnter Admin Username: ");
            readstring(username, 50);
            printf("Enter Password: ");
            readstring(password, 50);

            if (strcmp(username, "admin") == 0 && strcmp(password, "1234") == 0)
                adminMenu();
            else
                printf("Invalid Admin Credentials!\n");
        }
        else if (choice == 2) {
            if (loginUser(username))
                userMenu(username);
            else
                printf("Invalid Username or Password!\n");
        }
        else if (choice == 3) {
            registerUser();
        }
        else if (choice == 4) {
            printf("Exiting...\n");
            break;
        }
        else {
            printf("Invalid choice\n");
        }
    }
    return 0;
}

