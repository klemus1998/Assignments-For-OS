/*
Karla Lemus

This program takes in an input and stimulates the enforcement of the
restrictions put forth from the input using a Pthread mutexes and a
condition variable.
*/
#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

using namespace std;
#define MAXTHREADS 50

static int maxCarsAllowed;
static int maxNCarsAllowed;
static int maxSCarsAllowed;
static int currTotalCars;
static int currNBCars;
static int currSBCars;
static int numCarsWait;
static int totalNBCars;
static int totalSBCars;

static pthread_mutex_t myLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t clear = PTHREAD_COND_INITIALIZER;

//car struct that stores the number, the time the car takes to go 
//through the tunnel, and the direction it is going in
//i used the direction because I was trying to implement 
//everything in one function, but it didn't make it any better so I
//just kept it becuase it was easier to keep it there
struct car
{
public:
    int carNum;
    unsigned int travelTime;
    char direction;
};

//northbound car function that takes a struct function as an argument 
//and then has the car go through the tunnel 
void *northChild(void* dum);

//south car child function that makes a thread for a southbound car
//or waits if there is more cars going through the tunnel than needed
void *southChild(void* dum);


//main thread and where the child threads are created
int main()
{
    currTotalCars = 0;
    currNBCars = 0;
    currSBCars = 0;
    numCarsWait = 0;
    totalNBCars = 1;
    totalSBCars = 1;

    unsigned int arrivalNum;
    unsigned int travelTime;
    char d;
    int counter = 0;

    struct car carList;

    pthread_t tid[MAXTHREADS];

    cin >> maxCarsAllowed;
    cin >> maxNCarsAllowed;
    cin >> maxSCarsAllowed;

    cout << "Max number of cars in the tunnel: " << maxCarsAllowed << endl;
    cout << "Max number of northbound cars: " << maxNCarsAllowed << endl;
    cout << "Max number of southbound cars: " << maxSCarsAllowed << endl;    

    
    while (cin >> arrivalNum >> d >> travelTime)
    {
        carList.travelTime = travelTime;
        
        sleep(arrivalNum);
        carList.carNum = (d == 'N') ? totalNBCars++ : totalSBCars++;
        if (d == 'N')
        {
            pthread_create(&tid[counter], NULL, northChild, (void*) &carList);
        }

        else
        {
            pthread_create(&tid[counter], NULL, southChild, (void*) &carList);
        }

        counter++;
    }
    //wait for the thread to finish 
    for(int i = 0; i < counter; i++)
        pthread_join(tid[i], NULL);

    cout << totalNBCars << " northbound car(s) crossed the tunnel.\n";
    cout << totalSBCars << " southbound car(s) crossed the tunnel.\n";
    cout << numCarsWait << " car(s) had to wait.\n";
    return 0;
}

void *northChild(void* dum)
{
    struct car *dummer;
    dummer = (struct car *) dum;
    int carNumber;
    carNumber = dummer->carNum;
    unsigned int travelingTime;
    travelingTime = dummer->travelTime;
    bool carWaiting = false;

    pthread_mutex_lock(&myLock);
    cout << "Northbound car # " << carNumber << " arrives at the tunnel.\n";
    //if there are more than max cars allowed, or there are more northbound
    //cars in the tunnel than allowed, wait for the signal and then go
    //through
    while(currTotalCars >= maxCarsAllowed || currNBCars >= maxNCarsAllowed)
    {
        if(carWaiting == false)
        {
            cout << "-- Northbound car # " << carNumber << " has to wait.\n";
            numCarsWait++;
            carWaiting = true;
        }
        pthread_cond_wait(&clear, &myLock);
    }
    currNBCars++;
    currTotalCars++;
    cout << "Northbound car # " << carNumber << " enters the tunnel.\n";
    pthread_cond_signal(&clear);
    pthread_mutex_unlock(&myLock);

    
    sleep(travelingTime);

    pthread_mutex_lock(&myLock);
    currNBCars--;
    currTotalCars--;
    cout << "Northbound car # " << carNumber << " exits the tunnel.\n";
    pthread_cond_signal(&clear);
    pthread_cond_broadcast(&clear);
    pthread_mutex_unlock(&myLock);
}

void *southChild(void* dum)
{
    struct car *dummer;
    dummer = (struct car *) dum;

    int carNumber;
    carNumber = dummer->carNum;

    unsigned int travelingTime;
    travelingTime = dummer->travelTime;

    bool carWaiting = false;

    pthread_mutex_lock(&myLock);
    cout << "Southbound car # " << carNumber << " arrives at the tunnel.\n";
    //if there are more than max cars allowed, or there are more southbound
    //cars in the tunnel than allowed, wait for the signal and then go
    //through
    while(currTotalCars >= maxCarsAllowed || currSBCars >= maxSCarsAllowed)
    {
        if(carWaiting ==false)
        {
            cout << "-- Southbound car # " << carNumber << " has to wait.\n";
            numCarsWait++;
            carWaiting = true;
        }
        pthread_cond_wait(&clear, &myLock);
    }
    currSBCars++;
    currTotalCars++;
    cout << "Southbound car # " << carNumber << " enters the tunnel.\n";
    pthread_cond_signal(&clear);
    pthread_mutex_unlock(&myLock);

    sleep(travelingTime);

    pthread_mutex_lock(&myLock);
    currSBCars--;
    currTotalCars--;
    cout << "Southbound car # " << carNumber << " exits the tunnel.\n";
    pthread_cond_signal(&clear);
    pthread_cond_broadcast(&clear);
    pthread_mutex_unlock(&myLock);
}
