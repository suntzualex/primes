// Versie1 van priemgetallen programma.
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>

using namespace std;
/**
 * Eerste priemgetallen zoekprogramma met threads.
 * gebruik mutex om de kritieke onderdelen te beschermen.
 */
 // Definieer configuratie parameters die nodig zijn voor je programma.
int numberOfThreads = 2;
// Definieer de globale variabele nodig voor je programma
std::mutex m;
std::vector<bool> primeNumberCandidates;
int maxNumberToTestPrimeFor = 0;
int counter = 0; // start de globale counter op 0

/**
 * Hulpfunctie voor het omzetten van string argument
 * naar een int.
 */
int getIntegerFromString(string argument) {
    int result = 0;
    try {
        result = std::stoi(argument);
    }
    catch (std::invalid_argument exception) {
        //std::cerr << "Expected a number but got: "
        //<< argument << std::endl;
    }
    return result;
}

// Parses a string to form a vector of arguments.
// The separator is a space char (' ');
void parseArguments(const std::vector<std::string>& arguments) {
    if (arguments.size() == 0) {
        // dit zou nooit moeten voorkomen, maar als het gebeurt, handel het correct af.
        abort();
    }
    if (arguments.size() == 1) {
        std::cerr << "Usage: " << arguments[0]
            << " [max-number-to-test] [number-of-threads; default is 2]"
            << std::endl;
        exit(0);
    }
    if (arguments.size() > 3) {
        std::cerr << "Teveel argumenten: " << arguments.size() << std::endl;
        exit(0);
    }
    maxNumberToTestPrimeFor = getIntegerFromString(arguments[1]);
    if (maxNumberToTestPrimeFor <= 0) {
        std::cerr << "Fout: nummer moet een getal zijn."
            << std::endl;
        exit(0);
    }
    if (maxNumberToTestPrimeFor == 1) {
        std::cerr << "Fout: nummer moet groter zijn dan 1."
            << std::endl;
        exit(0);
    }
    if (arguments.size() == 3) {
        numberOfThreads = getIntegerFromString(arguments[2]);
        if (numberOfThreads <= 0 || numberOfThreads > maxNumberToTestPrimeFor) {
            std::cerr << "Voer een correct getal in voor het aantal threads.\n"
                      << "Het aantal mag niet kleiner zijn dan 2 of negatief.\n";
            exit(0);
        }
    }
}

// Bruteforce test of het een priemgetal is.
// bron: wikipedia pseudocode
bool isPrime(int n) {
 if(n <= 3) {
   return (n > 1); // groter dan 1 en kleiner of gelijk aan 3 is priem.
 }
 else if(n % 2 == 0 || n % 3 == 0) {
   return false;
 }
 int i = 5;
 while(i*i <= n) {
   if(n % i == 0 || n % (i + 2) == 0) {
     return false;
   }
  i += 6;
 }
return true;
}

/**
 * Functie zoekt naar priemgetallen (zonder threads)
 * zoekt via de functie isPrime(getal).
 */
void searchPrimeNumbers() {
    for (int i = 1; i < maxNumberToTestPrimeFor; i++) {
        primeNumberCandidates[i] = isPrime(i + 1);
    }
}



/**
 * Zoeken naar priemgetallen maar dan met gebruik
 * van threads. Gebruik een thread-local counter en 
 * pas de global counter aan. maar met een lock.
 */
void searchPrimeNumbersThreaded() {
   
    int threadLocalCounter = 0;
    cout << "thread local = " << threadLocalCounter << endl;
    while(counter+threadLocalCounter < maxNumberToTestPrimeFor){
       int index = counter+threadLocalCounter;
       primeNumberCandidates[index] = isPrime(index+1);
       threadLocalCounter++;
       m.lock(); // begin kritische gebied
       counter++;
       m.unlock(); // einde kritische gebied.
  }
}
// Na de berekening geef je de primes weer
void displayPrimeNumbers() {
    std::cout << "Gevonden priemgetallen: ";
    for (int i = 0; i < maxNumberToTestPrimeFor; i++) {
        if (primeNumberCandidates[i] == true) {
            std::cout << (i + 1) << ", ";
        }
    }
    // trailing comma en space weghalen.
    std::cout << "\b\b" << " "; // drie backspaces en spatie
    std::cout << std::endl;
}


int main(int argc, char* argv[]) {
    std::vector<std::string> arguments;
    // kijk niet naar de volgende regel
    std::copy(&argv[0], &argv[argc], std::back_inserter(arguments));
    // parseer de command line
    parseArguments(arguments);
    primeNumberCandidates.resize(maxNumberToTestPrimeFor); // after parse
    // voer het programma uit
    std::vector<std::thread> t;
    t.resize(numberOfThreads);

    for (int i = 0; i < numberOfThreads; i++) {
        t[i] = std::thread(searchPrimeNumbersThreaded);
    }
    for (int i = 0; i < numberOfThreads; ++i) {
        t[i].join();
    }
    // laat het resultaat zien
    displayPrimeNumbers();
    return 0;
}



