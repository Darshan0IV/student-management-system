#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "students.dat"

struct Student {
    char rollNo[20];
    char name[50];
    float marks;
    char contact[20];
};

// Function prototypes
void addStudent();
void displayStudents();
void searchStudent();
void modifyStudent();
void deleteStudent();
void clearInputBuffer();

int main() {
    int choice;
    while (1) {
        printf("\n*** Student Management System ***\n");
        printf("1. Add Student\n");
        printf("2. Display All Students\n");
        printf("3. Search Student\n");
        printf("4. Modify Student\n");
        printf("5. Delete Student\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer(); // Fix: clear buffer before calling any fgets() in sub-functions

        switch (choice) {
            case 1: addStudent(); break;
            case 2: displayStudents(); break;
            case 3: searchStudent(); break;
            case 4: modifyStudent(); break;
            case 5: deleteStudent(); break;
            case 6: 
                printf("Exiting the program. Goodbye!\n");
                exit(0);
            default:
                printf("Invalid choice! Please select between 1-6.\n");
        }
    }
    return 0;
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void addStudent() {
    struct Student s;
    FILE *fp = fopen(FILENAME, "ab");
    if (fp == NULL) {
        printf("Error opening file.\n");
        return;
    }

    printf("Enter Roll Number: ");
    fgets(s.rollNo, sizeof(s.rollNo), stdin);
    s.rollNo[strcspn(s.rollNo, "\n")] = 0;

    printf("Enter Name: ");
    fgets(s.name, sizeof(s.name), stdin);
    s.name[strcspn(s.name, "\n")] = 0;

    printf("Enter Marks: ");
    while (scanf("%f", &s.marks) != 1 || s.marks < 0 || s.marks > 100) {
        printf("Invalid input. Enter marks between 0 and 100: ");
        clearInputBuffer();
    }
    clearInputBuffer();

    printf("Enter Contact Info: ");
    fgets(s.contact, sizeof(s.contact), stdin);
    s.contact[strcspn(s.contact, "\n")] = 0;

    fwrite(&s, sizeof(s), 1, fp);
    fclose(fp);

    printf("Student added successfully!\n");
}

void displayStudents() {
    FILE *fp = fopen(FILENAME, "rb");
    if (fp == NULL) {
        printf("No records found.\n");
        return;
    }

    struct Student s;
    printf("\n------------------- Student Records -------------------------\n");
    printf("%-20s %-20s %-10s %-15s\n", "Roll No", "Name", "Marks", "Contact");
    printf("-------------------------------------------------------------\n");

    int found = 0;
    while (fread(&s, sizeof(s), 1, fp) == 1) {
        printf("%-20s %-20s %-10.2f %-15s\n", s.rollNo, s.name, s.marks, s.contact);
        found = 1;
    }

    if (!found) {
        printf("No records to display.\n");
    }

    fclose(fp);
}

void searchStudent() {
    FILE *fp = fopen(FILENAME, "rb");
    if (fp == NULL) {
        printf("No records found.\n");
        return;
    }

    char searchRollNo[20];
    printf("Enter Roll Number to search: ");
    fgets(searchRollNo, sizeof(searchRollNo), stdin);
    searchRollNo[strcspn(searchRollNo, "\n")] = 0;

    struct Student s;
    int found = 0;
    while (fread(&s, sizeof(s), 1, fp) == 1) {
        if (strcmp(s.rollNo, searchRollNo) == 0) {
            printf("\nStudent Found:\n");
            printf("Roll No: %s\n", s.rollNo);
            printf("Name: %s\n", s.name);
            printf("Marks: %.2f\n", s.marks);
            printf("Contact: %s\n", s.contact);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Student with Roll Number %s not found.\n", searchRollNo);
    }

    fclose(fp);
}

void modifyStudent() {
    FILE *fp = fopen(FILENAME, "rb+");
    if (fp == NULL) {
        printf("No records found.\n");
        return;
    }

    char searchRollNo[20];
    printf("Enter Roll Number to modify: ");
    fgets(searchRollNo, sizeof(searchRollNo), stdin);
    searchRollNo[strcspn(searchRollNo, "\n")] = 0;

    struct Student s;
    int found = 0;
    long pos;

    while (fread(&s, sizeof(s), 1, fp) == 1) {
        if (strcmp(s.rollNo, searchRollNo) == 0) {
            found = 1;
            pos = ftell(fp) - sizeof(s);
            printf("Current details:\n");
            printf("Name: %s\nMarks: %.2f\nContact: %s\n", s.name, s.marks, s.contact);

            printf("\nEnter new Name: ");
            fgets(s.name, sizeof(s.name), stdin);
            s.name[strcspn(s.name, "\n")] = 0;

            printf("Enter new Marks: ");
            while (scanf("%f", &s.marks) != 1 || s.marks < 0 || s.marks > 100) {
                printf("Invalid input. Enter marks between 0 and 100: ");
                clearInputBuffer();
            }
            clearInputBuffer();

            printf("Enter new Contact Info: ");
            fgets(s.contact, sizeof(s.contact), stdin);
            s.contact[strcspn(s.contact, "\n")] = 0;

            fseek(fp, pos, SEEK_SET);
            fwrite(&s, sizeof(s), 1, fp);
            printf("Student record updated successfully!\n");
            break;
        }
    }

    if (!found) {
        printf("Student with Roll Number %s not found.\n", searchRollNo);
    }

    fclose(fp);
}

void deleteStudent() {
    FILE *fp = fopen(FILENAME, "rb");
    FILE *tempFp = fopen("temp.dat", "wb");

    if (fp == NULL) {
        printf("No records found.\n");
        if (tempFp) fclose(tempFp);
        return;
    }
    if (tempFp == NULL) {
        printf("Error creating temporary file.\n");
        fclose(fp);
        return;
    }

    char searchRollNo[20];
    printf("Enter Roll Number to delete: ");
    fgets(searchRollNo, sizeof(searchRollNo), stdin);
    searchRollNo[strcspn(searchRollNo, "\n")] = 0;

    struct Student s;
    int found = 0;

    while (fread(&s, sizeof(s), 1, fp) == 1) {
        if (strcmp(s.rollNo, searchRollNo) == 0) {
            found = 1;
            continue; // skip writing (delete)
        }
        fwrite(&s, sizeof(s), 1, tempFp);
    }

    fclose(fp);
    fclose(tempFp);

    if (found) {
        remove(FILENAME);
        rename("temp.dat", FILENAME);
        printf("Student record deleted successfully.\n");
    } else {
        printf("Student with Roll Number %s not found.\n", searchRollNo);
        remove("temp.dat");
    }
}
