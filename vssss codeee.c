// tina shakerian , fatemeh ghaemi
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_WAITING_CHAIRS 5
#define NUM_STUDENTS 10

pthread_mutex_t mutex_lock;
sem_t students_sem;      // تعداد دانشجویان منتظر
sem_t ta_sem;            // سیگنال بیدار شدن TA
int waiting_students = 0; // تعداد دانشجویان در صف انتظار
void* ta_thread(void* arg) {
    while (1) {
        sem_wait(&students_sem); // منتظر دانشجوی بعدی
        pthread_mutex_lock(&mutex_lock);

        waiting_students--;
        printf("TA: Helping a student. Students waiting = %d\n", waiting_students);

        pthread_mutex_unlock(&mutex_lock);

        // کمک به دانشجو برای چند ثانیه
        sleep(2);

        sem_post(&ta_sem); // کار با یک دانشجو تمام شد
    }
    return NULL;
}
void* student_thread(void* arg) {
    int id = *(int*)arg;
    free(arg); // چون با malloc ساخته شده

    while (1) {
        sleep(rand() % 5 + 1); // ورود تصادفی

        pthread_mutex_lock(&mutex_lock);

        if (waiting_students < MAX_WAITING_CHAIRS) {
            waiting_students++;
            printf("Student %d: Waiting. Students waiting = %d\n", id, waiting_students);

            sem_post(&students_sem); // به TA بگو دانشجو منتظره
            pthread_mutex_unlock(&mutex_lock);

            sem_wait(&ta_sem); // منتظر کمک TA
            printf("Student %d: Got help from TA.\n", id);
        } else {
            pthread_mutex_unlock(&mutex_lock);
            printf("Student %d: No chair, will come back later.\n", id);
        }
    }
    return NULL;
}
int main() {
    srand(time(NULL));
    pthread_t ta;
    pthread_t students[NUM_STUDENTS];

    pthread_mutex_init(&mutex_lock, NULL);
    sem_init(&students_sem, 0, 0);
    sem_init(&ta_sem, 0, 0);

    pthread_create(&ta, NULL, ta_thread, NULL);

    for (int i = 0; i < NUM_STUDENTS; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&students[i], NULL, student_thread, id);
    }

    pthread_join(ta, NULL);
    for (int i = 0; i < NUM_STUDENTS; i++) {
        pthread_join(students[i], NULL);
    }

    pthread_mutex_destroy(&mutex_lock);
    sem_destroy(&students_sem);
    sem_destroy(&ta_sem);

    return 0;
}