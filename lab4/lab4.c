//run this file (after compilation using gcc command) like this: ./lab4 grades.txt grade_listing.txt

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure to store student data
struct Student {
    int student_number;
    double grade;
};

// Structure to store course data
struct Course {
    int course_number;
    struct Student *students;
    int num_students;
};

// Function to compare student numbers for sorting
int compare_student_numbers(const void *a, const void *b) {
    return ((struct Student *)a)->student_number - ((struct Student *)b)->student_number;
}

// Function to calculate the average grade for a course
double calculate_average(struct Course *course) {
    double sum = 0.0;
    for (int i = 0; i < course->num_students; i++) {
        sum += course->students[i].grade;
    }
    return sum / course->num_students;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "The format is: %s input_file output_file\n", argv[0]);
        return 1;
    }

    // Open the input file
    FILE *input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
        perror("Failed to open input file");
        return 1;
    }

    // Open the output file
    FILE *output_file = fopen(argv[2], "w");
    if (output_file == NULL) {
        perror("Failed to open output file");
        fclose(input_file);
        return 1;
    }

    struct Course *courses = NULL;
    int num_courses = 0;

    char line[25];
    while (fgets(line, sizeof(line), input_file) != NULL) {
        int course_number, student_number;
        double grade;
        if (sscanf(line, "%d,%d,%lf", &course_number, &student_number, &grade) != 3) {
            fprintf(stderr, "Error parsing line: %s", line);
            continue;
        }

        // Check if the course already exists
        int course_index = -1;
        for (int i = 0; i < num_courses; i++) {
            if (courses[i].course_number == course_number) {
                course_index = i;
                break;
            }
        }

        // If the course doesn't exist, create it
        if (course_index == -1) {
            num_courses++;
            courses = (struct Course *)realloc(courses, num_courses * sizeof(struct Course));
            if (courses == NULL) {
                perror("Memory allocation failed");
                fclose(input_file);
                fclose(output_file);
                return 1;
            }
            course_index = num_courses - 1;
            courses[course_index].course_number = course_number;
            courses[course_index].num_students = 0;
            courses[course_index].students = NULL;
        }

        // Add student and grade to the course
        courses[course_index].num_students++;
        courses[course_index].students = (struct Student *)realloc(
            courses[course_index].students, courses[course_index].num_students * sizeof(struct Student)
        );
        courses[course_index].students[courses[course_index].num_students - 1].student_number = student_number;
        courses[course_index].students[courses[course_index].num_students - 1].grade = grade;
    }

    // Sort courses by course number in ascending order
    for (int i = 0; i < num_courses - 1; i++) {
        for (int j = i + 1; j < num_courses; j++) {
            if (courses[i].course_number > courses[j].course_number) {
                struct Course temp = courses[i];
                courses[i] = courses[j];
                courses[j] = temp;
            }
        }
    }

    // Sort students and grades within each course by student number
    for (int i = 0; i < num_courses; i++) {
        qsort(courses[i].students, courses[i].num_students, sizeof(struct Student), compare_student_numbers);
    }

    // Write the results to the output file
    for (int i = 0; i < num_courses; i++) {
        fprintf(output_file, "course %d\n", courses[i].course_number);
        for (int j = 0; j < courses[i].num_students; j++) {
            fprintf(output_file, "%d,%.2lf\n", courses[i].students[j].student_number, courses[i].students[j].grade);
        }
        double average = calculate_average(&courses[i]);
        fprintf(output_file, "average %.2lf\n\n", average);
    }

    // Clean up and close files
    for (int i = 0; i < num_courses; i++) {
        free(courses[i].students);
    }
    free(courses);

    fclose(input_file);
    fclose(output_file);

    return 0;
}
