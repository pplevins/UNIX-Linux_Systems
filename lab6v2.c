//Pinchas Plevinsi
//322211558

#include <pthread.h>
#include <stdio.h>

// Total fuel capacity in liters
#define FUEL_CAPACITY 100

// Current fuel level in liters
int fuel_level = 0;

// Mutex to protect the fuel level variable
pthread_mutex_t fuel_level_mutex;

// Condition variable to signal when there is enough fuel
pthread_cond_t fuel_cond;

// Producer thread function
void *producer(void *arg) {
  while (1) {
    // Lock the fuel level mutex
    pthread_mutex_lock(&fuel_level_mutex);

    // Wait for the fuel level to drop below the capacity
    while (fuel_level >= FUEL_CAPACITY) {
      pthread_cond_wait(&fuel_cond, &fuel_level_mutex);
    }

    // Fill the tank with 15 liters of fuel
    fuel_level += 15;
    printf("Filled fuel... %d\n", fuel_level);

    // Signal to the consumer threads that there is more fuel
    pthread_cond_signal(&fuel_cond);

    // Unlock the fuel level mutex
    pthread_mutex_unlock(&fuel_level_mutex);

    // Sleep for 1 second
    sleep(1);
  }

  return NULL;
}

// Consumer thread function
void *consumer(void *arg) {
  int car_id = *((int *)arg);
  while (1) {
    // Lock the fuel level mutex
    pthread_mutex_lock(&fuel_level_mutex);

    // Wait for the fuel level to be above the car's request
    while (fuel_level < 90) {
      printf("Car %d: No fuel. Waiting...\n", car_id);
      pthread_cond_wait(&fuel_cond, &fuel_level_mutex);
    }

    // Fill the car with 45 liters of fuel
    fuel_level -= 45;

    // Signal to the producer thread that there is less fuel
    pthread_cond_signal(&fuel_cond);

    // Unlock the fuel level mutex
    pthread_mutex_unlock(&fuel_level_mutex);

    // Print a message that the car has been filled
    printf("car %d: Got fuel. Now left: %d\n", car_id, fuel_level);
    sleep(5); //The vehicle is driving. Fuel is being used
  }

  return NULL;
}

int main() {
  // Initialize the fuel level mutex and condition variable
  pthread_mutex_init(&fuel_level_mutex, NULL);
  pthread_cond_init(&fuel_cond, NULL);

  // Create two consumer threads
  pthread_t consumer_threads[2];
  int car_ids[2] = {0, 1};
  for (int i = 0; i < 2; i++) {
    pthread_create(&consumer_threads[i], NULL, consumer, &car_ids[i]);
  }

  // Create a producer thread
  pthread_t producer_thread;
  pthread_create(&producer_thread, NULL, producer, NULL);

  // Join all threads
  pthread_join(producer_thread, NULL);
  for (int i = 0; i < 2; i++) {
    pthread_join(consumer_threads[i], NULL);
  }

  // Destroy the fuel level mutex and condition variable
  pthread_mutex_destroy(&fuel_level_mutex);
  pthread_cond_destroy(&fuel_cond);

  return 0;
}
