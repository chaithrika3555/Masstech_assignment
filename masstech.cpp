#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <chrono>

// Define your data structure for processing
struct Data {
    int sensorData; // Example data field; replace with actual data fields
    // Add other data fields if needed
    // e.g., char sensorName[50];
};

// Global variables for shared data and synchronization
std::vector<Data> dataQueue;
std::mutex dataMutex;
std::condition_variable dataCV;

// Flag to signal the end of data input
bool inputDataComplete = false;

// Function to process data in a thread
void processData(int threadId) {
    while (true) {
        // Acquire the lock to access shared data
        std::unique_lock<std::mutex> lock(dataMutex);

        // Wait until there is data in the queue or data input is complete
        dataCV.wait(lock, [&] { return !dataQueue.empty() || inputDataComplete; });

        // Check if there is data in the queue
        if (!dataQueue.empty()) {
            // Retrieve data from the queue
            Data currentData = dataQueue.back();
            dataQueue.pop_back();

            // Release the lock before processing to allow other threads to access data
            lock.unlock();

            // Process the data (add your processing logic here)
            std::cout << "Thread " << threadId << " processed data: " << currentData.sensorData << std::endl;
        } else {
            // Release the lock if data input is complete
            lock.unlock();
            std::cout << "Thread " << threadId << " finished processing. No more data to process." << std::endl;
            break;
        }
    }
}

// Function to simulate data input from multiple sources
void simulateDataInput() {
    // Simulate data input (replace this with your actual data input logic)
    for (int i = 0; i < 10; ++i) {
        Data newData;
        // Add your logic to populate newData (e.g., read from file, sensor, etc.)
        newData.sensorData = i; // Example: Assign a sequential value

        // Acquire the lock before modifying the shared data queue
        std::unique_lock<std::mutex> lock(dataMutex);

        // Push the new data into the queue
        dataQueue.push_back(newData);

        // Release the lock and notify waiting threads
        lock.unlock();
        dataCV.notify_one();

        // Introduce a delay to simulate the data arrival rate
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        // Print a message indicating data has been added
        std::cout << "Data added to the queue: " << newData.sensorData << std::endl;
    }

    // Signal the end of data input
    inputDataComplete = true;
    dataCV.notify_all();

    // Print a message indicating data input is complete
    std::cout << "Data input is complete." << std::endl;
}

int main() {
    // Start the threads
    std::thread thread1(processData, 1);
    std::thread thread2(processData, 2);

    // Start simulating data input
    simulateDataInput();

    // Join the threads
    thread1.join();
    thread2.join();

    return 0;
}
